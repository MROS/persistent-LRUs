// 暫時用不到
// 待有時間序列化時再處理本檔案

#include <boost/asio.hpp>
#include "api.h"
#include "transaction.h"
#include "VC.h"
#include "constant.h"

using namespace boost::asio::ip;
using namespace boost::asio;

int main() {
	io_service iosev;
	tcp::socket socket(iosev);
	tcp::endpoint ep(ip::address_v4::from_string("127.0.0.1"), 1811);
	boost::system::error_code ec;
	socket.connect(ep, ec);
	if (ec) {
		std::cerr << boost::system::system_error(ec).what() << std::endl;
		return -1;
	}

//	VC vc;

//	Transaction tx2 { 2, 3, 70 };
//	Block block1 { 0, 0, 0, {}, {}};
//
//	for (int64_t i = 1; i <= 20; i++) {
//		TransactionWithProof tx {
//			.from = 1,
//			.to = i,
//			.value = 100,
//			.nonce = 0,
//			.block_hash = 0,
//			.proof = vector<Ec1>(EDRAX_BITS, vc.a->g1*0)};
//	}
//	Api(block1).write(socket);

//	Api(tx1).write(socket);
//	Api(tx2).write(socket);
//	Api(block).write(socket);
}