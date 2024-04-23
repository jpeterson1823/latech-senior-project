import numpy as np
import librosa
import data_handling as dh
import os

directory = "voice_clips/"
files = os.listdir(directory)
for i in range(0, len(files)):
    audio_file = directory + files[i]
    processed = dh.extract_mfcc(audio_file, 100)
    numpy_file_name = audio_file[:-4] + ".npy"
    np.save(numpy_file_name, processed)
