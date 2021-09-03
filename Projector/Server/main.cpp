#include <iostream>
#include <boost/asio.hpp>
#include <vector>

using namespace boost::asio;
using ip::tcp;
using std::string;
using std::cout;
using std::endl;

using namespace std;

string read_(tcp::socket & socket) {
	boost::asio::streambuf buf;
	boost::asio::read_until(socket, buf, "\n");
	string data = boost::asio::buffer_cast<const char*>(buf.data());
	return data;
}

void send_(tcp::socket & socket, const string& message) {
	const string msg = message + "\n";
	boost::asio::write(socket, boost::asio::buffer(message));
}

/*
void sendBinary(tcp::socket & socket, vector<BYTE>& data) {
	//const string msg = message + "\n";
	boost::asio::write(socket, boost::asio::buffer(data));	
}
*/
void receiveBinary(tcp::socket & socket, vector<BYTE>& binData) {
	boost::asio::streambuf buf;
	//boost::asio::read_until(socket, buf, "\n");
	//boost::asio::read()
	BYTE d1[12];
	size_t bytes_transferred = socket.receive(boost::asio::buffer(d1));
	
	

	//const BYTE* datas = boost::asio::buffer_cast<const BYTE*>(buf.data());

	for (int i = 0; i < 3; i++) {
		binData.push_back(d1[i]);
	}
}

void listen()
{

}

int main() {
	//boost::asio::io_service service;
	boost::asio::io_context context;
	//listen for new connection
	try {
		tcp::acceptor acceptor_(context, tcp::endpoint(tcp::v4(), 1978));
		//socket creation 
		tcp::socket socket_(context);
		//waiting for connection
		acceptor_.accept(socket_);

		BYTE msg_open[12] = { 0xeb , 0x90, 0x00, 0x0c, 0x00, 0x00, 0x08, 0x02, 0x00, 0x01, 0x01, 0x93 };
		BYTE msg_close[12] = { 0xeb , 0x90, 0x00, 0x0c, 0x00, 0x00, 0x08, 0x01, 0x00, 0x01, 0x00, 0x91 };
		BYTE msg_quit[12] = { 0x00 , 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
		bool bContinue = true;
		int compare = 0;
		while (bContinue) {
			BYTE d1[12];
			size_t bytes_transferred = socket_.receive(boost::asio::buffer(d1));
			if (!memcmp(d1, msg_open, 12)) {
				cout << "Open received";
			}
			else if (!memcmp(d1, msg_close, 12)) {
				cout << "Close received";
			}
			else if (!memcmp(d1, msg_quit, 12)) {
				bContinue = false;
			}
			cout << "\n";
		}
	}
	catch (boost::system::system_error& e) {
		const char* message = e.what();
		MessageBoxA(nullptr, e.what(), "", MB_OK);

	}

	//read operation
	//string message = read_(socket_);
	//cout << message << endl;
	//write operation
	//send_(socket_, "Hello From Server!");
	//cout << "Servent sent Hello message to Client!" << endl;
	



	return 0;
}
