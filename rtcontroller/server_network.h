#ifndef SERVER_NETWORK_H
#define SERVER_NETWORK_H

#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#include <net_protocol.h>

int InitNetwork();

int RunNetwork();

int CloseNetwork();


#endif // NETAGREEMENT_H
