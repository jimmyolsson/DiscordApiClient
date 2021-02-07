#pragma once
#include <boost/beast/core.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <memory>
#include <string>

#include "root_certificates.hpp"
#include "session.h"
#include "message_handler.h"

namespace utils 
{
namespace ssl = boost::asio::ssl;
namespace websocket = boost::beast::websocket;

class ws_client
{
private:
	ssl::context m_ssl_context;
	boost::asio::io_context m_io_context;
	message_handler m_message_handler;
	std::shared_ptr<session> m_ws_session;

	void on_message(std::string msg, )
	{

	}

public:
	ws_client(std::string host, std::string port)
		: m_ssl_context(ssl::context::tlsv12_client),
		m_message_handler( [this](auto&& msg)
		{
			m_ws_session->write(std::move(msg));
		})
	{
		//m_websocket.async_read(m_buffer, std::bind(&utils::session::on_read, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
		load_root_certificates(m_ssl_context);
		m_ssl_context.set_verify_mode(ssl::verify_peer);
		//m_ws_session->set_message_handler(std::bind(&);
		m_ws_session = std::make_shared<session>(std::move(host), std::move(port), m_io_context, m_ssl_context);
		m_ws_session->set_message_handler(
			[](auto msg, auto endpoint) 
		{
			std::cout << "asdasd\n";
		});
	}

	~ws_client() noexcept
	{
		m_io_context.stop();
	}

	void run();
};
}

