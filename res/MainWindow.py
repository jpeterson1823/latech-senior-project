from PyQt5.QtWidgets import *
from PyQt5.uic import loadUi

from apps.calendar import CalendarWindow
from apps.weather import MainWindow as WthrMain

class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        loadUi("res/ui/main.ui", self)
        self.w = None 
        self.t = None
        self.calBtn.clicked.connect(self.calendar)
        self.pairBtn.clicked.connect(self.pair)
        self.wthrBtn.clicked.connect(self.wthr)
        self.userBtn.clicked.connect(self.addUser)

    def calendar(self):
        if self.w is None:
            self.w = CalendarWindow()
            self.w.center_cal()
        self.w.show()
        
    def pair(self):
        #Pairing code
        if 1:
            messageBox = QMessageBox()
            messageBox.setText("No device detected. Ensure it is on.")
            messageBox.setStandardButtons(QMessageBox.Ok)
            messageBox.setStyleSheet("QLabel{min-width:300 px; font-size: 18px;} QPushButton{ width:100px; font-size: 18px; }")
            messageBox.exec()
        #else:
            #messageBox = QMessageBox()
            #messageBox.setText("Device paired successfully.")
            #messageBox.setStandardButtons(QMessageBox.Ok)
            #messageBox.setStyleSheet("QLabel{min-width:300 px; font-size: 24px;} QPushButton{ width:100px; font-size: 18px; }")
            #messageBox.exec()
        return
    
    def wthr(self):
        if self.t is None:
            self.t = WthrMain()
        self.t.center_screen()
        self.t.show()
        
    def addUser(self):
        messageBox = QMessageBox()
        messageBox.setText("No user to add")
        messageBox.setStandardButtons(QMessageBox.Ok)
        messageBox.setStyleSheet("QLabel{min-width:300 px; font-size: 18px;} QPushButton{ width:100px; font-size: 18px; }")
        messageBox.exec()
        
    def center(self):
        qtRectangle = self.frameGeometry()
        centerPoint = QDesktopWidget().availableGeometry().topLeft()
        qtRectangle.moveCenter(centerPoint)
        self.move(qtRectangle.center())