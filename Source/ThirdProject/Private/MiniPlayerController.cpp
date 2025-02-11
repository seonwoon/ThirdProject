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

	// ���� �÷��̾ ������
	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		// ���� �÷��̾��� EnhancedInputSubsystem�� ������
		if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			// �����Ϳ��� �Ҵ�� InputMappingContext�� �ִٸ�, �̸� Ȱ��ȭ
			// �켱����(Priority)�� 0���� ���� (���� ���ڰ� ���� �켱����)
			if (InputMappingContext)
			{
				InputSubsystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}
}