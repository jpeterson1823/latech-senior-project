import sounddevice as sd
import soundfile as sf
import threading
import os
import queue
import speech_recognition as sr
import time
import User
import numpy as np
import data_handling as dh
import noisereduce as nr


shared_dict = {"command": ""}

CURRENT_DIR = os.path.dirname(os.path.abspath(__file__))
audio_file1 = "testing_recording/sample_command1.wav"
audio_file2 = os.path.join(CURRENT_DIR, "testing_recording/sample_command2.wav")
duration = 5  # Record audio for 3 seconds
sample_rate = 16000  # Standard audio sample rate
channels = 1  # Mono audio
speech = sr.Recognizer()
audio_buffer = queue.Queue()
speech_detection_Event = threading.Event()
def record_buffer():
    while True:
        audio = sd.rec(int(duration * sample_rate), samplerate=sample_rate, channels=channels, dtype='int16')
        sd.wait()
        audio_buffer.put(audio)

def transcribe_buffer():
    while True:
        global shared_dict
        audio = audio_buffer.get()
        sf.write(audio_file1, audio, 16000)
        
        #sf.write(audio_file2, audio, 16000)
        with sr.AudioFile(audio_file1) as source:
            audio = speech.record(source)
            try:
                text = speech.recognize_sphinx(audio, keyword_entries=[("prism", 0.9), ("calendar", 0.9), ("weather", 0.9), ("show", 0.99), ("today", 0.9)])
            except sr.exceptions.UnknownValueError:
                text = "No Command Given"
                print(text)
        if "prism" in text:
            shared_dict["command"] = text
            data, samplerate = sf.read(audio_file1)
            y_reduced = nr.reduce_noise(y=data, sr=samplerate)
            sf.write(audio_file1, y_reduced, samplerate)
            numpy_arr = dh.extract_mfcc(audio_file1, 100)
            np.save("testing_recording/sample_command.npy", numpy_arr)
            speech_detection_Event.set()


def start_listening():
    pass

if __name__ == '__main__':
    return_dict = {1:False}
    u1 = User.User("JohnW", "Waskom_model", "My_audio/", "calen.ui")
    record_thread = threading.Thread(target=record_buffer)
    transcribe_thread = threading.Thread(target=transcribe_buffer)

    record_thread.start()
    transcribe_thread.start()

    speech_detection_Event.wait()

    parsed_command = shared_dict["command"]
    parsed_command = parsed_command.split("  ")
    print(parsed_command)
    u1.isThisUser("testing_recording/sample_command.npy", 1, return_dict)
    print(return_dict[1])

    record_thread.join()
    transcribe_thread.join()

