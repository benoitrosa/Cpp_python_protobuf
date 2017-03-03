#!/usr/bin/env python

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
		
	


