#!/usr/bin/env python3

import os, os.path, socket, sys

prog_name = os.path.basename(sys.argv[0])

if len(sys.argv) != 3:
    print(f"Usage: {prog_name} HOST PORT < in.txt > out.txt", file=sys.stderr)
    sys.exit(1)

server_addr = ( sys.argv[1], int(sys.argv[2]) )

# Wczytaj treść zapytania z stdin i wyślij do serwera jako datagram UDP/IPv4:

client_request = b""
while True:
    buf = os.read(0, 1024)
    if buf == b"":
        break
    client_request += buf

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.connect(server_addr)
if sock.send(client_request) != len(client_request):
    print(f"{prog_name}: partial send", file=sys.stderr)
    sys.exit(1)

# Odbierz datagram-odpowiedź (skoro używamy UDP, to jego długość na pewno
# będzie mniejsza od 64 KiB), ale nie czekaj na niego w nieskończoność:

sock.settimeout(5.0)
server_reply = sock.recv(65536)
sock.close()

if os.write(1, server_reply) != len(server_reply):
    print(f"{prog_name}: partial write", file=sys.stderr)
    sys.exit(1)

# W powyższym kodzie nie ma obsługi tych błędów, dla których jądro systemu
# zwraca -1. Python te błędy zamienia na wyjątki, a skoro powyższy kod
# wyjątków nie przechwytuje, to działanie programu zostanie przerwane
# w razie wystąpienia błędu z tej kategorii.
