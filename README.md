# High Concurrency HTTP Server

本项目是一个使用 C++ 编写的简易高并发 HTTP 静态资源服务器，包含从底层 TCP
连接管理到上层 HTTP 请求解析与响应生成的完整流程。项目采用 Reactor 模型、IO
多路复用和工作线程池，适合学习 Linux 网络编程和 HTTP/1.1 持久连接的实现方式。

> 本项目主要用于学习和实验，尚未实现完整的 HTTP 协议及生产级安全防护，请勿直接
> 作为公网生产服务器使用。

## 主要功能

- 基于 Reactor 模型处理网络事件
- 支持 `epoll`、`poll`、`select` 三种 IO 多路复用实现
- 使用工作线程池并发处理客户端连接
- 使用线程安全的缓冲池复用读写缓冲区
- 支持 HTTP `GET` 和 `HEAD` 请求
- 支持静态文件、目录列表和 404 页面响应
- 支持常见静态资源 MIME 类型
- 支持 HTTP/1.0 和 HTTP/1.1 连接语义
- 支持可配置的 HTTP/1.1 Keep-Alive
- 支持长连接空闲超时和基础 HTTP 流水线请求
- 使用 `sendfile` 实现普通文件零拷贝发送

## 项目结构

```text
common/         Buffer、BufferPool、Channel 和日志组件
http/           HTTP 请求解析和响应生成
reactor/        EventLoop 以及 epoll、poll、select 分发器
tcp/            TCP 服务器和连接生命周期管理
thread/         工作线程和线程池
test/           服务器对外提供的静态资源目录
main.cpp        程序入口和启动参数解析
CMakeLists.txt  CMake 构建入口
```

服务器启动后会将工作目录切换到 `test/`，因此 URL `/readme.html` 对应
`test/readme.html`。程序必须从项目根目录启动，否则无法找到 `test/`。

## 环境要求

- Linux，或 Windows 10/11 上的 WSL2
- CMake 3.15 或更高版本
- 支持 C++11 的 GCC/G++
- POSIX Threads
- 可选：ApacheBench，用于并发压测
- 可选：curl、netcat 或 Python 3，用于功能测试

Ubuntu/Debian 可以安装所需工具：

```sh
sudo apt update
sudo apt install -y build-essential cmake curl apache2-utils netcat-openbsd
```

## 编译

在项目根目录执行：

```sh
cmake -S . -B build
cmake --build build -j$(nproc)
```

构建完成后，可执行文件位于 `build/server`。

清理并重新构建：

```sh
rm -rf build
cmake -S . -B build
cmake --build build -j$(nproc)
```

### Windows + WSL

在 WSL 终端中进入项目目录后使用相同命令。例如，Windows 项目路径为
`C:\Users\用户名\Desktop\high-concurrency-server` 时，对应的 WSL 路径通常是：

```sh
cd /mnt/c/Users/用户名/Desktop/high-concurrency-server
cmake -S . -B build-wsl
cmake --build build-wsl -j$(nproc)
```

下文若使用 `build-wsl` 构建，请将启动命令中的 `build/server` 替换为
`build-wsl/server`。

## 启动服务器

必须在项目根目录运行：

```sh
./build/server <port> <threadnum> <buffernum> <keepalive> [keepalive_timeout]
```

参数说明：

| 参数 | 必填 | 含义 |
| --- | --- | --- |
| `port` | 是 | HTTP 监听端口，例如 `8080` |
| `threadnum` | 是 | 工作线程数量 |
| `buffernum` | 是 | 缓冲池预分配的 Buffer 数量 |
| `keepalive` | 是 | `1` 启用持久连接，`0` 禁用 |
| `keepalive_timeout` | 否 | 长连接空闲超时秒数，默认 `60`；非正数也使用默认值 |

例如，使用 4 个工作线程、100 个预分配 Buffer，开启长连接并设置 60 秒空闲超时：

```sh
./build/server 8080 4 100 1 60
```

看到“服务器程序已经启动了”后，可在浏览器访问：

