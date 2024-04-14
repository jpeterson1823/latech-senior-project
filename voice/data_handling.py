import os
import numpy as np
import random
import librosa
import torch

class CustomDataset(torch.utils.data.Dataset):
    def __init__(self, speaker_data, comparison_data, labels):
        self.speaker_data = speaker_data
        self.comparison_data = comparison_data
        self.labels = labels

    def __len__(self):
        return len(self.speaker_data)
    
    def __getitem__(self, idx):
        speaker_sample = self.speaker_data[idx]
        comparison_sample = self.comparison_data[idx]
        label = self.labels[idx]
        return speaker_sample, comparison_sample, label



def organize_sets(parent_folder):
    audio_file_folders = os.listdir(parent_folder)
    audio_file_folders.sort()
    audio_files = []
    for folder in audio_file_folders:
        file_path = parent_folder
        file_path += folder
        files = os.listdir(file_path)
        full_file_path = []
        for file in files:
            path = file_path + "/" + file
            full_file_path.append(path)

        audio_files.append(full_file_path)
    
    return audio_files


def create_test_group(speaker: list, list_of_speakers, speaker_index, max_length) -> list:

    temp_speaker = speaker
    file_index = random.randint(0, len(temp_speaker)-1)
    file = extract_mfcc(speaker[file_index], max_length)
    temp_speaker.pop(file_index)

    matching_speaker = random.choice([True, False])

    if (matching_speaker):
        file_to_match = extract_mfcc(speaker[random.randint(0, len(temp_speaker)-1)], max_length)

    else:
        while(True):
            index = random.randint(0, 9)
            if (index != speaker_index):
                file_to_match = extract_mfcc(list_of_speakers[index][random.randint(0, len(list_of_speakers[index])-1)], max_length)
                break
    group = (file, file_to_match, int(matching_speaker))

    return group


def extract_mfcc(audio_file_path, max_length, num_mfcc=13):
    audio_data, sample_rate = librosa.load(audio_file_path, sr=None)
    mfccs = librosa.feature.mfcc(y=audio_data, sr=sample_rate, n_mfcc=num_mfcc)


    if (mfccs.shape[1] < max_length):
        mfccs = np.pad(mfccs, ((0, 0), (0, max_length - mfccs.shape[1])), mode='constant')
    elif (mfccs.shape[1] > max_length):
        mfccs = mfccs[:, :max_length]

    # Define the desired min-max range (e.g., 0 to 1)
    min_value = 0
    max_value = 1

    # Calculate min and max values along the feature axis
    min_val = np.min(mfccs, axis=0)
    max_val = np.max(mfccs, axis=0)
    # Handle zero division and NaN/Infinity values
    max_val[max_val == min_val] = min_val[max_val == min_val] + 1.0  # Ensure max and min values are different
    min_val[np.isnan(min_val)] = 0.0  # Replace NaN values with 0
    min_val[np.isinf(min_val)] = 0.0  # Replace Infinity values with 0

    # Perform min-max scaling
    mfcc_data_scaled = min_value + (max_value - min_value) * (mfccs - min_val) / (max_val - min_val)
    """
    mean = np.mean(mfccs, axis=0)
    std = np.std(mfccs, axis=0)
    std[std == 0] = 1.0  # Set standard deviation to 1 if it's zero
    std[np.isnan(std)] = 1.0  # Replace NaN values with 1
    std[np.isinf(std)] = 1.0  # Replace Infinity values with 1


    # Perform standardization
    mfcc_data_standardized = (mfccs - mean) / std
    """

    return mfcc_data_scaled



