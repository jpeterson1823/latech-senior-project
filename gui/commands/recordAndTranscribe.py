import sounddevice as sd
import soundfile as sf
import threading
import os
import queue
import speech_recognition as sr
import numpy as np
import commands.data_handling as dh
import noisereduce as nr


shared_dict = {"command": "", "parsed": ""}

audio_file = "commands/command_audio/command.wav"
duration = 5  # Record audio for 3 seconds
sample_rate = 16000  # Standard audio sample rate
channels = 1  # Mono audio
speech = sr.Recognizer()
audio_buffer = queue.Queue()



def record_buffer():
    while True:
        audio = sd.rec(int(duration * sample_rate), samplerate=sample_rate, channels=channels, dtype='int16')
        sd.wait()
        audio_buffer.put(audio)

def transcribe_buffer(speech_detection_Event):
    while True:
        global shared_dict
        audio = audio_buffer.get()
        sf.write(audio_file, audio, 16000)
        
        #sf.write(audio_file2, audio, 16000)
        with sr.AudioFile(audio_file) as source:
            audio = speech.record(source)
            try:
                text = speech.recognize_sphinx(audio, keyword_entries=[("prism", 0.9), ("calendar", 0.85), ("weather", 0.9), ("show", 0.99), ("today", 0.9)])
            except sr.exceptions.UnknownValueError:
                text = "No Command Given"
        if "prism" in text:
            shared_dict["command"] = text
            data, samplerate = sf.read(audio_file)
            y_reduced = nr.reduce_noise(y=data, sr=samplerate)
            sf.write(audio_file, y_reduced, samplerate)
            numpy_arr = dh.extract_mfcc(audio_file, 100)
            np.save("commands/command_audio/command.npy", numpy_arr)
            speech_detection_Event.set()   
            parse(shared_dict)

def parse(shared_dict):
    command = shared_dict['command']
    command = command.split("  ")
    command = list(set(command))
    shared_dict["parsed"] = command


    
if __name__ == '__main__':

    print("Listening")
    record_thread = threading.Thread(target=record_buffer)
    transcribe_thread = threading.Thread(target=transcribe_buffer)

    record_thread.start()
    transcribe_thread.start()

    speech_detection_Event.wait()

    record_thread.join()
    transcribe_thread.join()

