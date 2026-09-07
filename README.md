## 🛠️ C++ 소스 코드 상세 명세 (Source Code Deep Dive)

### 1. 프로젝트 파일 디렉터리 구조
```text
Source/Puzzle_Platform/
├── Puzzle_Platform.Build.cs              # 엔진 모듈 의존성 정의
├── Puzzle_Platform.h / .cpp              # 기본 모듈 구현부
├── PuzzlePlatformGameInstance.h / .cpp   # Steam OSS 세션 라이프사이클 및 UI 관리
├── LobbyGameMode.h / .cpp                # 로비 대기실 인원 집계 및 서버 이동
├── Puzzle_PlatformGameMode.h / .cpp      # 인게임 룰 및 스폰 관리
├── Puzzle_PlatformCharacter.h / .cpp     # 이동 입력 및 네트워크 복제 캐릭터
├── MovingPlatform.h / .cpp               # 서버 연산 기반 왕복 이동 플랫폼 기믹
└── PlatformTrigger.h / .cpp              # 박스 콜리전 기반 발판 연동 트리거

```

---

### 2. 세션 및 네트워크 관리 (Session & GameMode)

#### 🎮 `PuzzlePlatformGameInstance` (.h / .cpp)

> `UGameInstance`를 상속받아 세션 인터페이스 바인딩, 방 생성/검색/참가, 레벨 간 UI 유지를 총괄하는 핵심 네트워크 관리 클래스입니다.
> 
> 

* **주요 멤버 변수**:
* `IOnlineSessionPtr SessionInterface`: Online Subsystem의 세션 제어 인터페이스 포인터.


* `TSharedPtr<FOnlineSessionSearch> SessionSearch`: 검색된 세션 리스트와 쿼리 필터를 보관하는 공유 포인터.


* `TSubclassOf<UUserWidget> MenuClass`: 메인 메뉴 UI 위젯 블루프린트 클래스.


* `TSubclassOf<UUserWidget> InGameMenuClass`: 인게임 일시정지(ESC) 메뉴 위젯 클래스.




* **핵심 함수 및 실행 로직**:
* `virtual void Init() override`:
* `IOnlineSubsystem::Get()`을 호출해 기본 서브시스템(Steam/NULL)을 식별하고 세션 인터페이스(`SessionInterface`)를 캐싱.


* `OnCreateSessionComplete`, `OnDestroySessionComplete`, `OnFindSessionsComplete`, `OnJoinSessionComplete` 델리게이트 바인딩 등록.




* `void Host(FString ServerName)`:
* `FOnlineSessionSettings` 객체를 생성하여 최대 접속 인원(`NumPublicConnections = 4`), LAN 여부(`bIsLANMatch`), 광고 여부(`bShouldAdvertise`), 상태 공유(`bUsesPresence`) 설정.


* `CreateSession()`을 호출하고 완료 시 `ServerTravel("/Game/Maps/Lobby?listen")`을 실행해 리슨 서버 개설.




* `void FindSessions()` / `void Join(uint32 Index)`:
* `FindSessions()`로 활성화된 세션을 비동기 검색한 후 결과를 UI 목록에 전달.


* 유저가 선택한 인덱스의 세션을 `JoinSession()`으로 연결하고, 성공 시 `APlayerController::ClientTravel`을 호출해 서버 IP로 접속.




* `void LoadMenu()` / `void InGameLoadMenu()`:
* 뷰포트에 메뉴 위젯을 인스턴스화하고 `FInputModeUIOnly` 모드로 마우스 커서를 활성화하여 화면 전환 제어.







---

#### 🚪 `LobbyGameMode` (.h / .cpp)

> 리슨 서버 기반 멀티플레이어 대기실을 통제하는 서버 전용 게임모드입니다.
> 
> 

* **주요 멤버 변수**:
* `uint32 NumberOfPlayers`: 현재 로비에 접속해 있는 플레이어 수 집계.


* `uint32 RequiredPlayers`: 본 게임 시작에 필요한 목표 인원수(기본값: 2~4).




* **핵심 함수 및 실행 로직**:
* `virtual void PostLogin(APlayerController* NewPlayer) override`:
* 새 클라이언트 접속 시 호출되며, `NumberOfPlayers`를 1 증가시킴.


* 접속 인원이 `RequiredPlayers`에 도달하면 `bUseSeamlessTravel = true` 설정 후 `GetWorld()->ServerTravel("/Game/Maps/Game?listen")`을 호출하여 모든 클라이언트를 메인 게임 레벨로 강제 이동.




* `virtual void Logout(AController* Exiting) override`:
* 플레이어 접속 종료 시 카운트를 차감하여 방 대기 상태 갱신.







---

#### 🧩 `Puzzle_PlatformGameMode` (.h / .cpp)

> 인게임 퍼즐 맵의 라이프사이클 및 기본 규칙을 관리합니다.
> 
> 

* **핵심 구현 내용**:
* 생성자에서 `DefaultPawnClass`를 `APuzzle_PlatformCharacter`로 지정하여 맵 진입 시 플레이어 폰 자동 생성 보장.


* 클라이언트 접속 및 스폰 위치(PlayerStart) 할당 규칙 통제.





---

### 3. 게임플레이 기믹 및 리플리케이션 (Gameplay & Replication)

#### 🚀 `MovingPlatform` (.h / .cpp)

> 두 지점 간을 왕복 이동하는 네트워크 복제 퍼즐 기믹 액터입니다.
> 
> 

* **주요 멤버 변수**:
* `float Speed`: 플랫폼 이동 속도 (`UPROPERTY(EditAnywhere, Category = "Movement")`).


* `FVector TargetLocation`: 에디터 뷰포트에서 직접 조절 가능한 목표 상대 좌표 (`UPROPERTY(EditAnywhere, Meta = (MakeEditWidget = true))`).


