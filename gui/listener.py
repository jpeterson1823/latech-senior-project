from http.server import BaseHTTPRequestHandler, HTTPServer
import threading
global gSensorAlert, gSensorAlertData

class RequestHandler(BaseHTTPRequestHandler):
    def _set_response(self):
        self.send_response(200)
        self.send_header('Content-type', 'text/plain')
        self.end_headers()

    def do_POST(self):
        global gSensorAlert, gSensorAlertData
        content_length = int(self.headers['Content-Length']) # <--- Gets the size of data
        post_data = self.rfile.read(content_length) # <--- Gets the data itself

        gSensorAlert = True
        gSensorAlertData = post_data

def _listen_thread_entry():
    gSensorAlert = False
    port = 23120
    http = HTTPServer(('', port), RequestHandler)

    try:
        http.serve_forever()
    except KeyboardInterrupt:
        pass
    http.server_close()

def OpenListeningServer():
    thread = threading.Thread(_listen_thread_entry, None)
    thread.start()
    return thread

if __name__ == "__main__":
    _listen_thread_entry()