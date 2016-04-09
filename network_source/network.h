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
#include <vector>

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

struct Packet
{
	static const int MAX_PACKET_SIZE = 2048;

	Packet& operator<<(const std::string& a);

	Packet& operator<<(int8_t a);
	Packet& operator<<(int16_t a);
	Packet& operator<<(int32_t a);
	Packet& operator<<(int64_t a);

	Packet& operator<<(uint8_t a);
	Packet& operator<<(uint16_t a);
	Packet& operator<<(uint32_t a);
	Packet& operator<<(uint64_t a);

	Packet& operator<<(float a);
	Packet& operator<<(double a);

	Packet& operator>>(std::string& a);

	Packet& operator>>(int8_t& a);
	Packet& operator>>(int16_t& a);
	Packet& operator>>(int32_t& a);
	Packet& operator>>(int64_t& a);

	Packet& operator>>(uint8_t& a);
	Packet& operator>>(uint16_t& a);
	Packet& operator>>(uint32_t& a);
	Packet& operator>>(uint64_t& a);

	Packet& operator>>(float& a);
	Packet& operator>>(double& a);

	bool isPacked(); //returns if the size of a data is bigger than maximum size
	void setCursor();
	void revertToCursor();
	void reset();
private:
	std::vector<char> data;
	int cursorPosition; //holds size of a vector to revert	
	void checkType(char type);
	enum class PrimitiveTypes
	{
		STRING,
		INT8,
		INT16,
		INT32,
		INT64,
		UINT8,
		UINT16,
		UINT32,
		UINT64,
		FLOAT,
		DOUBLE
	};
	friend class UdpSocket;
};

struct UdpSocket
{
	UdpSocket();
	~UdpSocket();
	bool isCreated();
	bool bind(uint16_t port);
	bool setNonBlocking();
	bool send(const IpAddress& address, const char* data, int dataSize);
	bool send(const IpAddress& address, const Packet& packet);
	int receive(char* buffer, int size, IpAddress& address);
	bool receive(Packet& packet, IpAddress& address);
	static bool initializeSockets();
	static void shutdownSockets();
private:
	bool created;
	int fd;
	static bool socketsInitialized;
};



#endif