from flask_socketio import SocketIO, emit
from flask import Flask
from random import random

app = Flask(__name__)
socketio = SocketIO(app)
thread = None

def background_thread():
    while True:
        socketio.sleep(0.1)
        f1 = round(random(), 3)
        f2 = round(random(), 3)
        f3 = round(random(), 3)
        f4 = round(random(), 3)
        print('{}, {}, {}, {}'.format(f1, f2, f3, f4))
        socketio.emit('digit', { 'f1': f1, 'f2': f2, 'f3': f3, 'f4': f4 })

@socketio.on('connect')
def test_connect():
    print('Connected')
    global thread
    if thread is None:
        thread = socketio.start_background_task(target=background_thread)

if __name__ == '__main__':
    socketio.run(app)
