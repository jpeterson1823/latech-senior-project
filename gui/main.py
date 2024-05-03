import sys
from PyQt5.QtWidgets import *
from res.MainWindow import MainWindow
import commands.recordAndTranscribe as rat

import commands.shared

def main():
    app = QApplication(sys.argv)
    window = MainWindow()
    window.center()
    window.show()

    commands.shared.init()
    tlist = rat.startThreads()
    appExitCode = app.exec()

    for t in tlist:
        t.join()
    sys.exit(appExitCode)

    

if __name__ == "__main__":
    main()
    