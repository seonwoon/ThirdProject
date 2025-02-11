#include "MiniPlayerController.h"
#include "EnhancedInputSubsystems.h"

AMiniPlayerController::AMiniPlayerController()
	: InputMappingContext(nullptr)
	, MoveAction(nullptr)
	, LookAction(nullptr)
{
}

void AMiniPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// 로컬 플레이어를 가져옴
	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		// 로컬 플레이어의 EnhancedInputSubsystem을 가져옴
		if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			// 에디터에서 할당된 InputMappingContext가 있다면, 이를 활성화
			// 우선순위(Priority)는 0으로 설정 (낮은 숫자가 높은 우선순위)
			if (InputMappingContext)
			{
				InputSubsystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}
}