from DPSComm import DPS_interface

from flask_socketio import SocketIO, emit
from flask import Flask
from random import random, randint

import numpy as np
import tensorflow as tf

class Model(object):
    def __init__(self):
        n_inputs = 12
        n_hidden1 = 20
        n_outputs = 2

        self.X = tf.placeholder(tf.float32, shape=(None, n_inputs), name="X")
        self.y = tf.placeholder(tf.int64, shape=(None), name="y")

        with tf.name_scope("dnn"):
            self.hidden1 = tf.layers.dense(self.X, n_hidden1, name="hidden1", activation=tf.nn.relu)
            self.logits = tf.layers.dense(self.hidden1, n_outputs, name="outputs")

        with tf.name_scope("loss"):
            self.xentropy = tf.nn.sparse_softmax_cross_entropy_with_logits(labels=self.y, logits=self.logits)
            self.loss = tf.reduce_mean(self.xentropy, name="loss")
            self.loss_summary = tf.summary.scalar('log_loss', self.loss)

        with tf.name_scope("eval"):
            self.correct = tf.nn.in_top_k(self.logits, self.y, 1)
            self.accuracy = tf.reduce_mean(tf.cast(self.correct, tf.float32))
            self.accuracy_summary = tf.summary.scalar('accuracy', self.accuracy)

        init = tf.global_variables_initializer()
        saver = tf.train.Saver()

        final_model_path = "./app_cti_model"
        self.sess = tf.Session()

        saver.restore(self.sess, final_model_path)


    def predict(self, X_test):
        results = self.sess.run(tf.argmax(self.logits, 1), feed_dict={self.X: X_test})
        return results




app = Flask(__name__)
socketio = SocketIO(app)
thread = None


def tense(np_list):
    results = model.predict(np.asarray([np_list]))
    print('emitting predictions')
    print(results)

    wear.calc_next_wear(results * 15)

    my_wear = wear.get_current_wear()

    my_wear = max(my_wear / 300, 1)

    socketio.emit('prediction', my_wear)


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
            (np_list, json) = dps.packet_reconstruct(pkt)
            print(json)
            payload = {
              'data': json
            }
            print('emitting packets')
            socketio.emit('digit', payload)

            thread = socketio.start_background_task(target=tense, np_list)


@socketio.on('connect')
def test_connect():
    print('Connected')
    global thread
    if thread is None:
        thread = socketio.start_background_task(target=background_thread)

if __name__ == '__main__':
    model = Model()
    wear = WearFunction(sampleRate = 25)
    socketio.run(app)
