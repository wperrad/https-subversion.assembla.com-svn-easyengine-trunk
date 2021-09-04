#include <iostream>
#include <boost/asio.hpp>
#include <vector>

//#include "../SocketUtils.h"

using namespace boost::asio;
using ip::tcp;
using std::string;
using std::cout;
using std::endl;

const int APPOTRONICS_MSG_LENGTH = 12;

BYTE msg_open[APPOTRONICS_MSG_LENGTH] = { 0xeb , 0x90, 0x00, 0x0c, 0x00, 0x00, 0x08, 0x02, 0x00, 0x01, 0x01, 0x93 };
BYTE msg_close[APPOTRONICS_MSG_LENGTH] = { 0xeb , 0x90, 0x00, 0x0c, 0x00, 0x00, 0x08, 0x01, 0x00, 0x01, 0x00, 0x91 };
BYTE msg_quit[APPOTRONICS_MSG_LENGTH] = { 0x00 , 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

boost::asio::io_context iocontext;
boost::asio::ip::tcp::socket mTcpSocket(iocontext);

void serializeMessage(const BYTE* bin_message, string& message, int message_count)
{
	for (int i = 0; i < message_count; ++i) {
		message.push_back(bin_message[i]);
	}
}

bool writeIntoSocket(const BYTE* command_bytes_to_send)
{
	boost::system::error_code ec;
	string command_string_to_send;
	for (int i = 0; i < APPOTRONICS_MSG_LENGTH; ++i) {
		command_string_to_send.push_back(command_bytes_to_send[i]);
	}

	boost::asio::write(mTcpSocket, boost::asio::buffer(command_string_to_send), ec);
	if (ec)
	{
		return false;
	}
	return true;
}

bool writeIntoSocket(const std::string& command_string_to_send)
{
	boost::system::error_code ec;
	boost::asio::write(mTcpSocket, boost::asio::buffer(command_string_to_send), ec);
	if (ec)
	{
		return false;
	}
	return true;
}


int main(int argc, char** argv) {
	if (argc < 3) {
		cout << "Usage : client ip port\nPress ENTER to quit.";
		std::cin.get();
		return 0;
	}
	char* ip = argv[1];
	char* sport = argv[2];
	int port = atoi(sport);

	//connection
	bool bConnectToServer = false;
	cout << "Connection au projecteur\n";
	while (!bConnectToServer) {
		try {
			mTcpSocket.connect(tcp::endpoint(boost::asio::ip::address::from_string(ip), port));
			bConnectToServer = true;
		}
		catch (boost::system::system_error& e) {
			cout << "Impossible de joindre le projecteur, nouvelle tentative dans 5 seconde\n";
			Sleep(5000);
		}
	}
	// request/message from client
	const string msg = "Hello from Client!\n";
	boost::system::error_code error;
	
	
	bool bContinue = true;
	
	while (bContinue) {
		string message;
		cout << "Entrez 'O' pour open, 'C' pour close et 'Q' pour quitter : ";
		char c;
		std::cin >> c;
		if (c == 'o' || c == 'O') {
			serializeMessage(msg_open, message, APPOTRONICS_MSG_LENGTH);
			writeIntoSocket(message);
		}
		else if (c == 'c' || c == 'C') {
			serializeMessage(msg_close, message, APPOTRONICS_MSG_LENGTH);
			writeIntoSocket(message);
		}
		else if (c == 'q' || c == 'Q') {
			mTcpSocket.send(boost::asio::buffer(msg_quit));
			bContinue = false;
		}
	}
	
	return 0;
}