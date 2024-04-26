import sys

# clay's imports
import mysql.connector
from subprocess import run

from weather import MainWindow as WthrMain

from PyQt5.QtWidgets import *
from PyQt5.uic import loadUi
from PyQt5 import QtCore, QtGui

result_ip = run(["docker inspect -f \
    '{{range.NetworkSettings.Networks}}{{.IPAddress}}{{end}}' mysql-compose"],
    shell=True, capture_output=True, text=True).stdout.strip()

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

        db = mysql.connector.connect(
                host=result_ip,
                user='gui-user',
                password='Prism-4-GUI',
                database='PRISM_DB'
            )
        cursor = db.cursor()

        query = "SELECT task, completed FROM GUI_Data WHERE date = %s"
        row = (date,)
        cursor.execute(query, row)
        results = cursor.fetchall()
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
                host=result_ip,
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
                query = "UPDATE GUI_Data SET completed = 'YES' WHERE task = %s AND date = %s"
            else:
                query = "UPDATE GUI_Data SET completed = 'NO' WHERE task = %s AND date = %s"
            row = (task, date,)
            cursor.execute(query, row)
        db.commit()

        messageBox = QMessageBox()
        messageBox.setText("Changes saved.")
        messageBox.setStandardButtons(QMessageBox.Ok)
        messageBox.setStyleSheet("QLabel{min-width:300 px; font-size: 24px;} QWidget{background-color:#0000FF; color: black} QPushButton{ width:100px; font-size: 18px; }")
        messageBox.exec()

    def addNewTask(self, vct):
        
        db = mysql.connector.connect(
                host=result_ip,
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

            query = "INSERT INTO GUI_Data (task, completed, date, Users_idUsers) VALUES (%s,%s,%s,1)"
            row = (voiceTask, "NO", date,)

            cursor.execute(query, row)
            db.commit()
            self.updateTaskList(date)
            self.taskLineEdit.clear()
        else:        
            date = self.calendarWidget.selectedDate().toPyDate()

            query = "INSERT INTO GUI_Data (task, completed, date, Users_idUsers) VALUES (%s,%s,%s,1)"
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
