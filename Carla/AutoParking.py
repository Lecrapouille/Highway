# -*- coding: future_fstrings -*-
#!/usr/bin/env python3
################################################################################
### Carla simulation with self parking (only one-trial parallel maneuver) based on
### - (fr) "Estimation et controle pour le pilotage automatique de vehicule" by
###   Sungwoo Choi
### - (en) "Easy Path Planning and Robust Control for Automatic Parallel Parking"
###   by Sungwoo CHOI, Clement Boussard, Brigitte d'Andrea-Novel.
################################################################################

import glob
import os
import sys
import weakref

# Carla simulator: https://carla.org/
try:
    sys.path.append(glob.glob('../carla/dist/carla-*%d.%d-%s.egg' % (
        sys.version_info.major,
        sys.version_info.minor,
        'win-amd64' if os.name == 'nt' else 'linux-x86_64'))[0])
except IndexError:
    pass

import carla
import time
import random
import numpy as np
import cv2
import math

try:
    import pygame
    from pygame.locals import K_ESCAPE
    from pygame.locals import K_q
except ImportError:
    raise RuntimeError('cannot import pygame, make sure pygame package is installed')

client = None

# Camera's picture dimensions
IM_WIDTH = int(1920/2) # 480
IM_HEIGHT = int(1080/2) # 270
IM_FOV = 110
surfaces = [None, None]
myfonts = [None]

VMAX = 1.0 # Max speed [m/s]
ADES = 1.0 # Desired acceleration [m/s/s]
KP = 2.0 # PID: K coef [no unit]
KI = 1.0 # PID: I coef [no unit]
KD = 1.0 # PID: D coef [no unit]
MAX_INTEG_SAT = 0.7
MIN_INTEG_SAT = -MAX_INTEG_SAT
PARKING_BATAILLE_LENGTH = 5.0 # Parking spot size [m]

################################################################################
### Carla's constants
################################################################################

# Carla's vehicle max acceleration = +/-3m/s/s => command = +/-1
CARLA_MAX_ACC = 3.0

# Carla's vehicel max steering angle = +/-70 deg => command = +/-1
CARLA_MAX_STEER = 70.0

################################################################################
### Monitor states as Julia script
################################################################################

f = open("data.jl", "w")
f.write("using Plots\n\n")
f.write("# time  ref_speed  obs_speed  PID_err  PID_out  throttle  brake  reverse\n")
f.write("D=[0 0 0 0  0 0 0 0")

################################################################################
### Callbacks
################################################################################

def camera_callback(id, image):
    image.convert(carla.ColorConverter.Raw)
    array = np.frombuffer(image.raw_data, dtype=np.dtype("uint8"))
    array = np.reshape(array, (image.height, image.width, 4))
    array = array[:, :, :3]
    array = array[:, :, ::-1]
    surfaces[id] = pygame.surfarray.make_surface(array.swapaxes(0, 1))

################################################################################
### Helpers
################################################################################

def distance(front_car, back_car):
    x = front_car.x - back_car.x
    y = front_car.y - back_car.y
    return math.sqrt(x * x + y * y)

def deg2rad(deg):
    return deg * 0.01745329251994

def rad2deg(rad):
    return rad * 57.295779513

def constrain(val, mini, maxi):
    return max(min(val, maxi), mini)

def constrain_normalization(val, minmax):
    return max(-minmax, min(minmax, val)) / minmax

def text(txt, x, y, display):
    surf = myfonts[0].render(txt, False, (0, 0, 0))
    display.blit(surf, (x, y))

def rear_position(vehicle):
    wheels = vehicle.get_physics_control().wheels
    return (wheels[2].position + wheels[3].position) / 200.0

def get_intrinsic(width, height, fov):
    k = np.identity(3)
    k[0, 2] = width / 2.0
    k[1, 2] = height / 2.0
    k[0, 0] = k[1, 1] = width / (2.0 * np.tan(fov * np.pi / 360.0))
    return k

