#include "session.h"
#include <spdlog/spdlog.h>

void utils::session::run()
{
	m_resolver.async_resolve(m_host, m_port, std::bind(&utils::session::on_resolve, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
}

void utils::session::on_resolve(beast::error_code ec, tcp::resolver::results_type results)
{
	if (ec)
		return fail(ec, "resolve");

	// Make the connection on the IP address we get from a lookup
	asio::async_connect(m_websocket.next_layer().next_layer(), results.begin(), results.end(), std::bind(&utils::session::on_connect, shared_from_this(), std::placeholders::_1));
}

void utils::session::on_connect(beast::error_code ec)
{
	if (ec)
		return fail(ec, "connect");

	// Perform the SSL handshake
	m_websocket.next_layer().async_handshake(asio::ssl::stream_base::client, std::bind(&utils::session::on_ssl_handshake, shared_from_this(), std::placeholders::_1));
}

void utils::session::on_ssl_handshake(beast::error_code ec)
{
	if (ec)
		return fail(ec, "ssl_handshake");

	// Perform the websocket handshake
	m_websocket.async_handshake(m_host, "/", std::bind(&utils::session::on_handshake, shared_from_this(), std::placeholders::_1));
}

void utils::session::on_handshake(beast::error_code ec)
{
	if (ec)
		return fail(ec, "handshake");

	// Send the message
	m_websocket.async_read(m_buffer, std::bind(&utils::session::on_read, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
}

void utils::session::write(std::string&& data)
{
	m_websocket.async_write(std::move(asio::buffer(data)), std::bind(&utils::session::on_write, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
}

void utils::session::set_message_handler(std::function<void(std::string, ws_endpoint*)> func)
{
	on_message_func = func;
}

void utils::session::on_read(beast::error_code ec, std::size_t bytes_transferred)
{
	boost::ignore_unused(bytes_transferred);
	if (ec)
		return fail(ec, "read");

	on_message_func("", nullptr);

	// Clear buffer
	//m_buffer.consume(m_buffer.size());

	// Read again
	m_websocket.async_read(m_buffer, std::bind(&utils::session::on_read, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
}

void utils::session::on_write(beast::error_code ec, std::size_t bytes_transferred)
{
	boost::ignore_unused(bytes_transferred);

	if (ec)
		return fail(ec, "write");

	// Read a message into our buffer
}

void utils::session::on_close(beast::error_code ec)
{
	if (ec)
		return fail(ec, "close");

	m_websocket.async_close(websocket::close_code::normal, std::bind(&utils::session::on_read, shared_from_this(), std::placeholders::_1));

	// The make_printable() function helps print a ConstBufferSequence
	std::cout << beast::buffers(m_buffer.data()) << "\n";
}

void utils::session::fail(beast::error_code ec, std::string&& from)
{
	m_logger->error("utils::session::fail: ec: " + ec.message() + from);
}
