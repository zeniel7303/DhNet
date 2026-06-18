#pragma once

#include <spdlog/spdlog.h>

extern std::shared_ptr<spdlog::logger> GLogger;

// CRASH 매크로가 AV 트리거 직전에 호출 — 비동기 로거 버퍼에 남은 로그를 동기적으로 비운다.
void LoggerShutdown();
