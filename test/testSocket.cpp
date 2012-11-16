//============================================================================
// Name        : testSocket.cpp
// Author      : Xiaochuan Qin
// Version     :
// Copyright   : All rights reserved.
// Description : Hello World in C++, Ansi-style
//============================================================================
#include <unistd.h>
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include "Socket.h"

using namespace std;

int simpleEchoServer(ServerSocket& server) {
	string buffer;
	while (true) {
		Socket client = server.accept();
		while (true) {
			buffer = client.read(10);
			cout << "client says: " << buffer << "( Bytes read "
					<< buffer.length() << " )" << endl;
			if (buffer.find("quit") == 0) {
				client.close();
				cout << " quiting..." << endl;
				server.close();
				return 0;
			}
			if (buffer.find("close") == 0) {
				client.close();
				cout << " close and change user" << endl;
				break;
			}
			client.write("server echo: " + buffer);
		}
	}
	return 0;
}

int asyncEchoServer(ServerSocket& server) {
	SocketSet sset;
	string buffer;
	for (int i = 0; i < 1; ++i) {
		Socket * pclient = new Socket(server.accept());
		pclient->set_nonblocking(10);
		sset.insert(pclient);
		cout << "User " << pclient->fd() << " joined" << endl;
	}
	socket_ptr_list readable_list;
	socket_ptr_list writeable_list;
	socket_ptr_list client_list;
	sset.set_timeout(1, 0); //
	while (sset.size() > 0) {
		sset.poll(&client_list);
	//	cout << "sizes: " << client_list.size() << " out of " << sset.size() << endl;
		for (socket_ptr_list::iterator iter = client_list.begin();
				iter != client_list.end(); ++iter) {
			try {
				int n2read = (*iter)->byte_pending_read();
				if( n2read > 0)
					buffer = (*iter)->read(100);
				else{
					cout << "User " << (*iter)->fd() << " may have been disconnected abnormally."<< endl;
				}
			} catch (socket_exception& e) {
				cout << e.what() << endl;
				(*iter)->close();
				sset.erase(*iter);
				cout << "User " << (*iter)->fd() << "disconnected abnormally."
						<< endl;
			}

			cout << " client " << (*iter)->fd() << " says: " << buffer
					<< "( " << buffer.length() << " bytes  has been read. )"
					<< endl;

			if (buffer.find("close") == 0) {
				MARK_SOURCEPOINT;
				BEGIN_TRYBLOCK;
				try{
				(*iter)->close();
				sset.erase(*iter); MARK_SOURCEPOINT;
				cout << "User " << (*iter)->fd()
						<< " sent closing command.Disconnected " << endl;
				throw std::runtime_error("test mark source point");
				}catch(...){
					cout << "at" << LAST_ERRORPOINT;
					throw;
				}
				END_TRYBLOCK;
			//	throw std::runtime_error("throw marker test again");
			}else{
				try {
					(*iter)->write("server echo: " + buffer);
					cout << "Echoed to user " << (*iter)->fd() << endl;
				} catch (socket_exception& e) {
					cout << e.what() << endl;
					sset.erase(*iter);
					cout << "User " << (*iter)->fd() << "disconnected abnormally."
							<< endl;
				}
			}
		}
	}
	return 0;

}

int main() {
ServerSocket server("127.0.0.1", 54188);
BEGIN_TRYBLOCK;
try {
	//simpleEchoServer(server);
	asyncEchoServer(server);
} catch (socket_exception& e) {
	cout << e.what() << endl;
	return -1;
} catch(std::runtime_error& re){
	cout << "At " << LAST_ERRORPOINT<< endl;
	cout << re.what() << endl;
}
END_TRYBLOCK;
return 0;
}
