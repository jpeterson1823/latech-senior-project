import torch
import os
import random
import numpy as np
import gui_main as gui


class User():
    def __init__(self, name, model, audio_files_path, calendar):
        self.user_name = name
        self.Model_path = model
        self.audio_folder = audio_files_path
        self.calendar = calendar

    def isThisUser(self, comparison_array) -> bool:
        tmp_list = os.listdir(self.audio_folder)
        choices = []
        num_comparisons = 9
        for _ in range(num_comparisons):
            choice = random.choice(tmp_list)
            tmp_list.remove(choice)
            choice = self.audio_folder + choice
            choices.append(choice)

        model = torch.load(self.Model_path)
        model.eval()
        with torch.no_grad():
            comparison_tensor = torch.tensor(np.load(comparison_array))
            outputs_avg = 0
            for i in range(len(choices)):
                user_tensor = torch.tensor(np.load(choices[i]))
                output = model.forward(user_tensor, comparison_tensor)
                outputs_avg += round(output.item(), 2)
            outputs_avg /= num_comparisons
            if outputs_avg > 0.5:
                return True
            else:
                return False


