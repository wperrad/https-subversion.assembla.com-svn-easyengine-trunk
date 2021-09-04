#include <iostream>
#include <boost/asio.hpp>
#include <vector>
#include <sstream>

using namespace boost::asio;
using ip::tcp;
using std::string;
using std::cout;
using std::endl;

using namespace std;

const int APPOTRONICS_MSG_LENGTH = 12;
const int APPOTRONICS_INFO_LENGTH = 9;

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


void convertBinToText(const BYTE* data, string& text, int byteCount)
{
	for (int i = 0; i < byteCount; ++i) {
		int b = data[i];
		int h = b >> 4;	
		int l = b - (b >> 4 << 4);
		ostringstream oss;
		oss << std::hex << h << std::hex << l << " ";
		text += oss.str();
	}
}

void convertBinToText(const string& data, string& text)
{
	for (int i = 0; i < data.size(); ++i) {
		int b = data[i];
		int h = b >> 4;
		int l = b - (b >> 4 << 4);
		ostringstream oss;
		oss << std::hex << h << std::hex << l << " ";
		text += oss.str();
	}
}

int main() {
	boost::asio::io_context context;
	//listen for new connection
	try {
		tcp::acceptor acceptor_(context, tcp::endpoint(tcp::v4(), 9761));
		//socket creation 
		tcp::socket socket_(context);
		//waiting for connection
		acceptor_.accept(socket_);
		cout << "Connexion entrante";

		BYTE msg_open[12] = { 0xeb , 0x90, 0x00, 0x0c, 0x00, 0x00, 0x08, 0x02, 0x00, 0x01, 0x01, 0x93 };
		BYTE msg_close[12] = { 0xeb , 0x90, 0x00, 0x0c, 0x00, 0x00, 0x08, 0x01, 0x00, 0x01, 0x00, 0x91 };
		BYTE msg_quit[12] = { 0x00 , 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
		bool bContinue = true;
		int compare = 0;
		string infos = "projecteur";
		while (bContinue) {
			
			BYTE d1[12];
			boost::system::error_code ec;
			boost::asio::streambuf response;
			size_t bytes_transferred = boost::asio::read(socket_, boost::asio::buffer(d1), ec); // no condition, so read till EOF
			if (bytes_transferred == 0)
				continue;
			string text;
			convertBinToText(d1, text, bytes_transferred);
			cout << "\nMessage recu : " << text;
			if (bytes_transferred == APPOTRONICS_MSG_LENGTH) {
				if (!memcmp(d1, msg_open, 12)) {
					cout << "\nOpen received";
				}
				else if (!memcmp(d1, msg_close, 12)) {
					cout << "\nClose received";
				}
				else if (!memcmp(d1, msg_quit, 12)) {
					bContinue = false;
				}
			}
			else {
				//std::stringstream oss = response;
				using boost::asio::buffers_begin;

				auto bufs = response.data();
				std::string result(buffers_begin(bufs), buffers_begin(bufs) + response.size());
				
				cout << result;
			}
		}
	}
	catch (boost::system::system_error& e) {
		const char* message = e.what();
		MessageBoxA(nullptr, e.what(), "", MB_OK);

	}
	return 0;
}
