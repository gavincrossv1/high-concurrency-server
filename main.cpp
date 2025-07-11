#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "TcpServer.h"
#include "BufferPool.h"
#include <signal.h>

int main(int argc, char *argv[])
{
    signal(SIGPIPE, SIG_IGN);  // 忽略管道破裂信号,否则sendfile会关停程序

#if 1
    if (argc < 4)
    {
        printf("./a.out port threadnum buffernum\n");
        return -1;
    }
    int port = atoi(argv[1]);
    int threadnum = atoi(argv[2]);
    int buffernum = atoi(argv[3]);
    // 切换服务器的工作路径
    chdir("/mnt/c/Users/xujiawen/Desktop/高并发服务器代码实现/test");
#else
    unsigned short port = 10000;
    chdir("./test");
#endif
    // 初始化 BufferPool 单例
    BufferPool::instance(10240, buffernum);
    // 启动服务器
    TcpServer *server = new TcpServer(port, threadnum);
    server->run();

    return 0;
}