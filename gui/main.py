import sys
from PyQt5.QtWidgets import *
from res.MainWindow import MainWindow
import commands.recordAndTranscribe as rat

import sensorman
import voiceman

def main():
    # pyqt
    app = QApplication(sys.argv)
    window = MainWindow()
    window.center()
    window.show()

    # voice
    #TODO: combine voiceman and commands.shared into single import
    tlist = rat.startThreads()
    tlist.append(voiceman.startThread(window))

    # sensor sensorman
    tlist += sensorman.openListeningServer()

    # start qtapp
    appExitCode = app.exec()

    # join threads and exit
    sensorman.closeListeningServer()
    voiceman.stopThread()
    for t in tlist:
        t.join()
    sys.exit(appExitCode)

    
if __name__ == "__main__":
    main()
    