// TCP Server
#include "socket_server.h"


int main( int argc, char** argv )
{
    sockConn::server TcpServer(3000);

    TcpServer.run();
	return 0;
}

