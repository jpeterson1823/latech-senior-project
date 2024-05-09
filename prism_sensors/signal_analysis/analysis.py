import matplotlib.pyplot as plt
import sys

def loadPingData(file_path) -> tuple[list[int], list[int]]:
    with open(file_path, 'r') as f:
        data = [int(x.strip()) for x in f.readlines()]
    entry = list(range(0, len(data)))
    return (entry,data)

class Point:
    def __init__(self, x, y):
        self.x = x
        self.y = y

def getMaximums(data):
    peaks = []
    last = Point(0,0)
    increasing = True
    for i in range(len(data)):
        p = Point(i, data[i])
        # if going downwards
        if last.y > data[i]:
            # if changed direction while increasing, local max found
            if increasing:
                peaks.append(p)
                increasing = False
            # if not increasing, then falling
        # otherwise going upwards
        else:
            increasing = True
        last = p
    # fill inbetwee with values
    padded = [0] * len(data)
    for peak in peaks:
        padded[peak.x] = peak.y

    # smooth padded
    last = 0
    for peak in peaks:
        i = last + 1
        while i <= peak.x:
            padded[i] = peak.y
            i+= 1
        last = peak.x
    return padded

def rollingAvg(data):
    d = []
    acc = 0
    for i in range(len(data)):
        acc += data[i]
        d.append(acc/(i+1))
    return d

def derivative(data):
    dxdy = []
    last = data[0]
    for i in range(1, len(data)):
        dxdy.append((data[i] - last))
    return dxdy


if __name__ == "__main__":
    # read values in from file
    if len(sys.argv) > 1:
        file_path = sys.argv[1]
    else:
        file_path = "./ping.txt"


    entry,data = loadPingData(file_path)
    #data = [p.y for p in getMaximums(data)]

    clean = getMaximums(data)

    fig = plt.figure()
    ax1 = fig.add_subplot()
    ax1.plot(list(range(len(data)-500)), data[500:], 'r', label='raw')
    ax1.plot(list(range(len(clean)-500)), clean[500:], 'b', label='clean')
    plt.xlabel('entry')
    plt.ylabel('data')
    plt.legend()
    plt.title('Ping Analysis')
    plt.show()
