#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include "scenes.h"
#include "render_window.h"
#include "gui.h"
#include <iostream>
#include <memory>
#include <string>
#include <chrono>
#include <sstream>
#include <network.h>
using namespace std;

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define CONSOLE_WIDTH 400
#define CONSOLE_HEIGHT 100



int main()
{
	/*
	UdpSocket::initializeSockets();

	UdpSocket socket;
	IpAddress address("localhost", 13337);
	Packet packet;
	packet << 1.0f << 2.0f << 3.0f << 4.0f << 5.0f;
	socket.send(address, packet);

	UdpSocket::shutdownSockets();
	*/

	File file("res/graphic/jumping.png");
	
	File file2;
	file2.setSize(file.size);

	Packet packet;
	
	while(!file2.isDownloaded())
	{
		packet.reset();
		file.printToPacket(packet, file2.mask);
		packet >> file2;
	}

	
	file2.saveToFile("res/graphic/concr1_copy.png");

	return 0;
}