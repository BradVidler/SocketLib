//Authors: Brad Vidler, Lucas Buis
//Filename: server_main.cpp
//Purpose: Demonstrates a server which echos messages from clients
//Date: Friday, April 13th, 2012

#include "Socket.h"
#include <iostream>
#include <windows.h>
#include <process.h>
#include <string>
using namespace std;

unsigned __stdcall Answer(void* a) 
{
  Socket* s = (Socket*) a;

  while (1) 
  {
    std::string r = s->GetLine();
    if (r.empty())
		break;
	else if ((r != "\n") && (r != "") && (r != " "))
	{
		cout << "Message from client: " + r;
		s->SendLine("Response from server: " + r);
	}
  }

  delete s;

  return 0;
}

int main(int argc, char* argv[])
{
	//HWND Window;
	//Window = AllocConsole();
	//	ShowWindow( Window, SW_HIDE ); 
	try
	{
		cout << "I am a server!\n";

		SocketServer ss(2000, 5);
		cout << "Socket created!\n";

		cout << "Listening! Try sending me a message!\n";
		while (1) 
		{
			Socket* s=ss.Accept();

			unsigned ret;
			_beginthreadex(0,0,Answer,(void*) s,0,&ret);

			s->SendLine("Welcome to the server!");
		}
	}
	catch (const char* s)
	{
		cout << s << endl;
		cout << "ERROR\n";
	}
	catch (std::string s) 
	{
		cout << s << endl;
		cout << "ERROR\n";
    } 
    catch (...) 
	{
        cout << "unhandled exception\n";
    }
	//return 0;
}