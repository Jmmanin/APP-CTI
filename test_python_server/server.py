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


# 0                   1
# 1   1   1   1   1   1
# 0   26  49  57  65  88
#
#
#
# 0   0.16  0.33  0.50  0.66  0.83  1
# 0   0.16  0.33  0.50  0.66  0.83  1
#
#  0.0 * 88    0.2 * 88    0.4 * 88    0.6 * 88    0.8 * 88    1.0 * 88
#  0           26          49          57          65          88
#
#  0          0.29        0.55        0.64        0.73        1
# +0         +0.09       +0.15       +0.04       -0.07       +0
#
#  0          0.29        0.26        0.09        0.09        0.27