def project_to_image(world_point, camera_transform, intrinsic):
    # Passage matrix from world space to camera space
    world_2_camera = camera_transform.get_inverse_matrix()
    # Reshape the 3D point to be converted from world space (A 4D vector to be multiplied by 4x4 matrix)
    point_in_world_ref = np.array([[world_point.x], [world_point.y], [world_point.z], [1.0]])
    # Convert the 3D point from world space to camera space
    point_in_camera_ref = np.dot(world_2_camera, point_in_world_ref)
    # Convert from UE4's coordinate system to OpenCV camera coordinates (see lidar_to_camea.py)
    ue4_to_opencv = np.array([[0, 1, 0], [0, 0, -1], [1, 0, 0]])
    point_in_camera_ref = np.dot(ue4_to_opencv, point_in_camera_ref[:3])
    # Project the 3D point to the 2D camera image using intrinsic parameters fo the camera
    point_in_camera_image = np.dot(intrinsic, point_in_camera_ref)
    # Return the normalized vector
    return (int(point_in_camera_image[0] / point_in_camera_image[2]), int(point_in_camera_image[1] / point_in_camera_image[2]))

################################################################################
### Ultra basic and naive PID to maintain constant the acceleration.
### https://skill-lync.com/student-projects/designing-a-controller-for-controlling-
### lateral-and-longitudinal-movement-of-self-driving-car-using-python-and-test-it-
### by-using-carla-simulator
################################################################################
class PID(object):
    def __init__(self, kp, ki, kd):
        self.kp = kp
        self.ki = ki
        self.kd = kd
        self.error = 0.0
        self.last_error = 0.0
        self.sum_error = 0.0
        self.prev_ret = 0.0
        self.integrating = False

    # Ultra basic PID regulator: no integral antiwindup, no filtering on derived
    # error ...
    # http://ww1.microchip.com/downloads/en/AppNotes/Atmel-2558-Discrete-PID-Controller-on-tinyAVR-and-megaAVR_ApplicationNote_AVR221.pdf
    def update(self, dt, reference, observation):
        self.error = reference - observation
        # P term
        pterm = self.kp * self.error
        # I term + antiwindup
        if self.integrating:
            tmp = self.sum_error + self.error
            if tmp > MAX_INTEG_SAT:
                tmp = MAX_INTEG_SAT
            elif tmp < MIN_INTEG_SAT:
                tmp = MIN_INTEG_SAT
            self.sum_error = tmp
        iterm = self.ki * self.sum_error # * dt
        # D term
        dterm = self.kd * (self.error - self.last_error) # / dt
        self.last_error = self.error
        # PID term
        ret = pterm + dterm + iterm
        # Antiwindup
        self.integrating = (np.abs(ret - self.prev_ret) < 0.01)
        self.prev_ret = ret
        return ret

