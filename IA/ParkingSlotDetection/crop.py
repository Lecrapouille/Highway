# Crop and rotate an image of parking slot

import scipy.io
import numpy as np
import cv2
import glob, os
from shutil import copyfile

def norm(P1, P2, scale):
  length = np.sqrt((P1[0] - P2[0])**2 + (P1[1] - P2[1])**2)
  dx = P2[0] - P1[0]
  dy = P2[1] - P1[1]
  N = [-dy * scale / length, dx * scale / length]
  return N

def crop(folder_src, file_src, folder_dst, file_dst):
  print("Cropping", file_src)

  debug = cv2.imread(folder_src + 'pics/' + file_src + '.jpg')
  img = cv2.imread(folder_src + 'pics/' + file_src + '.jpg', 0)
  mat = scipy.io.loadmat(folder_src + 'mats/' + file_src + '.mat')
  count_slots = int(np.size(mat['slots']) / 4)

  for i in range(count_slots):
    slot = mat['slots'][i]
    P1 = mat['marks'][slot[0]-1]
    P2 = mat['marks'][slot[1]-1]
    type = slot[2]
    angle = slot[3]

    margin = 24
    N = norm(P1, P2, margin)
    M = norm(P1, P1 + N, margin)
    rect = [P1 - N + M, P1 + N + M, P2 + N - M, P2 - N - M]

    # Debug
    pts = np.array(rect, np.int32).reshape((-1, 1, 2))
    debug = cv2.polylines(debug, [pts], True, (255, 0, 0), 2)

    # Crop and reshape the slot
    width, height = 192, 48
    src_pts = np.array(rect, dtype="float32")
    dst_pts = np.array([[0, height-1], [0, 0], [width-1, 0], [width-1, height-1]], dtype="float32")
    M = cv2.getPerspectiveTransform(src_pts, dst_pts)
    warped = cv2.warpPerspective(img, M, (width, height))
    cv2.imwrite(folder_dst + file_dst + '_' + str(type) + '.jpg', warped)

def crop_all(folder_src, folder_dst):
  os.chdir(folder_src + 'pics/')
  i = 0
  for file in glob.glob("*.jpg"):
    i = i + 1
    crop(folder_src, os.path.splitext(file)[0], folder_dst, str(i).zfill(4))


#crop_all('DeepPS/training', 'dataset_classifier2/training')
#crop_all('DeepPS/testing/all', 'dataset_classifier2/testing')
