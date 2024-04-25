import sys
import sqlite3
import requests
import json

#dataUrl = "http://localhost:40553/php/demo.php"
dataUrl = "http://172.20.10.6:40553/php/gui.php"

from weather import MainWindow as WthrMain

from PyQt5.QtWidgets import *
from PyQt5.uic import loadUi
from PyQt5 import QtCore, QtGui

#conn = sqlite3.connect("data.db")
#c = conn.cursor()
#c.execute("CREATE TABLE tasks(task TEXT, completed TEXT, date TEXT)")
#c.execute("DROP TABLE tasks")

class MainWindow(QMainWindow):
    
    def __init__(self):
        super().__init__()
        loadUi("main.ui", self)
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

class CalendarWindow(QWidget):
    def __init__(self):
        super(CalendarWindow, self).__init__()
        loadUi("calen.ui", self)
        self.todaysDate = self.calendarWidget.selectedDate().toPyDate()
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

        data = {'key1': date}
        capt = requests.post(dataUrl, data=data)
        results = capt.json()
        print(results)
        
        #db = sqlite3.connect("data.db")
        #cursor = db.cursor()

        #query = "SELECT task, completed FROM tasks WHERE date = ?"
        #row = (date,)
        #results = cursor.execute(query, row).fetchall()
        
        for result in results:
            item = QListWidgetItem(str(result[0]))
            item.setFlags(item.flags() | QtCore.Qt.ItemIsUserCheckable)
            if result[1] == "YES":
                item.setCheckState(QtCore.Qt.Checked)
            elif result[1] == "NO":
                item.setCheckState(QtCore.Qt.Unchecked)
            self.tasksListWidget.addItem(item)

    def saveChanges(self):
       
        #db = sqlite3.connect("data.db")
        #cursor = db.cursor()
        date = str(self.calendarWidget.selectedDate().toPyDate())
        print(date)


        for i in range(self.tasksListWidget.count()):
            item = self.tasksListWidget.item(i)
            task = item.text()
            print(task)
                        
            if item.checkState() == QtCore.Qt.Checked:
                yesorno = 'YES'
                print('YES')
            else:
                yesorno = 'NO'
                print('NO')
            
            data = {'key2': date, 'key3':task, 'key4': yesorno}
            print(data)
            x = requests.post(dataUrl, data=data)
            print(x.json())

        messageBox = QMessageBox()
        messageBox.setText("Changes saved.")
        messageBox.setStandardButtons(QMessageBox.Ok)
        messageBox.setStyleSheet("QLabel{min-width:300 px; font-size: 24px;} QWidget{background-color:#0000FF; color: black} QPushButton{ width:100px; font-size: 18px; }")
        messageBox.exec()

    def addNewTask(self, vct):
        
        db = sqlite3.connect("data.db")
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

            query = "INSERT INTO tasks(task, completed, date) VALUES (?,?,?)"
            row = (voiceTask, "NO", date,)

            cursor.execute(query, row)
            db.commit()
            self.updateTaskList(date)
            self.taskLineEdit.clear()
        else:        
            date = self.calendarWidget.selectedDate().toPyDate()

            query = "INSERT INTO tasks(task, completed, date) VALUES (?,?,?)"
            row = (newTask, "NO", date,)

            cursor.execute(query, row)
            db.commit()
            self.updateTaskList(date)
            self.taskLineEdit.clear()

    def center_cal(self):
        qtRectangle = self.frameGeometry()
        centerPoint = QDesktopWidget().availableGeometry().center()
        qtRectangle.moveCenter(centerPoint)
        self.move(QApplication.desktop().screen().rect().center()- self.rect().center())
        
if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = MainWindow()
    window.center()
    window.show()
    sys.exit(app.exec())