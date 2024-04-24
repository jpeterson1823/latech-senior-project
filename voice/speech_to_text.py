from vosk import Model, KaldiRecognizer
import pyaudio

model = Model("vosk-model-en-us-0.42-gigaspeech/")
recognizer = KaldiRecognizer(model, 16000)

# Recognize from the microphone
cap = pyaudio.PyAudio()
stream = cap.open(format=pyaudio.paInt16, channels=1, rate=16000, input=True,frames_per_buffer=8192)
stream.start_stream()

def text_from_speech():
    data = stream.read(4096)
    if len(data) == 0:
        break

    if recognizer.AcceptWaveform(data):
        return recognizer.Result()