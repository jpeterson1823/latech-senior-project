import torch
import torch.nn as nn
import torch.nn.functional as F
import os

class LinearModel(nn.Module):
    def __init__(self):
        super(LinearModel, self).__init__()
        self.fc1 = nn.Linear(193, 128)
        self.dropout1 = nn.Dropout(0.1)
        self.fc2 = nn.Linear(128, 128)
        self.dropout2 = nn.Dropout(0.25)
        self.fc3 = nn.Linear(128, 256)
        self.dropout3 = nn.Dropout(0.5)
        self.fc4 = nn.Linear(512, 2)

    def forward(self, speaker_data, comparison_data):
        # Process speaker_data
        x1 = self.fc1(speaker_data)
        x1 = F.relu(x1)
        x1 = self.dropout1(x1)
        x1 = F.relu(self.fc2(x1))
        x1 = self.dropout2(x1)
        x1 = F.relu(self.fc3(x1))
        x1 = self.dropout3(x1)
        

        # Process comparison_data
        x2 = F.relu(self.fc1(comparison_data))
        x2 = self.dropout1(x2)
        x2 = F.relu(self.fc2(x2))
        x2 = self.dropout2(x2)
        x2 = F.relu(self.fc3(x2))
        x2 = self.dropout3(x2)
        
        x = torch.cat((x1, x2), dim=0)

        x = self.fc4(x)
        x = F.softmax(x, dim=0)

        return x


