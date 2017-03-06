#!/usr/bin/env python

import socket
import struct
import signal
import time

from kinMsg_pb2 import request,response, position3D

import numpy as np



class tcp_server: 

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

		self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		self.socket.bind((TCP_IP, TCP_PORT))
		self.socket.listen(1)
		self.conn, self.addr = self.socket.accept()
		
		self.connection = True
		print "Connection established, starting main loop"

		self.run()


	def signal_handler(self, signal, frame):

		print "SIGINT caught, exiting ..."
		if self.connection:
			self.connection = False

	def run(self):
		while (self.connection):
	
			# Listen for client request with 4 characters header
			data_hdr = self.conn.recv(4)

			if not data_hdr:
				break
			sz = int(data_hdr)

			data = self.conn.recv(sz)
			self.message.ParseFromString(data)
			self.connection = not self.message.endconnection

			# Craft a response and send it
			self.data_m.endconnection = False
			numEl=10
			self.data_m.shape.numpoints = numEl

			

			for i in range(numEl):
				P = position3D()
				P.x = 0.2*i
				P.y = 0.1*i
				P.z = 0.35*i
				self.data_m.shape.P.extend([P])

			s = self.data_m.SerializeToString()
			totallen = 4 + self.data_m.ByteSize()
			self.conn.sendall(str(totallen).zfill(4) + s)

		print "Closing socket ..."
		self.conn.close()


if __name__ == '__main__':
	client = tcp_server()
		
	


