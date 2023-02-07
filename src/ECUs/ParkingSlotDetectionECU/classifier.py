# DCNN for image classification for parking slots

import tensorflow as tf
from tensorflow import keras

import numpy as np
import glob, sys, os, io
import skimage.io

# Input image dimensions: 192x48 grey color
width = 192
height = 48
canal = 1

# Compute the scaled norm of the segment line P1P2
def norm(P1, P2, scale):
    length = np.sqrt((P1[0] - P2[0])**2 + (P1[1] - P2[1])**2)
    dx = P2[0] - P1[0]
    dy = P2[1] - P1[1]
    N = [-dy * scale / length, dx * scale / length]
    return N

# Crop the image of a parking slot
def crop(folder_src, file_src, folder_dst, file_dst):
    print("Cropping", file_src)

    # The image to crop
    img = cv2.imread(folder_src + 'pics/' + file_src + '.jpg', 0) # 0: to force grey picture
    # file defining segment lines (aka slots)
    mat = scipy.io.loadmat(folder_src + 'mats/' + file_src + '.mat')
    # 4 fields by data
    count_slots = int(np.size(mat['slots']) / 4)

    # Crop the oriented bounding box
    for i in range(count_slots):
        # Slots = segment lines
        slot = mat['slots'][i]
        # Marks = vertices
        P1 = mat['marks'][slot[0]-1]
        P2 = mat['marks'][slot[1]-1]
        # Extra information of the parking slot: type and angle of lanes
        type = slot[2]
        # angle = slot[3] # not used for this example

        # Create the oriented rectangle as bounding box of the segment line
        margin = 24 # pixels
        N = norm(P1, P2, margin) # normal of P1P2
        M = norm(P1, P1 + N, margin) # normal of P1P3

        # Oriented rectangle bounding the segment line
        rect = [P1 - N + M, P1 + N + M, P2 + N - M, P2 - N - M]

        # Rotate the oriented rectangle and save into a new image
        src_pts = np.array(rect, dtype="float32")
        dst_pts = np.array([[0, height-1], [0, 0], [width-1, 0], [width-1, height-1]], dtype="float32")
        M = cv2.getPerspectiveTransform(src_pts, dst_pts)
        warped = cv2.warpPerspective(img, M, (width, height))

        # Store the parking slot type in the file name.
        # Note: angle is not used for the moment
        cv2.imwrite(folder_dst + file_dst + '_' + str(type) + '.jpg', warped)

# Crop all parking slot images from a folder and save cropped images in the destination folder
# picture files shall be in the folder pics/ in the 'folder_src'
# matlab files shall be in the folder pics/ in the 'folder_src'
def crop_all(folder_src, folder_dst):
    os.chdir(folder_src + 'pics/')
    i = 0
    for file in glob.glob("*.jpg"):
        i = i + 1
        crop(folder_src, os.path.splitext(file)[0], folder_dst, str(i).zfill(4))

# Prepare the dataset and normalize pixels
def dataset(path, msg):
    x = []
    y = []
    cwd = os.getcwd()
    os.chdir(path)
    for file in glob.glob("*.jpg"): # Grey pictures
        print("Loading", msg, file)
        image = skimage.io.imread(f'{file}') # imread(f'{path}/{file}')
        x.append(image)
        y.append(file.split(".")[0].split("_")[1]) # Extract the type of the parking slot
        xx = np.array(x, dtype=np.float).reshape(-1, height, width, canal)
    os.chdir(cwd)
    return xx / xx.max(), np.array(y, dtype=np.float) # Return normalized pixels [0 .. +1]

# Uncomment and run it once to generate inputs pictures and labels from the official DeepPS.zip database
#crop_all('DeepPS/training', 'dataset_classifier2/training')
#crop_all('DeepPS/testing/all', 'dataset_classifier2/testing')

# Prepare the dataset for the DCNN
x_train, y_train = dataset('dataset_classifier/training/', 'training')
x_test, y_test = dataset('dataset_classifier/testing/', 'testing')

# Deep convolutional network (DCNN): classification
model = keras.models.Sequential()
model.add(keras.layers.Input((height,width,canal)))
model.add(keras.layers.Conv2D(40, (3,9), strides=(1,3), activation='relu'))
model.add(keras.layers.MaxPooling2D(pool_size=(2,2), strides=(2,2)))
model.add(keras.layers.Conv2D(112, (3,5), padding="valid", activation='relu'))
model.add(keras.layers.MaxPooling2D(pool_size=(3,3), padding="valid"))
model.add(keras.layers.Conv2D(160, (3,3), padding="valid", activation='relu'))
model.add(keras.layers.Conv2D(248, (3,3), padding="valid", activation='relu'))
model.add(keras.layers.MaxPooling2D(pool_size=(3,3), padding="valid"))
model.add( keras.layers.Flatten())
model.add(keras.layers.Dense(1024, activation='relu'))
model.add(keras.layers.Dropout(0.5))
model.add(keras.layers.Dense(7, activation='softmax'))
model.summary()
model.compile(optimizer = 'adam', loss = 'sparse_categorical_crossentropy', metrics = ['accuracy'])

# Training
batch_size = 5
epochs = 20
history = model.fit(x_train, y_train,
                    batch_size = batch_size, epochs = epochs, verbose = 1,
                    validation_data = (x_test, y_test))

# Accuracy
max_val_accuracy = max(history.history["val_accuracy"])
print("Max validation accuracy is : {:.4f}".format(max_val_accuracy))
score = model.evaluate(x_test, y_test, verbose=0)
print('Test loss      : {:5.4f}'.format(score[0]))
print('Test accuracy  : {:5.4f}'.format(score[1]))

# Check some prediction (ideally a folder with pictures not used for validation should be used)
y_sigmoid = model.predict(x_test)
y_pred    = np.argmax(y_sigmoid, axis=-1)
np.savetxt("predictions.csv", np.transpose([y_test, y_pred]), delimiter=",", header="labels,predictions")
