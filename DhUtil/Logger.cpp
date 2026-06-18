#include "pch.h"
#include "Logger.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/async.h>

namespace
{
	std::shared_ptr<spdlog::logger> CreateLogger()
	{
		spdlog::init_thread_pool(8192, 1);

		auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
		auto fileSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
			"logs/dhnet-server.log", 10 * 1024 * 1024, 5);

		auto logger = std::make_shared<spdlog::async_logger>(
			"dhnet",
			spdlog::sinks_init_list{ consoleSink, fileSink },
			spdlog::thread_pool(),
			spdlog::async_overflow_policy::block);

		logger->set_level(spdlog::level::info);
		logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [tid %t] %v");
		logger->flush_on(spdlog::level::warn);

		spdlog::flush_every(std::chrono::seconds(3));

		return logger;
	}

	// 아무것도 참조하지 않는 가드 — 다른 전역 객체와의 소멸 순서와 무관하게 안전하게
	// spdlog 전역 리소스(스레드 풀 등)를 정리한다.
	struct LoggerShutdownGuard
	{
		~LoggerShutdownGuard() { spdlog::shutdown(); }
	} GLoggerShutdownGuard;
}

std::shared_ptr<spdlog::logger>& Logger::Get()
{
	static std::shared_ptr<spdlog::logger> instance = CreateLogger();
	return instance;
}

void LoggerShutdown()
{
	Logger::Get()->flush();
}
