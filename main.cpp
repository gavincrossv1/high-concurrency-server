#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "TcpServer.h"
#include <signal.h>

int main(int argc, char *argv[])
{
    signal(SIGPIPE, SIG_IGN);  // 忽略管道破裂信号,否则sendfile会关停程序
#if 1
    if (argc < 3)
    {
        printf("./a.out port threadnum\n");
        return -1;
    }
    unsigned short port = atoi(argv[1]);
    unsigned short threadnum = atoi(argv[2]);
    // 切换服务器的工作路径
    chdir("/mnt/c/Users/xujiawen/Desktop/高并发服务器代码实现/test");
#else
    unsigned short port = 10000;
    chdir("./test");
#endif
    // 启动服务器
    TcpServer *server = new TcpServer(port, threadnum);
    server->run();

    return 0;
}