#include "message_handler.h"
#include <boost/thread.hpp>
#include <string_view>
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <fmt/format.h>
#include <map>
#include "session.h"

enum Opcode
{
	Dispatch = 0,
	Hello = 10
};

constexpr std::string_view MessageCreate{ "MESSAGE_CREATE" };

void message_handler::handle(std::string message)
{
	rapidjson::Document doc;
	doc.Parse(message.data());

	int opcode = doc["op"].GetInt();
	switch (opcode)
	{
	case Opcode::Hello:
	{
		m_logger->info("Hello received");
		on_hello(doc);
		break;
	}
	case Opcode::Dispatch:
	{
		const auto a = doc["t"].GetString();
		const auto iter = dispatch_table.find(a);
		if (iter != dispatch_table.end())
		{
			iter->second(std::move(doc));
		}
		else if(iter == dispatch_table.end())
		{
			m_logger->warn("Unhandeled dispatch");
		}
		break;
	}
	default:
		m_logger->info("Unhandled opcode: {}", opcode);
		break;
	}

	m_logger->info(message.data());
}

void message_handler::on_hello(const rapidjson::Document& data)
{
	constexpr std::string_view heartbeat{ "{\"op\": 1,\"d\": 0 }" };
	std::string identify{ "{\"op\": 2,\"d\": {\"token\": \"\",\"properties\" : {\"$os\": \"linux\",\"$browser\" : \"disco\",\"$device\" : \"disco\"}},\"s\": 1,\"t\": \"IDENTIFY\"}" };

	m_write_func(std::move(identify));

	int ms = data["d"]["heartbeat_interval"].GetInt();

	rapidjson::Document responseData;
	responseData.Parse(heartbeat.data());

	int counter = 0;
	while (true)
	{
		responseData["d"].SetInt(counter);
		boost::this_thread::sleep_for(boost::chrono::milliseconds(ms));

		m_write_func(std::move(responseData.GetString()));
		counter++;
	}
}

void message_handler::on_message_create(const rapidjson::Document& data)
{
}