################################################################################
### Car with self parking (only one-trial parallel maneuver) based on
### - (fr) "Estimation et controle pour le pilotage automatique de vehicule" by
###   Sungwoo Choi
### - (en) "Easy Path Planning and Robust Control for Automatic Parallel Parking"
###   by Sungwoo CHOI, Clement Boussard, Brigitte d'Andrea-Novel.
################################################################################
class CitroenC3(object):
    # Carla
    world = None
    vehicle = None

    # Citroen C3 dimensions (https://www.bsegfr.com/images/books/5/8/index.47.jpg)
    width =  1.728 # [m]
    length = 3.941 # [m]
    wheelbase = 2.466 # [m]
    front_overhang = 0.815 # [m]
    back_overhang = length - wheelbase - front_overhang # [m]
    max_steering_angle = 30.0 # [deg]

    # Citroen C3 turning radius (computed during the init)
    Rmin = 0.0 # Turning radius of the fake front rear [m]
    Rimin = 0.0 # Turning radius of the internal wheel [m]
    Remin = 0.0 # Turning radius of the external wheel [m]
    Rwmin = 0.0 # Rimin + car width / 2 [m]
    Lmin = 0.0 # Min parking spot size to make a one-trial maneuver [m]

    # One-trial parallel maneuver: path waypoints.
    #  i: initial, f: final, c1, c2 center of the two turning circles,
    #  s: position for the 1st turn, t: position for the 2nd turn
    Xi = Yi = Xf = Yf = Xc1 = Yc1 = Xc2 = Yc2 = Xt = Yt = Xs = Ys = 0.0
    min_central_angle = 0.0 # angle to turn [rad]

    # One-trial parallel maneuver: references to cruise control
    time = 0.0
    speeds = [] # car speed references [m/s]
    accelerations = [] # car acceleration references [m/s/s]
    steerings = [] # Steering angle references [rad]
    reverse = 1 # +1: marche avant -1: marche arriere

    # PID regulator
    pid = None

    # --------------------------------------------------------------------------
    # Init Carla stuffs and vehicle states for the auto-parking
    # --------------------------------------------------------------------------
    def __init__(self, world):
        self.pid = PID(KP, KI, KD)

        # Carla /opt/carla-simulator/CarlaUE4/Content/Carla/Blueprints
        blueprint_library = world.get_blueprint_library()
        car_bp = blueprint_library.filter('vehicle.citroen.c3')[0]
        transform = world.get_map().get_spawn_points()[0]
        self.vehicle = world.spawn_actor(car_bp, transform)

        # LOL https://github.com/carla-simulator/carla/issues/1565
        self.vehicle.apply_control(carla.VehicleControl(
            throttle = 0.0, brake = 1.0, steer = 0.0, reverse = 1
        ))
        time.sleep(1.0)

        # Compute the minimal turning radius when the steering angle of the car is maximal
        # http://www.autoturn.ch/giration/standard_r.html
        self.Rmin = self.wheelbase / math.sin(deg2rad(self.max_steering_angle))
        self.Rimin = math.sqrt(self.Rmin**2 - self.wheelbase**2) - (self.width / 2.0)
        self.Remin = math.sqrt((self.Rimin + self.width)**2 + (self.wheelbase + self.front_overhang)**2)
        self.Rwmin = self.Rimin + 0.5 * self.width
        self.Lmin = self.back_overhang + math.sqrt(self.Remin**2 - self.Rimin**2)

        print("Rmin=", self.Rmin, "\nRimin=", self.Rimin, "\nRemin=", self.Remin,
              "\nRwmin=", self.Rwmin, "\nLmin=", self.Lmin)

    # --------------------------------------------------------------------------
    # Store a reference for the cruise control: pair (speed, acceleration...) + duration.
    # When storing this pair, transform the duration to a time value (= sum from previous time)
    # --------------------------------------------------------------------------
    def add_reference(self, reference, value, duration):
        if duration < 0.0:
            raise Exception("Negative time")

        (_,t) = (0.0, 0.0) if len(reference) == 0 else reference[-1]
        return reference.append((value, t + duration))

    # --------------------------------------------------------------------------
    # Get the reference for the cruise control depending on the current time
    # --------------------------------------------------------------------------
    def read_reference(self, reference, time):
        # Dummy array: return idle reference
        if len(reference) == 0:
            self.index = None
            print("dummy array", time)
            return 0.0

        # Search the current time
        N = len(reference) - 1
        d, t = reference[N]
        if time <= t:
            for i in range (0,N): # Could be optimize if we memorize the last index
                d, t = reference[i]
                if time < t:
                    self.index = i
                    return d

        # Array out of bound: return idle reference
        self.index = None
        return 0.0

    # --------------------------------------------------------------------------
    # Compute path waypoints for the one-trial parallel maneuver.
    # --------------------------------------------------------------------------
    def compute_path_planning(self, start_x, start_y, end_x, end_y):
        # Initial position
        self.Xi = start_x
        self.Yi = start_y
        print("xyi", self.Xi, self.Yi)

        # Final destination: the parking slot
        self.Xf = end_x
        self.Yf = end_y
        print("xyf", self.Xf, self.Yf)

        # C1: center of the ending turn (end position of the 2nd turning maneuver)
        self.Xc1 = self.Xf + self.back_overhang
        self.Yc1 = self.Yf + self.Rwmin
        print("xyc1", self.Xc1, self.Yc1)

        # C2: center of the starting turn (begining position of the 1st turning
        # maneuver). Note: the X-coordinate cannot yet be computed.
        self.Yc2 = self.Yi - self.Rwmin

        # Tangential intersection point of C1 and C2.
        self.Yt = 0.5 * (self.Yc1 + self.Yc2)
        d = self.Rwmin * self.Rwmin - (self.Yt - self.Yc1)**2
        if d < 0.0:
            print("Car is too far away on Y-axis (greater than its turning radius)")
            return False
        self.Xt = self.Xc1 + math.sqrt(d)
        print("xt", self.Xt, self.Yt)

        # Position of the car for starting the 1st turn.
        self.Xs = 2.0 * self.Xt - self.Xc1
        self.Ys = self.Yi

        # X position of C1.
        self.Xc2 = self.Xs
        print("xys", self.Xs, self.Ys)
        print("xyc2", self.Xc2, self.Yc2)

        # Minimal central angle for making the turn = atanf((Xt - Xc1) / (Yc1 - Yt))
        self.min_central_angle = math.atan2(self.Xt - self.Xc1, self.Yc1 - self.Yt)
        print("XYi", self.Xi, ",", self.Yi, "XYs", self.Xs, self.Ys)
        print("min_central_angle", rad2deg(self.min_central_angle))

    # --------------------------------------------------------------------------
    # Compute path references for the one-trial parallel maneuver.
    # --------------------------------------------------------------------------
    def generate_trajectory_references(self, vmax, ades):
        # Duration to turn front wheels to the maximal angle [s]
        DURATION_TO_TURN_WHEELS = 0.5

        # Be sure to use absolute values
        vmax = math.fabs(vmax)
        ades = math.fabs(ades)

        self.time = 0.0
        self.speeds = []
        self.accelerations = []
        self.steerings = []

        #################################
        ### Kinematics reference: speed
        #################################

        # Init reference to idle the car.
        self.add_reference(self.speeds, 0.0, DURATION_TO_TURN_WHEELS)
        self.add_reference(self.steerings, 0.0, DURATION_TO_TURN_WHEELS)

        # Init car position -> position when starting the 1st turn
        if self.Xi > self.Xs:
            t = (self.Xi - self.Xs) / vmax;
            self.add_reference(self.speeds, -vmax, t)
            self.add_reference(self.steerings, 0.0, t)
        else:
            t = (self.Xs - self.Xi) / vmax;
            self.add_reference(self.speeds, vmax, t)
            self.add_reference(self.steerings, 0.0, t)

        # Pause the car to turn the wheel
        self.add_reference(self.speeds, 0.0, DURATION_TO_TURN_WHEELS)
        self.add_reference(self.steerings, self.max_steering_angle, DURATION_TO_TURN_WHEELS)

        # 1st turn: start position -> P
        t = (self.min_central_angle * self.Rwmin) / vmax
        self.add_reference(self.speeds, -vmax, t)
        self.add_reference(self.steerings, self.max_steering_angle, t)

        # Pause the car to turn the wheel
        self.add_reference(self.speeds, 0.0, DURATION_TO_TURN_WHEELS)
        self.add_reference(self.steerings, -self.max_steering_angle, DURATION_TO_TURN_WHEELS)

        # 2nd turn: P -> final
        self.add_reference(self.speeds, -vmax, t)
        self.add_reference(self.steerings, -self.max_steering_angle, t)

        # Pause the car to turn the wheel
        self.add_reference(self.speeds, 0.0, DURATION_TO_TURN_WHEELS)
        self.add_reference(self.steerings, 0.0, DURATION_TO_TURN_WHEELS)

        # Centering in the parking spot
        t = math.fabs((PARKING_BATAILLE_LENGTH - self.length) / 2.0) / vmax
        self.add_reference(self.speeds, vmax, t)
        self.add_reference(self.steerings, 0.0, t)

        # Init reference to idle the car
        self.add_reference(self.speeds, 0.0, 0.0)
        self.add_reference(self.steerings, 0.0, 0.0)

        # Summary all speed references
        print("Summary speed references")
        N = len(self.speeds)
        for i in range (0,N):
           (s,t) = self.speeds[i]
           print("time", t, "speed", s)

        """
        ####################################
        ### Dynamic refrences: Acceleration
        ####################################
        # Kinematics equations:
        #   v = a t
        #   x = a t^2 / 2
        t1 = vmax / ades
        d1 = 0.5 * ades * t1 * t1
        d2 = self.min_central_angle * self.Rwmin
        t2 = (d2 - 2.0 * d1) / vmax
        di = math.fabs(self.Xi - self.Xs)
        ti = (di - 2.0 * d1) / vmax
        d3 = math.fabs((PARKING_BATAILLE_LENGTH - self.length) / 2.0)
        t3 = d3 / vmax
        AW = self.max_steering_angle / DURATION_TO_TURN_WHEELS

        self.add_reference(self.accelerations, 0.0, 1.0)
        self.add_reference(self.steerings, 0.0, 1.0)

        # Init car position -> position when starting the 1st turn
        acc = -ades if self.Xi >= self.Xs else ades
        self.add_reference(self.accelerations, acc, t1)
        self.add_reference(self.steerings, 0.0, t1)
        self.add_reference(self.accelerations, 0.0, ti)
        self.add_reference(self.steerings, 0.0, ti)
        self.add_reference(self.accelerations, -acc, t1)
        self.add_reference(self.steerings, 0.0, t1)

        # Turn wheel
        self.add_reference(self.accelerations, 0.0, DURATION_TO_TURN_WHEELS)
        self.add_reference(self.steerings, -AW, DURATION_TO_TURN_WHEELS)

        # Turn 1
        self.add_reference(self.accelerations, -ades, t1)
        self.add_reference(self.steerings, 0.0, t1)
        self.add_reference(self.accelerations, 0.0, t2)
        self.add_reference(self.steerings, 0.0, t2)
        self.add_reference(self.accelerations, ades, t1)
        self.add_reference(self.steerings, 0.0, t1)

        # Turn wheel
        self.add_reference(self.accelerations, 0.0, DURATION_TO_TURN_WHEELS)
        self.add_reference(self.accelerations, 0.0, DURATION_TO_TURN_WHEELS)
        self.add_reference(self.steerings, AW, DURATION_TO_TURN_WHEELS)
        self.add_reference(self.steerings, AW, DURATION_TO_TURN_WHEELS)

        # Turn 2
        self.add_reference(self.accelerations, -ades, t1)
        self.add_reference(self.steerings, 0.0, t1)
        self.add_reference(self.accelerations, 0.0, t2)
        self.add_reference(self.steerings, 0.0, t2)
        self.add_reference(self.accelerations, ades, t1)
        self.add_reference(self.steerings, 0.0, t1)

        # Turn wheel
        self.add_reference(self.accelerations, 0.0, DURATION_TO_TURN_WHEELS)
        self.add_reference(self.steerings, -AW, DURATION_TO_TURN_WHEELS)

        # Centering in the parking spot
        self.add_reference(self.accelerations, ades, t3)
        self.add_reference(self.steerings, 0.0, t3)
        self.add_reference(self.accelerations, -ades, t3)
        self.add_reference(self.steerings, 0.0, t3)

        # Final
        self.add_reference(self.accelerations, 0.0, DURATION_TO_TURN_WHEELS)
        self.add_reference(self.steerings, 0.0, DURATION_TO_TURN_WHEELS)

        print("accelerations=", self.accelerations)
        print("steerings=", self.steerings)
        """

    # --------------------------------------------------------------------------
    # Compute the one-trail parallel maneuver. To be computed once!
    # FIXME gerer wheelbase quand on fait car.location
    # --------------------------------------------------------------------------
    def trajectory(self, front_spot, back_spot):
        print("Dist:", distance(front_spot, back_spot))
        if distance(front_spot, back_spot) < self.Lmin:
            print("No enough space to park the car in a 1-trial parallel maneuver.")
            print("TODO: See C++ code for N-trial parallel maneuver")
            return False

        # Carla's world coordinate (x_carla, y_carla) => Parking's world
        # coordinates (x_parking, y_parking):
        #   x_parking = y_carla
        #   y_parking = x_carla
        (xi, yi) = self.position().y, self.position().x
        (xf, yf) = back_spot.y, back_spot.x
        if self.compute_path_planning(xi, yi, xf, yf) == False:
            return False

        self.generate_trajectory_references(VMAX, ADES)
        return True

    # --------------------------------------------------------------------------
    # Update the cruise control
    # --------------------------------------------------------------------------
    def update(self, dt):
        # References
        reference_speed = self.read_reference(self.speeds, self.time)
        reference_steer = self.read_reference(self.steerings, self.time)

        # Cruise control and normalized values -1 .. +1
        obs_speed = self.observe_speed()
        controled_acceleration = self.pid.update(dt, reference_speed, obs_speed)
        f.write(f';\n{self.time} {reference_speed} {obs_speed} {self.pid.error} {controled_acceleration} ')
        self.control(constrain_normalization(controled_acceleration, CARLA_MAX_ACC),
                     constrain_normalization(reference_steer, CARLA_MAX_STEER),
                     reference_speed)
        self.time += dt

    # --------------------------------------------------------------------------
    # Convert the PID output into throttle/brake and steering angle commands
    # Inputs are already normalized (-1 .. +1)
    # --------------------------------------------------------------------------
    def control(self, acceleration, angle, reference_speed):
        # Reverse gear ?
        if reference_speed < 0.0:
            self.reverse = -1
            throttle = (-acceleration if acceleration <= 0.0 else 0.0)
            brake = (acceleration if acceleration > 0.0 else 0.0)
        else:
            self.reverse = 1
            throttle = (acceleration if acceleration > 0.0 else 0.0)
            brake = (-acceleration if acceleration <= 0.0 else 0.0)

        # Saturate Carla steering angle = +/-70 deg => command = +/-1 but reality is around 30 deg
        steering_ratio = 1.0 # self.max_steering_angle / CARLA_MAX_STEER

        self.vehicle.apply_control(carla.VehicleControl(
            throttle = constrain(throttle, 0.0, 1.0),
            brake = constrain(brake, 0.0, 1.0),
            steer = constrain(angle, -steering_ratio, steering_ratio),
            reverse = (self.reverse == -1)
        ))
        f.write(f'{throttle} {brake} {self.reverse}')

    # --------------------------------------------------------------------------
    # Return the position of the middle of the rear axle inside the world's
    # coordinates
    # --------------------------------------------------------------------------
    def position(self):
        return rear_position(self.vehicle)

    # --------------------------------------------------------------------------
    # Return the car longitudinal speed [m/s].
    # --------------------------------------------------------------------------
    def observe_speed(self):
        v = self.vehicle.get_velocity()
        s = math.sqrt(v.x**2 + v.y**2 + v.z**2)
        return s * self.reverse

    # --------------------------------------------------------------------------
    # Convert [m/s] to [km/h]
    # --------------------------------------------------------------------------
    def kmh(self):
        return int(3.6 * self.observe_speed())

    # --------------------------------------------------------------------------
    # Return the car longitudinal acceleration [m/s/s]
    # --------------------------------------------------------------------------
    def observe_acceleration(self):
        a = self.vehicle.get_acceleration()
        s = math.sqrt(a.x**2 + a.y**2 + a.z**2)
        return s

    # --------------------------------------------------------------------------
    # Display information on the screen.
    # --------------------------------------------------------------------------
    def debug(self, display):
        text(f'Ref #{self.index}', 100, 80, display)
        a = self.observe_acceleration()
        text(f'Acc {a:.2f}', 100, 100, display)
        s = self.observe_speed()
        text(f'Speed {s:.2f}', 100, 120, display)
        r = (self.reverse == -1)
        text(f'Reverse {r}', 100, 140, display)

