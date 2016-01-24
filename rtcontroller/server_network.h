#ifndef SERVER_NETWORK_H
#define SERVER_NETWORK_H

#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#include <net_protocol.h>
#include <mpc07/mpc07.h>
#include <okcapture/ok_capture.h>

class Servernetwork
{
public:
    Servernetwork(int); //give a port
    ~Servernetwork();
    int InitNetwork();
    int Judge(int, SOCKADDR_IN, CMPC07Controller* , int ,int);
    int RunNetwork();
    int CloseNetwork();

private:
     SOCKET Connect_socket;
     SOCKADDR_IN Local_server;
     int Port; //port
     int running; //if is sending instruction
     unsigned int elapsed;
     SOCKADDR_IN nowaddr;
     ULONG lastip; //

};



#endif // NETAGREEMENT_H
