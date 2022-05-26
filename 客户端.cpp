#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>

#pragma comment (lib, "Ws2_32.lib")//搜索和链接Ws2_32.lib文件
#pragma comment (lib, "Mswsock.lib")//搜索和链接Mswsock.lib文件
#pragma comment (lib, "AdvApi32.lib")//搜索和链接Mswsock.lib文件
#pragma comment (lib, "AdvApi32.lib")//搜索和链接Mswsock.lib文件

using namespace std;

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "23943"

int __cdecl main()//int argc, char** argv)
/*声明两个实参，一个整型值，一个指向字符串指针的指针
argc 是argument count的缩写表示传入main函数中的参数个数，包括这个程序本身
argv 是 argument vector的缩写表示传入main函数中的参数列表，其中argv[0]表示这个程序的名字*/
{
    //argc = 2;
    const char* ip = "127.0.0.1";
    WSADATA wsaData;//声明一个WSADATA对象
    SOCKET ConnectSocket = INVALID_SOCKET;//声明一个套接字对象并且初始化为：INVALID_SOCKET（一个值）
    struct addrinfo* result = NULL,//声明3个指向结构体的指针result，* ptr，hints
        * ptr = NULL,
        hints;
    const char* sendbuf = "这是一次TCP传输测试：C++客户端发送消息给python服务端";//声明一个指向char常量类型的指针（字节数：python一共6个字节，其余汉字48个字节）
    //const char* sendbuf = "测试";
    char recvbuf[DEFAULT_BUFLEN]="\0";//声明一个含有512个字符的数组char类型变量
    int iResult;//声明一个整型值，未初始化
    int recvbuflen = DEFAULT_BUFLEN;//声明一个整型值，并且初始化未512

    //判断是否输入ip地址
    /*
    if (argc != 2) {//如果不等于2，有两个参数argv[0]和argv[1]，一个是argv[0]，其为程序的名字，argv[1]应该为控制台手动输入的参数
        printf("usage: %s server-name\n", argv[0]);
        return 1;
    }
    */

    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);//MAKEWORD(2, 2)表示调用2.2版本的socket启动套接字wsaData，如果成功返回0
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));//ZeroMemory（）函数用0在内存中填充一块区域，其中&hints为开始地址，sizeof(hints)为填充的长度（以字节计算）
    hints.ai_family = AF_UNSPEC;//初始化结构体中其中一个组员的值，设置地址族为未指定的地址族
    hints.ai_socktype = SOCK_STREAM;//初始化结构体中其中一个组员的值，设置套接字类型为OOB 数据传输机制的有序、可靠、双向、基于连接的字节流
    hints.ai_protocol = IPPROTO_TCP;//初始化结构体中其中一个组员的值，设置协议类型为TCP类型

    iResult = getaddrinfo(ip, DEFAULT_PORT, &hints, &result);
    /*
    getaddrinfo函数提供从 ANSI 主机名到地址的独立于协议的转换，如果成功返回0
    argv[1]：ip地址；
    DEFAULT_PORT：端口号；
    &hints：一个指向addrinfo的地址（里面具有套接字的设置参数）
    &result：其中包含有关主机的响应信息
    */
    if (iResult != 0) {//如果不成功，报错
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) 
    {
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,ptr->ai_protocol);//依次获取ptr中的ai_family，ai_socktype，ai_protocol作为参数用于创建一个绑定到特定传输服务提供者的套接字
        /*
        如果在创建套接字ConnectSocket时没有错误时返回：连接套接字的描述符：ConnectSocket，如果发生错误时会返回INVALID_SOCKET，引发报错
        */
        if (ConnectSocket == INVALID_SOCKET) 
        {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            return 1;
        }

        /*
        connect（）函数建立到指定套接字的连接，成功时返回：0；发生错误时返回：SOCKET_ERROR，报错退出
        ConnectSocket：未连接套接字的描述符的标识
        ptr->ai_addr：获取ptr中的组员ai_addr
        ptr->ai_addrlen：以字节为单位表示指向的sockaddr结构的长度
        */
        iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) 
        {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);//freeaddrinfo（）释放 getaddrinfo函数在addrinfo结构中动态分配的地址信息

    /*
    在发送消息前先判断连接是否中断，如果中断则报错
    */
    if (ConnectSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }

    /*
    定义发送数据的部分代码
    如果发送成功，返回发送的总字节数；发送失败则报错
    随后打印发送的字节数
    */
    iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
    if (iResult == SOCKET_ERROR) {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }
    if (iResult > 0)
    {
        std::cout << "这是一条由C++发送给python的消息：" << sendbuf << "\n";
        printf("发送的字节数: %ld\n", iResult);
    }

    iResult = shutdown(ConnectSocket, SD_SEND);//关闭套接字的发送功能，如果关闭失败则报错
    if (iResult == SOCKET_ERROR) {//是否报错
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    /*
    定义接受数据的部分代码
    如果接收成功，返回发送的总字节数；如果字节数大于0，打印输出字节数；如果字节数等于0，打印输出关闭连接；没有字节则接收失败则报错
    */

    do {

        iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0)
        {
            std::cout << "这是接收的来自python的消息："<<recvbuf << "\n";
            printf("接收到的数据的字节长度: %d\n", iResult);
        }
        else
        {
            if (iResult==0)
                { printf("关闭套接字\n"); }
            else
                { printf("接收失败并且存在一个错误: %d\n", WSAGetLastError());}
        }
    } while (iResult > 0);

    /*
    关闭套接字
    停止Winsock 2 DLL ( Ws2_32.dll )库的使用
    */
    closesocket(ConnectSocket);
    WSACleanup();
    return 0;
}