```text
http://127.0.0.1:8080/readme.html
```

使用 `Ctrl+C` 停止服务器。

## HTTP 持久连接规则

服务器启动参数和客户端请求共同决定响应后是否保留 TCP 连接：

| 服务端 `keepalive` | 客户端请求 | 处理结果 |
| --- | --- | --- |
| `0` | 任意 HTTP 请求 | 响应后关闭 |
| `1` | HTTP/1.1，无 `Connection` | 保持连接 |
| `1` | HTTP/1.1 + `Connection: keep-alive` | 保持连接 |
| `1` | HTTP/1.1 + `Connection: close` | 响应后关闭 |
| `1` | HTTP/1.0，无 `Connection` | 响应后关闭 |
| `1` | HTTP/1.0 + `Connection: keep-alive` | 保持连接 |
| `1` | 无法确定响应体长度 | 响应后关闭 |

每次收到数据都会刷新连接的最后活跃时间。连接持续空闲超过
`keepalive_timeout` 后，服务器会主动关闭它。超时检查复用 Reactor 事件循环，
不会创建额外线程；当前事件循环约每 2 秒检查一次，因此关闭时间可能存在约 2 秒误差。

目录列表响应当前没有预先计算 `Content-Length`，所以访问 `/` 等目录时服务器会通过
关闭连接标记响应结束。测试长连接时应请求 `/readme.html` 等普通文件。

## 功能测试

以下命令默认服务器运行在 `127.0.0.1:8080`。

### curl 获取静态文件

查看响应头和响应体：

```sh
curl -i http://127.0.0.1:8080/readme.html
```

只发送 `HEAD` 请求：

```sh
curl -I http://127.0.0.1:8080/readme.html
```

验证 404 响应：

```sh
curl -i http://127.0.0.1:8080/not-found.html
```

显式要求服务器关闭连接：

```sh
curl -i -H 'Connection: close' http://127.0.0.1:8080/readme.html
```

启用长连接时，普通文件的响应头应包含 `Connection: keep-alive`；显式发送
`Connection: close` 时，响应头应改为 `Connection: close`。

### 验证同一 TCP 连接复用

用 netcat 建立连接后一次输入两个请求：

```sh
printf 'GET /readme.html HTTP/1.1\r\nHost: localhost\r\n\r\nGET /readme.html HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n' \
	| nc 127.0.0.1 8080
```

预期收到两个 `HTTP/1.1 200 OK` 响应：第一个响应保持连接，第二个响应后关闭连接。
这同时验证了基础 HTTP 流水线请求处理。

### 验证空闲超时

为方便观察，先使用较短超时启动服务：

```sh
./build/server 8080 4 100 1 3
```

另开一个终端建立空闲连接：

```sh
nc 127.0.0.1 8080
```

连接建立后不输入任何内容。大约 3～5 秒后，服务端会输出长连接空闲超时日志并关闭
连接。由于扫描周期约为 2 秒，实际时间不保证恰好为 3 秒。

## ApacheBench 压测

ApacheBench 的命令是 `ab`。建议压测普通静态文件 `/readme.html`，不要压测目录 `/`，
否则目录响应会关闭连接，无法正确比较 Keep-Alive 效果。

### 短连接压测

先以关闭 Keep-Alive 的方式启动服务器：

```sh
./build/server 8080 4 1000 0
```

然后在另一个终端执行 10000 次请求，并保持 100 个并发请求：

```sh
ab -n 10000 -c 100 http://127.0.0.1:8080/readme.html
```

### 长连接压测

以启用 Keep-Alive 的方式重新启动服务器：

```sh
./build/server 8080 4 1000 1 60
```

`ab` 必须添加 `-k` 才会主动复用 HTTP 连接：

```sh
ab -k -n 10000 -c 100 http://127.0.0.1:8080/readme.html
```

参数含义：

| 参数 | 含义 |
| --- | --- |
| `-n 10000` | 总请求数为 10000 |
| `-c 100` | 同时保持 100 个并发请求 |
| `-k` | 启用 HTTP Keep-Alive |
| `-t 30` | 可选，按持续时间测试 30 秒 |

