// Copyright 2020 Robert Carneiro, Derek Meer, Matthew Tabak, Eric Lujan
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
// associated documentation files (the "Software"), to deal in the Software without restriction,
// including without limitation the rights to use, copy, modify, merge, publish, distribute,
// sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or
// substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
// NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#ifndef _GNET
#define _GNET

#include "../Database/GList.h"
#include "socket.h"
#include <errno.h>
#include <iostream>
#include <map>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/signal.h>
#include <unistd.h>
#include <vector>
/*#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/ssl.h>*/

class GList;

/*
 * THE ENGINE
 *
 * Handshake Client
 * HANDSHAKE_CLIENT|name\|
 *
 * Handshake Server
 * HANDSHAKE_SERVER|name|encKey\|
 *
 * Logout Server
 * LOGOUT_SERVER\|
 *
 * Logout Client
 * LOGOUT_CLIENT\|
 *
 * Bad Request
 * BAD_REQUEST\|
 *
 */

namespace GNet {

class Connection;
class Service;

// Service Arguments Class
class newServiceArgs
{
public:
	class GServer* serverInstance;
	class Connection* cConnection;
	shmea::GList sockData;
	pthread_t* sThread;
	std::string command;
};

class GServer
{
	friend Sockets;
	friend Service;

	GNet::Sockets socks;

	// Key is ip address
	std::map<std::string, Connection*>* clientConnections;
	std::map<std::string, Connection*>* serverConnections;

	int sockfd;
	Connection* localConnection;
	pthread_t* commandThread;
	pthread_t* writerThread;
	pthread_mutex_t* clientMutex;
	pthread_mutex_t* serverMutex;
	pthread_mutex_t* writersMutex;
	pthread_cond_t* writersBlock;
	bool LOCAL_ONLY;
	bool running;
	std::map<std::string, Service*>* service_depot;

	static void* commandLauncher(void*);
	void commandCatcher(void*);

	static void* LaunchInstanceLauncher(void*);
	void LaunchInstanceHelper(void*);

	void wakeWriter();
	static void* ListWLauncher(void*);
	void ListWriter(void*);
	void LaunchLocalInstance(const std::string&);
	void LogoutInstance(Connection*);

	int getSockFD();
	const std::map<std::string, Connection*>& getClientConnections();
	const std::map<std::string, Connection*>& getServerConnections();
	pthread_mutex_t* getClientMutex();
	pthread_mutex_t* getServerMutex();

	bool isConnection(int, const fd_set&);
	Connection* setupNewConnection(int);
	Connection* findExistingConnection(const std::vector<Connection*>&, const fd_set&);

public:
	static const int CONTENT_TYPE = 0;
	static const int RESPONSE_TYPE = 1;
	static const int ACK_TYPE = 2;

	GServer();
	~GServer();

	void NewService(const shmea::GList&, GNet::Connection* = NULL, int = CONTENT_TYPE,
					bool = false);

	Service* ServiceLookup(std::string);
	unsigned int addService(std::string, Service*);
	Connection* getConnection(std::string);
	void LaunchInstance(const std::string&, const std::string&);
	const bool& getRunning();
	void stop();
	void run(bool);
	bool isNetworkingDisabled();

	Connection* getLocalConnection();
	void removeClientConnection(Connection*);
	void removeServerConnection(Connection*);
}; // GServer

class LaunchInstanceHelperArgs
{
public:
	GServer* serverInstance;
	std::string clientName;
	std::string serverIP;
};

}; // GNet

#endif
