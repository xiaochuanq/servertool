#include "SocketStream.h"
#include <ext/stdio_filebuf.h>

std::istream* SocketStream::CreateInStream(int fd)
{
    int posix_handle = dup(fd);
    __gnu_cxx::stdio_filebuf<char> filebuf(posix_handle, std::ios::in);
    return new std::istream(&filebuf);
}

std::ostream* SocketStream::CreateOutStream(int fd)
{
    int posix_handle = dup(fd);
    __gnu_cxx::stdio_filebuf<char> filebuf(posix_handle, std::ios::out);
    return new std::ostream(&filebuf);
}

std::iostream* SocketStream::CreateInOutStream(int fd)
{
    int posix_handle = dup(fd);
    __gnu_cxx::stdio_filebuf<char> filebuf(posix_handle, std::ios::in | std::ios::out);
    return new std::iostream(&filebuf);
}
