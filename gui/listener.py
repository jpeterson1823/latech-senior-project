from http.server import BaseHTTPRequestHandler, HTTPServer
from http.client import HTTPMessage
import threading
global gSensorQueue, gListenServer, gServerActive

class RequestHandler(BaseHTTPRequestHandler):
    def _set_response(self):
        self.send_response(200)
        self.send_header('Content-type', 'text/plain')
        self.end_headers()

    def do_POST(self):
        global gSensorQueue
        # parse data packet
        content_length = int(self.headers['Content-Length']) # <--- Gets the size of data
        post_data = self.rfile.read(content_length) # <--- Gets the data itself
        # push info to queue
        gSensorQueue.append((self.headers, post_data))

def getAlertQueue() -> list[tuple[HTTPMessage, str]]:
    global gSensorQueue
    return gSensorQueue
def alertQueueEmpty() -> bool:
    global gSensorQueue
    return len(gSensorQueue) == 0
def peekAlertQueue() -> tuple[HTTPMessage, str] | None:
    global gSensorQueue
    if not alertQueueEmpty():
        return gSensorQueue[0]
    return None
def popAlertQueue() -> tuple[HTTPMessage, str] | None:
    global gSensorQueue
    if not alertQueueEmpty():
        return gSensorQueue.pop(0)
    return None
def pushAlertQueue(val: tuple[HTTPMessage, str]):
    global gSensorQueue
    gSensorQueue.append(val)

def _listen_thread_entry():
    global gListenServer, gServerActive
    try:
        gServerActive = True
        gListenServer.serve_forever()
    except KeyboardInterrupt:
        pass
    gListenServer.server_close()

def _listen_thread_queue_handler():
    global gSensorQueue, gServerActive
    while gServerActive:
        if not alertQueueEmpty():
            print(popAlertQueue())

def _init():
    global gSensorQueue, gListenServer, gServerActive, gServerPort
    gSensorQueue = []
    gServerActive = False
    gServerPort = 23120
    gListenServer = HTTPServer(('', gServerPort), RequestHandler)

def openListeningServer() -> list[threading.Thread]:
    global gSensorQueue, gListenServer, gServerActive, gServerPort
    _init()
    gSensorQueue = []

    tlist = []
    tlist.append(threading.Thread(target=_listen_thread_entry))
    tlist.append(threading.Thread(target=_listen_thread_queue_handler))
    tlist[0].start()
    tlist[1].start()
    return tlist

def closeListeningServer():
    global gListenServer
    gListenServer.shutdown()

if __name__ == "__main__":
    _listen_thread_entry()