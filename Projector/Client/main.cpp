#include <iostream>
#include <boost/asio.hpp>
#include <vector>



using namespace boost::asio;
using ip::tcp;
using std::string;
using std::cout;
using std::endl;


int main(int argc, char** argv) {
	if (argc < 3) {
		cout << "Usage : client ip port\nPress ENTER to quit.";
		std::cin.get();
		return 0;
	}
	char* ip = argv[1];
	char* sport = argv[2];
	int port = atoi(sport);

	//boost::asio::io_service io_service;
	boost::asio::io_context io_context;
	//socket creation
	tcp::socket socket(io_context);
	//connection
	bool bConnectToServer = false;
	cout << "Connection au projecteur\n";
	while (!bConnectToServer) {
		try {
			socket.connect(tcp::endpoint(boost::asio::ip::address::from_string(ip), port));
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
	
	BYTE msg_open[12] = { 0xeb , 0x90, 0x00, 0x0c, 0x00, 0x00, 0x08, 0x02, 0x00, 0x01, 0x01, 0x93 };
	BYTE msg_close[12] = { 0xeb , 0x90, 0x00, 0x0c, 0x00, 0x00, 0x08, 0x01, 0x00, 0x01, 0x00, 0x91 };
	BYTE msg_quit[12] = { 0x00 , 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	bool bContinue = true;
	while (bContinue) {
		cout << "Entrez 'O' pour open, 'C' pour close et 'Q' pour quitter : ";
		char c;
		std::cin >> c;
		if (c == 'o' || c == 'O') {
			socket.send(boost::asio::buffer(msg_open));
		}
		else if (c == 'c' || c == 'C') {
			socket.send(boost::asio::buffer(msg_close));
		}
		else if (c == 'q' || c == 'Q') {
			socket.send(boost::asio::buffer(msg_quit));
			bContinue = false;
		}
	}
	
	return 0;
}