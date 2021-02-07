#include "http_client.h"

namespace http = boost::beast::http;

boost::optional<http::response<http::dynamic_body>> services::http_client::send_request(http::verb request_type, std::string_view target, std::string_view body)
{
	return {};
}
