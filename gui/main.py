import sys
from PyQt5.QtWidgets import *
from res.MainWindow import MainWindow
import commands.recordAndTranscribe as rat

import commands.shared
import listener

def main():
    # pyqt
    app = QApplication(sys.argv)
    window = MainWindow()
    window.center()
    window.show()

    # voice
    commands.shared.init()
    tlist = rat.startThreads()

    # sensor listener
    tlist += listener.openListeningServer()

    # start qtapp
    appExitCode = app.exec()

    # join threads and exit
    listener.closeListeningServer()
    for t in tlist:
        t.join()
    sys.exit(appExitCode)

    

if __name__ == "__main__":
    main()
    