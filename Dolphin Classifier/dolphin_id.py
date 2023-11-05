
# Might make your life easier for appending to lists
from collections import defaultdict

# Third party libraries
import numpy as np
from sklearn.model_selection import train_test_split
import tensorflow as tf
from tensorflow.keras import layers
from sklearn.metrics import confusion_matrix
# Only needed if you plot your confusion matrix
import matplotlib.pyplot as plt

# our libraries
from lib.partition import split_by_day
import lib.file_utilities as util
from lib.buildmodels import build_model



def dolphin_classifier(data_directory):
    """
    Neural net classification of dolphin echolocation clicks to species
    :param data_directory:  root directory of data
    :return:  None
    """

    plt.ion()  # enable interactive plotting

    use_onlyN = 20  # debug, only read this many files for each species
    # Read and parse files, then partition based on day
    file_names = util.get_files(data_directory + "/Gg", stop_after=use_onlyN)
    files = util.parse_files(file_names)
    split_Gg = split_by_day(files)
    # Read and parse Gg and Lo separately
    file_names = util.get_files(data_directory + "/Lo", stop_after=use_onlyN)
    files = util.parse_files(file_names)
    split_Lo = split_by_day(files)
    # Combine Gg and Lo then get list of all partitioned days
    split = split_Lo | split_Gg
    days = list(split.keys())

    #for i in split_Gg:
        #print(split_Gg[i][0])

    # Split days between training and testing
    train_day, test_day = (train_test_split(days))
    X_train = []
    Y_train = []
    # Loop through each training day, and each recording, and each click
    for i in train_day:
        for j in range(len(split[i])):
            for k in range(len(split[i][j][3])):
                # Add features to X_train
                X_train.append(split[i][j][3][k])
                # Add 0 or 1 label to Y_train
                if split[i][j][1] == 'Gg':
                    Y_train.append(0)
                else:
                    Y_train.append(1)
    # Convert X and Y train to tensors
    X_train = tf.convert_to_tensor(X_train)
    Y_train = tf.convert_to_tensor(Y_train)

    X_test = []
    Y_test = []
    # Repeat for test data
    for i in test_day:
        for j in range(len(split[i])):
            for k in range(len(split[i][j][3])):
                X_test.append(split[i][j][3][k])
                if split[i][j][1] == 'Gg':
                    Y_test.append(0)
                else:
                    Y_test.append(1)
    X_test = tf.convert_to_tensor(X_test)

    # Create L2 regularizer and learning model
    regularizer = tf.keras.regularizers.L2(0.01)
    model = tf.keras.models.Sequential([
        layers.Dense(100, activation='relu', kernel_regularizer=regularizer, input_dim=20),
        layers.Dense(100, activation='relu', input_dim=100),
        layers.Dense(100, activation='relu', input_dim=100),
        layers.Dense(2, activation='softmax', input_dim=100)
    ])
    model.compile(optimizer='Adam', loss='categorical_crossentropy', metrics=['accuracy'])
    # Create one one hot vectors for labels and train model
    onehotlabels = tf.keras.utils.to_categorical(Y_train, num_classes=2)
    model.fit(X_train, onehotlabels, batch_size=100, epochs=10)
    # Predict on test data
    results = model.predict(X_test)
    results = np.argmax(results, axis=1)

    # Create confusion matrix
    confusion = [[0,0],[0,0]]
    for i in range(len(Y_test)):
        confusion[Y_test[i]][results[i]] += 1
    fig, ax = plt.subplots()
    ax.matshow(confusion, cmap=plt.cm.Blues, alpha=0.3)
    for i in range(2):
        for j in range(2):
            ax.text(x=j, y=i, s=confusion[i][j], va='center',ha='center', size='xx-large')
    plt.xlabel('Predicted')
    plt.ylabel('Actual')
    plt.title('Confusion Matrix')
    plt.figtext(x=0.01, y=0.05, s="O=Gg   1=Lo")
    plt.show()







def split_by_site(recordings):
    """
        Given a list of RecordingInfoTypes, split them into a list of lists where
        each list represents one day of recording.
        :param recordings:
        :return:
        """

    bysite = defaultdict(list)
    for r in recordings:
        # For each recording, append to a list keyed on the start day
        site = r[0]
        bysite[site].append(r)

    return bysite
if __name__ == "__main__":
    data_directory = "features"  # root directory of data
    dolphin_classifier(data_directory)
