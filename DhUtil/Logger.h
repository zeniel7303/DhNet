#pragma once

#include <spdlog/spdlog.h>

// 함수 지역 static(Meyer's Singleton) — 다른 전역 정적 객체의 생성자가 몇 번째로 실행되든
// 최초 호출 시점에 항상 안전하게 lazy 생성된다. SIOF(정적 초기화 순서 문제)에서 자유로움.
class Logger
{
public:
	static std::shared_ptr<spdlog::logger>& Get();
};

// CRASH 매크로가 AV 트리거 직전에 호출 — 비동기 로거 버퍼에 남은 로그를 동기적으로 비운다.
void LoggerShutdown();
