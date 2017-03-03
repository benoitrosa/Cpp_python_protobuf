#pragma once

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <iomanip>
#include <fcntl.h>

#include <arpa/inet.h>
#include <iostream>
#include <sstream>

#include "kinMsg.pb.h"
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>


namespace sockConn
{

class server
{

public:

        server(const unsigned short kPortNumber = 65535);
        ~server();

        bool run();

private:
        unsigned short m_portNumber;

        int m_status;
        struct addrinfo m_hints;
        struct addrinfo * m_res;
        int m_listner;

        bool setupConnection();
        bool waitForConnection(int& connectionID);

        google::protobuf::uint32 readHdr(char *buf);
        bool readBody(int csock);
        bool readBody(int csock, google::protobuf::uint32 size);
        bool sendResponse(int csock);

        kinMsg::request input_msg;
        kinMsg::response output_msg;

}; //class server


} // namespace sockConn