按时间持续压测的示例：

```sh
ab -k -t 30 -c 100 http://127.0.0.1:8080/readme.html
```

不要一开始就设置过高并发。建议依次测试 `-c 10`、`-c 50`、`-c 100`、
`-c 500`，观察吞吐量、延迟和失败请求的变化。

### 重点关注的压测指标

| 输出项 | 含义 |
| --- | --- |
| `Complete requests` | 成功完成的请求总数 |
| `Failed requests` | 失败请求数，正常情况下应为 0 |
| `Keep-Alive requests` | 实际使用 Keep-Alive 的请求数 |
| `Requests per second` | 每秒处理请求数，即 RPS/QPS |
| `Time per request` | 请求平均耗时；注意 ab 会同时输出按并发摊销前后的两个值 |
| `Transfer rate` | 每秒传输的数据量 |
| `Percentage of the requests served` | 请求耗时的百分位分布，例如 P50、P90、P99 附近数据 |

比较短连接和长连接时，应保持文件、请求数、并发数、线程数和 Buffer 数相同，只改变
服务端 `keepalive` 参数以及客户端是否使用 `-k`。

> 当前 Debug 日志会为每次请求输出内容，终端 IO 会显著影响压测成绩。需要测量网络框架
> 本身性能时，应在 Release 构建中关闭或降低逐请求日志，再进行对比。

## 常见问题

### 启动后提示切换 `./test` 失败

服务器不是从项目根目录启动的。回到包含 `test/` 的项目根目录，再执行：

```sh
./build/server 8080 4 100 1 60
```

### 浏览器或 curl 无法连接

确认服务器仍在运行，端口没有被其他程序占用，并检查监听端口：

```sh
ss -lntp | grep 8080
```

如果服务器运行在 WSL2，通常可以在 Windows 浏览器中直接访问
`http://localhost:8080/readme.html`。若无法访问，请检查 Windows 防火墙和 WSL 网络配置。

### `ab: command not found`

Ubuntu/Debian 中安装 ApacheBench：

```sh
sudo apt install apache2-utils
```

### `apr_socket_recv: Connection reset by peer`

先降低并发数，并确认服务端与 `ab` 的 Keep-Alive 配置匹配。启用 `ab -k` 时服务端也应
以 `keepalive=1` 启动。还应确认压测 URL 是普通文件，而不是会主动关闭连接的目录。

### 为什么空闲连接没有立即在指定秒数关闭

空闲时间使用单调时钟计算，但扫描依赖 EventLoop 的约 2 秒分发周期，因此实际关闭时间
可能略晚于配置值。这是当前轻量实现的预期行为。

## 当前限制

- 仅实现 HTTP/1.0 和 HTTP/1.1 的基础请求处理，不支持 HTTP/2、HTTP/3 和 TLS
- 主要支持 `GET`、`HEAD`，没有完整实现请求体和 `POST`
- 目录响应没有 `Content-Length`，因此不会保持长连接
- 没有实现分块传输编码、Range 请求、虚拟主机和完整的 HTTP 错误处理
- 文件发送流程仍以当前简化模型为主，不等同于生产级异步文件发送状态机
- 没有请求头大小、请求速率等完整的抗攻击限制

## 核心代码说明

- `main.cpp`：解析启动参数、初始化缓冲池并启动服务器
- `TcpServer`：创建监听 socket、接受连接并分配工作 EventLoop
- `TcpConnection`：管理 TCP 连接、读写事件、持久连接和空闲超时
- `HttpRequest`：解析请求行和请求头，处理静态资源请求
- `HttpResponse`：生成状态行和响应头
- `EventLoop`：执行 Reactor 事件循环、任务队列和空闲超时扫描
- `EpollDispatcher`、`PollDispatcher`、`SelectDispatcher`：不同 IO 多路复用后端
- `ThreadPool`、`WorkerThread`：工作线程调度
- `Buffer`、`BufferPool`：网络缓冲区及其复用