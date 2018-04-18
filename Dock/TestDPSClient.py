from DPSComm import DPS_interface

from flask_socketio import SocketIO, emit
from flask import Flask
from random import random, randint

app = Flask(__name__)
socketio = SocketIO(app)
thread = None

def background_thread():
    try:
        dps = DPS_interface()
    except ConnectionRefusedError:
        print("Failed to link to server. Shutting down.")
        return
    dps.select_mode('l')

    while True:
        socketio.sleep(0.04)
        (pkt, resp) = dps.get_live_packet()
        if resp != 1:
            (fuck_list, json) = dps.packet_reconstruct(pkt)
            print(json)
            payload = {
              'data': json
            }
            print('emitting packets')
            socketio.emit('digit', payload)

        # f1 = round(random(), 6)
        # f2 = round(random(), 6)
        # f3 = round(random(), 6)
        # f4 = round(random(), 6)
        #
        # p1 = round(random(), 6)
        # p2 = round(random(), 6)
        # p3 = round(random(), 6)
        # p4 = round(random(), 6)
        # p5 = round(random(), 6)
        #
        # o1 = randint(-90, 45)
        # o2 = randint(-90, 0)
        # o3 = randint(-20, 20)
        #
        # t = randint(30, 32)
        #
        # payload = {
        #   'data': json
        # }
        #
        # socketio.emit('digit', payload)

@socketio.on('connect')
def test_connect():
    print('Connected')
    global thread
    if thread is None:
        thread = socketio.start_background_task(target=background_thread)

if __name__ == '__main__':
    socketio.run(app)
