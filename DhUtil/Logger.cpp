#include "pch.h"
#include "Logger.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/async.h>

namespace
{
	std::string& LogFileNameRef()
	{
		static std::string name = "dhnet-server.log";
		return name;
	}

	// SetLogFileName()이 Get()보다 늦게 호출되면(예: 로거 생성 전이라 가정한 전역 정적
	// 객체의 생성자가 실은 LOG_*를 먼저 호출하는 경우) 이미 만들어진 파일명으로 로거가
	// 굳어진 뒤라 조용히 무시된다 — 이 플래그로 그 상황을 최소한 로그에 남긴다.
	std::atomic<bool>& LoggerCreatedFlag()
	{
		static std::atomic<bool> created{ false };
		return created;
	}

	std::shared_ptr<spdlog::logger> CreateLogger()
	{
		spdlog::init_thread_pool(8192, 1);

		auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
		auto fileSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
			"logs/" + LogFileNameRef(), 10 * 1024 * 1024, 5);

		auto logger = std::make_shared<spdlog::async_logger>(
			"dhnet",
			spdlog::sinks_init_list{ consoleSink, fileSink },
			spdlog::thread_pool(),
			spdlog::async_overflow_policy::block);

		logger->set_level(spdlog::level::info);
		logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [tid %t] %v");
		logger->flush_on(spdlog::level::warn);

		// spdlog::flush_every()의 주기 flush는 레지스트리(spdlog::apply_all)에 등록된
		// 로거에만 적용된다 — 등록 없이는 WARN 이상 로그가 안 나오는 동안(예: 이 로거가
		// info 레벨만 계속 찍는 프로세스) 큐에 쌓인 로그가 flush_on 조건을 못 만나 파일에
		// 전혀 반영되지 않은 채 무기한 버퍼링될 수 있다. Get()이 Meyer's Singleton이라
		// 프로세스당 CreateLogger()가 정확히 한 번만 실행되므로 이름 중복 등록 위험은 없지만,
		// 혹시 모를 충돌(spdlog_ex)이 LOG_* 호출 스택 전체로 전파되어 CRASH()의 통제된
		// fail-fast 경로 밖에서 std::terminate로 이어지는 걸 막기 위해 방어적으로 감싼다.
		try
		{
			spdlog::register_logger(logger);
		}
		catch (const std::exception&)
		{
			// 로거 이름("dhnet") 충돌 등 — 등록 실패해도 flush_every 주기 flush만 못 받을 뿐,
			// flush_on(warn)과 명시적 LoggerShutdown()은 여전히 정상 동작하므로 계속 진행한다.
		}
		spdlog::flush_every(std::chrono::seconds(3));

		LoggerCreatedFlag().store(true, std::memory_order_release);
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

void Logger::SetLogFileName(const std::string& _name)
{
	if (LoggerCreatedFlag().load(std::memory_order_acquire))
	{
		// 이미 CreateLogger()가 실행되어 파일명이 굳어진 뒤 — 조용히 무시하지 않고
		// (이미 만들어진, 아마 원치 않는 파일로 쓰이는) 로거에라도 남겨 알아챌 수 있게 한다.
		LOG_WARN("Logger::SetLogFileName(\"{}\") called after logger already created — ignored, still using \"{}\"",
			_name, LogFileNameRef());
		return;
	}
	LogFileNameRef() = _name;
}
