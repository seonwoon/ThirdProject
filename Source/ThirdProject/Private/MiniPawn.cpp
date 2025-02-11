#include "MiniPawn.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "MiniPlayerController.h"

AMiniPawn::AMiniPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	// 1) Capsule Component: 루트 컴포넌트로 사용 (충돌, 이동 기준)
	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComp"));
	RootComponent = CapsuleComp;
	CapsuleComp->InitCapsuleSize(34.f, 88.f);
	CapsuleComp->SetSimulatePhysics(false);

	// 2) Skeletal Mesh: Pawn의 외형 표현
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(CapsuleComp);
	MeshComp->SetSimulatePhysics(false);

	// 3) SpringArm: 카메라의 거리를 제어하며, 회전도 가능하도록 설정
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(CapsuleComp);
	SpringArmComp->TargetArmLength = 300.f;
	// Pawn의 회전과 별개로 카메라가 독립적으로 회전하도록 함
	SpringArmComp->bUsePawnControlRotation = false;

	// 4) Camera: 실제 화면에 보일 카메라
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);

	// 입력 값 초기화
	MovementInput = FVector2D::ZeroVector;
	LookInput = FVector2D::ZeroVector;
	Velocity = FVector::ZeroVector;
}

void AMiniPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 1) 카메라의 현재 회전 값에서 Yaw만 추출
	const FRotator CameraRotation = CameraComp->GetComponentRotation();
	const FRotator YawOnlyRotation(0.f, CameraRotation.Yaw, 0.f);

	// 2) 카메라 기준으로 Forward, Right 벡터 계산
	// Unreal 기준: X = Forward, Y = Right, Z = Up
	const FVector CameraForward = FRotationMatrix(YawOnlyRotation).GetUnitAxis(EAxis::X);
	const FVector CameraRight = FRotationMatrix(YawOnlyRotation).GetUnitAxis(EAxis::Y);

	// 3) 입력값에 따른 이동 방향 결정
	//    MovementInput.X: 전후 (W/S), MovementInput.Y: 좌우 (A/D)
	const FVector InputDirection = (CameraForward * MovementInput.X) + (CameraRight * MovementInput.Y);

	// 4) 목표 이동 속도 계산 (정규화하여 최대 속도 적용)
	const FVector DesiredVelocity = InputDirection.GetSafeNormal() * MaxWalkSpeed;

	// 5) 현재 속도에 보간(interpolation) 적용하여 부드러운 가감속 구현
	Velocity = FMath::VInterpTo(Velocity, DesiredVelocity, DeltaTime, VelocityInterpSpeed);

	// 6) 월드 좌표계에서 이동 처리 (충돌 체크 포함)
	AddActorWorldOffset(Velocity * DeltaTime, /*bSweep=*/true);

	// 7) Pawn 자체 회전 처리 (마우스 좌우 입력 기반)
	if (!FMath::IsNearlyZero(LookInput.X))
	{
		const float YawRotation = LookInput.X * TurnRate * DeltaTime;
		AddActorLocalRotation(FRotator(0.f, YawRotation, 0.f));
	}

	// 8) 스프링암 회전 처리 (마우스 상하 입력 기반: 카메라 Pitch 제어)
	if (!FMath::IsNearlyZero(LookInput.Y))
	{
		// 현재 스프링암의 로컬 회전값을 가져온다.
		FRotator CurrentArmRotation = SpringArmComp->GetRelativeRotation();
		// 마우스 입력에 따라 Pitch 조정 (LookUpRate 적용)
		float NewPitch = CurrentArmRotation.Pitch - (LookInput.Y * LookUpRate * DeltaTime);
		// Pitch 값을 최소/최대 값으로 제한
		NewPitch = FMath::Clamp(NewPitch, MinPitch, MaxPitch);
		CurrentArmRotation.Pitch = NewPitch;
		SpringArmComp->SetRelativeRotation(CurrentArmRotation);
	}
}

// 입력 바인딩
void AMiniPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// EnhancedInputComponent로 캐스팅하여 Enhanced Input 시스템 사용
	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// 현재 컨트롤러가 AGoodPersonController임을 확인 후 입력 액션을 가져옴
		if (AMiniPlayerController* MyController = Cast<AMiniPlayerController>(GetController()))
		{
			// --- MoveAction 바인딩 ---
			if (MyController->MoveAction)
			{
				// 입력 발생 시 Move() 호출
				EnhancedInput->BindAction(
					MyController->MoveAction,
					ETriggerEvent::Triggered,
					this,
					&AMiniPawn::Move
				);
				// 입력 종료 시 StopMove() 호출하여 이동 입력 리셋
				EnhancedInput->BindAction(
					MyController->MoveAction,
					ETriggerEvent::Completed,
					this,
					&AMiniPawn::StopMove
				);
			}

			// --- LookAction 바인딩 ---
			if (MyController->LookAction)
			{
				// 입력 발생 시 Look() 호출
				EnhancedInput->BindAction(
					MyController->LookAction,
					ETriggerEvent::Triggered,
					this,
					&AMiniPawn::Look
				);
				// 입력 종료 시 StopLook() 호출하여 Look 입력 리셋
				EnhancedInput->BindAction(
					MyController->LookAction,
					ETriggerEvent::Completed,
					this,
					&AMiniPawn::StopLook
				);
			}
		}
	}
}

// 입력 콜백 함수
// Move 액션 처리: 2D 벡터 입력 저장
void AMiniPawn::Move(const FInputActionValue& Value)
{
	MovementInput = Value.Get<FVector2D>();
}

// Move 입력 종료: 입력값을 0으로 리셋
void AMiniPawn::StopMove(const FInputActionValue& Value)
{
	MovementInput = FVector2D::ZeroVector;
}

// Look 액션 처리: 2D 벡터 입력 저장 (Yaw, Pitch)
void AMiniPawn::Look(const FInputActionValue& Value)
{
	LookInput = Value.Get<FVector2D>();
}

// Look 입력 종료: 입력값을 0으로 리셋
void AMiniPawn::StopLook(const FInputActionValue& Value)
{
	LookInput = FVector2D::ZeroVector;
}

