#include "api.h"
#include "transaction.h"
#include <boost/asio.hpp>

using namespace boost::asio::ip;
using namespace boost::asio;
using namespace boost::system;

int main() {
	io_service iosev;
	tcp::socket socket(iosev);
	tcp::endpoint ep(ip::address_v4::from_string("127.0.0.1"), 1811);
	error_code ec;
	socket.connect(ep, ec);
	if (ec) {
		std::cerr << system_error(ec).what() << std::endl;
		return -1;
	}

//	Transaction tx2 { 2, 3, 70 };
//	Block block { 0, 0, 0, { tx1, tx2 } };

	Transaction tx { 1, 0, 100 };
	for (int64_t i = 1; i <= 20; i++) {
		tx.to = i;
		Api(tx).write(socket);
	}

//	Api(tx1).write(socket);
//	Api(tx2).write(socket);
//	Api(block).write(socket);
}