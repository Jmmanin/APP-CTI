#Python client for talking to the 
import socket
import sys

class DPS_interface():
    
    DOCK_PORT = None
    PACKET_SIZE = None
    MAX_SAMPLE_RATE = None
    MAX_STREAM_LENGTH = None
    sock = None
    
    def __init__(self):
        self.DOCK_PORT = 4648
        self.PACKET_SIZE = 2 # the 2 byte demo packet
        self.MAX_SAMPLE_RATE = 60
        self.MAX_STREAM_LENGTH = 300 * self.MAX_SAMPLE_RATE * self.PACKET_SIZE
        # initialize the socket on creation
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        server_address = ('localhost', self.DOCK_PORT)
        self.sock.connect(server_address)
        self.sock.settimeout(None)

    # use 2 for live packet acceptance, 1 for proc stream protocol
    def select_mode(self, mode):
        # send mode and wait for ack or nack
        try:
            mode = str(mode)
        except ValueError:
            print("ValueError Raised on input type. Must a string of at least size 1 char")
            return 3

        self.sock.send(mode.encode()) # sends single int mode

        resp = self.sock.recv(1)
        resp = resp.decode('ascii')
        if resp == 'a':
            print("Mode Recieved and Accepted")
            return 0
        elif resp == 'n':
            print("Mode Recieved but mode is rejected by server.")
            return 1
        else:
            print("Mode Unrecieved or response unrecognized. Resp: '" + resp + "'")
            return 2

    def get_live_packet(self):
        self.release_mode('n')

        in_pkt = bytearray()

        self.sock.settimeout(0.5)  # need a timeout in case packets are streaming in
        try:   
            in_pkt = self.sock.recv(self.PACKET_SIZE)
        except socket.timeout:
            print("packet not deliver in time frame.")
            return (None, 1)
        self.sock.settimeout(None) # need to reset to the blocking nature of the interface

        if len(in_pkt) < self.PACKET_SIZE:
            return (in_pkt, 1)

        return (in_pkt, 0)

    def get_recorded_run(self, stream_id):  #sends id for target and gets back the processed buffer as bytes array.
        self.release_mode('n')
        try:
            stream_id = int(stream_id)
        except ValueError:
            return (None, 2)

        self.sock.sendall(str(stream_id).encode())
        ack = self.sock.recv(1)
        ack = ack.decode('ascii')
        if ack != 'a':
            return (None, 3) # something wierd happened on the ack
    
        in_size = self.sock.recv(sys.getsizeof(int()))  # recieve stream size, jump out if nothing there
        if int(in_size) == 0:
            return (None, 1)

        in_stream = self.sock.recv(int(in_size))        # recieve the proper size of the stream
        return (in_stream, 0)

    def release_mode(self, rst_msg='r'):    #call with 'r' or no argument to trigger the mode release, call with any other argument to hold mode
        self.sock.sendall(rst_msg.encode())

