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
import numpy as np
import time

start = time.time()
my_model = torch.load("Petersons_model")
my_model.eval()

with torch.no_grad():
    test_file = np.load("perterson_untitled5.npy")
    user_dir = "voice_clips/"
    audio_files = os.listdir(user_dir)
    numpy_files = []
    for i in range(len(audio_files)):
        if audio_files[i][-4:] == ".npy":
            numpy_files.append(audio_files[i])
    
    comparison_tensor = torch.tensor(test_file)
    outputs = []
    for i in range(len(numpy_files)):
        path = user_dir + numpy_files[i]
        user_tensor = torch.tensor(np.load(path))
        output = my_model.forward(user_tensor, comparison_tensor)
        outputs.append(round(output.item(), 3))
    end = time.time()
    total = end - start
    print(outputs)
    print("Total time for comparison: "+ str(round(total, 2)) + "s")