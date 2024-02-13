import serial

def main():
    #port = "/dev/ttyACM0"
    port = "COM6"
    baud = 115200

    s = serial.Serial(port, baud)

    # send ready
    s.write(b"\x00\n")
    print("READY SENT")

    # get request
    req = s.readline()
    print(f"Request: {req}")

    if (req == "kenchiki.hajime\n"):
        s.write(b"bingus;FizzBuzz23!\n")
        print("Response Sent!")

    else:
        s.write(b"ack\n")
        print(f"Serial Received: {s.readline()}")


if __name__ == "__main__":
    main()