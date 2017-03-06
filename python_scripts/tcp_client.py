#!/usr/bin/env python

'''
This is a small TCP client that handles a protobuf communication example. 

The client will try to establish a connection with a TCP server on localhost:3000

When a connection is established, the client will: 

	- Send a request to the server
	- Wait for a reply with a protobuf message (dummy XYZ positions of an object)
	- Print x,y,and z (formatted as a numpy array) in the standard output

The client will loop until either CTRL+C is pressed (in which case it will cleanly close the socket and exit), or until the client request includes an endconnection=True message
'''


import socket
import struct
import signal
import time

from kinMsg_pb2 import request,response

import numpy as np



class tcp_client: 

	def __init__(self):
		# intialize messages
		self.message = request()
		self.message.endconnection = False
		self.data_m = response()

		# signal handler for ctrl+c
		signal.signal(signal.SIGINT, self.signal_handler)

		# initialize connection, and run the main program
		self.initConnection()


	def initConnection(self):
		TCP_IP = ''
		TCP_PORT = 3000
		BUFFER_SIZE = 1024

		self.client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		self.client_socket.connect((TCP_IP, TCP_PORT))
		self.connection = True

		print "Connection established, starting main loop"

		self.run()


	def signal_handler(self, signal, frame):

		print "SIGINT caught, exiting ..."
		if self.connection:
			self.connection = False

	def run(self):
		while (self.connection):
	
			# send request
			self.message.endconnection = False
			s = self.message.SerializeToString()
			totallen = 4 + self.message.ByteSize()
			self.client_socket.sendall(str(totallen).zfill(4) + s)

			# get response from server with robot data
			data_hdr = self.client_socket.recv(4)
			sz = int(data_hdr)
			data = self.client_socket.recv(sz)
			self.data_m.ParseFromString(data)

			self.connection = not self.data_m.endconnection

			numpoints = self.data_m.shape.numpoints
			P = np.array(self.data_m.shape.P._values)
			x = np.array([el.x for el in P])
			y = np.array([el.y for el in P])
			z = np.array([el.z for el in P])
			print x
			print y
			print z

		print "Closing socket ..."
		self.client_socket.close()


if __name__ == '__main__':
	client = tcp_client()
		
	


