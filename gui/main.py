import sys
from PyQt5.QtWidgets import *
from res.MainWindow import MainWindow

def main():
    app = QApplication(sys.argv)
    window = MainWindow()
    window.center()
    window.show()
    sys.exit(app.exec())

if __name__ == "__main__":
    main()
    