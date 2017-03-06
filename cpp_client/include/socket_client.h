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

class client
{

public:

        client(const unsigned short kPortNumber = 65535);
        ~client();

        bool run();

private:
        unsigned short m_portNumber;

        int m_status;
        struct addrinfo m_hints;
        struct addrinfo * m_res;
        int m_socket;

        bool setupConnection();

        bool sendRequest();
        google::protobuf::uint32 readHdr(char *buf);
        bool readResponse();
        bool readResponse(google::protobuf::uint32 siz);


        kinMsg::request input_msg;
        kinMsg::response output_msg;

}; //class client


} // namespace sockConn
