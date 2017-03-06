#include "socket_server.h"

#include <iomanip>
#include <fcntl.h>

void * get_in_addr(struct sockaddr * sa)
{
    if(sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

namespace sockConn
{

server::server(const unsigned short kPortNumber) : m_portNumber(kPortNumber)
{
    if (!setupConnection()) std::cout << "Error setting up connection" << std::endl;
}

server::~server()
{
}

bool server::run()
{
    int connectionID;
    char buffer[4];
    int bytecount=0;
    bool connection = true;


    if (!waitForConnection(connectionID)) std::cerr << "Error accepting connection" << ::std::endl;

    while(connection)
    {
        google::protobuf::uint32 size;
        if((bytecount = recv(connectionID, buffer, 4, MSG_PEEK))== -1)
        {
            ::std::cerr << "Error receiving data "<< ::std::endl;
        }
        else if (bytecount == 0)
             break;

        try
        {
            size = readHdr(buffer);

            if(readBody(connectionID, size))
            {
                sendResponse(connectionID);
            }

            // if client is requesting disconnection, break the while(true)
            else
            {
                std::cout << "Disconnection requested by client. Exiting ..." << std::endl;
                connection = false;
            }
        }
        catch(...)
        {
            std::cerr << "Erro receiving message from client" << std::endl;
        }
    }
}


bool server::setupConnection()
{
    try{
        // Before using hint you have to make sure that the data structure is empty
        memset(& m_hints, 0, sizeof m_hints);
        // Set the attribute for hint
        m_hints.ai_family = AF_INET; // IPV4 AF_INET
        m_hints.ai_socktype = SOCK_STREAM; // TCP Socket SOCK_DGRAM
        m_hints.ai_flags = AI_PASSIVE;

        // Fill the res data structure and make sure that the results make sense.
        m_status = getaddrinfo(NULL, ::std::to_string(m_portNumber).c_str() , &m_hints, &m_res);
        if(m_status != 0)
        {
            ::std::cerr << "getaddrinfo error: " << gai_strerror(m_status) << ::std::endl;
            return 0;
        }

        // Create Socket and check if error occured afterwards
        m_listner = socket(m_res->ai_family,m_res->ai_socktype, m_res->ai_protocol);
        if(m_listner < 0 )
        {
            ::std::cerr << "socket error: " << gai_strerror(m_status) << ::std::endl;
            return 0;
        }

        // Bind the socket to the address of my local machine and port number
        m_status = bind(m_listner, m_res->ai_addr, m_res->ai_addrlen);
        if(m_status < 0)
        {
            ::std::cerr << "bind error: " << gai_strerror(m_status) << ::std::endl;
            return 0;
        }

        m_status = listen(m_listner, 10);
        if(m_status < 0)
        {
            ::std::cerr << "listen error: " << gai_strerror(m_status) << ::std::endl;
            return 0;
        }

        // Free the res linked list after we are done with it
        freeaddrinfo(m_res);
    }
    catch( const std::exception & e )
    {
        std::cerr << e.what();
        return 0;
    }
    return 1;

}

bool server::waitForConnection(int& connectionID)
{

    struct sockaddr_storage client_addr;
    socklen_t addr_size;
    char s[INET6_ADDRSTRLEN]; // an empty string

    // Calculate the size of the data structure
    addr_size = sizeof client_addr;

    ::std::cout << "Server ready, accepting connections ..." << ::std::endl;

    while(true){
        // Accept a new connection and return back the socket desciptor
        connectionID = accept(m_listner, (struct sockaddr *) & client_addr, &addr_size);
        if(connectionID < 0)
        {
            ::std::cerr << "accept socket error: " << gai_strerror(connectionID) << ::std::endl;
            continue;
        }
        else
        {
            inet_ntop(client_addr.ss_family, get_in_addr((struct sockaddr *) &client_addr),s ,sizeof s);
            ::std::cout << "Connected to " << s << ::std::endl;
            return true;
        }
    }
}

google::protobuf::uint32 server::readHdr(char *buf)
{
  google::protobuf::uint32 size;

  char tmp[5];
  memcpy ( tmp, buf, 4 );
  tmp[4] = '\0';
  size = atoi(tmp);

//  google::protobuf::io::ArrayInputStream ais(buf,4);
//  google::protobuf::io::CodedInputStream coded_input(&ais);
//  coded_input.ReadVarint32(&size);//Decode the HDR and get the size
  return size;

}


bool server::readBody(int csock,google::protobuf::uint32 siz)
{
    int bytecount;
    char buffer_full [siz+4];
    char buffer [siz];
    kinMsg::request message;
    message.set_endconnection(true);


    //Read the entire buffer including the hdr
    if((bytecount = recv(csock, (void *)buffer_full, siz+4, 0))== -1)
        ::std::cerr << "Error receiving data " <<  errno  << ::std::endl;

    strncpy(buffer, buffer_full+4, siz);

    //Assign ArrayInputStream with enough memory
    google::protobuf::io::ArrayInputStream ais(buffer,siz);
    google::protobuf::io::CodedInputStream coded_input(&ais);


    //After the message's length is read, PushLimit() is used to prevent the CodedInputStream
    //from reading beyond that length.Limits are used when parsing length-delimited
    //embedded messages
    google::protobuf::io::CodedInputStream::Limit msgLimit = coded_input.PushLimit(siz);

    //De-Serialize
    message.ParseFromCodedStream(&coded_input);
    //Once the embedded message has been parsed, PopLimit() is called to undo the limit
    coded_input.PopLimit(msgLimit);

    if (message.has_endconnection())
    {
        return !message.endconnection();
    }

    return false;
}



bool server::readBody(int csock)
{
    int bytecount;
    kinMsg::request message;
    message.set_endconnection(true);
    google::protobuf::uint32 siz;

    // First, read 4-characters header for extracting data size
    char buffer_hdr[5];
    if((bytecount = recv(csock, buffer_hdr, 4, 0))== -1)
        ::std::cerr << "Error receiving data "<< ::std::endl;

    ::std::cout << bytecount << ::std::endl;

    // place a null-terminating character to avoid weird atoi behavior
    buffer_hdr[4] = '\0';
    int i = atoi(buffer_hdr);
    siz = i;

    ::std::cout << siz << ::std::endl;

    // Second, read the data
    char buffer [siz];
    if((bytecount = recv(csock, (void *)buffer, siz, 0))== -1)
        ::std::cerr << "Error receiving data " <<  errno  << ::std::endl;

    ::std::cout << bytecount << ::std::endl;

    //Assign ArrayInputStream with enough memory
    google::protobuf::io::ArrayInputStream ais(buffer,siz);
    google::protobuf::io::CodedInputStream coded_input(&ais);

    //After the message's length is read, PushLimit() is used to prevent the CodedInputStream
    //from reading beyond that length.Limits are used when parsing length-delimited
    //embedded messages
    google::protobuf::io::CodedInputStream::Limit msgLimit = coded_input.PushLimit(siz);

    //De-Serialize
    message.ParseFromCodedStream(&coded_input);
    //Once the embedded message has been parsed, PopLimit() is called to undo the limit
    coded_input.PopLimit(msgLimit);

    if (message.has_endconnection())
    {
        return !message.endconnection();
    }

    return false;
}




bool server::sendResponse(int csock)
{

    int bytecount;


    // First, generate a dummy message
    kinMsg::response response;
    response.set_endconnection(false);
    int numEl =10;
    response.mutable_shape()->set_numpoints(numEl);

    for ( int i = 0; i < numEl; ++i ) {
        kinMsg::position3D* pos3D = response.mutable_shape()->add_p();
        pos3D->set_x(0.2*i);
        pos3D->set_y(0.1*i);
        pos3D->set_z(0.35*i);
    }

    // generate the data which should be sent over the network
    std::string response_s;
    response.SerializeToString(&response_s);
    int length = response_s.size();

    char hdr_send[4];
    std::ostringstream ss;
    ss << std::setw( 4 ) << std::setfill( '0' ) << length;
    ss.str().copy(hdr_send,4);

    char to_send[4+length];

    for (int i=0;i<4;++i)
    {
        to_send[i] = hdr_send[i];
    }

    for (int i=0;i<length;++i)
    {
        to_send[i+4] = response_s[i];
    }

    // send header with number of bytes
    if((bytecount = send(csock, (void *)to_send, 4+length, MSG_DONTWAIT ))== -1)
      ::std::cerr << "Error sending data " <<  errno  << ::std::endl;


    return true;


}


} // namespace sockConn
