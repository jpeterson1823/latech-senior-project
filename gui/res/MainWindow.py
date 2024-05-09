from PyQt5.QtWidgets import *
from PyQt5.uic import loadUi
from PyQt5.QtCore import pyqtSignal, QObject

from apps.calendar import CalendarWindow
from apps.weather import MainWindow as WthrMain
import subprocess
import multiprocessing
from multiprocessing import Process
from res.User import User
import mysql.connector
import os.path
import commands.recordAndTranscribe as rat
import numpy.typing as npt
from time import sleep



class Command(QObject):
    calendar_command = pyqtSignal()
    weather_command = pyqtSignal()
    pair_command = pyqtSignal()

class MainWindow(QMainWindow):
    
    def __init__(self):
        super().__init__()
        loadUi("res/ui/main.ui", self)
        self.pullData()
        self.w = None 
        self.t = None
        self.users = []
        self.data = self.pullData()
        print(self.data)
        self.activeUserIndex = None
        self.compare_path = "commands/command_audio/command.npy"
        self.command = Command()
        self.command.calendar_command.connect(self.calendar)
        self.command.weather_command.connect(self.wthr)
        self.command.pair_command.connect(self.pair)
        self.calBtn.clicked.connect(self.calendar)
        self.pairBtn.clicked.connect(self.pair)
        self.wthrBtn.clicked.connect(self.wthr)
        self.userBtn.clicked.connect(self.addUser)

        if len(self.data) > 0:
            print("Instantiating Users")
            self.instantiateExistingUsers(self.data)
            print(len(self.users))

    # subject to change depending on how the database is setup
    def instantiateExistingUsers(self, users_entry: list):
        print("made it here")
        for i in range(len(users_entry)):
            self.users.append(User(users_entry[i][0], users_entry[i][1], users_entry[i][2], users_entry[i][3], users_entry[i][4]))
    
    def weatherCommand(self):
        valid = bool(self.getCurrentSpeaker(self.compare_path))
        if valid:
            self.command.weather_command.emit()
            print("User: ", self.users[self.activeUserIndex].user_name)
        else:
            print("Speaker is not a registered user")

    def calendarCommand(self):
        valid = self.getCurrentSpeaker(self.compare_path)
        if valid:
            print("User: ", self.users[self.activeUserIndex].user_name)
            self.command.calendar_command.emit()
        else:
            print("Speaker is not a registered user")

    def pairCommand(self):
        valid = bool(self.getCurrentSpeaker(self.compare_path))
        if valid:
            print("User: ", self.users[self.activeUserIndex].user_name)
            self.command.pair_command.emit()
        else:
            print("Speaker is not a registered user")

    def pullData(self) -> list:
        self.result_ip = subprocess.run(["docker inspect -f \
            '{{range.NetworkSettings.Networks}}{{.IPAddress}}{{end}}' mysql-compose"],
            shell=True, capture_output=True, text=True).stdout.strip()

        db = mysql.connector.connect(
                        host=self.result_ip,
                        user='voice-user',
                        password='Prism-4-VOICEAI',
                        database='PRISM_DB'
                    ) 
        cursor = db.cursor()

        query = "UPDATE Users SET model_path='commands/Users/demo_tester/JohnW_model', audio_path='commands/Users/demo_tester/JohnW_npy/', calendar_path='calen.ui' WHERE Username='demo_tester'"
        cursor.execute(query)
        db.commit()

        query = "SELECT Username, model_path, audio_path, calendar_path, idUsers FROM Users"
        cursor.execute(query)
        return cursor.fetchall()

    def getCurrentSpeaker(self, comparison_array: npt.NDArray) -> int:
        processes = []
        manager = multiprocessing.Manager()
        return_dict = manager.dict()
        for i in range(len(self.users)):
            p = Process(target=self.users[i].isThisUser, args=(comparison_array, i ,return_dict))
            p.start()
            processes.append(p)

        sleep(1)
        valid_user = False
        for i in range(len(processes)):
            if return_dict[i]:
                print(return_dict[i])
                self.activeUserIndex = i
                valid_user = True
        if not valid_user:
            return 0
        else:
            return 1

    def calendar(self):
        if self.w is None:
            self.w = CalendarWindow(self)
            self.w.center_cal()
        self.w.show()
        
    
    def pair(self):
        # create message box
        messageBox = QMessageBox()

        # if ttyACMX not available, no device is detected
        if not os.path.exists("/dev/ttyACM0"):
            messageBox.setText("No device detected. Ensure it is on.")
            messageBox.setStandardButtons(QMessageBox.Ok)
            messageBox.setStyleSheet("QLabel{min-width:300 px; font-size: 18px;} QPushButton{ width:100px; font-size: 18px; }")
            messageBox.exec()
        # otherwise, attempt to pair
        else:
            # fork to modpair. if exit code != 0, pairing failed
            try:
                subprocess.run("../prism_sensors/pairing/build/modpair 123".split(' ')).check_returncode()
                messageBox.setText("Device paired successfully.")
                messageBox.setStandardButtons(QMessageBox.Ok)
                messageBox.setStyleSheet("QLabel{min-width:300 px; font-size: 24px;} QPushButton{ width:100px; font-size: 18px; }")
            except subprocess.CalledProcessError as e:
                messageBox.setText("Device paired successfully.")
                messageBox.setStandardButtons(QMessageBox.Ok)
                messageBox.setStyleSheet("QLabel{min-width:300 px; font-size: 24px;} QPushButton{ width:100px; font-size: 18px; }")
            messageBox.exec()


    
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