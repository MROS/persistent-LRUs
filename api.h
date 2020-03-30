#pragma once
#include <cstdint>
#include <memory>
#include <sstream>
#include <string>
#include <memory>
#include <cereal/archives/binary.hpp>
#include <boost/asio.hpp>
#include "transaction.h"
#include "block.h"

enum class Type: uint32_t {
	TX, BLOCK
};

struct Header {
	Type type;
	uint32_t length; // data 的長度
};

struct Api {
	Header header;
	std::string data;

	Api() = default;
	explicit Api(Transaction &tx) {
		header.type = Type::TX;
		std::stringstream ss;
		cereal::BinaryOutputArchive archive( ss );
		archive(tx);
		ss >> data;
		header.length = data.size();
	}
	void write(boost::asio::ip::tcp::socket &socket) {
		using namespace boost::asio;
		using namespace std;

		boost::system::error_code ec;

		socket.write_some(buffer( &header, sizeof(Header)), ec);
		if (ec) { cerr << boost::system::system_error(ec).what() << endl; }

		socket.write_some(buffer(data.c_str(), header.length), ec);
		if (ec) { cerr << boost::system::system_error(ec).what() << endl; }

	}
	// 若成功讀取，返回真
	// 若中途遇到 eof ，返回假
	// 其他例外則直接拋出
	bool read(boost::asio::ip::tcp::socket &socket) {
		using namespace boost::asio;
		using namespace std;

		boost::system::error_code ec;

		// XXX: 有可能會沒有讀滿 sizeof(Header) 嗎？
		socket.read_some(buffer( &this->header, sizeof(Header) ), ec);

		if (ec == boost::asio::error::eof) { return false; }
		else if (ec) { throw boost::system::system_error(ec); }

		auto data_len = this->header.length;
		char *ptr = new char(data_len);
		socket.read_some(buffer(ptr, data_len), ec);

		if (ec == boost::asio::error::eof) { return false; }
		else if (ec) { throw boost::system::system_error(ec); }

		this->data = string(ptr, data_len);
		delete ptr;
		return true;
	}
};
