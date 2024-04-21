import os
import subprocess

for _ in range(20):
    p = subprocess.Popen(["python3", "generate_training_and_validation.py", "training/", "100"])
    status = os.wait()
    s = subprocess.Popen(["python3", "main.py", "continue"])
    status = os.wait()