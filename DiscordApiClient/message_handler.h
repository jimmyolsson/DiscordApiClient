#pragma once

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <string_view>
#include <unordered_map>
#include <boost/date_time.hpp>
#include <boost/filesystem.hpp>
#include <string>

namespace beast = boost::beast;
namespace net = boost::asio;
namespace websocket = boost::beast::websocket;

using tcp = boost::asio::ip::tcp;

class message_handler
{
private:
	std::shared_ptr<spdlog::logger> m_logger;
	std::function<void(std::string&&)> m_write_func;

	void on_hello(const rapidjson::Document& data);
	void on_message_create(const rapidjson::Document& data);

	std::map<std::string, std::function<void(const rapidjson::Document& d)>> dispatch_table {
		{"MESSAGE_CREATE", [this](const auto& d){ on_message_create(d); }}
	};

public:
	message_handler(std::function<void(std::string&&)> writef)
		: m_write_func(writef),
		m_logger(spdlog::stdout_color_mt("console"))
	{
		auto day = boost::gregorian::day_clock::local_day();

		boost::filesystem::ofstream("discord-log.log");

		//m_logger = spdlog::basic_logger_mt("file_logger", "discord-log.log"); 
	}

	~message_handler() noexcept
	{
		spdlog::drop_all();
	}

	void handle(std::string message);
};