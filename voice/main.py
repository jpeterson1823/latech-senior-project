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



MAX_LENGTH = 100


if __name__ == "__main__":

    try:
        action = sys.argv[1]
        if (len(sys.argv) == 3):
            if (sys.argv[2].lower() == "debug"):
                DEBUG = True
            else:
                DEBUG = False
            
        else:
            DEBUG = False

    except IndexError:
        print("Usage: python3 main1.py <train_or_validate>")
        exit()

    if (action == "train"):
        folder_path = "training/"
    elif (action == "validate"):
        folder_path = "validation/"
    elif (action == "continue"):
        folder_path = "training/"
    else:
        raise IndexError

    preprocess_start = time.time()
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
            same_speaker = 1.0
        else:
            same_speaker = 0.0
        labels.append(same_speaker)
    

    dataset = dh.CustomDataset(speaker_sets, comparison_sets, labels)
    
    loader = torch.utils.data.DataLoader(dataset, batch_size=32, shuffle=True)
    preprocess_end = time.time()
    print(f"Preprocessing time: {preprocess_end - preprocess_start}")
    my_model = model.LinearModel()
    loss_fn = nn.BCELoss()

    epoch_number = 0
    EPOCHS = 100
    optimizer = torch.optim.Adam(my_model.parameters(), lr=0.001)

    if (action == "train"):   
        training_start = time.time()
        for epoch in range(EPOCHS):
            start_time = time.time()
            running_loss = 0
            correct_predictions = 0
            total_samples = 0

            for file_set in range(len(labels)):
                speaker_data, comparison_data, label = dataset.__getitem__(file_set)

                speaker_data_tensor = torch.tensor(speaker_data)
                comparison_data_tensor = torch.tensor(comparison_data)
                
                temp_label = label
                label = torch.tensor([label])

                outputs = my_model.forward(speaker_data_tensor, comparison_data_tensor)
                predicted = outputs
                if (predicted >= 0.6):
                    predicted = 1.0
                else:
                    predicted = 0.0
                # Calculate loss
                loss = loss_fn(outputs, label)
                
                # Backward pass and optimize
                optimizer.zero_grad()
                loss.backward()
                optimizer.step()
                
                # Update running loss
                running_loss += loss.item()
                
                # Calculate accuracy
                answer = label
                total_samples += 1
                
                correct_predictions += int((predicted == answer))
            
            # Calculate average loss and accuracy for the epoch
            avg_loss = running_loss / len(labels)
            accuracy = correct_predictions / total_samples
            
            # Write to SummaryWriter
            writer = SummaryWriter('logs')
            writer.add_scalar('training_loss', avg_loss, epoch)
            writer.add_scalar('training_accuracy', accuracy, epoch)
            
            # Print epoch statistics
            print(f'Epoch [{epoch+1}/{EPOCHS}], Loss: {avg_loss:.4f}, Accuracy: {accuracy:.4f}')
            end_time = time.time()
            time_elapsed = end_time - start_time
            print(f"Epoch: {epoch} took {time_elapsed}s")
        training_end = time.time()
        training_elapsed = training_end - training_start
        training_elapsed /= 60
        print("Total Training Time: ", training_elapsed, " minutes")
        torch.save(my_model, "VoiceRecognitionModel")
            

    elif (action == "continue"):
        my_model = torch.load("VoiceRecognitionModel")
        training_start = time.time()
        for epoch in range(EPOCHS):
            start_time = time.time()
            running_loss = 0
            correct_predictions = 0
            total_samples = 0

            for file_set in range(len(labels)):
                speaker_data, comparison_data, label = dataset.__getitem__(file_set)

                speaker_data_tensor = torch.tensor(speaker_data)
                comparison_data_tensor = torch.tensor(comparison_data)
                
                temp_label = label
                label = torch.tensor([label])

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
                predicted = outputs
                if (predicted >= 0.6):
                    predicted = 1.0
                else:
                    predicted = 0.0
                answer = label
                total_samples += 1
                
                correct_predictions += int((predicted == answer))
            
            # Calculate average loss and accuracy for the epoch
            avg_loss = running_loss / len(labels)
            accuracy = correct_predictions / total_samples
            
            # Write to SummaryWriter
            writer = SummaryWriter('logs')
            writer.add_scalar('training_loss', avg_loss, epoch)
            writer.add_scalar('training_accuracy', accuracy, epoch)
            
            # Print epoch statistics
            print(f'Epoch [{epoch+1}/{EPOCHS}], Loss: {avg_loss:.4f}, Accuracy: {accuracy:.4f}')
            end_time = time.time()
            time_elapsed = end_time - start_time
            print(f"Epoch: {epoch} took {time_elapsed}s")
        training_end = time.time()
        training_elapsed = training_end - training_start
        training_elapsed /= 60
        print("Total Training Time: ", training_elapsed, " minutes")
        torch.save(my_model, "VoiceRecognitionModel")

    elif (action == "validate"):
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
                
                temp_label = label
                label = torch.tensor([label])

                outputs = my_model.forward(speaker_data_tensor, comparison_data_tensor)
                print(outputs)

                # Calculate loss
                loss = loss_fn(outputs, label)
                
                # Backward pass and optimize
                optimizer.zero_grad()
                #loss.backward()
                optimizer.step()
                
                # Update running loss
                running_loss += loss.item()
                
                # Calculate accuracy
                predicted = outputs
                if (predicted >= 0.6):
                    predicted = 1.0
                else:
                    predicted = 0.0
                
                answer = label
                total_samples += 1
                
                correct_predictions += int((predicted == answer))
                if (DEBUG):
                    print("Predicted:", predicted, " Correct Answer:", label)
            
            # Calculate average loss and accuracy for the epoch
            avg_loss = running_loss / len(labels)
            accuracy = correct_predictions / total_samples
            
            # Write to SummaryWriter
            writer = SummaryWriter('logs')
            writer.add_scalar('training_loss', avg_loss)
            writer.add_scalar('training_accuracy', accuracy)
             
            # Print epoch statistics
            print(f'Loss: {avg_loss:.4f}, Accuracy: {accuracy:.4f}')
