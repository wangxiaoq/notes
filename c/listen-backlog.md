### listen函数的backlog参数

```
int listen(int sockfd, int backlog);
```

listen函数的作用是将套接字由主动状态变为被动状态，第一个参数表示socket fd，第二个参数表示的是已完成TCP三次握手，等待accept的最大客户端的数目，亲测可靠。例子：

```
#include <event.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <strings.h>
#include <iostream>

#define PORT 34567
#define BACKLOG 1

void on_accept(int fd, short events, void *arg)
{
    std::cout << "on_accept" << std::endl;
}

int main(int argc, char *argv[])
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    int on = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    struct sockaddr_in saddr;
    bzero(&saddr, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(PORT);
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    bind(fd, reinterpret_cast<struct sockaddr*>(&saddr), sizeof(saddr));
    listen(fd, BACKLOG);

    event listen_ev;
    event_set(&listen_ev, fd, EV_READ|EV_PERSIST, on_accept, NULL);
    event_base *base = event_base_new();
    event_base_set(base, &listen_ev);
    event_add(&listen_ev, NULL);
    event_base_dispatch(base);

    return 0;
}
```

此程序为服务器程序，只不过不进行accept。将backlog设置为2，也就是说能与该程序建立完全TCP连接的客户端最多为两个。
编译程序后运行，打开另外三个终端窗口，分别在其中运行如下命令：

```
telnet 127.0.0.1 34567
```

使用telnet作为客户端连接该程序。打开新的终端窗口查看连接建立的情况：

```
tcp        2      0 0.0.0.0:34567           0.0.0.0:*               LISTEN     
tcp        0      0 127.0.0.1:43064         127.0.0.1:34567         ESTABLISHED
tcp        0      0 127.0.0.1:43062         127.0.0.1:34567         ESTABLISHED
tcp        0      0 127.0.0.1:43060         127.0.0.1:34567         ESTABLISHED
tcp        0      0 127.0.0.1:34567         127.0.0.1:43062         ESTABLISHED
tcp        0      0 127.0.0.1:34567         127.0.0.1:43060         ESTABLISHED
tcp        0      0 127.0.0.1:34567         127.0.0.1:43064         SYN_RECV  
```

可以看到建立双向连接的只有两个客户端，第三个客户端只是建立的单向连接，服务器状态为SYN_RECV，表示已收到客户端的SYN请求包，但未完全建立三次握手。

*ref*

*https://blog.csdn.net/yangbodong22011/article/details/60399728*
