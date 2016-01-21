#include "server_network.h"


//Start the net server
int InitNetwork()
{
    WORD wVersionRequested = MAKEWORD(1, 1); //version 1.1
    WSADATA wsaData;
    int err;
    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) return -1;                //Startup error
    return 0;
}

//RUN the server
int RunNetwork()
{
    SOCKET Connect_Socket;
    Connect_Socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); //UDP
    SOCKADDR_IN local_server,fromip;
    int addr_len = sizeof(SOCKADDR);
    local_server.sin_family = AF_INET;
    local_server.sin_port = htons(9000);  // Port Number
    local_server.sin_addr.S_un.S_addr = INADDR_ANY; //inet_addr('0.0.0.0')
    int ret = bind(Connect_Socket, (SOCKADDR*)&local_server, addr_len);
    if (ret != 0) {
        retrun -1;          // Wrong binding
    }
    char recvBuf[50],sendBuf[50];
    while (1) {
        ret = recvfrom(Connect_Socket,recvBuf, 50, 0, (SOCKADDR*)&fromip, &addr_len);
        printf("%s\n", recvBuf);
        if (ret = -1) sendBuf="";
        sendto(Connect_Socket, sendBuf, strlen(sendBuf)+1, 0, (SOCKADDR*)&fromip, addr_len);

        Sleep(1000);
    }

    return 0;
}

int CloserNetwork()
{

}
