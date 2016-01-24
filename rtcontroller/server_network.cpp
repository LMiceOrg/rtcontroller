#include "server_network.h"
#include <iostream>

const char* Faultip = "ERROR 401 Occupied by other clients! Please wait!\n";
const char* Acceptip = "Accepted 200 Please send instructions!\n";
const char* Obtainins ="Accepted 300 Finish! Please send an instruction!\n";
const char* Already = "Accepted 400 Your are already online!\n";
const char* Wronginstrct = "ERROR 402 Undefined instructions!\n";
const char* Okreturn = "Finished 200 Finished this set of instructions!\n";
const char* TLE = "ERROR 403 Time limit exceeds of your instructions! Please reconnect!\n";
const int nNetTimeout = 1000;
int addr_len = sizeof(SOCKADDR);

Servernetwork::Servernetwork(int port)
{
    Port = port;
    running = 0;
    lastip = 0;
    elapsed = 0;
}

Servernetwork::~Servernetwork()
{
    std::cout<<"Go Bye!"<<std::endl;
}

//server state
/*obtain the angle, axis
0 ok!Run instructions
1 check online ? if true, you can send instruction
2 undefined instructions {3 Error with input angel}
4 Finish
5 TLE
*/

int Servernetwork::Judge(int flag, SOCKADDR_IN fromip, CMPC07Controller *CMP, int axis, int angle)
{
    int ret;
    switch (flag) {
    case 0:
        if (fromip.sin_addr.S_un.S_addr != lastip) {
           sendto(Connect_socket, Faultip , strlen(Faultip)+1, 0, (SOCKADDR*)&fromip, addr_len);
           break;
        }
        elapsed = 0;
        ret = CMP->canWork();
        printf("%d\n", ret);
        CMP->work(angle,0);

        sendto(Connect_socket,Obtainins , strlen(Obtainins)+1, 0, (SOCKADDR*)&fromip, addr_len);
        //move the camera;
        //obtain mtig
        break;

    case 1:

        if (running == 1 && fromip.sin_addr.S_un.S_addr == lastip) {
            sendto(Connect_socket, Already , strlen(Already)+1, 0, (SOCKADDR*)&fromip, addr_len);
            break;
        }
        if (running == 1) {
           sendto(Connect_socket, Faultip , strlen(Faultip)+1, 0, (SOCKADDR*)&fromip, addr_len);
           break;
        }

        nowaddr = fromip;
        lastip = fromip.sin_addr.S_un.S_addr;
        running = 1;
        sendto(Connect_socket, Acceptip , strlen(Acceptip)+1, 0, (SOCKADDR*)&fromip, addr_len);
        break;

    case 2: //maybe 3

        sendto(Connect_socket, Wronginstrct, strlen(Wronginstrct)+1, 0, (SOCKADDR*)&fromip, addr_len);
        break;

    case 4:

        sendto(Connect_socket, Okreturn, strlen(Okreturn)+1, 0, (SOCKADDR*)&fromip, addr_len);
        running = 0;
        break;

    case 5:

        sendto(Connect_socket, TLE, strlen(TLE)+1, 0, (SOCKADDR*)&fromip, addr_len);
        running = 0;
        break;

    //Sleep(1000);
    }
    return 0;
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


int Servernetwork::RunNetwork()
{
    //SOCKET Connect_Socket;
    Connect_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); //UDP
    SOCKADDR_IN fromip;
    Local_server.sin_family = AF_INET;
    Local_server.sin_port = htons(Port);       // Port Number
    Local_server.sin_addr.S_un.S_addr = INADDR_ANY; //inet_addr('0.0.0.0')

    int ret = bind(Connect_socket, (SOCKADDR*)&Local_server, addr_len);
    setsockopt(Connect_socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&nNetTimeout,sizeof(int));

    if (ret != 0) {
        printf("Binding Mistake!\n");
        return -1;          // Wrong binding
    }
    char recvBuf[50];
    int flag, axis, angle;

    ret = -1;
    CMPC07Controller CMP;
    Ok_capture camera;
    camera.Capture_single(1);
    
    while (1) {
        while (ret == -1) {
            ret = recvfrom(Connect_socket,recvBuf, 50, 0, (SOCKADDR*)&fromip, &addr_len); //阻塞
            if (running == 1 && ++elapsed >= 60) Judge(5, nowaddr, &CMP, axis, angle);
        }
        printf("%s\n", recvBuf);
        printf("%ld\n", fromip.sin_addr.S_un.S_addr);
        flag = Analyze_order(recvBuf, &axis, &angle);
        Judge(flag, fromip, &CMP, axis, angle);
        ret = -1;
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
