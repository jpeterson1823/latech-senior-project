import os
import random as r
import shutil
import sys


parent_directory = "../ST-AEDS-20180100_1-OS/"
user_dir = "voice_clips"
speaker_directories = os.listdir(parent_directory)
user_files = os.listdir(user_dir)

# has been altered to give files from one speaker only and give comparisons to that one speaker
def populate_dataset(dir_name: str, set_size: int):

    for i in range(set_size):
        
        same_speaker = r.choices([True, False])
        temp_dir_name = dir_name + "set_" + str(i) + "_" + str(same_speaker)[1:-1]
        os.mkdir(temp_dir_name)

        # pick a random speaker
        #speaker = r.randint(0, len(speaker_directories)-1)
        #speaker = 1
        #speaker_path = user_dir + "/" + speaker
        #speaker_files = os.listdir(speaker_path)

        # pick a random file
        user_file_index = r.randint(0, len(user_files)-1)
        src_file = user_dir + "/" + user_files[user_file_index]
        dest = temp_dir_name + "/"
        if (same_speaker[0]):
            while(True):
                comparison_file_index = r.randint(0, len(user_files)-1)
                if (comparison_file_index != user_file_index):
                    break
            comparison_dir = user_files
            comparison_file_path = user_dir + "/" + user_files[comparison_file_index]

        else:
            # pick a new speaker for comparison
            comparison_speaker_index = r.randint(0, len(speaker_directories)-1)
            comparison_path = parent_directory + "/" + speaker_directories[comparison_speaker_index]
            comparison_dir = os.listdir(comparison_path)

            # pick a random file for comparison
            comparison_file_index = r.randint(0, len(comparison_dir)-1)
            comparison_file_path = comparison_path + "/" + comparison_dir[comparison_file_index]

        user_dest = dest + user_files[user_file_index]
        shutil.copy(src_file, user_dest)

        comparison_dest = dest + comparison_dir[comparison_file_index]
        shutil.copy(comparison_file_path, comparison_dest)








try:
    folder_to_populate = sys.argv[1]
    number_of_validation_sets = int(sys.argv[2])

    if (folder_to_populate[-1] != '/'):
        raise IndexError
    # check if a folder_to_populate already exists and delete it
    folder_to_populate_list = os.listdir(folder_to_populate)
    if (len(folder_to_populate_list) != 0 ):
        shutil.rmtree(folder_to_populate)
        os.mkdir(folder_to_populate)

    print(folder_to_populate, number_of_validation_sets)
    populate_dataset(folder_to_populate, number_of_validation_sets)
except IndexError:
    print("Usage: python3 generate_training_and_validation.py <folder_name_with_a_slash> <number_of_sets>")