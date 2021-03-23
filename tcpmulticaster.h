

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
		if (subscriptions.count(topic) == 0) {
			set<TCPSocket*> myset;
			subscriptions[topic] = myset;
		}
		subscriptions[topic].insert(sock);
	}
	
	void unsubscribe(TCPSocket* sock, string topic){
		std::lock_guard<std::mutex> lock(mutex);
		subscriptions[topic].erase(sock);
	}
	
	void send(char* buffer, int len, string topic){
		std::lock_guard<std::mutex> lock(mutex);	
		for (std::set<TCPSocket*>::iterator it = subscriptions[topic].begin(); it != subscriptions[topic].end(); ++it)
			try { 
					(*it)->send(buffer, len); 
			} 
			catch (SocketException& e) { cout << "oeps " << e.what() << endl; /*it = subscriptions[topic].erase(it);*/ }
	}

};

#endif // MULTICASTER_H
