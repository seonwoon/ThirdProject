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

	// 에디터에서 설정할 입력 매핑 컨텍스트 (IMC)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputMappingContext* InputMappingContext;

	// 에디터에서 설정할 Move 입력 액션
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* MoveAction;

	// 에디터에서 설정할 Look 입력 액션
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* LookAction;

protected:
	virtual void BeginPlay() override;
};
