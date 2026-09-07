GitHub README의 아키텍처 항목에 바로 삽입할 수 있도록 클래스별 핵심 책임, 주요 함수, 네트워크 동기화 방식을 정리한 구조화 명세입니다.

---

### Source Code Architecture Overview

| 범주 | 클래스명 | 핵심 역할 | 네트워크 / 동기화 방식 |
| --- | --- | --- | --- |
| **Session** | `PuzzlePlatformGameInstance`<br> | Steam OSS 세션 수명 주기 관리 및 UI 전환

 | `ServerTravel`, `ClientTravel` 기반 레벨 이동

 |
| **GameMode** | `LobbyGameMode`<br> | 플레이어 대기실 진입 인원 카운트 및 시작 제어

 | 서버 권한(Authority) 전용 로직 제어

 |
| **GameMode** | `Puzzle_PlatformGameMode`<br> | 메인 퍼즐 인게임 룰 및 스폰 관리

 | 서버 권한 전용 게임 규칙 연산

 |
| **Gimmick** | `MovingPlatform`<br> | 왕복 이동형 퍼즐 기믹 액터

 | `bReplicates = true`, 서버 연산 및 위치 복제

 |
| **Gimmick** | `PlatformTrigger`<br> | 충돌 감지 기반 발판 활성화 스위치

 | 서버 충돌 오버랩 이벤트로 타깃 액터 트리거

 |
| **Character** | `Puzzle_PlatformCharacter`<br> | 이동 입력 및 멀티플레이 동기화 캐릭터 베이스

 | 기본 `ACharacter` 컴포넌트 리플리케이션 활용

 |
| **Config** | `Puzzle_Platform.Build.cs`<br> | 빌드 규칙 및 외부 엔진 모듈 의존성 정의

 | `OnlineSubsystemSteam`, `UMG` 모듈 링크

 |

---

### 세부 구현 명세 (Core Class Details)

**1. 네트워크 및 세션 관리 (Network & Session)**

* **`PuzzlePlatformGameInstance` (.h / .cpp)**

* **핵심 책임**: `UGameInstance`를 상속받아 세션 수명 주기(Session Lifecycle) 관리.


* **주요 기능**:
* Online Subsystem 인터페이스를 활용한 방 생성(`Host`), 세션 검색(`Find`), 참가(`Join`) 로직 처리.


* 메인 메뉴 및 인게임 일시정지 UI 위젯 생성, 화면 전환 흐름 통제.


* 세션 상태에 따른 `ServerTravel` 및 `ClientTravel` 호출로 맵 로딩 지휘.






* **`LobbyGameMode` (.h / .cpp)**

* **핵심 책임**: 리슨 서버 접속 대기실 환경 제어.


* **주요 기능**:
* `PostLogin`, `Logout` 이벤트를 오버라이드하여 현재 로비 접속자 수 실시간 집계.


* 설정된 목표 정원 충족 시 본 게임 맵으로 `ServerTravel`을 실행하는 게임 흐름 제어.






* **`Puzzle_PlatformGameMode` (.h / .cpp)**

* **핵심 책임**: 퍼즐 맵의 기본 규칙(Match State) 및 플레이어 스폰 위치 할당.





---

**2. 퍼즐 기믹 및 리플리케이션 (Gameplay Mechanics & Replication)**

* **`MovingPlatform` (.h / .cpp)**

* **핵심 책임**: 두 지점 간을 왕복 이동하는 네트워크 동기화 기믹 플랫폼.


* **동기화 설계**:
* 생성자에서 `bReplicates = true` 및 `SetReplicateMovement(true)` 설정.


* `HasAuthority()` 검사를 통해 오직 서버에서만 물리 이동 및 방향 전환 연산을 수행하고, 클라이언트는 서버 위치를 보간(Interpolate)하여 렌더링하도록 강제.


* **퍼즐 인터랙션**:
* `ActiveTriggers` 정수 변수를 보유하여 연결된 트리거가 활성화 상태일 때만 이동을 개시하도록 조건 분기.






* **`PlatformTrigger` (.h / .cpp)**

* **핵심 책임**: 플레이어 진입/이탈을 감지하여 발판의 움직임을 제어하는 트리거 스위치.


* **주요 기능**:
* `UBoxComponent` 기반 오버랩 이벤트(`OnComponentBeginOverlap`, `OnComponentEndOverlap`) 처리.


* 에디터에서 지정 가능한 타깃 액터 배열(`TArray<AMovingPlatform*>`)을 순회하며 `AddActiveTrigger()` 및 `RemoveActiveTrigger()` 호출.






* **`Puzzle_PlatformCharacter` (.h / .cpp)**

* **핵심 책임**: 조작 입력 처리 및 캐릭터 무브먼트 리플리케이션 기본 템플릿.





---

**3. 프로젝트 모듈 빌드 구성 (Build System)**

* **`Puzzle_Platform.Build.cs`**

* **선언된 핵심 모듈**:
* `Core`, `CoreUObject`, `Engine`, `InputCore`: 언리얼 엔진 기본 런타임 의존성.


* `UMG`, `Slate`, `SlateCore`: 로비 및 메인 메뉴 UI 제작을 위한 모듈.


* `OnlineSubsystem`, `OnlineSubsystemSteam`: P2P 홀펀칭 및 로비 세션 연동을 위한 필수 서브시스템 모듈.
