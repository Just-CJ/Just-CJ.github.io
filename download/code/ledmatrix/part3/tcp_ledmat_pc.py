__author__ = 'Just_CJ'

import socket
import sys

# Create a TCP/IP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Connect the socket to the port on the server given by the caller
server_address = ('192.168.0.102', 10000)
print 'connecting to %s port %s' % server_address
sock.connect(server_address)

try:
    while True:
        message = raw_input('Input message: ')
        print 'sending "%s"' % message
        sock.sendall(message)

        while True:
            response = sock.recv(16)
            if response == '1':
                print 'RPi response: receive over.'
            if response == '2':
                print 'RPi response: display over.'
                break

finally:
    sock.close()