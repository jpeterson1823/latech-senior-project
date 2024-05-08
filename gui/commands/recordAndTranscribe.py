import sounddevice as sd
import soundfile as sf
import threading
import queue
import speech_recognition as sr
import time

import voiceman

audio_file = "commands/command_audio/command.wav"
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
        time.sleep(0.5)

def transcribe_buffer():
    global speech_detection_Event
    while True:
        
        audio = audio_buffer.get()
        sf.write(audio_file, audio, 16000)
        
        #sf.write(audio_file2, audio, 16000)
        with sr.AudioFile(audio_file) as source:
            audio = speech.record(source)
            try:
                text = speech.recognize_sphinx(audio, keyword_entries=[("prism", 0.99), ("calendar", 0.99), ("weather", 0.99), ("show", 0.999), ("today", 0.99), ("pair", 0.99), 
                                                                       ("schedule", 0.99), ("device", 0.99)])
            except sr.exceptions.UnknownValueError:
                text = "No Command Given"
        if "prism" in text:
            voiceman.poll()["command"] = text
            #data, samplerate = sf.read(audio_file)
            #y_reduced = nr.reduce_noise(y=data, sr=samplerate)
            #sf.write(audio_file, y_reduced, samplerate)
            #numpy_arr = dh.extract_mfcc(audio_file, 100)
            #np.save("commands/command_audio/command.npy", numpy_arr)
            parse()
        time.sleep(0.5)

def parse():
    print("was given a command")
    command = voiceman.poll()['command']
    command = command.split("  ")
    command = list(set(command))
    for i in range(len(command)):
        command[i] = command[i].strip()

    print(command)
    voiceman.poll()["parsed"] = command

def startThreads() -> threading.Thread:
    print("Listening")
    record_thread = threading.Thread(target=record_buffer)
    transcribe_thread = threading.Thread(target=transcribe_buffer)

    record_thread.start()
    print("starting record thread")
    transcribe_thread.start()
    print("starting transcribe thread")


    return [record_thread, transcribe_thread]


    
if __name__ == '__main__':

    startThreads()