* `FVector GlobalStartLocation`: 월드 기준 시작 위치 벡터.


* `FVector GlobalTargetLocation`: 월드 기준 목표 도착 위치 벡터 (`GlobalStartLocation + TargetLocation`).


* `int32 ActiveTriggers`: 현재 이 플랫폼을 활성화하고 있는 연결 트리거의 수 (`UPROPERTY(EditAnywhere)`).




* **핵심 함수 및 동기화 설계**:
* **생성자 초기화**:
* `bReplicates = true` 및 `SetReplicateMovement(true)`를 활성화하여 액터의 위치와 회전을 서버에서 클라이언트로 자동 복제.


* `PrimaryActorTick.bCanEverTick = true`, `SetMobility(EComponentMobility::Movable)` 설정.




* `virtual void Tick(float DeltaTime) override`:
* `HasAuthority()` 검사를 통해 **오직 서버에서만 이동 연산을 독점 실행**.


* `if (ActiveTriggers > 0)` 조건을 검사하여 스위치가 눌린 상태에서만 이동 처리.


* 현재 위치와 목표 위치 간의 거리(`FVector::Dist`)를 계산하고 방향 벡터(`GetSafeNormal()`)와 `Speed * DeltaTime`을 곱해 `SetActorLocation` 수행.


* 목표 지점에 도달(`JourneyLength` 초과) 시, `GlobalStartLocation`과 `GlobalTargetLocation`을 Swap하여 반대 방향으로 왕복하도록 전환.




* `void AddActiveTrigger()` / `void RemoveActiveTrigger()`:
* 외부 트리거 액터가 호출하여 `ActiveTriggers` 카운트를 증감시키는 상태 제어 인터페이스 함수.







---

#### 🔲 `PlatformTrigger` (.h / .cpp)

> 플레이어의 진입/이탈 충돌을 감지해 타깃 플랫폼에 작동 신호를 보내는 스위치 액터입니다.
> 
> 

* **주요 멤버 변수**:
* `class UBoxComponent* TriggerVolume`: 플레이어 오버랩을 감지하는 트리거 박스 컴포넌트.


* `class UStaticMeshComponent* Mesh`: 압력판 외형을 렌더링하는 스태틱 메시 컴포넌트.


* `TArray<AMovingPlatform*> PlatformsToTrigger`: 이 트리거가 제어할 대상 플랫폼 배열 (`UPROPERTY(EditAnywhere, Category = "Trigger")`).




* **핵심 함수 및 오버랩 로직**:
* `void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, ...)`:
* 캐릭터 액터가 박스에 진입하면 `PlatformsToTrigger` 배열을 순회하며 각 플랫폼의 `AddActiveTrigger()`를 호출해 이동 개시.




* `void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, ...)`:
* 캐릭터가 박스를 벗어나면 `RemoveActiveTrigger()`를 호출해 카운트를 감소시키며, 카운트가 0이 되면 플랫폼이 정지.







---

#### 🏃 `Puzzle_PlatformCharacter` (.h / .cpp)

> 이동 및 시점 회전 입력을 처리하고 네트워크 상에서 위치가 보간되는 기본 플레이어 캐릭터입니다.
> 
> 

* **주요 구성 요소**:
* `USpringArmComponent` & `UCameraComponent`: 3인칭 쿼터/숄더 뷰 구성을 위한 카메라 컴포넌트 구조.


* `bUseControllerRotationPitch / Yaw / Roll = false`: 카메라 방향과 캐릭터의 전방 방향을 독립시켜 부드러운 회전 보장.


* `GetCharacterMovement()->bOrientRotationToMovement = true`: 입력 방향으로 캐릭터가 자동 회전하도록 설정.




* **입력 매핑**:
* `MoveForward`, `MoveRight`, `Turn`, `LookUp` 축 매핑과 `Jump` 액션 바인딩을 통해 이동 처리.





---

### 4. 빌드 설정 및 모듈 의존성 (`Puzzle_Platform.Build.cs`)

```csharp
using UnrealBuildTool;

public class Puzzle_Platform : ModuleRules
{
    public Puzzle_Platform(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        // 핵심 런타임 및 네트워크/UI 의존성 모듈 정의
        PublicDependencyModuleNames.AddRange(new string[] { 
            "Core", 
            "CoreUObject", 
            "Engine", 
            "InputCore", 
            "HeadMountedDisplay", 
            "UMG",                  // 메인 메뉴 및 로비 UI 바인딩
            "Slate",                // UI 렌더링 코어
            "SlateCore", 
            "OnlineSubsystem",      // 네트워크 세션 인터페이스
            "OnlineSubsystemSteam"  // Steam P2P 및 로비 연동 서브시스템
        });
    }
}
```[cite: 1]

---

### 5. 네트워크 데이터 흐름 요약

```text
[클라이언트 A (방장)]                      [Steam Master Server]                    [클라이언트 B (참가자)]
        │                                         │                                         │
        ├──────── CreateSession (Host) ──────────►│                                         │
        │◄─────── Session Created ────────────────┤                                         │
        │                                         │◄────── FindSessions (Search) ───────────┤
        │                                         ├─────── Session List Returned ──────────►│
        │                                         │                                         │
        │                                         │◄────── JoinSession ─────────────────────┤
        │◄───────────────────────── ClientTravel (P2P 연결) ────────────────────────────────┤
        │                                                                                   │
   [ServerTravel] (목표 인원 도달 시 인게임 맵 동시 전환)                                      │
        │                                                                                   │
   [MovingPlatform] ── Tick 연산 (서버 권한 전용 HasAuthority) ─── Location Replication ──► [클라이언트 동기화]

```

```

```
