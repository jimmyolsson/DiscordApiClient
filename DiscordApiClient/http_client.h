#pragma once

#include "root_certificates.hpp"

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/error.hpp>
#include <boost/asio/ssl/stream.hpp>

#include <spdlog/spdlog.h>

namespace services 
{
	namespace http = boost::beast::http;
	using tcp = boost::asio::ip::tcp;

	class http_client
	{
	public:
		http_client(const std::string& address, int port, const std::string& token, std::shared_ptr<spdlog::logger>& log)
			: m_host(address),
			m_port(port),
			m_token(token),
			m_logger(log),
			m_ssl_context(ssl::context::tlsv12_client),
			m_resolver(m_io_context),
			m_ssl_stream(m_io_context, m_ssl_context)
		{
			load_root_certificates(m_ssl_context);

			// Always verify the remote server's certificate
			m_ssl_context.set_verify_mode(ssl::verify_peer);
		};

		boost::optional<http::response<http::dynamic_body>> send_request(http::verb request_type, std::string_view target, std::string_view body);

	private:
		const int m_port;
		const std::string m_host;
		const std::string m_token;
		std::shared_ptr<spdlog::logger>& m_logger;

		ssl::context m_ssl_context;
		boost::asio::io_context m_io_context;
		tcp::resolver m_resolver;
		ssl::stream<tcp::socket> m_ssl_stream;

		//const int m_version = 1;
		const std::string_view m_user_agent = "DiscordBot v1.0";
	};
}