import serial

def simulate():
    port = "/dev/ttyACM0"
    #port = "COM6"
    baud = 115200

    s = serial.Serial(port, baud)

    # send ready
    s.write(b"\x00\n")
    print("READY SENT")

    # get request
    req = s.readline()
    print(f"Request: {req}")

    if (req == b"kenchiki.hajime\r\n"):
        s.write(b"bingus;FizzBuzz23!\n")
        print("Response Sent!")

        print("Awaiting ACK...", end="")
        a = s.readline()
        if (a == b"ack\r\n"):
            print(" Sensor ACK'd")
        else:
            print(" NO ACK RECEIVED!")

    else:
        s.write(b"ack\n")
        print(f"Serial Received: {s.readline()}")
    
    print("\nBEGINNING SERIAL MONITOR\n################################\n\n")
    try:
        while(True):
            print(s.readline().decode("utf-8"), end="")
    except:
        exit(0)
