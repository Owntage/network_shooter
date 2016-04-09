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

Packet& Packet::operator<<(const std::string& a)
{
	int oldDataSize = data.size();
	data.resize(data.size() + a.size() + 1);
	data[oldDataSize] = '\0'; //it is like a c-string, but null-character is in the beginning
	std::copy(a.begin(), a.end(), data.begin() + oldDataSize + 1);
	data.push_back(static_cast<char>(PrimitiveTypes::STRING));
	return *this;
}

Packet& Packet::operator<<(int8_t a)
{
	data.push_back(a);
	data.push_back(static_cast<int>(PrimitiveTypes::INT8));
	return *this;
}

Packet& Packet::operator<<(uint8_t a)
{
	data.push_back(a);
	data.push_back(static_cast<int>(PrimitiveTypes::INT16));
	return *this;
}

Packet& Packet::operator<<(int16_t a)
{
	data.resize(data.size() + 2);
	*( (int16_t*) &data[data.size() - 2]) = htons(a);
	data.push_back(static_cast<char>(PrimitiveTypes::INT16));
	return *this;
}

Packet& Packet::operator<<(uint16_t a)
{
	*this << (int16_t) a;
	data.back() = static_cast<char>(PrimitiveTypes::UINT16);
	return *this;
}

Packet& Packet::operator<<(int32_t a)
{
	data.resize(data.size() + 4);
	*( (int32_t*) &data[data.size() - 4]) = htonl(a);
	data.push_back(static_cast<char>(PrimitiveTypes::INT32));
	return *this;
}

Packet& Packet::operator<<(uint32_t a)
{
	*this << (int32_t) a;
	data.back() = static_cast<char>(PrimitiveTypes::UINT32);
	return *this;
}

Packet& Packet::operator<<(int64_t a)
{
	std::string a_str = stringConverter<int64_t, std::string>(a);
	*this << a_str;
	data.back() = static_cast<char>(PrimitiveTypes::INT64);
	return *this;
}

Packet& Packet::operator<<(uint64_t a)
{
	std::string a_str = stringConverter<uint64_t, std::string>(a);
	*this << a_str;
	data.back() = static_cast<char>(PrimitiveTypes::UINT64);
	return *this;
}

Packet& Packet::operator<<(float a)
{
	std::string a_str = stringConverter<float, std::string>(a);
	*this << a_str;
	data.back() = static_cast<char>(PrimitiveTypes::FLOAT);
	return *this;
}

Packet& Packet::operator<<(double a)
{
	std::string a_str = stringConverter<double, std::string>(a);
	*this << a_str;
	data.back() = static_cast<char>(PrimitiveTypes::DOUBLE);
	return *this;
}

void Packet::checkType(char type)
{
	if(data.back() != type)
	{
		std::cout << "warning! trying to read the wrong type (" << (int) type << ")" << std::endl;
		std::cout << "actual type: " << (int) data.back() << std::endl;
	}
}

Packet& Packet::operator>>(std::string& a)
{
	checkType((char) PrimitiveTypes::STRING);
	data.pop_back();
	int ptr = data.size() - 1;
	while(ptr >= 0 && data[ptr] != '\0')
	{
		ptr--;
	}
	a.assign(data.begin() + ptr + 1, data.end());
	data.resize(data.size() - a.size() - 1);
	return *this;
}

Packet& Packet::operator>>(int8_t& a)
{
	checkType((char) PrimitiveTypes::INT8);
	data.pop_back();
	a = data.back();
	data.pop_back();
	return *this;
}

Packet& Packet::operator>>(uint8_t& a)
{
	checkType((char) PrimitiveTypes::UINT8);
	*this >> (int8_t&) a;
	return *this;
}

Packet& Packet::operator>>(int16_t& a)
{
	checkType((char) PrimitiveTypes::INT16);
	data.pop_back();
	a = ntohs(*((int16_t*) &data[data.size() - 2]));
	data.resize(data.size() - 2);
	return *this;
}

Packet& Packet::operator>>(uint16_t& a)
{
	checkType((char) PrimitiveTypes::UINT16);
	data.back() = (char) PrimitiveTypes::INT16;
	*this >> (int16_t&) a;
	return *this;
}

Packet& Packet::operator>>(int32_t& a)
{
	checkType((char) PrimitiveTypes::INT32);
	data.pop_back();
	a = ntohl(*((int32_t*) &data[data.size() - 4]));
	data.resize(data.size() - 4);
	return *this;
}

Packet& Packet::operator>>(uint32_t& a)
{
	checkType((char) PrimitiveTypes::UINT32);
	data.back() = (char) PrimitiveTypes::INT32;
	*this >> (int32_t&) a;
	return *this;
}

Packet& Packet::operator>>(int64_t& a)
{
	std::string a_str;
	checkType((char) PrimitiveTypes::INT64);
	data.back() = (char) PrimitiveTypes::STRING;
	*this >> a_str;
	a = stringConverter<std::string, int64_t>(a_str);
	return *this;
}

Packet& Packet::operator>>(uint64_t& a)
{
	std::string a_str;
	checkType((char) PrimitiveTypes::UINT64);
	data.back() = (char) PrimitiveTypes::STRING;
	*this >> a_str;
	a = stringConverter<std::string, uint64_t>(a_str);
	return *this;
}

Packet& Packet::operator>>(float& a)
{
	std::string a_str;
	checkType((char) PrimitiveTypes::FLOAT);
	data.back() = (char) PrimitiveTypes::STRING;
	*this >> a_str;
	a = stof(a_str);
	return *this;
}

Packet& Packet::operator>>(double& a)
{
	std::string a_str;
	checkType((char) PrimitiveTypes::DOUBLE);
	data.back() = (char) PrimitiveTypes::STRING;
	*this >> a_str;
	a = stod(a_str);
	return *this;
}

bool Packet::isPacked()
{
	return data.size() > MAX_PACKET_SIZE;
}

void Packet::setCursor()
{
	cursorPosition = data.size();
}

void Packet::revertToCursor()
{
	if(data.size() > cursorPosition)
	{
		data.resize(cursorPosition);
	}
}

void Packet::reset()
{
	data.clear();
	cursorPosition = 0;
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

bool UdpSocket::send(const IpAddress& address, const Packet& packet)
{
	return send(address, &packet.data[0], packet.data.size());
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

bool UdpSocket::receive(Packet& packet, IpAddress& receiveAddress)
{
	char buffer[Packet::MAX_PACKET_SIZE];
	int bytes = receive(buffer, Packet::MAX_PACKET_SIZE, receiveAddress);
	if(bytes <= 0)
	{
		return false;
	}
	packet.reset();
	packet.data.resize(bytes);
	std::copy(buffer, buffer + bytes, packet.data.begin());
	return true;
}









