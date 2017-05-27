__author__ = 'Just_CJ'

import socket
import led_mat
import time
import thread


def process_data(data):
    str = data+' '
    pre_mat = 8*[8*[0]]
    for i in range(len(str)):
        cur_mat = led_mat.get_font_mat(ord(str[i]))
        for j in range(8):
            for k in range(8):
                led_mat.mat[k] = pre_mat[k][j:8]+cur_mat[k][0:j] # shift
            time.sleep(0.1)
        pre_mat = cur_mat



# display thread
thread.start_new_thread(led_mat.display, ())

# Create a TCP/IP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Bind the socket to the address given on the command line
server_address = ('', 10000)
sock.bind(server_address)
print 'starting up on %s port %s' % sock.getsockname()
sock.listen(1)

while True:
    print 'waiting for a connection'
    connection, client_address = sock.accept()
    try:
        print 'client connected:', client_address
        while True:
            print 'waiting for a message'
            data = connection.recv(40)
            print 'received "%s"' % data
            connection.sendall('1')
            process_data(data)
            connection.sendall('2')
    finally:
        connection.close()