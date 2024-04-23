import data_handling as dh
import LinearModel as model
from torch.utils.tensorboard import SummaryWriter
import torch
import torch.nn as nn
from datetime import datetime
import sys
import os
import torch.nn.functional as F
import time

my_model = torch.load("Petersons_model")
my_model.eval()

with torch.no_grad():
    test_path = "untitled5.wav"
    user_dir = "voice_clips/"
    audio_files = os.listdir(user_dir)
    comparison_tensor = torch.tensor(dh.extract_mfcc(test_path, 100))
    outputs = []
    for i in range(len(audio_files)):
        audio_file_path = user_dir + audio_files[i]
        user_tensor = torch.tensor(dh.extract_mfcc(audio_file_path, 100))
        output = my_model.forward(user_tensor, comparison_tensor)
        outputs.append(output.item())

    print(outputs)
