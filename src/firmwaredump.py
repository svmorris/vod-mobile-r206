import sys
import time
import serial
import select
import datetime
import threading

PORT = '/dev/ttyUSB0'  # change this to your port (e.g., COM3 on Windows)
BAUD = 115200

# --- Open the serial port ---
ser = serial.Serial(PORT, BAUD, timeout=0.1)

# --- Background thread for reading ---
def read_loop():
    buffer = b""
    transfer_running = False
    transfer_ended = False
    MAX_BYTES = 1000  # stop writing after this many total bytes
    data_len = 0
    while True:
        if ser.in_waiting:               # if bytes are waiting
            data = ser.read(ser.in_waiting)
            if b'-------------------' in data and b'"-------------------"' not in data:
                print("Transfer started...")
                transfer_running = True
                continue
            #
            # if not data:
            #     print("Transfer ended...")
            #     transfer_started = False

            if transfer_running:
                remaining = MAX_BYTES - data_len  # how many bytes we can still write

                if remaining > 0:
                    # Split into what we can write now vs what stays buffered
                    bindata = data[:remaining]      # part that fits
                    data = data[remaining:]         # leftover that exceeds the limit

                    data_len += len(bindata)

                    with open("dump.bin", "ab") as dumpfile:
                        dumpfile.write(bindata)

                    print("<<<", "'", bindata.hex(), "'")

                    # Stop once we've reached or passed the limit
                    if data_len >= MAX_BYTES:
                        transfer_running = False
            else:
                # Already reached limit → keep all data for normal processing
                print("<<<", "'", data.hex(), "'")

reader = threading.Thread(target=read_loop, daemon=True)
reader.start()

# --- Main loop for sending ---
try:
    time.sleep(1)
    ser.write(b'\n')
    ser.write(b'echo "-------------------" && dd if=/dev/block/mtdblock1 count=1000 bs=1')




    while True:
        ser.write(input(">> ").encode("utf-8"))
        ser.write(b"\n")
        time.sleep(1)


except KeyboardInterrupt:
    print("\nExiting...")
finally:
    ser.close()
