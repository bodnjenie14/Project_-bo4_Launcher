#pragma once

namespace logger
{
	enum LogLevel
	{
		LOG_LEVEL_DEBUG = 0,
		LOG_LEVEL_INFO = 1,
		LOG_LEVEL_WARN = 2,
		LOG_LEVEL_ERROR = 3,
		LOG_LEVEL_CRITICAL = 4,
		LOG_LEVEL_INCOMING = 5,
		LOG_LEVEL_RESPONSE = 6,
		LOG_LEVEL_PLAYER_ID = 7,
	};

	enum LogLabel
	{
		LOG_LABEL_INITIALIZER = 0,
		LOG_LABEL_BLACKBOX = 1,
		LOG_LABEL_SOCKET_TCP = 2,
		LOG_LABEL_SOCKET_UDP = 3,
		LOG_LABEL_SERVER_HTTP = 4,
		LOG_LABEL_UPDATE = 5,

		
	};

	void write(const char* file, std::string str);
	void write(LogLevel level, LogLabel label, const char* fmt, ...);

	void log_packet_buffer(const char* stub, const char* buffer, size_t length);
}
