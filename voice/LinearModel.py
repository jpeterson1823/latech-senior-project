import torch
import torch.nn as nn
import torch.nn.functional as F
import os

class LinearModel(nn.Module):
    def __init__(self):
        super(LinearModel, self).__init__()
        self.fc1 = nn.Linear(193, 128)
        self.dropout1 = nn.Dropout(0.1)
        self.fc2 = nn.Linear(256, 256)
        self.dropout2 = nn.Dropout(0.25)
        self.fc3 = nn.Linear(256, 512)
        self.dropout3 = nn.Dropout(0.5)
        self.fc4 = nn.Linear(512, 1)
        self.sigmoid = nn.Sigmoid()

    def forward(self, speaker_data, comparison_data):
        # Process speaker_data
        x1 = F.relu(self.fc1(speaker_data))
        # Process comparison_data
        x2 = F.relu(self.fc1(comparison_data))
        
        x = torch.cat((x1, x2), dim=0)

        x = F.relu(self.fc2(x))
        x = self.dropout2(x)
        x = F.relu(self.fc3(x))
        
        x = self.fc4(x)
        x = self.sigmoid(x)
        return x


