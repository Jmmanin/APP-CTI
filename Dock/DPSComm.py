#Python client for talking to the 
import socket
import sys
import json

class DPS_interface():
    
    DOCK_PORT = None
    PACKET_SIZE = None
    MAX_SAMPLE_RATE = None
    MAX_STREAM_LENGTH = None
    sock = None
    
    '''
        Initialize a new connection. Leaves an open connection object in the class memory space
        - user must try: except: Any connection refused error
    '''
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


    '''
        Tells the server what to send or expect
        - use 'l' to set up a live packet connection
        - use 'p' to set up a pre-recorded run connection
    '''
    def select_mode(self, mode):
        # send mode and wait for ack or nack
        try:
            mode = str(mode)
        except ValueError:
            # print("ValueError Raised on input type. Must a string of at least size 1 char")
            return 3

        self.sock.send(mode.encode()) # sends single int mode

        resp = self.sock.recv(1)
        resp = resp.decode('ascii')
        if resp == 'a':
            # print("Mode Recieved and Accepted")
            return 0
        elif resp == 'n':
            # print("Mode Recieved but mode is rejected by server.")
            return 1
        else:
            # print("Mode Unrecieved or response unrecognized. Resp: '" + resp + "'")
            return 2

    '''
        If the mode is 'l', this gets the next available packet from the queue in the server
        -If none is available, the function returns (None, 1) after 0.5 seconds
    '''
    def get_live_packet(self):
        self.release_mode('n')

        in_pkt = bytearray()

        self.sock.settimeout(0.5)  # need a timeout in case packets are streaming in
        try:   
            in_pkt = self.sock.recv(self.PACKET_SIZE)
        except socket.timeout:
            # print("packet not deliver in time frame.")
            return (None, 1)
        self.sock.settimeout(None) # need to reset to the blocking nature of the interface

        if len(in_pkt) < self.PACKET_SIZE:
            return (in_pkt, 1)

        return (in_pkt, 0)

    '''
        If mode is in 'p', this gets the .prdat payload for the requested stream id
        - returns (None, 1) if nothing is available for the requested id
    '''

    def get_recorded_run(self, stream_id):  #sends id for target and gets back the processed buffer as bytes array.
        self.release_mode('n')
        try:
            stream_id = int(stream_id)
        except ValueError:
            return (None, 2)
        #print("sending id: " + str(stream_id))
        self.sock.sendall(str(stream_id).encode())
        ack = self.sock.recv(1) 
        ack = ack.decode('ascii')
        # print("recieved: "+ ack)
        if ack != 'a':
            return (None, 3) # something wierd happened on the ack
    
        in_size = self.sock.recv(16)  # recieve stream size, jump out if nothing there
        in_size = str(in_size.decode('ascii'))
        in_size = int(in_size.strip('\0'))
        # print("recieved packet size of: " + str(in_size))
        if int(in_size) == 0:
            return (None, 1)

        in_stream = self.sock.recv(int(in_size))        # recieve the proper size of the stream
        return (in_stream, 0)

    '''
        tells the server to switch context
        - 'r' (default) if r or no argument is presented, the server waits for a select_mode call
        - 'k' the server will release the connection and wait for a new DPS_Interface() call
    '''
    def release_mode(self, rst_msg='r'):    #call with 'r' or no argument to trigger the mode release, call with any other argument to hold mode
        # print("Length of release string: " + len(rst_msg))
        self.sock.sendall(rst_msg.encode())

    def packet_reconstruct(self, byte_pkt):

        output_flat = list()
        output_json = {}
        # angles
        output_flat.append(float(byte_pkt[0:4]))
        output_flat.append(float(byte_pkt[4:8]))
        output_flat.append(float(byte_pkt[8:12]))
        output_flat.append(float(byte_pkt[12:16]))

        # pressures
        output_flat.append(float(byte_pkt[16:20]))
        output_flat.append(float(byte_pkt[20:24]))
        output_flat.append(float(byte_pkt[24:28]))
        output_flat.append(float(byte_pkt[28:32]))
        output_flat.append(float(byte_pkt[32:36]))

        # orientations
        output_flat.append(float(byte_pkt[36:40]))
        output_flat.append(float(byte_pkt[40:44]))
        output_flat.append(float(byte_pkt[44:48]))

        # temp
        output_flat.append(int(byte_pkt[48:52]))

        # convert values to json
        output_json['finger-bend'] = output_flat[0:4]
        output_json['palm-pressure'] = output_flat[4:9]
        output_json['orientation'] = output_flat[9:12]
        output_json['temp'] = output_flat[12]

        return (output_flat, output_json)