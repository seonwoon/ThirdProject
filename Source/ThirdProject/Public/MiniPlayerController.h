#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MiniPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;

UCLASS()
class THIRDPROJECT_API AMiniPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AMiniPlayerController();

	// �����Ϳ��� ������ �Է� ���� ���ؽ�Ʈ (IMC)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputMappingContext* InputMappingContext;

	// �����Ϳ��� ������ Move �Է� �׼�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* MoveAction;

	// �����Ϳ��� ������ Look �Է� �׼�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* LookAction;

protected:
	virtual void BeginPlay() override;
};
