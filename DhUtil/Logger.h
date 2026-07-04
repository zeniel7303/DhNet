#pragma once

#include <spdlog/spdlog.h>
#include <string>

// 함수 지역 static(Meyer's Singleton) — 다른 전역 정적 객체의 생성자가 몇 번째로 실행되든
// 최초 호출 시점에 항상 안전하게 lazy 생성된다. SIOF(정적 초기화 순서 문제)에서 자유로움.
class Logger
{
public:
	static std::shared_ptr<spdlog::logger>& Get();

	// 첫 Get() 호출 전에 설정해야 한다(이후 호출은 이미 생성된 로거에 영향 없음).
	// DhNet_Server/DhNet_Client/DhNet_StressTest처럼 여러 프로세스가 이 Logger를
	// 공유하는 경우, 같은 로그 파일에 서로 다른 프로세스가 동시에 write/rotate하면
	// 안전하지 않으므로 실행파일마다 고유 파일명을 지정해야 한다. 미호출 시 기존
	// 기본값("dhnet-server.log")이 그대로 사용된다.
	static void SetLogFileName(const std::string& _name);
};

// CRASH 매크로가 AV 트리거 직전에 호출 — 비동기 로거 버퍼에 남은 로그를 동기적으로 비운다.
void LoggerShutdown();
