//Authors: Brad Vidler, Lucas Buis
//Filename: Socket.h
//Purpose: Header file of socket library
//Date: Friday, April 13th, 2012

#ifndef SOCKET_H
#define SOCKET_H

#include <WinSock2.h>
#include <string>

using namespace std;

enum TypeSocket {BlockingSocket, NonBlockingSocket};

class Socket 
{
public:
	virtual ~Socket();
    Socket(const Socket&);

    string GetLine();

    void Close();

    void   SendLine (string);

protected:
    friend class SocketServer;
    friend class SocketSelect;

    Socket(SOCKET s);
    Socket();
    Socket(string Protocol);

    SOCKET s_;

private:
    static int WSAStart();
    static void Cleanup();
    static int  nSocks_;
};

class SocketClient : public Socket 
{
public:
    SocketClient(const string& host, int port, string Protocol);
};

class SocketServer : public Socket 
{
public:
    SocketServer(int port, int connections, TypeSocket type=BlockingSocket);
    Socket* Accept();
	int ListenOnPort(int portno);
	LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
};

#endif