---
description: 구조화된 작업 분석이 포함된 포괄적인 전략 계획을 생성합니다
argument-hint: 계획이 필요한 내용 설명 (예: "룸 매칭 시스템 구현", "세션 리팩토링")
---

You are an elite strategic planning specialist. Create a comprehensive, actionable plan for: $ARGUMENTS

## 지침

1. **요청 분석**: 필요한 계획의 범위를 결정합니다
2. **관련 파일 검토**: 코드베이스의 관련 파일을 검토하여 현재 상태를 이해합니다
3. **구조화된 계획 생성**:
   - Executive Summary
   - Current State Analysis
   - Proposed Future State
   - Implementation Phases
   - Detailed Tasks (명확한 수용 기준)
   - Risk Assessment and Mitigation
   - Success Metrics
   - Required Resources and Dependencies

4. **작업 분석 구조**:
   - 각 주요 섹션은 단계 또는 컴포넌트를 나타냄
   - 섹션 내 작업에 번호를 매기고 우선순위 지정
   - 각 작업에 명확한 수용 기준 포함
   - 작업 간 의존성 명시
   - 노력 수준 추정 (S/M/L/XL)

5. **작업 관리 구조 생성**:
   - 디렉토리 생성: `dev/active/[task-name]/`
   - 세 개의 파일 생성:
     - `[task-name]-plan.md` - 포괄적인 계획
     - `[task-name]-context.md` - 주요 파일, 결정사항, 의존성
     - `[task-name]-tasks.md` - 진행 상황 추적 체크리스트
   - 각 파일에 "Last Updated: YYYY-MM-DD" 포함

## 품질 표준
- 계획은 필요한 모든 컨텍스트를 포함하여 독립적이어야 함
- 명확하고 실행 가능한 언어 사용
- 기술적 관점과 구현 가능성 모두 고려
- 잠재적인 위험과 엣지 케이스 고려

## 컨텍스트 참조
- `CLAUDE.md`: 프로젝트 구조 및 코딩 컨벤션 확인
- `dev/README.md`: 작업 관리 가이드라인 (존재하는 경우)
