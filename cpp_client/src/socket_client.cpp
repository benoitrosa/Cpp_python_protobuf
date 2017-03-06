#include "socket_client.h"

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

client::client(const unsigned short kPortNumber) : m_portNumber(kPortNumber)
{
    if (!setupConnection()) std::cout << "Error setting up connection" << std::endl;
}

client::~client()
{
}

bool client::run()
{

    bool connection = true;

    while(connection)
    {

        try
        {
            sendRequest();

            if( ! readResponse())
            {
                std::cout << "Disconnection requested by client. Exiting ..." << std::endl;
                connection=false;
            }
        }
        catch(...)
        {
            std::cerr << "Erro receiving message from client" << std::endl;
        }
    }
}


bool client::setupConnection()
{
    try{
        // Before using hint you have to make sure that the data structure is empty
        memset(& m_hints, 0, sizeof m_hints);
        // Set the attribute for hint
        m_hints.ai_family = AF_INET; // IPV4 AF_INET
        m_hints.ai_socktype = SOCK_STREAM; // TCP Socket SOCK_DGRAM
        m_hints.ai_flags = 0;
        char s[INET_ADDRSTRLEN];

        // Fill the res data structure and make sure that the results make sense.
        m_status = getaddrinfo(NULL, ::std::to_string(m_portNumber).c_str() , &m_hints, &m_res);
        if(m_status != 0)
        {
            ::std::cerr << "getaddrinfo error: " << gai_strerror(m_status) << ::std::endl;
            return 0;
        }

        // Create Socket and check if error occured afterwards
        m_socket = socket(m_res->ai_family,m_res->ai_socktype, m_res->ai_protocol);
        if(m_socket < 0 )
        {
            ::std::cerr << "socket error: " << gai_strerror(m_status) << ::std::endl;
            return 0;
        }

        // Connect to the server using the socket
        m_status = connect(m_socket, m_res->ai_addr, m_res->ai_addrlen);
        if(m_status < 0)
        {
            ::std::cerr << "bind error: " << gai_strerror(m_status) << ::std::endl;
            return 0;
        }

        inet_ntop(m_res->ai_family, get_in_addr((struct sockaddr *)  m_res->ai_addr),s ,sizeof s);
        ::std::cout << "Connected to " << s << ::std::endl;

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

google::protobuf::uint32 client::readHdr(char *buf)
{
  google::protobuf::uint32 size;

  char tmp[5];
  memcpy ( tmp, buf, 4 );
  tmp[4] = '\0';
  size = atoi(tmp);
  return size;
}


bool client::readResponse(google::protobuf::uint32 siz)
{
    int bytecount;
    char buffer [siz];
    kinMsg::response message;
    message.set_endconnection(true);

    //Read the entire buffer including the hdr
    if((bytecount = recv(m_socket, (void *)buffer, siz, 0))== -1)
        ::std::cerr << "Error receiving data " <<  errno  << ::std::endl;

    // Deserialize using protobuf functions
    google::protobuf::io::ArrayInputStream ais(buffer,siz);
    google::protobuf::io::CodedInputStream coded_input(&ais);
    google::protobuf::io::CodedInputStream::Limit msgLimit = coded_input.PushLimit(siz);
    message.ParseFromCodedStream(&coded_input);
    coded_input.PopLimit(msgLimit);


    if (message.has_shape())
    {
        ::std::cout << "[" ;
        for (int i=0; i< message.shape().p_size();++i)
        {
            ::std::cout << "[" << message.shape().p(i).x()
                      << "," << message.shape().p(i).y()
                      << "," << message.shape().p(i).z()
                      << "]";
        }
        ::std::cout << "]" << ::std::endl;

    }

    if (message.has_endconnection())
    {
        return !message.endconnection();
    }

    return false;
}



bool client::readResponse()
{
    int bytecount;

    // First, read 4-characters header for extracting data size
    char buffer_hdr[5];
    if((bytecount = recv(m_socket, buffer_hdr, 4, 0))== -1)
        ::std::cerr << "Error receiving data "<< ::std::endl;

    google::protobuf::uint32 siz = readHdr(buffer_hdr);

    return readResponse(siz);
}




bool client::sendRequest()
{
    int bytecount;

    // Generate the request
    kinMsg::request request;
    request.set_endconnection(false);


    // generate the data which should be sent over the network
    std::string request_s;
    request.SerializeToString(&request_s);
    int length = request_s.size();

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
        to_send[i+4] = request_s[i];
    }

    // send header with number of bytes
    if((bytecount = send(m_socket, (void *)to_send, 4+length, 0 ))== -1)
      ::std::cerr << "Error sending data " <<  errno  << ::std::endl;


    return true;
}


} // namespace sockConn
