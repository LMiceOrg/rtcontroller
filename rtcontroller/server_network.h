#ifndef SERVER_NETWORK_H
#define SERVER_NETWORK_H

#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")


int InitNetwork();
int RunNetwork();
int CloserNetwork();


#endif // NETAGREEMENT_H
