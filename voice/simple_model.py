import torch
import torch.nn as nn
import torch.nn.functional as F
import os


class SimpleModel(torch.nn.Module):
    def __init__(self):
        super(SimpleModel, self).__init__()
        # First convolutional neural network
        self.conv1 = nn.Conv2d(1, 256, kernel_size=3, stride=1, padding=1)
        self.conv2 = nn.Conv2d(256, 512, kernel_size=3, stride=1, padding=1)
        self.pool = nn.MaxPool2d(kernel_size=2, stride=2, padding=0)
        
        # Second convolutional neural network
        self.conv3 = nn.Conv2d(512, 1024, kernel_size=3, stride=1, padding=1)
        
        # Fully connected layers
        self.fc1 = nn.Linear(12288, 1024)
        self.fc2 = nn.Linear(1024, 2)  # Output layer with 2 neurons for binary classification
        self.dropout1 = nn.Dropout(p=0.5)
        
    def forward(self, speaker_data, comparison_data):
        # Process speaker data
        x1 = F.relu(self.conv1(speaker_data))
        x1 = self.pool(x1)
        x1 = F.relu(self.conv2(x1))
        x1 = self.pool(x1)
        x1 = F.relu(self.conv3(x1))
        x1 = self.pool(x1)
        x1 = x1.view(x1.size(0), -1)  # Flatten
        x1 = F.relu(self.fc1(x1))
        x1 = self.dropout1(x1)
        
        # Process comparison data
        x2 = F.relu(self.conv1(comparison_data))
        x2 = self.pool(x2)
        x2 = F.relu(self.conv2(x2))
        x2 = self.pool(x2)
        x2 = F.relu(self.conv3(x2))
        x2 = self.pool(x2)
        x2 = x2.view(x2.size(0), -1)  # Flatten
        x2 = F.relu(self.fc1(x2))
        x2 = self.dropout1(x2)
        
        # Concatenate the outputs of x1 and x2 along batch dimension
        x = torch.cat((x1, x2), dim=0)

        # Pass through the final fully connected layer
        x = self.fc2(x)
        
        # Apply softmax activation to get probabilities
        
        return x[0]
        

