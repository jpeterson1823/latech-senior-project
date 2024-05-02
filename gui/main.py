import sys
from PyQt5.QtWidgets import *
from res.MainWindow import MainWindow
import threading

def main():
    app = QApplication(sys.argv)
    window = MainWindow()
    window.center()
    window.show()
    record_thread, transcribe_thread = window.startRecording()
    sys.exit(app.exec())
    record_thread.join()
    transcribe_thread.join()

if __name__ == "__main__":
    main()
    