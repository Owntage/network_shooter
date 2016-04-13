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
#include <fstream>
#include <iostream>
#include <iterator>

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

	Packet() : offset(0) , stringByte((char) PrimitiveTypes::STRING)
	{
		data.push_back((char) PrimitiveTypes::END);
	}

	static const int MAX_PACKET_SIZE = 2048;

	Packet& operator<<(const std::string& a);

	Packet& operator<<(int8_t a);
	//Packet& operator<<(bool a);
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
	//Packet& operator>>(bool& a);
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
	bool isEnd();
	void setCursor();
	void revertToCursor();
	void reset();
	int getSize();
private:
	std::vector<char> data;
	int cursorPosition; //holds size of a vector to revert	
	int offset;
	char stringByte;
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
		DOUBLE,
		BOOL,
		END
	};
	friend class UdpSocket;
};



struct UdpSocket
{
	UdpSocket();
	~UdpSocket();
	bool isCreated();
	bool bind(uint16_t port);
	int getLocalPort();
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


#define FILE_PART_SIZE 256

struct FilePart
{
	FilePart() : isReceived(false) {}
	char bytes[FILE_PART_SIZE];
	bool isReceived;

	template<typename STREAM_T>
	friend STREAM_T& operator<<(STREAM_T& s, FilePart& fp)
	{
		for(int i = 0; i < FILE_PART_SIZE; i++)
		{
			s << (uint8_t) fp.bytes[i];
		}
		return s;
	}

	template<typename STREAM_T>
	friend STREAM_T& operator>>(STREAM_T& s, FilePart& fp)
	{
		for(int i = 0; i < FILE_PART_SIZE; i++)
		{
			uint8_t temp;
			s >> temp;
			fp.bytes[i] = temp;
		}
		return s;
	}
};

struct FileMask
{
	std::vector<bool> mask;

	template<typename STREAM_T>
	friend STREAM_T& operator<<(STREAM_T& s, FileMask& fm)
	{
		s << (int) fm.mask.size();
		for(int i = 0; i < fm.mask.size(); i++)
		{
			s << (uint8_t) fm.mask[i];
		}
		return s;
	}

	template<typename STREAM_T>
	friend STREAM_T& operator>>(STREAM_T& s, FileMask& fm)
	{
		int size;
		s >> size;
		for(int i = 0; i < size; i++)
		{
			
			uint8_t temp;
			s >> temp;
			fm.mask.push_back(temp);
		}
		return s;
	}
};

struct File
{
	File(int size): sizeIsKnown(true)
	{
			setSize(size);
	}
	File(): sizeIsKnown(false) {}

	File(std::string filename)
	{
		std::ifstream file(filename.c_str(), std::ios::in | std::ios::binary);
		
		std::vector<char> buffer((
				std::istreambuf_iterator<char>(file)), 
				(std::istreambuf_iterator<char>()));

		setSize(buffer.size());

		for(int i = 0; i < buffer.size(); i++)
		{
			parts[i / FILE_PART_SIZE].bytes[i % FILE_PART_SIZE] = buffer[i];
		}
	}

	void setSize(int size)
	{
		this->size = size;
		parts.resize(size  / FILE_PART_SIZE + 1);
		mask.mask.resize(size / FILE_PART_SIZE + 1);
		sizeIsKnown = true;
	}
	
	void saveToFile(std::string filename)
	{
		std::ofstream file(filename.c_str(), std::ios::binary | std::ios::out);
		for(int i = 0; i < parts.size(); i++)
		{
			file.write(parts[i].bytes, FILE_PART_SIZE);
		}
	}

	bool isDownloaded() const
	{
		if(!sizeIsKnown) return false;
		
		for(int i = 0; i < parts.size(); i++)
		{
			if(!parts[i].isReceived) return false;
		}
		return true;
	}

	

	template<typename STREAM_T>
	friend STREAM_T& operator>>(STREAM_T& s, File& f)
	{
		int number;
		s >> number;
		while(number != -1)
		{
			std::cout << "reading block." << std::endl;
			std::cout << "number: " << number << std::endl;
			s >> f.parts[number];
			f.parts[number].isReceived = true;
			f.mask.mask[number] = true;
			s >> number;
		}
		return s;
	}

	bool printToPacket(Packet& p, FileMask& fm)
	{
		bool isFull = false;
		int counter = 0;
		for(int i = 0; i < parts.size(); i++)
		{
			
			if(!fm.mask[i])
			{
				
				p.setCursor();
				p << i << parts[i];
				counter++;
				if(p.isPacked())
				{
					p.revertToCursor();
					
					p << (int) -1;
					isFull = true;
					break;
				}
			}
		}
		if(!isFull)
		{
			std::cout << "-1 added to the end of packet" << std::endl;
			p << -1;
		}
		return isFull;
	}
	bool sizeIsKnown;
	int size;
	std::vector<FilePart> parts;
	FileMask mask;
};


#endif