################################################################################
### Simulation: init world for doing an automatic parallel parking
################################################################################
def main():
    actor_list = []
    parked_cars = []

    try:
        # Window
        pygame.init()
        pygame.font.init()
        display = pygame.display.set_mode((IM_WIDTH * 2, IM_HEIGHT), pygame.HWSURFACE | pygame.DOUBLEBUF)
        pygame.display.flip()
        myfonts[0] = pygame.font.SysFont('Comic Sans MS', 30)

        # Carla
        client = carla.Client('localhost', 2000)
        client.set_timeout(2.0)
        world = client.load_world("Town03")

        # Ego car
        player = CitroenC3(world)
        location = player.position()
        actor_list.append(player.vehicle)
        print("Ego:", location)

        # Placed parked cars after the player's car
        offset_y = -2.0
        blueprint_library = world.get_blueprint_library()
        car_bp = blueprint_library.filter('vehicle.*')[0]
        #for i in (0,1,2):
        i = 0
        offset = carla.Location(x=-2.5, y=(i * PARKING_BATAILLE_LENGTH) + offset_y, z=0.0)
        if car_bp.has_attribute('color'):
            color = random.choice(car_bp.get_attribute('color').recommended_values)
            car_bp.set_attribute('color', color)
            pnj = world.spawn_actor(car_bp, carla.Transform(location + offset, carla.Rotation(pitch=0, yaw=90, roll=0)))
            actor_list.append(pnj)
            parked_cars.append(pnj)

        # Placed parked cars before the player's car
        #for i in (2,3):
        i = 2
        offset = carla.Location(x=-2.5, y=(i * -PARKING_BATAILLE_LENGTH) + offset_y, z=0.0)
        if car_bp.has_attribute('color'):
            color = random.choice(car_bp.get_attribute('color').recommended_values)
            car_bp.set_attribute('color', color)
            pnj = world.spawn_actor(car_bp, carla.Transform(location + offset, carla.Rotation(pitch=0, yaw=90, roll=0)))
            actor_list.append(pnj)
            parked_cars.append(pnj)

        # Ego car's camera
        camera_bp = world.get_blueprint_library().find('sensor.camera.rgb')
        camera_bp.set_attribute('image_size_x', f'{IM_WIDTH}')
        camera_bp.set_attribute('image_size_y', f'{IM_HEIGHT}')
        camera_bp.set_attribute('fov', f'{IM_FOV}')

        # Main camera view (bird's eye view)
        relative_location = carla.Transform(carla.Location(x=-PARKING_BATAILLE_LENGTH, y=0.0, z=15.0), carla.Rotation(pitch=-90, yaw=0, roll=0))
        main_camera = world.spawn_actor(camera_bp, relative_location, attach_to=player.vehicle)
        main_camera.listen(lambda data: camera_callback(0, data))
        actor_list.append(main_camera)

        # Debug camera view (lateral view)
        relative_location = carla.Transform(carla.Location(x=0.0, y=-5.0, z=1.0), carla.Rotation(pitch=0, yaw=90, roll=0))
        debug_camera = world.spawn_actor(camera_bp, relative_location, attach_to=player.vehicle)
        debug_camera.listen(lambda data: camera_callback(1, data))
        actor_list.append(debug_camera)

        # Compute trajectory to parking
        # See C++ code for the state machine for detecting an empty spot (to be moved inside the update())
        print("Parked car 0:", rear_position(parked_cars[0]))
        print("Parked car 1:", rear_position(parked_cars[1]))
        print("Ego:", player.position())
        # Vehicle location => Parking location
        front_spot = rear_position(parked_cars[0]) - carla.Vector3D(0, 0.66, 0) # Citroen C3 overhang
        back_spot = rear_position(parked_cars[1]) - carla.Vector3D(0, 0.66 - 3.941, 0) # Citroen C3 overhang - length
        if player.trajectory(front_spot, back_spot) == False:
            print("Auto parking aborted")

        # Runtime loop
        call_exit = False
        previous_time = pygame.time.get_ticks()
        while True:
            # Delta time
            world.wait_for_tick()
            t = pygame.time.get_ticks()
            dt = (t - previous_time) / 1000.0
            previous_time = t

            # Drive to parking spot
            # See C++ code for the state machine for detecting an empty spot
            player.update(dt)

            # WIP
            """
            camera_transform = main_camera.get_transform()
            k = main_camera.get_intrinsic(IM_WIDTH, IM_HEIGHT, IM_FOV)
            #vehicle_transform = parked_cars[0].get_transform()
            #bounding_box = parked_cars[0].bounding_box.get_world_vertices(vehicle_transform)
            #project_to_image(bounding_box[6], camera_transform, k)
            project_to_image(rear_position(parked_cars[0]), camera_transform, k)
            """

            # Debug parking dimension
            """
            world.debug.draw_string(rear_position(parked_cars[0]) - carla.Vector3D(0, 0.66, 0), 'O', draw_shadow=False,
                                    color=carla.Color(r=0, g=255, b=0), life_time=1000.0,
                                    persistent_lines=True)
            world.debug.draw_string(rear_position(parked_cars[1]) - carla.Vector3D(0, 0.66 - 3.941, 0), 'X', draw_shadow=False,
                                    color=carla.Color(r=0, g=255, b=0), life_time=1000.0,
                                    persistent_lines=True)
            """

            # Game events
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    call_exit = True
                elif event.type == pygame.KEYDOWN:
                    if event.key == K_ESCAPE or event.key == K_q:
                        call_exit = True
                        break

            # Swap buffers
            if surfaces[0] is not None:
                display.blit(surfaces[0], (0, 0))
            if surfaces[1] is not None:
                display.blit(surfaces[1], (IM_WIDTH, 0))

            # HUD Debug
            player.debug(display)

            pygame.display.flip()
            if call_exit:
                break

    finally:

        # Finalize the Julia script
        print('destroying actors')
        for actor in actor_list:
            actor.destroy()
        f.write(']\n\nt=D[:,1]; rS=D[:,2]; oS=D[:,3]; pe=D[:,4]; ')
        f.write('po=D[:,5]; th=D[:,6]; b=D[:,7]; r=D[:,8]\n')
        f.write('p=plot(t, rS, label="Ref speed", linetype=:steppre)\n')
        f.write('plot!(t, oS, label="Obs speed", linetype=:steppre)\n')
        f.write('#plot!(t, pe, label="PID err", linetype=:steppre)\n')
        f.write('plot!(t, po, label="PID out (acc)", linetype=:steppre)\n')
        f.write('plot!(t, th, label="throttle", linetype=:steppre)\n')
        f.write('#plot!(t, b, label="brake", linetype=:steppre)\n')
        f.write('#plot!(t, r, label="reverse", linetype=:steppre)\n')
        f.write('savefig(p, "plot.pdf")\n')
        f.close()
        print('done.')

if __name__ == '__main__':
    main()
