#!/usr/bin/env python3
#

from socket import socket, AF_INET, SOCK_DGRAM
from my_secrets import MY_UDP_LOG_HOST  # type: ignore

HOST = MY_UDP_LOG_HOST
PORT = 4142
SIZE = 2048

sock = socket(AF_INET, SOCK_DGRAM)
sock.bind((HOST, PORT))
while True:
    (mesg, addr) = sock.recvfrom(SIZE)
    print("From {}: {}".format(addr, mesg.decode().strip()))
