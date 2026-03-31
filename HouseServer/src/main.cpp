#include "./kernel/CKernel.h"
#include "./Mediator/TcpMediator.h"
#include <iostream>
#include <stdlib.h>

using namespace std;

int main(int argc, char *argv[])
{
    int port = 8000;
    if( argc >= 2 )
    {
        port = atoi(argv[1]);
    }
    CKernel * pKernel = CKernel::GetInstance();

    //开启服务 给定端口, 可以使用输入的port
    pKernel->StartServer( port );
    cout << "port:" << port << endl ;
    


    pKernel->CloseServer();

    return 0;
}
