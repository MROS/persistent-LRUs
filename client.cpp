#include "api.h"
#include "transaction.h"
#include <boost/asio.hpp>


using namespace boost::asio::ip;
using namespace boost::asio;
using namespace boost::system;

int main() {
	Transaction tx = {0, 1, 77};
	Api api(tx);

	io_service iosev;
	tcp::socket socket(iosev);
	tcp::endpoint ep(ip::address_v4::from_string("127.0.0.1"), 1881);
	error_code ec;
	socket.connect(ep, ec);
	if (ec) {
		std::cout << system_error(ec).what() << std::endl;
		return -1;
	}
	api.write(socket);
}