import socket
from threading import Thread

def tcp_rec():
    tcp_rec_sock = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
    tcp_rec_sock.bind(('192.168.0.187',6000));
    tcp_rec_sock.listen(5)
    print('waiting for data')
    while(True):
        client, addr = tcp_rec_sock.accept()
        print(client.recv(1024),addr)

def main():
    t=Thread(target=tcp_rec)
    t.start()
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind(('192.168.0.187',5998))
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
    
    sock.sendto('test'.encode(),('255.255.255.255',5998))
    print('start') 
    while True:
        data, addr = sock.recvfrom(1024)
        print (data.decode(),addr)
        tcp_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        src_ip_addr, src_port = addr 
        tcp_sock.connect((src_ip_addr,5999))
        tcp_sock.send(bytes([1]))
        tcp_sock.close()

if __name__ == '__main__':
    main()
