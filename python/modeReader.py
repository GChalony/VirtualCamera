import time

MODE = 0

def read_mode():
    with open("mode.txt", "r") as f:
        m = int(f.readline())
    return m

if __name__ == "__main__":
    while True:
        MODE = read_mode()
        print(f"{MODE=}")
        time.sleep(1)

