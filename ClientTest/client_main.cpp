//Authors: Brad Vidler, Lucas Buis
//Filename: client_main.cpp
//Purpose: Demonstrates communication with a server which echos messages
//Date: Friday, April 13th, 2012

#include "Socket.h"
#include <iostream>
#include <process.h>
using namespace std;

unsigned __stdcall Send(void* a) 
{
	Socket* s = (Socket*) a;

	string msg;
	cout << "Enter a message to send: ";
	getline(cin, msg);
	s->SendLine(msg);

	return 0;
}

unsigned __stdcall Receive(void* a) 
{
	Socket* s = (Socket*) a;

    while (1) 
    {
		string response =  s->GetLine();
		if ((response != "\n") && (response != "") && (response != " "))
		{
			cout << response;
			Send(a);
		}
    }
}

int main(int argc, char* argv[])
{
	try
	{
		cout << "I am a client!\n";

		SocketClient sc("localhost", 2000, "TCP");

		SocketClient* s = &sc;
		cout << "Connected to server!\n";

		string msg;

		unsigned ret;
		_beginthreadex(0,0,Receive,(void*) s,0,&ret);
		//_beginthreadex(0,0,Send,(void*) s,0,&ret);
		
		while (1) 
		{
			
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