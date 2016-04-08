#ifndef NETWORK_H
#define NETWORK_H

#define PLATFORM_WINDOWS 1
#define PLATFORM_MAC 2
#define PLATFORM_UNIX 3

#if defined(_WIN32)
#define PLATFORM PLATFORM_WINDOWS
#elif defined(__APPLE__)
#define PLATFORM PLATFORM_MAC
#else 
#define PLATFORM PLATFORM_UNIX
#endif

#if PLATFORM == PLATFORM_WINDOWS
#include <WinSock2.h>
#elif PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#endif

#if PLATFORM == PLATFORM_WINDOWS 
#pragma comment( lib, "wsock32.lib")
#endif

#include <stdint.h>
#include <string>

struct IpAddress
{
	IpAddress() : correct(true) {}
	IpAddress(const std::string& addr, uint16_t port);
	uint32_t address; //address is stored in network order
	uint16_t port; //network order
	bool isCorrect();
private:
	bool correct;
};

struct UdpSocket
{
	UdpSocket();
	~UdpSocket();
	bool isCreated();
	bool bind(uint16_t port);
	bool setNonBlocking();
	bool send(const IpAddress& address, const char* data, int dataSize);
	int receive(char* buffer, int size, IpAddress& address);
	static bool initializeSockets();
	static void shutdownSockets();
private:
	bool created;
	int fd;
	static bool socketsInitialized;
};



#endif