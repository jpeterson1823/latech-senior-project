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


def extract_mfcc(audio_file_path, max_length, num_mfcc=40):
    X, sample_rate = librosa.load(audio_file_path, res_type='kaiser_fast')

    mfccs = np.mean(librosa.feature.mfcc(y=X, sr=sample_rate, n_mfcc=num_mfcc).T, axis=0).astype('float32')
    stft = np.abs(librosa.stft(X, n_fft=512)).astype('float32')
    chroma = np.mean(librosa.feature.chroma_stft(S=stft, sr=sample_rate, n_fft=512).T, axis=0).astype('float32')
    mel = np.mean(librosa.feature.melspectrogram(y=X, sr=sample_rate).T, axis=0).astype('float32')
    contrast = np.mean(librosa.feature.spectral_contrast(S=stft, sr=sample_rate, n_fft=512).T, axis=0).astype('float32')
    tonnetz = np.mean(librosa.feature.tonnetz(y=librosa.effects.harmonic(X), sr=sample_rate).T, axis=0).astype('float32')

    processed_data = np.concatenate((mfccs, chroma, mel, contrast, tonnetz), axis=0).astype('float32')
    for i in range(len(processed_data)):
        processed_data[i] = processed_data[i].astype('float32')

    return processed_data
