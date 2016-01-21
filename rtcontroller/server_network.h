#ifndef SERVER_NETWORK_H
#define SERVER_NETWORK_H

#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#include <net_protocol.h>
#include <mpc07/mpc07.h>

class Servernetwork
{
public:
    Servernetwork(SOCKET, SOCKADDR_IN, int);
    ~Servernetwork();
    int InitNetwork();
    int RunNetwork();
    int CloseNetwork();

private:
     SOCKET Connect_socket;
     SOCKADDR_IN Local_server;
     int PROTOCOL;
     int Port;
     int running;

};



#endif // NETAGREEMENT_H
