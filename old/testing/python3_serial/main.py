from time import sleep
import serial, os

# make sure not to touch \x00 at end of str
def endecrypt_xor(s: bytes, key: bytes):
    barr = bytearray()
    for i in range(len(s)):
        barr.append(s[i] ^ key[i%len(key)])
    return barr

def main():
    #key = b"~key!"
    #text = b"Testing"
    #for i in range(len(text)):
    #    print(text[i])
    #print(f"\"{text}\"")
    #ciphertext = endecrypt_xor(text, key)
    #deciphered = endecrypt_xor(ciphertext, key)

    #print(f"Ciphertext: {ciphertext}")
    #print(f"Deciphered: {deciphered}")

    # wait for ttyACM0 to appear
    while True:
        if not os.path.exists("/dev/ttyACM0"):
            print("Waiting on ttyACM0...")
            sleep(5)
        else:
            break

    s = serial.Serial(
            port="/dev/ttyACM0",
            baudrate=115200,
            timeout=0,
            parity=serial.PARITY_ODD,
            stopbits=serial.STOPBITS_TWO,
            bytesize=serial.EIGHTBITS
        )


    data = get_eeprom_data(s)
    print(f"Data Retrieved: \"{data}\"")

    #data = b"tis_some_data\x00"
    #print(f"Data being written:\n{data}");
    #set_eeprom_data(s, data)
    #print("Write Complete")
    
    
    
def get_eeprom_data(s: serial.Serial) -> bytes:
    try:
        # make sure serial connection is open
        if not s.is_open:
            s.open()

        # write get command to picokey
        s.write(b"PICO_MOD::GET\n")
        s.flush()

        max_loop = 10000
        loop = 0
        while True:
            data = s.readline()

            # make sure that command isnt read on accident
            if data == b"PICO_MOD::GET\r\n":
                sleep(0.5)

            # check for picomod acknowledge
            elif data == b"PICO_MOD::ACK\r\n":
                sleep(0.5)
                # retrieve data after ack
                return bytes(bytearray(s.readline())[:-2])

            loop += 1
            sleep(0.1)
            if loop >= max_loop:
                print("MAX LOOP REACHED IN get_eeprom_data() !")
                break
    except serial.SerialException as se:
        print(f"[get_eeprom_data()] SerialException: {se}")
        return None

def set_eeprom_data(s: serial.Serial, data: bytes):
    try:
        if not s.is_open:
            s.open()
        s.write(b"PICO_MOD::SET\n")
        s.flush()

        max_loops=10000
        loops = 0
        while True:
            in_data = s.readline()
            if in_data == b"PICO_MOD::SET\r\n":
                sleep(1)
            elif in_data == b"PICO_MOD::ACK\r\n":
                sleep(1)
                s.write(data)
                s.flush()
                s.readline()
                return
            else:
                sleep(0.05)
            loops += 1
            if loops >= max_loops:
                print(f"MAX LOOP REACHED IN set_eeprom_data() !")
                return
    except serial.SerialException as se:
        print(f"[set_eeprom_data()] SerialException: {se}")
        return

def get_data_test(s: serial.Serial):
    while True:
        try:
            if not s.is_open:
                s.open()

            s.write(b"PICO_MOD::GET\n")
            s.flush()

            print("Reading...")
            while True:
                data = s.readline()
                if data == b"PICO_MOD::GET\r\n":
                    sleep(1)
                elif data == b"PICO_MOD::ACK\r\n":
                    sleep(2)
                    print(f"DATA: {s.readline().decode('utf-8')}")
                    break

            print("DONE")
            sleep(1)
            s.close()
            exit(0)
        except serial.SerialException as e:
            print(f"SerialError: {e}")
            exit(1)

def set_data_test(s: serial.Serial):
    while True:
        try:
            if not s.is_open:
                s.open()
            sleep(1)

            s.write(b"PICO_MOD::SET\n")
            s.flush()

            print("Setting...")
            while True:
                data = s.readline()
                if data == b"PICO_MOD::SET\r\n":
                    sleep(1)
                elif data == b"PICO_MOD::ACK\r\n":
                    print("ACK")
                    sleep(0.5)
                    s.write(b"testing\x00")
                    s.flush()
                    print(s.readline())
                    sleep(5)
                    break
                else: 
                    sleep(0.1)

            print("DONE")
            sleep(1)
            s.close()
            exit(0)
        except serial.SerialException as e:
            print(f"SerialError: {e}")
            exit(1)

if __name__ == "__main__":
    main()
