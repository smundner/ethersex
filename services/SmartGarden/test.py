import socket
import struct
from threading import Thread

ip = "192.168.4.3"

def udp_rec():
    udp_rec_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    udp_rec_sock.bind((ip,6000));
    print('waiting for data\n')
    while(True):
        data, addr = udp_rec_sock.recvfrom(1024)
        s_recv = struct.Struct('2B 4H')
        
        print(s_recv.unpack(data),addr)

def main():
    t=Thread(target=udp_rec)
    t.start()
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind((ip,5998))
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
    
    sock.sendto('\n'.encode(),('255.255.255.255',5998))
    print('start') 
    while True:
        data, addr = sock.recvfrom(1024)
        print (data,addr)
        if ip in addr:
            continue
        print('send data')
        tcp_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        src_ip_addr, src_port = addr 
        tcp_sock.connect((src_ip_addr,6000))
        rgb=bytes([255])
        print(rgb)
        tcp_sock.send(rgb)
        tcp_sock.close()

if __name__ == '__main__':
    main()
