#pragma once
#include <memory>

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/asio/thread_pool.hpp>

#include <functional>
#include <string>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_sinks.h>

#include "root_certificates.hpp"

namespace utils 
{
namespace beast = boost::beast;
namespace asio = boost::asio;
namespace websocket = boost::beast::websocket;

class session : public std::enable_shared_from_this<session>
{
	using tcp = boost::asio::ip::tcp;
	using ws_endpoint = websocket::stream<asio::ssl::stream<tcp::socket>>;

private:
	void on_resolve(beast::error_code ec, tcp::resolver::results_type results);
	void on_connect(beast::error_code ec);
	void on_ssl_handshake(beast::error_code ec);
	void on_handshake(beast::error_code ec);
	void on_read(beast::error_code ec, std::size_t bytes_transferred);
	void on_write(beast::error_code ec, std::size_t bytes_transferred);

	void fail(beast::error_code ec, std::string&& from);

	tcp::resolver m_resolver;
	ws_endpoint m_websocket;
	std::function<void(std::string, ws_endpoint*)> on_message_func;

	beast::multi_buffer m_buffer;
	std::string m_host;
	std::string m_port;

public:
	session(std::string host, std::string port, asio::io_context& ioc, ssl::context& ctx)
		: m_host(host),
		m_port(port),
		m_resolver(ioc),
		m_websocket(ioc, ctx)
	{
			//::signal_set signals(m_context, SIGINT, SIGTERM);
			//signals.async_wait([&](const beast::error_code&, int)
			//{
			//	ioc.stop();
			//});
	}

	~session() noexcept
	{
		spdlog::drop_all();
	}

	void run();
	void write(std::string&& data);
	void set_message_handler(std::function<void(std::string, ws_endpoint*)> func);
};
}