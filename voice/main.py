import data_handling as dh
import simple_model as model
from torch.utils.tensorboard import SummaryWriter
import torch
import torch.nn as nn
from datetime import datetime
import sys
import os
import torch.nn.functional as F



MAX_LENGTH = 100


if __name__ == "__main__":

    try:
        action = sys.argv[1]
    except IndexError:
        print("Usage: python3 main1.py <train_or_validate>")
        exit()

    if (action == "train"):
        folder_path = "training/"
    elif (action == "validate"):
        folder_path = "validation/"
    else:
        raise IndexError

    folders = sorted(os.listdir(folder_path))

    # extract mfccs and place into array
    speaker_sets = []
    comparison_sets = []
    labels = []
    for folder in folders:
        sub_folder = folder_path + folder + "/"
        files = os.listdir(sub_folder)
        file_path = sub_folder + files[0]
        speaker_sets.append(dh.extract_mfcc(file_path, MAX_LENGTH))
        file_path = sub_folder + "/" + files[1]
        comparison_sets.append(dh.extract_mfcc(file_path, MAX_LENGTH)) 

        same_speaker = folder.split('_')
        if (same_speaker[-1] == "True"):
            same_speaker = [0.0, 1.0]
        else:
            same_speaker = [1.0, 0.0]
        labels.append(same_speaker)
    
    
    dataset = dh.CustomDataset(speaker_sets, comparison_sets, labels)
    
    loader = torch.utils.data.DataLoader(dataset, batch_size=32, shuffle=True)

    my_model = model.SimpleModel()
    loss_fn = nn.CrossEntropyLoss()

    epoch_number = 0
    EPOCHS = 64
    optimizer = torch.optim.AdamW(my_model.parameters(), lr=0.0001)

    if (action == "train"):   
        for epoch in range(EPOCHS):
            running_loss = 0
            correct_predictions = 0
            total_samples = 0

            for file_set in range(len(labels)):
                speaker_data, comparison_data, label = dataset.__getitem__(file_set)

                speaker_data_tensor = torch.tensor(speaker_data)
                comparison_data_tensor = torch.tensor(comparison_data)
                speaker_data_tensor = speaker_data_tensor.unsqueeze(0).unsqueeze(0)  # Shape: [1, 1, 13, 100]
                comparison_data_tensor = comparison_data_tensor.unsqueeze(0).unsqueeze(0)  # Shape: [1, 1, 13, 100]
                temp_label = label
                label = torch.tensor(label)

                outputs = my_model.forward(speaker_data_tensor, comparison_data_tensor)

                # Calculate loss
                loss = loss_fn(outputs, label)
                
                # Backward pass and optimize
                optimizer.zero_grad()
                loss.backward()
                optimizer.step()
                
                # Update running loss
                running_loss += loss.item()
                
                # Calculate accuracy
                predicted = torch.argmax(outputs)
                answer = torch.argmax(label)
                total_samples += 1
                
                correct_predictions += int((predicted.item() == answer.item()))
            
            # Calculate average loss and accuracy for the epoch
            avg_loss = running_loss / len(labels)
            accuracy = correct_predictions / total_samples
            
            # Write to SummaryWriter
            writer = SummaryWriter('logs')
            writer.add_scalar('training_loss', avg_loss, epoch)
            writer.add_scalar('training_accuracy', accuracy, epoch)
            
            # Print epoch statistics
            print(f'Epoch [{epoch+1}/{EPOCHS}], Loss: {avg_loss:.4f}, Accuracy: {accuracy:.4f}')
        torch.save(my_model, "VoiceRecognitionModel")



    else:
        my_model = torch.load("VoiceRecognitionModel")
        my_model.eval()
        with torch.no_grad():

            running_loss = 0
            correct_predictions = 0
            total_samples = 0
            # Iterate over validation data loader
            for file_set in range(len(labels)):
                speaker_data, comparison_data, label = dataset.__getitem__(file_set)

                speaker_data_tensor = torch.tensor(speaker_data)
                comparison_data_tensor = torch.tensor(comparison_data)
                speaker_data_tensor = speaker_data_tensor.unsqueeze(0).unsqueeze(0)  # Shape: [1, 1, 13, 100]
                comparison_data_tensor = comparison_data_tensor.unsqueeze(0).unsqueeze(0)  # Shape: [1, 1, 13, 100]
                temp_label = label
                label = torch.tensor(label)

                outputs = my_model.forward(speaker_data_tensor, comparison_data_tensor)

                # Calculate loss
                loss = loss_fn(outputs, label)
                
                # Backward pass and optimize
                optimizer.zero_grad()
                #loss.backward()
                optimizer.step()
                
                # Update running loss
                running_loss += loss.item()
                
                # Calculate accuracy
                predicted = torch.argmax(outputs)
                answer = torch.argmax(label)
                total_samples += 1
                
                correct_predictions += int((predicted.item() == answer.item()))
            
            # Calculate average loss and accuracy for the epoch
            avg_loss = running_loss / len(labels)
            accuracy = correct_predictions / total_samples
            
            # Write to SummaryWriter
            writer = SummaryWriter('logs')
            writer.add_scalar('training_loss', avg_loss)
            writer.add_scalar('training_accuracy', accuracy)
        
            # Print epoch statistics
            print(f'Loss: {avg_loss:.4f}, Accuracy: {accuracy:.4f}')
