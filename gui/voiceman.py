from threading import Thread
import time

global gVoiceCommand, gVoiceActive
global gMainWindow

def update(command: str, parsed: str):
    global gVoiceCommand
    gVoiceCommand["command"] = command
    gVoiceCommand["parsed"] = parsed

def poll():
    global gVoiceCommand
    return gVoiceCommand

def _reset_command():
    global gVoiceCommand
    gVoiceCommand = {"command": "", "parsed": ""}


def _thread_init():
    global gVoiceCommand, gVoiceActive
    _reset_command()
    gVoiceActive = False

def _voice_man_thread_entry(mainWindow):
    global gVoiceCommand, gVoiceActive
    while (True):
        print(gVoiceCommand)
        if gVoiceCommand['parsed'] != "":
            if "calendar" in gVoiceCommand['parsed']:
                mainWindow.calendarCommand()
            elif "weather" in gVoiceCommand['parsed']:
                mainWindow.weatherCommand()
                print("passed event to mainWindow")
            _reset_command()

        time.sleep(1)


def startThread(mainWindow) -> Thread:
    global gVoiceCommand, gVoiceActive, gMainWindow
    gMainWindow = mainWindow
    _thread_init()
    t = Thread(target=_voice_man_thread_entry, args=[mainWindow])
    t.start()
    gVoiceActive = True
    return t

def stopThread():
    global gVoiceActive
    gVoiceActive = False
