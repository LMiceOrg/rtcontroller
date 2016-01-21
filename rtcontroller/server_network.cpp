#include "server_network.h"
#include <iostream>

const char* Faultip = "ERROR 401 Occupied by other clients! Please wait!\n";
const char* Acceptip = "Accpted 200 Please send instructions!\n";

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
/*obtain the angle, axis
0 ok!Run instructions
1 check online ? if true, you can send instruction
2 undefined instructions {3 Error with input angel}
4 Busy
5 Finish
*/

int RunNetwork()
{
    SOCKET Connect_Socket;
    Connect_Socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); //UDP
    SOCKADDR_IN local_server, fromip;
    int addr_len = sizeof(SOCKADDR);
    local_server.sin_family = AF_INET;
    local_server.sin_port = htons(9000);       // Port Number
    local_server.sin_addr.S_un.S_addr = INADDR_ANY; //inet_addr('0.0.0.0')
    int ret = bind(Connect_Socket, (SOCKADDR*)&local_server, addr_len);
    if (ret != 0) {
        return -1;          // Wrong binding
    }
    char recvBuf[50], sendBuf[50];
    int flag, axis, angle, running = 0;
    ULONG lastip = 0;
    ret = -1;
    while (1) {
        while (ret != -1) ret = recvfrom(Connect_Socket,recvBuf, 50, 0, (SOCKADDR*)&fromip, &addr_len);
        printf("%s\n", recvBuf);
        flag = Analyze_order(recvBuf, &axis, &angle);
        if (flag == 0) {
            if (fromip.sin_addr.S_un.S_addr != lastip) {
               sendto(Connect_Socket, Faultip , strlen(Faultip)+1, 0, (SOCKADDR*)&fromip, addr_len);
               continue;
            }
            //move the camera;
            //obtain mtig

        }
        if (flag == 1) {
            if (running == 1) {
               sendto(Connect_Socket, Faultip , strlen(Faultip)+1, 0, (SOCKADDR*)&fromip, addr_len);
               continue;
            }
            lastip == fromip.sin_addr.S_un.S_addr;
            sendto(Connect_Socket, Acceptip , strlen(Faultip)+1, 0, (SOCKADDR*)&fromip, addr_len);
        }
        if (flag == )
        //Protocol
        sendto(Connect_Socket, sendBuf, strlen(sendBuf)+1, 0, (SOCKADDR*)&fromip, addr_len);

        //Sleep(1000);
    }
    closesocket(Connect_Socket);
    return 0;
}

int CloseNetwork()
{
    int err;
    err = WSACleanup();
    if (err != 0) return -1;
    return 0;
}
