import mysql.connector
from PyQt5.QtWidgets import *
from PyQt5.uic import loadUi
from PyQt5 import QtCore
import subprocess
from res.User import User

class CalendarWindow(QWidget):
    def __init__(self):
        super(CalendarWindow, self).__init__()
        self.result_ip = subprocess.run(["docker inspect -f \
            '{{range.NetworkSettings.Networks}}{{.IPAddress}}{{end}}' mysql-compose"],
            shell=True, capture_output=True, text=True).stdout.strip()
        loadUi("res/ui/calen.ui", self)
        self.todaysDate = self.calendarWidget.selectedDate().toPyDate()
        self.users = [User()]
        self.activeUserIndex = 0
        print("Today is:", self.todaysDate)
        self.calendarWidget.selectionChanged.connect(self.calendarDateChanged)
        self.calendarDateChanged()
        self.saveButton.clicked.connect(self.saveChanges)
        self.addButton.clicked.connect(self.addNewTask)
        self.todayButton.clicked.connect(self.today)
        
    def today(self):
        self.calendarWidget.setSelectedDate(self.todaysDate)

    def calendarDateChanged(self):
        print("The calendar date was changed.")
        dateSelected = self.calendarWidget.selectedDate().toPyDate()
        print("Date selected:", dateSelected)
        self.updateTaskList(dateSelected)

    def updateTaskList(self, date):
        self.tasksListWidget.clear()

        db = mysql.connector.connect(
                host=self.result_ip,
                user='gui-user',
                password='Prism-4-GUI',
                database='PRISM_DB'
            )
        cursor = db.cursor()

        query = "SELECT task, completed FROM GUI_Data WHERE date = %s AND Users_idUsers = %s"
        row = (date, self.users[self.activeUserIndex].user_ID)
        results = cursor.execute(query, row).fetchall()
        for result in results:
            item = QListWidgetItem(str(result[0]))
            item.setFlags(item.flags() | QtCore.Qt.ItemIsUserCheckable)
            if result[1] == "YES":
                item.setCheckState(QtCore.Qt.Checked)
            elif result[1] == "NO":
                item.setCheckState(QtCore.Qt.Unchecked)
            self.tasksListWidget.addItem(item)

    def saveChanges(self):
        db = mysql.connector.connect(
                host=self.result_ip,
                user='gui-user',
                password='Prism-4-GUI',
                database='PRISM_DB'
            )
        cursor = db.cursor()
        date = self.calendarWidget.selectedDate().toPyDate()

        for i in range(self.tasksListWidget.count()):
            item = self.tasksListWidget.item(i)
            task = item.text()
            if item.checkState() == QtCore.Qt.Checked:
                query = "UPDATE GUI_Data SET completed = 'YES' WHERE task = %s AND date = %s AND Users_idUsers = %s"
            else:
                query = "UPDATE GUI_Data SET completed = 'NO' WHERE task = %s AND date = %s AND Users_idUsers = %s"
            row = (task, date, self.users[self.activeUserIndex].user_ID)
            cursor.execute(query, row)
        db.commit()

        messageBox = QMessageBox()
        messageBox.setText("Changes saved.")
        messageBox.setStandardButtons(QMessageBox.Ok)
        messageBox.setStyleSheet("QLabel{min-width:300 px; font-size: 24px;} QWidget{background-color:#0000FF; color: black} QPushButton{ width:100px; font-size: 18px; }")
        messageBox.exec()

    def addNewTask(self, vct, user):
        
        db = mysql.connector.connect(
                host=self.result_ip,
                user='gui-user',
                password='Prism-4-GUI',
                database='PRISM_DB'
            )
        cursor = db.cursor()

        newTask = str(self.taskLineEdit.text())
        voiceTask = vct
        newTaskStrip = newTask.strip()
        if newTask == "" and voiceTask is False:
            messageBox = QMessageBox()
            messageBox.setText("Must contain text.")
            messageBox.setStandardButtons(QMessageBox.Ok)
            messageBox.setStyleSheet("QLabel{min-width:300 px; font-size: 24px;} QWidget{background-color:#0000FF; color: black} QPushButton{ width:100px; font-size: 18px; }")
            messageBox.exec()
            return
        elif newTask == "" and voiceTask is not False:
            date = self.calendarWidget.selectedDate().toPyDate()

            query = "INSERT INTO GUI_Data (task, completed, date, Users_idUsers) VALUES (%s,%s,%s,%s)"
            row = (voiceTask, "NO", date, self.users[self.activeUserIndex].user_ID)

            cursor.execute(query, row)
            db.commit()
            self.updateTaskList(date)
            self.taskLineEdit.clear()
        else:        
            date = self.calendarWidget.selectedDate().toPyDate()

            query = "INSERT INTO GUI_Data (task, completed, date, Users_idUsers) VALUES (%s,%s,%s,%s)"
            row = (newTask, "NO", date, self.users[self.activeUserIndex].user_ID)

            cursor.execute(query, row)
            db.commit()
            self.updateTaskList(date)
            self.taskLineEdit.clear()

    def center_cal(self):
        qtRectangle = self.frameGeometry()
        centerPoint = QDesktopWidget().availableGeometry().center()
        qtRectangle.moveCenter(centerPoint)
        self.move(QApplication.desktop().screen().rect().center()- self.rect().center())
