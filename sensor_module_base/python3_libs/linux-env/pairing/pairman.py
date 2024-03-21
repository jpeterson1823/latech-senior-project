import serial, platform

def open_serial():
    # determine port based on OS
    os = platform.system()

    if os == "Linux":
        port = "/dev/ttyACM0"
    elif os == "Windows":
        port = "COM0"