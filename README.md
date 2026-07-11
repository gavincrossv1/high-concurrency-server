本项目是一个基于多线程和多路复用（epoll/poll/select）的高并发 HTTP 服务器实现，采用 C++ 编写，支持高效的网络 IO、线程池、缓冲池等机制，适合学习和研究高性能服务器架构。

## 目录结构

```
common/      # 通用组件（Buffer、BufferPool、Channel、日志等）
http/        # HTTP 协议解析与响应
reactor/     # Reactor 反应堆模型及多路复用实现（Epoll/Poll/Select）
tcp/         # TCP 服务器与连接管理
thread/      # 线程池与工作线程
test/        # 测试用例与资源
main.cpp     # 服务器入口
CMakeLists.txt  # 构建脚本
```

## 主要功能

- 支持多线程并发处理客户端请求
- 支持 epoll、poll、select 三种 IO 多路复用模型
- 实现了线程池和缓冲池，提升资源复用效率
- 支持 HTTP 请求解析与响应，自动处理静态文件和目录
- 采用单例模式管理缓冲池，线程安全
- 详细的日志输出，便于调试和定位问题

## 编译与运行

### 依赖

- Linux 环境
- CMake >= 3.15
- g++ >= 7.0
- POSIX 线程库（pthread）

### 编译

在项目根目录下执行：

```sh
mkdir build
cd build
cmake ..
make
```

### 运行

进入 build 目录后，运行服务器：

```sh
./server <port> <threadnum> <buffernum> <keepalive> [keepalive_timeout]
```

参数说明：

- `port`：监听端口号
- `threadnum`：工作线程数量
- `buffernum`：缓冲池大小
- `keepalive`：是否启用 HTTP 持久连接，`1` 启用，`0` 禁用
- `keepalive_timeout`：可选，长连接空闲超时秒数，默认 `60`

例如：

```sh
./server 8080 4 100 1 60
```

启用持久连接后，HTTP/1.1 请求默认复用 TCP 连接；客户端发送
`Connection: close` 时，服务器会在当前响应发送完成后关闭连接。
连接在指定时间内没有收到任何数据时，服务器也会主动关闭连接。超时检查
复用 Reactor 的事件循环，不会额外创建检测线程，检查精度约为 2 秒。

具体行为如下：

服务端参数	客户端请求	处理结果
0	HTTP/1.1 默认请求	响应后关闭
0	HTTP/1.1 + Connection: keep-alive	响应后关闭
0	HTTP/1.0 + Connection: keep-alive	响应后关闭
1	HTTP/1.1，无 Connection	保持连接
1	HTTP/1.1 + Connection: keep-alive	保持连接
1	HTTP/1.1 + Connection: close	响应后关闭
1	HTTP/1.0，无 Connection	响应后关闭
1	HTTP/1.0 + Connection: keep-alive	保持连接
1	无法确定响应体长度	响应后关闭


## 代码说明

- 入口文件为 main.cpp，负责参数解析、初始化缓冲池和启动服务器。
- TCP 服务器核心逻辑在 `TcpServer` 和 `TcpConnection`。
- HTTP 协议解析与响应在 `HttpRequest` 和 `HttpResponse`。
- Reactor 反应堆模型及多路复用在 `EventLoop`、`EpollDispatcher`、`PollDispatcher`、`SelectDispatcher`。
- 线程池与工作线程在 `ThreadPool` 和 `WorkerThread`。
- 缓冲池实现见 `BufferPool`。