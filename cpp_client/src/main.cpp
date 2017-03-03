// TCP Server
#include "socket_client.h"


int main( int argc, char** argv )
{
    sockConn::client TcpClient(3000);

    TcpClient.run();
	return 0;
}

