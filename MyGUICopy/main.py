import sys
import sqlite3

from weather import MainWindow as WthrMain

#from PyQt5.QtWidgets import QLabel, QMainWindow, QWidget, QApplication, QListWidgetItem, QMessageBox, QPushButton
from PyQt5.QtWidgets import *
from PyQt5.uic import loadUi
from PyQt5 import QtCore

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

        db = sqlite3.connect("data.db")
        cursor = db.cursor()

        query = "SELECT task, completed FROM tasks WHERE date = ?"
        row = (date,)
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
       
        db = sqlite3.connect("data.db")
        cursor = db.cursor()
        date = self.calendarWidget.selectedDate().toPyDate()

        for i in range(self.tasksListWidget.count()):
            item = self.tasksListWidget.item(i)
            task = item.text()
            if item.checkState() == QtCore.Qt.Checked:
                query = "UPDATE tasks SET completed = 'YES' WHERE task = ? AND date = ?"
            else:
                query = "UPDATE tasks SET completed = 'NO' WHERE task = ? AND date = ?"
            row = (task, date,)
            cursor.execute(query, row)
        db.commit()

        messageBox = QMessageBox()
        messageBox.setText("Changes saved.")
        messageBox.setStandardButtons(QMessageBox.Ok)
        messageBox.setStyleSheet("QLabel{min-width:300 px; font-size: 24px;} QWidget{background-color:#0000FF; color: black} QPushButton{ width:100px; font-size: 18px; }")
        messageBox.exec()

    def addNewTask(self):
        
        db = sqlite3.connect("data.db")
        cursor = db.cursor()

        newTask = str(self.taskLineEdit.text())
        newTaskStrip = newTask.strip()
        if newTask == "":
            messageBox = QMessageBox()
            messageBox.setText("Must contain text.")
            messageBox.setStandardButtons(QMessageBox.Ok)
            messageBox.setStyleSheet("QLabel{min-width:300 px; font-size: 24px;} QWidget{background-color:#0000FF; color: black} QPushButton{ width:100px; font-size: 18px; }")
            messageBox.exec()
            return
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
        self.move(qtRectangle.topLeft())

if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = MainWindow()
    window.center()
    window.show()
    sys.exit(app.exec())