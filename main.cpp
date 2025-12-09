#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "TcpServer.h"
#include "BufferPool.h"
#include <signal.h>

int main(int argc, char *argv[])
{
    signal(SIGPIPE, SIG_IGN); // 忽略管道破裂信号,否则sendfile会关停程序

#if 1
    if (argc < 4)
    {
        printf("./a.out port threadnum buffernum\n");
        return -1;
    }
    int port = atoi(argv[1]);
    int threadnum = atoi(argv[2]);
    int buffernum = atoi(argv[3]);
    // 切换服务器的工作路径(必须在项目的根目录下运行)
    // 输出当前目录
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        printf("当前目录: %s\n", cwd);
    }

    // 尝试切换到 test 目录
    printf("尝试切换到 test 目录...\n");
    if (chdir("./test") == 0)
    {
        getcwd(cwd, sizeof(cwd));
        printf("切换成功，新目录: %s\n", cwd);
    }
    else
    {
        printf("切换到 ./test 失败");
    }


#else
    unsigned short port = 10000;
    int threadnum = 32;
    int buffernum = 200;
    chdir("./test");
#endif
        // 初始化 BufferPool 单例
        BufferPool::instance(10240, buffernum);
        // 启动服务器
        TcpServer *server = new TcpServer(port, threadnum);
        server->run();

        return 0;
    }