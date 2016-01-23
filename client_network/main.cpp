//For Client network .

#include <winsock2.h>
#include <iostream>
#pragma comment(lib, "ws2_32.lib")
using namespace std;

int port = 27073; //port
int addr_len = sizeof(SOCKADDR);
int nNetTimeout = 1000; //设置等待超时1秒

int equal(char cha, char chb)
{
    if (cha >= 'A' && cha <= 'Z') cha += 'a'-'A';
    if (chb >= 'A' && chb <= 'Z') chb += 'a'-'A';
    return (cha == chb);
}

char *strsub(char* source, const char *sub)
{
    char *temp, *find = source;
    const char* temq = sub;
    while (*find) {
        for (temp = find, temq = sub; *temq && equal(*temp, *temq); temp++, temq++);
        if (*temq == '\0') return find;
        find++;
    }
    return 0;
}

int Turn_on_broadcast (SOCKET sock, bool opt)
{
    return setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char*)&opt, sizeof(opt));
}

int main()
{
    WORD wVersionRequested;
    WSADATA wsaData;
    wVersionRequested = MAKEWORD(1, 1);

    WSAStartup( wVersionRequested, &wsaData );
    int ret, totalwaits = 0;

    SOCKET sockClient = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); //UDP

    Turn_on_broadcast(sockClient, true); //打开广播
    setsockopt(sockClient, SOL_SOCKET, SO_RCVTIMEO, (char *)&nNetTimeout,sizeof(int));
    //setsockopt(s,IPPROTO_IP,IP_MULTICAST_LOOP,(char *)&optval,sizeof(int));

    SOCKADDR_IN client_send, client_receive, fromip;
    //接收套接字
    client_receive.sin_family = AF_INET;
    client_receive.sin_port = htons(port);             //接受端口
    client_receive.sin_addr.s_addr = INADDR_ANY;
    //绑定
    ret = bind(sockClient, (SOCKADDR*)&client_receive, addr_len);
    if (ret != 0) {
        printf("Binding Mistake!\n");
        return 0;
    }
    //发送套接字
    client_send.sin_addr.S_un.S_addr = INADDR_BROADCAST;
    client_send.sin_family = AF_INET;
    client_send.sin_port = htons(32145);      //发送端口 客户端端口

    //发送上线提醒
    ret = -1;
    char recvBuf[40], instructions[30] , *retstring;
    while (1) {
        sendto(sockClient,"IM on", 6, 0, (SOCKADDR *)&client_send, addr_len);
        while (ret == -1) {
            ret = recvfrom(sockClient, recvBuf, 40, 0, (SOCKADDR*)&fromip, &addr_len); //阻塞方式 fromip: server ip
            Sleep(1000);
            ++ totalwaits;
            if (totalwaits > 20) {
                printf("The RT is running !Please wait!\n");
                return 0;
            }
        }
        printf("Receive %d bytes/n", ret);
        if (strsub(recvBuf, "Accpted 200") != 0) break;
    }
    Turn_on_broadcast(sockClient, false);

    //输入指令集
    totalwaits = 0;
    ret = -1;
    while (1) {
        cin.getline(instructions, 30);
        sendto(sockClient, instructions, strlen(instructions)+1, 0, (SOCKADDR *)&fromip, addr_len); //发出指令
        while (ret == -1) {
            ret = recvfrom(sockClient, recvBuf, 40, 0, (SOCKADDR*)&fromip, &addr_len); //阻塞方式 指令相应
            Sleep(1000);
            ++ totalwaits;
            if (totalwaits > 20) {
                printf("Server Turn off\n");
                return 0;
            }
        }
        printf("Receive %d bytes/n", ret);
        if (strsub(recvBuf, "Accpted 300") != 0) break;
    }

//    recv(sockClient, recvBuf, 100, 0);
//    printf("%s\n", recvBuf);
//    send(sockClient, "hello world", strlen("hello world") + 1, 0);

    closesocket(sockClient);
    WSACleanup();

    return 0;
}
