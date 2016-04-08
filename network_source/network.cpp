#include "network.h"
#include <iostream>
#include <cctype>
#include <algorithm>
#include <sstream>
#include <regex>

bool UdpSocket::socketsInitialized = false;

int numberOfDigits(const std::string& addr, int offset)
{
	int result = 0;
	while(offset + result < addr.size() && std::isdigit(addr[offset + result]))
	{
		result++;
	}
	return result;
}

std::stringstream converter_stream;

template<typename FROM_T, typename TO_T>
static TO_T stringConverter(FROM_T t)
{
	TO_T result;
	converter_stream << t;
	converter_stream >> result;
	return result;
}

IpAddress::IpAddress(const std::string& addr, uint16_t port)
{
	this->port = htons(port);
	uint32_t numbers[4];
	if(addr == "localhost")
	{
		correct = true;
		numbers[0] = 127;
		numbers[1] = 0;
		numbers[2] = 0;
		numbers[3] = 1;
	}
	else
	{
		std::regex addr_regex("^(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$"); 
		if(std::regex_match(addr, addr_regex))
		{
			int offset = 0;
			int length;
			for(int i = 0; i < 4; i++)
			{
				length = numberOfDigits(addr, offset);
				numbers[i] = stringConverter<std::string, int>(addr.substr(offset, length));
				offset += length + 1;
			}
			correct = true;
		}
		else
		{
			std::cout << "address string does not match address regex" << std::endl;
			correct = false;
		}
	}
	address = htonl( (numbers[0] << 24) | (numbers[1] << 16) | (numbers[2] << 8) | numbers[3]);
}

bool IpAddress::isCorrect()
{
	return correct;
}

bool UdpSocket::initializeSockets()
{
	if(!socketsInitialized)
	{

	#if PLATFORM == PLATFORM_WINDOWS
	WSADATA wsaData;
	socketsInitialized = WSAStartup(MAKEWORD(2, 2), &wsaData) == NO_ERROR;
	return socketsInitialized;
	#else
	socketsInitialized = true;
	return true;
	#endif

	}
	else
	{
		return true;
	}
}

void UdpSocket::shutdownSockets()
{
	#if PLATFORM == PLATFORM_WINDOWS
	WSACleanup();
	#endif
}

UdpSocket::UdpSocket()
{
	bool initResult = initializeSockets();
	fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(fd <= 0 || !initResult)
	{
		created = false;
		std::cout << "failed to create udp socket" << std::endl;
	}
	else
	{
		created = true;

	}
}



UdpSocket::~UdpSocket()
{
	#if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
	close(fd);
	#elif PLATFORM == PLATFORM_WINDOWS
	closesocket(fd);
	#endif
}

bool UdpSocket::isCreated()
{
	return created;
}

bool UdpSocket::bind(uint16_t port)
{
	sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);
	if(::bind(fd, (const sockaddr*) &address, sizeof(sockaddr_in)) < 0)
	{
		std::cout << "failed to bind socket to port: " << port << std::endl;
		return false;
	}
	else
	{
		return true;
	}
}

bool UdpSocket::setNonBlocking()
{
	#if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX

	int nonBlocking = 1;
	if (fcntl( handle, F_SETFL, O_NONBLOCK, nonBlocking ) == -1)
	{
		std::cout << "failed to set socket into non-blocking mode" << std::endl;
		return false;
	}

	#elif PLATFORM == PLATFORM_WINDOWS

	DWORD nonBlocking = 1;
	if (ioctlsocket( fd, FIONBIO, &nonBlocking ) != 0)
	{
		std::cout << "failed to set socket into non-blocking mode" << std::endl;
		return false;
	}

	#endif
}

bool UdpSocket::send(const IpAddress& address, const char* data, int dataSize)
{
	sockaddr_in send_address;
	send_address.sin_family = AF_INET;
	send_address.sin_addr.s_addr = address.address;
	send_address.sin_port = address.port;
	int sent_bytes = sendto( fd, data, dataSize, 0, (sockaddr*) &send_address, sizeof(sockaddr_in));

	if (sent_bytes != dataSize)
	{
		std::cout << "failed to send data" << std::endl;
		return false;
	}
	return true;
}

int UdpSocket::receive(char* data, int size, IpAddress& receiveAddress)
{
	#if PLATFORM == PLATFORM_WINDOWS
	typedef int socklen_t;
	#endif

	sockaddr_in from;
	socklen_t fromLength = sizeof(from);

	int bytes = recvfrom(fd, data, size, 0, (sockaddr*) &from, &fromLength);

	if(bytes <= 0)
	{
		return bytes;
	}

	receiveAddress.address = from.sin_addr.s_addr;
	receiveAddress.port = from.sin_port;

	return bytes;

}









