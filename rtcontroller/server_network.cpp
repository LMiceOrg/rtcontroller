#include "server_network.h"
#include <iostream>

const char* Faultip = "ERROR 401 Occupied by other clients! Please wait!\n";
const char* Acceptip = "Accpted 200 Please send instructions!\n";
const char* Wronginstrct = "ERROR 402 Undefined instructions!\n";
const char* Okreturn = "Finished 201 Finished this set of instructions!\n";

Servernetwork::Servernetwork(SOCKET sock, SOCKADDR_IN sockaddr, int port)
{
    Connect_socket = sock;
    Local_server = sockaddr;
    Port = port;
    running = 0;
}

//Start the net server
int Servernetwork::InitNetwork()
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

int Servernetwork::RunNetwork()
{
    //SOCKET Connect_Socket;
    Connect_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); //UDP
    SOCKADDR_IN fromip;
    int addr_len = sizeof(SOCKADDR);
    Local_server.sin_family = AF_INET;
    Local_server.sin_port = htons(Port);       // Port Number
    Local_server.sin_addr.S_un.S_addr = INADDR_ANY; //inet_addr('0.0.0.0')
    int ret = bind(Connect_socket, (SOCKADDR*)&Local_server, addr_len);
    if (ret != 0) {
        return -1;          // Wrong binding
    }
    char recvBuf[50], sendBuf[50];
    int flag, axis, angle;
    ULONG lastip = 0;

    ret = -1;
    while (1) {
        while (ret != -1) ret = recvfrom(Connect_socket,recvBuf, 50, 0, (SOCKADDR*)&fromip, &addr_len);
        printf("%s\n", recvBuf);
        flag = Analyze_order(recvBuf, &axis, &angle);
        switch (flag) {
        case 0:
            if (fromip.sin_addr.S_un.S_addr != lastip) {
               sendto(Connect_socket, Faultip , strlen(Faultip)+1, 0, (SOCKADDR*)&fromip, addr_len);
               continue;
            }
            //move the camera;
            //obtain mtig

            break;
        case 1:
            if (running == 1) {
               sendto(Connect_socket, Faultip , strlen(Faultip)+1, 0, (SOCKADDR*)&fromip, addr_len);
               break;
            }
            lastip = fromip.sin_addr.S_un.S_addr;
            sendto(Connect_socket, Acceptip , strlen(Acceptip)+1, 0, (SOCKADDR*)&fromip, addr_len);
            break;
        case 2: //maybe 3
            sendto(Connect_socket, Wronginstrct, strlen(Wronginstrct)+1, 0, (SOCKADDR*)&fromip, addr_len);
            break;
        case 4:
            sendto(Connect_socket, Wronginstrct, strlen(Wronginstrct)+1, 0, (SOCKADDR*)&fromip, addr_len);
            break;

        //Sleep(1000);
        }
    }
    closesocket(Connect_socket);
    return 0;
}

int Servernetwork::CloseNetwork()
{
    int err;
    err = WSACleanup();
    if (err != 0) return -1;
    return 0;
}
