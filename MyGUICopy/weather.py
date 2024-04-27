from PyQt5.QtGui import *
from PyQt5.QtWidgets import *
from PyQt5.QtCore import *

from MainWindow import Ui_MainWindow

from datetime import datetime
import subprocess
import json
import os
import sys
import requests
from urllib.parse import urlencode

OPENWEATHERMAP_API_KEY = 'ASK_FOR_API'

def from_ts_to_time_of_day(ts):
    dt = datetime.fromtimestamp(ts)
    return dt.strftime("%I%p").lstrip("0")

class WorkerSignals(QObject):
    
    finished = pyqtSignal()
    error = pyqtSignal(str)
    result = pyqtSignal(dict, dict)

class WeatherWorker(QRunnable):
    
    signals = WorkerSignals()
    is_interrupted = False

    def __init__(self, location):
        super(WeatherWorker, self).__init__()
        self.location = location # string. Passed from WeatherWorker(variable)

    @pyqtSlot()
    def run(self):
        try:
            params = dict(
                q=self.location,
                appid=OPENWEATHERMAP_API_KEY
            )

            url = 'http://api.openweathermap.org/data/2.5/weather?%s&units=metric' % urlencode(params)
            r = requests.get(url)
            weather = json.loads(r.text)

            if weather['cod'] != 200:
                raise Exception(weather['message'])

            url = 'http://api.openweathermap.org/data/2.5/forecast?%s&units=metric' % urlencode(params)
            r = requests.get(url)
            forecast = json.loads(r.text)

            self.signals.result.emit(weather, forecast)

        except Exception as e:
            self.signals.error.emit(str(e))

        self.signals.finished.emit()

class MainWindow(QMainWindow, Ui_MainWindow):

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.setupUi(self)

        self.pushButton.clicked.connect(self.update_weather)

        self.threadpool = QThreadPool()

        self.show()
        
        #self.update_weather(voicePlace) # Forces forecast to auto appear instead of having to click button

    def alert(self, message):
        alert = QMessageBox.warning(self, "Warning", message)

    def update_weather(self, place):
        if place is False:
            worker = WeatherWorker(self.lineEdit.text())
        else:
            worker = WeatherWorker(place)
        worker.signals.result.connect(self.weather_result)
        worker.signals.error.connect(self.alert)
        self.threadpool.start(worker)

    def weather_result(self, weather, forecasts):
        self.latitudeLabel.setText("%.2f °" % weather['coord']['lat'])
        self.longitudeLabel.setText("%.2f °" % weather['coord']['lon'])

        self.wi = round((weather['wind']['speed']) * 2.23694,1)

        self.windLabel.setText(f'{self.wi} mph')
        
        self.Ftemp = round((((weather['main']['temp'])*1.8) + 32.0), 1)
        self.psiPress = round((weather['main']['pressure']/10), 1)
        self.hum = weather['main']['humidity']

        self.temperatureLabel.setText(f'{self.Ftemp} °F')
        self.pressureLabel.setText(f'{self.psiPress} kPa')
        self.humidityLabel.setText(f'{self.hum}%')

        self.sunriseLabel.setText(from_ts_to_time_of_day(weather['sys']['sunrise']))

        self.weatherLabel.setText("%s (%s)" % (
            weather['weather'][0]['main'],
            weather['weather'][0]['description']
        )
                                  )

        self.set_weather_icon(self.weatherIcon, weather['weather'])

        for n, forecast in enumerate(forecasts['list'][:5], 1):
            getattr(self, 'forecastTime%d' % n).setText(from_ts_to_time_of_day(forecast['dt']))
            self.set_weather_icon(getattr(self, 'forecastIcon%d' % n), forecast['weather'])
            
            self.ForTemp = round((((forecast['main']['temp'])*1.8) + 32.0), 1)
            
            getattr(self, 'forecastTemp%d' % n).setText(f'{self.ForTemp} °F')

    def set_weather_icon(self, label, weather):
        label.setPixmap(
            QPixmap(os.path.join('images', "%s.png" %
                                 weather[0]['icon']
                                 )
                    )

        )
        
    def center_screen(self):
        qtRectangle = self.frameGeometry()
        centerPoint = QDesktopWidget().availableGeometry().topLeft()
        qtRectangle.moveCenter(centerPoint)
        self.move(qtRectangle.center())
        
    def focusInEvent(self):
        try:
            subprocess.Popen(["matchbox-keyboard"])
            loc = str(self.lineEdit.text())
            print(f'Loc is {loc}') 
            return loc
        except FileNotFoundError:
            pass

    def focusOutEvent(self):
        subprocess.Popen(["killall","matchbox-keyboard"])
        

if __name__ == '__main__':
    app = QApplication(sys.argv)
    window = MainWindow()
    window.center_screen()
    app.exec_()
