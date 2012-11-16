#ifndef SOCKETSTREAM_H
#define SOCKETSTREAM_H

#include "Socket.h"
#include <iostream>
#include <cstdio>
#include <fstream>
#include <istream>
#include <ostream>

class SocketStream
{
public:
    static std::istream* CreateInStream(Socket& sock){
        return CreateInStream(sock->fd());
    };
    static std::ostream* CreateOutStream(Socket& sock){
        return CreateOutStream(sock->fd());
    };
    static std::iostream* CreateInOutStream(Socket& sock){
        return CreateInOutStream(sock->fd());
    };
protected:
    static std::istream* CreateInStream(int fd);
    static std::ostream* CreateOutStream(int fd);
    static std::iostream* CreateInOutStream(int fd);
};

#endif
