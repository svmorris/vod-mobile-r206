
import time

offset = 0
while True:
    print(f"nandread -d /dev/mtd/mtd2 -L 4096 -S {offset} -f test.bin")
    offset += 4096
    time.sleep(0.1)
