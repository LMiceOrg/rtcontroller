#include <iostream>
#include "server_network.h"

#ifndef _DEBUG
#include "Python.h"
#endif

using namespace std;

int main()
{
    int ret;
    Servernetwork server(27073);
    server.InitNetwork();
    ret = server.RunNetwork();
    server.CloseNetwork();
    return 0;
}

