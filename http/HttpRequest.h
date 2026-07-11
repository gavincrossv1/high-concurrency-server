#pragma once
#include "Buffer.h"
#include <stdbool.h>
#include "HttpResponse.h"
#include <map>
using namespace std;

// 当前的解析状态
enum class PrecessState : char
{
    ParseReqLine,
    ParseReqHeaders,
    ParseReqBody,
    ParseReqDone
};

enum class HttpParseResult : char
{
    Incomplete,
    KeepAlive,
    Close
};
// 定义http请求结构体
class HttpRequest
{
public:
    HttpRequest();
    ~HttpRequest();
    // 重置
    void reset();
    // 添加请求头
    void addHeader(const string key, const string value);
    // 获取请求头
    string getHeader(const string key);
    // 获取所有请求头
    const map<string, string> &getHeaders() const { return m_reqHeaders; }
    // 设置所有请求头
    void setHeaders(const map<string, string> &headers) { m_reqHeaders = headers; }
    // 解析请求行
    bool parseRequestLine(Buffer *readBuf);
    // 解析请求头
    bool parseRequestHeader(Buffer *readBuf);
    // 解析http请求协议
    HttpParseResult parseHttpRequest(Buffer *readBuf, HttpResponse *response,
                                     Buffer *sendBuf, int socket, bool keepAliveEnabled);
    // 处理http请求协议
    bool processHttpRequest(HttpResponse *response);
    // 解码字符串
    string decodeMsg(string from);
    const string getFileType(const string name);
    static void sendDir(string dirName, Buffer *sendBuf, int cfd);
    static void sendFile(string dirName, Buffer *sendBuf, int cfd);
    static void sendNoData(string fileName, struct Buffer *sendBuf, int cfd); // head头专用
    inline void setMethod(string method)
    {
        m_method = method;
    }
    inline string getMethod() const
    {
        return m_method;
    }
    inline void seturl(string url)
    {
        m_url = url;
    }
    inline string getUrl() const
    {
        return m_url;
    }
    inline void setVersion(string version)
    {
        m_version = version;
    }
    inline string getVersion() const
    {
        return m_version;
    }
    bool shouldKeepAlive() const;
    // 获取处理状态
    inline PrecessState getState()
    {
        return m_curState;
    }
    inline void setState(PrecessState state)
    {
        m_curState = state;
    }
    // 便于扩展：直接设置请求行
    void setRequestLine(const string &method, const string &url, const string &version)
    {
        m_method = method;
        m_url = url;
        m_version = version;
    }

private:
    char *splitRequestLine(const char *start, const char *end,
                           const char *sub, function<void(string)> callback);
    int hexToDec(char c);

private:
    string m_method;
    string m_url;
    string m_version;
    map<string, string> m_reqHeaders;
    PrecessState m_curState;
};
