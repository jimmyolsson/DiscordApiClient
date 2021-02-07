#include "ws_client.h"

void utils::ws_client::run()
{
	m_ws_session->run();
	m_io_context.run();
}
