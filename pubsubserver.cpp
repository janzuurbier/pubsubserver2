#include <iostream>
#include <thread>
#include "PracticalSocket.h"
#include "tcpmulticaster.h"
using namespace std;

tcp_multicaster themulticaster;



void handle_publisher(TCPSocket* sock, string topic){
	char buffer[103];
	string remoteadr = "unknown";
	
	try{
		remoteadr = sock->getForeignAddress().getAddress() + ":" + to_string(sock->getForeignAddress().getPort());
		while (true) {
			int n = sock->recvFully(buffer, 2);
			if (n == 0) break;
			int len = (buffer[0]-48)*10 + buffer[1]-48;
			n = sock->recvFully(buffer + 2, len);
			if (n == 0) break;
			len += 2;
			buffer[len ] = '\0';				
			themulticaster.send(buffer, len,  topic, remoteadr);
			//cout << buffer << endl;	
		}
	}
	catch(SocketException& e) {}
	themulticaster.remove_publisher(sock, topic, remoteadr);
	
}

void handle_subscriber(TCPSocket* sock, string topic){
	string remoteadr = "unknown";
	char buffer[2];
	try{
	 while(true){
		remoteadr = sock->getForeignAddress().getAddress() + ":" + to_string( sock->getForeignAddress().getPort());	
		int n = sock->recvFully(buffer, 2);
		if(n == 0) break;
	 }
	}
	catch(SocketException& e) {
		
	}
	themulticaster.unsubscribe(sock, topic, remoteadr);
	
	
}
	



int main(int argc, char *argv[]) {
	if(argc != 3) {
		cout << "missing arguments:  pubsubserver <ipadres> <port> "  << endl;
		return -1;
	}
	const char* adres = argv[1];
	string portstring = argv[2];
	int port = stoi(portstring);
	try {
		
		SocketAddress localadr (adres, port, SocketAddress::UDP_SOCKET);
		TCPServerSocket sock;
		sock.bind(localadr);
				
		cout << " waiting...." << endl;
  
		for (;;) {                          
			TCPSocket* clientsock = sock.accept();
			istream& sockstream = clientsock->getStream();
			string type, topic;
			sockstream >> type >> topic;
									
			if(type == "subscription"){
				themulticaster.subscribe(clientsock, topic);
				thread t(handle_subscriber, clientsock, topic);
				t.detach();
			}
			else if (type == "publisher"){
				themulticaster.add_publisher(clientsock, topic);
				thread t(handle_publisher, clientsock, topic);
				t.detach();
			}
		}

  } catch (SocketException &e) { cerr << e.what() << endl;  }
  return 0;
}
