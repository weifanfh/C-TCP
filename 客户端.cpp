#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>

#pragma comment (lib, "Ws2_32.lib")//����������Ws2_32.lib�ļ�
#pragma comment (lib, "Mswsock.lib")//����������Mswsock.lib�ļ�
#pragma comment (lib, "AdvApi32.lib")//����������Mswsock.lib�ļ�
#pragma comment (lib, "AdvApi32.lib")//����������Mswsock.lib�ļ�

using namespace std;

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "23943"

int __cdecl main()//int argc, char** argv)
/*��������ʵ�Σ�һ������ֵ��һ��ָ���ַ���ָ���ָ��
argc ��argument count����д��ʾ����main�����еĲ����������������������
argv �� argument vector����д��ʾ����main�����еĲ����б�����argv[0]��ʾ������������*/
{
    //argc = 2;
    const char* ip = "127.0.0.1";
    WSADATA wsaData;//����һ��WSADATA����
    SOCKET ConnectSocket = INVALID_SOCKET;//����һ���׽��ֶ����ҳ�ʼ��Ϊ��INVALID_SOCKET��һ��ֵ��
    struct addrinfo* result = NULL,//����3��ָ��ṹ���ָ��result��* ptr��hints
        * ptr = NULL,
        hints;
    const char* sendbuf = "����һ��TCP������ԣ�C++�ͻ��˷�����Ϣ��python�����";//����һ��ָ��char�������͵�ָ�루�ֽ�����pythonһ��6���ֽڣ����຺��48���ֽڣ�
    //const char* sendbuf = "����";
    char recvbuf[DEFAULT_BUFLEN]="\0";//����һ������512���ַ�������char���ͱ���
    int iResult;//����һ������ֵ��δ��ʼ��
    int recvbuflen = DEFAULT_BUFLEN;//����һ������ֵ�����ҳ�ʼ��δ512

    //�ж��Ƿ�����ip��ַ
    /*
    if (argc != 2) {//���������2������������argv[0]��argv[1]��һ����argv[0]����Ϊ��������֣�argv[1]Ӧ��Ϊ����̨�ֶ�����Ĳ���
        printf("usage: %s server-name\n", argv[0]);
        return 1;
    }
    */

    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);//MAKEWORD(2, 2)��ʾ����2.2�汾��socket�����׽���wsaData������ɹ�����0
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));//ZeroMemory����������0���ڴ������һ����������&hintsΪ��ʼ��ַ��sizeof(hints)Ϊ���ĳ��ȣ����ֽڼ��㣩
    hints.ai_family = AF_UNSPEC;//��ʼ���ṹ��������һ����Ա��ֵ�����õ�ַ��Ϊδָ���ĵ�ַ��
    hints.ai_socktype = SOCK_STREAM;//��ʼ���ṹ��������һ����Ա��ֵ�������׽�������ΪOOB ���ݴ�����Ƶ����򡢿ɿ���˫�򡢻������ӵ��ֽ���
    hints.ai_protocol = IPPROTO_TCP;//��ʼ���ṹ��������һ����Ա��ֵ������Э������ΪTCP����

    iResult = getaddrinfo(ip, DEFAULT_PORT, &hints, &result);
    /*
    getaddrinfo�����ṩ�� ANSI ����������ַ�Ķ�����Э���ת��������ɹ�����0
    argv[1]��ip��ַ��
    DEFAULT_PORT���˿ںţ�
    &hints��һ��ָ��addrinfo�ĵ�ַ����������׽��ֵ����ò�����
    &result�����а����й���������Ӧ��Ϣ
    */
    if (iResult != 0) {//������ɹ�������
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) 
    {
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,ptr->ai_protocol);//���λ�ȡptr�е�ai_family��ai_socktype��ai_protocol��Ϊ�������ڴ���һ���󶨵��ض���������ṩ�ߵ��׽���
        /*
        ����ڴ����׽���ConnectSocketʱû�д���ʱ���أ������׽��ֵ���������ConnectSocket�������������ʱ�᷵��INVALID_SOCKET����������
        */
        if (ConnectSocket == INVALID_SOCKET) 
        {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            return 1;
        }

        /*
        connect��������������ָ���׽��ֵ����ӣ��ɹ�ʱ���أ�0����������ʱ���أ�SOCKET_ERROR�������˳�
        ConnectSocket��δ�����׽��ֵ��������ı�ʶ
        ptr->ai_addr����ȡptr�е���Աai_addr
        ptr->ai_addrlen�����ֽ�Ϊ��λ��ʾָ���sockaddr�ṹ�ĳ���
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

    freeaddrinfo(result);//freeaddrinfo�����ͷ� getaddrinfo������addrinfo�ṹ�ж�̬����ĵ�ַ��Ϣ

    /*
    �ڷ�����Ϣǰ���ж������Ƿ��жϣ�����ж��򱨴�
    */
    if (ConnectSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }

    /*
    ���巢�����ݵĲ��ִ���
    ������ͳɹ������ط��͵����ֽ���������ʧ���򱨴�
    ����ӡ���͵��ֽ���
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
        std::cout << "����һ����C++���͸�python����Ϣ��" << sendbuf << "\n";
        printf("���͵��ֽ���: %ld\n", iResult);
    }

    iResult = shutdown(ConnectSocket, SD_SEND);//�ر��׽��ֵķ��͹��ܣ�����ر�ʧ���򱨴�
    if (iResult == SOCKET_ERROR) {//�Ƿ񱨴�
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    /*
    ����������ݵĲ��ִ���
    ������ճɹ������ط��͵����ֽ���������ֽ�������0����ӡ����ֽ���������ֽ�������0����ӡ����ر����ӣ�û���ֽ������ʧ���򱨴�
    */

    do {

        iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0)
        {
            std::cout << "���ǽ��յ�����python����Ϣ��"<<recvbuf << "\n";
            printf("���յ������ݵ��ֽڳ���: %d\n", iResult);
        }
        else
        {
            if (iResult==0)
                { printf("�ر��׽���\n"); }
            else
                { printf("����ʧ�ܲ��Ҵ���һ������: %d\n", WSAGetLastError());}
        }
    } while (iResult > 0);

    /*
    �ر��׽���
    ֹͣWinsock 2 DLL ( Ws2_32.dll )���ʹ��
    */
    closesocket(ConnectSocket);
    WSACleanup();
    return 0;
}