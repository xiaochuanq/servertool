/*
 * main.cpp
 *
 *  Created on: Aug 16, 2011
 *      Author: xiaochuanq
 */
#include <iostream>
#include <string>
#include "Socket.h"

using namespace std;

int main()
{
	Socket sock;
	try{
		sock.connect("127.0.0.1", 54188);
		while(1){
			string buffer;
			cin >> buffer;
			sock.write( buffer );
			buffer = sock.read(100);
			cout << buffer << endl;
		}
	}
	catch(socket_exception& e){
		cout << e.what() << endl;
		sock.close();
	}
	sock.close();
	return 0;
}
