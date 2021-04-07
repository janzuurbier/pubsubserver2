

#ifndef MULTICASTER_H
#define MULTICASTER_H

#include <map>
#include <set>
#include <mutex>
#include "PracticalSocket.h"

using namespace std;



class tcp_multicaster
{
	
private:
	map<string, set<TCPSocket*> > subscriptions;
	std::mutex   mtx;
	
	
public:
	tcp_multicaster(){}

	
	void subscribe(TCPSocket* sock, string topic){
		std::lock_guard<std::mutex> lock(mutex);
		cout << "SUBSCRIBER: " << sock->getForeignAddress().getAddress() << ":" << sock->getForeignAddress().getPort() << " TOPIC: "  << topic << endl;
		if (subscriptions.count(topic) == 0) {
			set<TCPSocket*> myset;
			subscriptions[topic] = myset;
		}
		subscriptions[topic].insert(sock);
	}
	
	void unsubscribe(TCPSocket* sock, string topic){
		std::lock_guard<std::mutex> lock(mutex);
		cout << "UNSUBSCRIBE: " << sock->getForeignAddress().getAddress() << ":" << sock->getForeignAddress().getPort() << " TOPIC: "  << topic << endl;
		subscriptions[topic].erase(sock);
		delete sock;
	}
	
	void send(char* buffer, int len, string topic, string from){
		std::lock_guard<std::mutex> lock(mutex);	
		cout <<  "**MESSAGE: "  << buffer + 2 << " FROM: " << from << " TOPIC: "  << topic << endl;
		for (std::set<TCPSocket*>::iterator it = subscriptions[topic].begin(); it != subscriptions[topic].end(); ++it)
			try { 
					(*it)->send(buffer, len); 
			} 
			catch (SocketException& e) { cout << "oeps " << e.what() << endl; /*it = subscriptions[topic].erase(it);*/ }
	}
	
	void add_publisher(TCPSocket* sock, string topic){
		std::lock_guard<std::mutex> lock(mutex);
		cout << "PUBLISHER: " << sock->getForeignAddress().getAddress() << ":" << sock->getForeignAddress().getPort() << " TOPIC: "  << topic <<  endl;	
	}
	
	void remove_publisher(TCPSocket* sock, string topic){
		std::lock_guard<std::mutex> lock(mutex);
		cout << "PUBLISHER DISCONNECT: " << sock->getForeignAddress().getAddress() << ":" << sock->getForeignAddress().getPort() << " TOPIC: "  << topic << endl;
		delete sock;	
	}

};

#endif // MULTICASTER_H
