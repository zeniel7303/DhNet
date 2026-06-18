#include "pch.h"
#include "Logger.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/async.h>

std::shared_ptr<spdlog::logger> GLogger;

void LoggerShutdown()
{
	if (GLogger)
		GLogger->flush();
}

namespace
{
	class LoggerInit
	{
	public:
		LoggerInit()
		{
			spdlog::init_thread_pool(8192, 1);

			auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
			auto fileSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
				"logs/dhnet-server.log", 10 * 1024 * 1024, 5);

			GLogger = std::make_shared<spdlog::async_logger>(
				"dhnet",
				spdlog::sinks_init_list{ consoleSink, fileSink },
				spdlog::thread_pool(),
				spdlog::async_overflow_policy::block);

			GLogger->set_level(spdlog::level::info);
			GLogger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [tid %t] %v");
			GLogger->flush_on(spdlog::level::warn);

			spdlog::flush_every(std::chrono::seconds(3));
		}

		~LoggerInit()
		{
			LoggerShutdown();
			spdlog::shutdown();
		}
	} GLoggerInit;
}
