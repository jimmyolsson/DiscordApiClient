#include <boost/thread.hpp>
#include <boost/asio/thread_pool.hpp>
#include <iostream>
#include "ws_client.h"

int main()
{
	auto const host = "gateway.discord.gg";
	auto const port = "443";

	utils::ws_client c{ host, port };
	c.run();
	std::cin.get();

	return EXIT_SUCCESS;
}
