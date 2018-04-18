# To support both python 2 and python 3
from __future__ import division, print_function, unicode_literals

# Common imports
import numpy as np
import os
import csv
from datetime import datetime

import tensorflow as tf

X_data = np.genfromtxt('data/X_train.csv', delimiter=',')
y_data = np.genfromtxt('data/y_train.csv', delimiter=',')

state = np.random.get_state()
np.random.shuffle(X_data)
np.random.set_state(state)
np.random.shuffle(y_data)

X_train, X_valid, X_test = np.split(X_data, [int(.6 * len(X_data)), int(.8 * len(X_data))])
y_train, y_valid, y_test = np.split(y_data, [int(.6 * len(y_data)), int(.8 * len(y_data))])

# 4 fingers
# 5 palm
# 3 orientation
# 1 temp (We don't care about this right now...)
# = 12 inputs
n_inputs = 12
n_hidden1 = 20
n_outputs = 2
learning_rate = 0.001
n_epochs = 500
# let's keep batch size at 1 for simplicity

X = tf.placeholder(tf.float32, shape=(None, n_inputs), name="X")
y = tf.placeholder(tf.int64, shape=(None), name="y")

with tf.name_scope("dnn"):
    hidden1 = tf.layers.dense(X, n_hidden1, name="hidden1", activation=tf.nn.relu)
    logits = tf.layers.dense(hidden1, n_outputs, name="outputs")

with tf.name_scope("loss"):
    xentropy = tf.nn.sparse_softmax_cross_entropy_with_logits(labels=y, logits=logits)
    loss = tf.reduce_mean(xentropy, name="loss")
    loss_summary = tf.summary.scalar('log_loss', loss)

with tf.name_scope("train"):
    optimizer = tf.train.GradientDescentOptimizer(learning_rate)
    training_op = optimizer.minimize(loss)

with tf.name_scope("eval"):
    correct = tf.nn.in_top_k(logits, y, 1)
    accuracy = tf.reduce_mean(tf.cast(correct, tf.float32))
    accuracy_summary = tf.summary.scalar('accuracy', accuracy)

init = tf.global_variables_initializer()
saver = tf.train.Saver()

def log_dir(prefix=""):
    now = datetime.utcnow().strftime("%Y%m%d%H%M%S")
    root_logdir = "tf_logs"
    if prefix:
        prefix += "-"
    name = prefix + "run-" + now
    return "{}/{}/".format(root_logdir, name)

logdir = log_dir("app_cti")

file_writer = tf.summary.FileWriter(logdir, tf.get_default_graph())

m, n = X_train.shape

checkpoint_path = "./.tmp/app_cti_model.ckpt"
checkpoint_epoch_path = checkpoint_path + ".epoch"
final_model_path = "./app_cti_model"

best_loss = np.infty
#
# with tf.Session() as sess:
#     if os.path.isfile(checkpoint_epoch_path):
#         # if the checkpoint file exists, restore the model and load the epoch number
#         with open(checkpoint_epoch_path, "rb") as f:
#             start_epoch = int(f.read())
#         print("Training was interrupted. Continuing at epoch", start_epoch)
#         saver.restore(sess, checkpoint_path)
#     else:
#         start_epoch = 0
#         sess.run(init)
#
#     for epoch in range(start_epoch, n_epochs):
#         for iteration in range(m):
#             X_batch = X_train[iteration].reshape(1, n_inputs)
#             y_batch = y_train[iteration].reshape(1)
#             sess.run(training_op, feed_dict={X: X_batch, y: y_batch})
#         accuracy_val, loss_val, accuracy_summary_str, loss_summary_str = sess.run([accuracy, loss, accuracy_summary, loss_summary], feed_dict={X: X_valid, y: y_valid})
#         file_writer.add_summary(accuracy_summary_str, epoch)
#         file_writer.add_summary(loss_summary_str, epoch)
#         if epoch % 5 == 0:
#             print("Epoch:", epoch,
#                   "\tValidation accuracy: {:.3f}%".format(accuracy_val * 100),
#                   "\tLoss: {:.5f}".format(loss_val))
#             saver.save(sess, checkpoint_path)
#             with open(checkpoint_epoch_path, "wb") as f:
#                 f.write(b"%d" % (epoch + 1))
#             if loss_val < best_loss:
#                 saver.save(sess, final_model_path)
#                 best_loss = loss_val
#
# os.remove(checkpoint_epoch_path)

with tf.Session() as sess:
    saver.restore(sess, final_model_path)
    accuracy_val = accuracy.eval(feed_dict={X: X_test, y: y_test})
    results = sess.run(tf.argmax(logits, 1), feed_dict={X: X_test})
    print(results)

print(accuracy_val)
