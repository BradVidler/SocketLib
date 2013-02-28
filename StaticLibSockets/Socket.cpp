//Authors: Brad Vidler, Lucas Buis
//Filename: Socket.cpp
//Purpose: Header file of socket library
//Date: Friday, April 13th, 2012

#include "Socket.h"
#include <iostream>

#pragma comment (lib,"ws2_32.lib")

using namespace std;

int Socket::nSocks_= 0;

//automatic WSA startup
int Socket::WSAStart() 
{
	if (!nSocks_) 
    {
		WSADATA	wsaData;
		int iResult = WSAStartup( MAKEWORD(2,2), &wsaData );
		if( iResult != NO_ERROR ) 
			return EXIT_FAILURE;
		else
		{
			++nSocks_;
			return EXIT_SUCCESS;
		}
	}
}

//automatic WSA cleanup
void Socket::Cleanup() 
{
	WSACleanup();
}

//default constructor
Socket::Socket() : s_(0) 
{
	WSAStart();
	s_ = socket(AF_INET,SOCK_STREAM,0); //TCP

	if (s_ == INVALID_SOCKET)
		throw "INVALID_SOCKET";
}

//constructor to allow TCP or UDP
Socket::Socket(string Protocol) : s_(0) 
{
	WSAStart();
	if (Protocol == "TCP")
		s_ = socket(AF_INET,SOCK_STREAM,0);
	else if (Protocol == "TCP")
		s_ = socket(AF_INET,SOCK_DGRAM,0);
	else
		throw "INVALID_SOCKET_PROTOCOL";

	if (s_ == INVALID_SOCKET)
		throw "INVALID_SOCKET";
}

Socket::Socket(SOCKET s) : s_(s) 
{
  WSAStart();
};

//deconstructor
Socket::~Socket() 
{
  --nSocks_;
  if (!nSocks_) 
	  Cleanup();
}

//not sure what this is
Socket::Socket(const Socket& o) 
{
  s_ = o.s_;
  nSocks_++;
}

//Automatic socket cleanup
void Socket::Close() 
{
	closesocket(s_);
}

//used to get a line from the server
string Socket::GetLine() 
{
  string line;
  while (1) 
  {
    char c;

    switch(recv(s_, &c, 1, 0)) 
	{
      case 0: //disconnected, return what we have
        return line;
      case -1:
        return "";
	  default:
		  line += c;
    }

    if (c == '\n') //end of line  
		return line;
  }
}

//used to send a line to the server
void Socket::SendLine(string line) 
{
  line += '\n';
  send(s_,line.c_str(),line.length(),0);
}

//create a server socket for sending data to a client
SocketServer::SocketServer(int port, int connections, TypeSocket type)
{
  sockaddr_in sa;

  memset(&sa, 0, sizeof(sa));

  sa.sin_family = PF_INET;             
  sa.sin_port = htons(port);          
  s_ = socket(AF_INET, SOCK_STREAM, 0); //TCP
  if (s_ == INVALID_SOCKET) {
    throw "INVALID_SOCKET";
  }

  if(type==NonBlockingSocket) 
  {
    u_long arg = 1;
    ioctlsocket(s_, FIONBIO, &arg);
  }

  if (bind(s_, (sockaddr *)&sa, sizeof(sockaddr_in)) == SOCKET_ERROR) {
    closesocket(s_);
    throw "INVALID_SOCKET";
  }
  
  listen(s_, connections);                               
}

//wait for incoming connections to the server
Socket* SocketServer::Accept() 
{
  SOCKET new_sock = accept(s_, 0, 0);
  if (new_sock == INVALID_SOCKET) 
  {
    int rc = WSAGetLastError();
    if(rc==WSAEWOULDBLOCK) 
      return 0;
    else 
      throw "Invalid Socket";
  }

  Socket* r = new Socket(new_sock);
  return r;
}

//client socket to make requests to a server
SocketClient::SocketClient(const string& host, int port, string Protocol) : Socket(Protocol) 
{
	string error;

	// Reserve a buffer to store the error string
	char buff[256];

  hostent *he;
  if ((he = gethostbyname(host.c_str())) == 0) 
  {
    error = strerror_s(buff, 100, errno);
    throw error;
  }

  sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr = *((in_addr *)he->h_addr);
  memset(&(addr.sin_zero), 0, 8); 

  if (::connect(s_, (sockaddr *) &addr, sizeof(sockaddr))) 
  {
    error = strerror_s(buff, 100, WSAGetLastError());
    throw error;
  }
}

//The code below WAS going to be used with WSASyncSelect, however, 
//asyncronous sockets are messy with console apps. 
//Threads are used instead in the apps themselves
#define MY_MESSAGE_NOTIFICATION      1048 //Custom notification message

//This is our message handler/window procedure
LRESULT CALLBACK SocketServer::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) //handle the messages
    {
    case MY_MESSAGE_NOTIFICATION: //Is a message being sent?
        {
            switch (lParam) //If so, which one is it?
            {
            case FD_ACCEPT:
                //Connection request was made
				Accept();
                break;

            case FD_CONNECT:
                //Connection was made successfully
				Socket::SendLine("Welcome to our epic server!");
                break;

            case FD_READ:
                //Incoming data; get ready to receive
                break;

            case FD_CLOSE:
                //Lost the connection
                break;
            }
        }
        break;

        //Other normal window messages here…

    default: //The message doesn't concern us
        return DefWindowProc(hwnd, message, wParam, lParam);
    }
    //break;
}

//Listens on a specified port for incoming connections or data
int SocketServer::ListenOnPort(int portno)
{
    WSAStart();

    SOCKADDR_IN addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons (portno); 

    //Accept a connection from any IP
    addr.sin_addr.s_addr = htonl (INADDR_ANY);  

    s_ = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (s_ == INVALID_SOCKET)
    {
        return false;
    }

    if (bind(s_, (LPSOCKADDR)&addr, sizeof(addr)) == SOCKET_ERROR)
    {
       //bind failed
        return false;
    }

    //start listening and allow as many connections as possible
    listen(s_, SOMAXCONN);

	//set socket to asyncronous A.K.A listen on a separate thread
	WSAAsyncSelect (s_, GetActiveWindow(), MY_MESSAGE_NOTIFICATION, (FD_ACCEPT | FD_CONNECT | FD_READ | FD_CLOSE));
}

