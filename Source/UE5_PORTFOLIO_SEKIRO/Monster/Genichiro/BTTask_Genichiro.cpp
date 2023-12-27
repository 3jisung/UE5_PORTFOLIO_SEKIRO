// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Genichiro.h"
#include "Kismet/GameplayStatics.h"

UBTTask_Genichiro::UBTTask_Genichiro()
{
	bNotifyTick = true;
	bNotifyTaskFinished = true;
}

void UBTTask_Genichiro::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	UBlackboardComponent* BlockBoard = OwnerComp.GetBlackboardComponent();

	if (nullptr == BlockBoard)
	{
		UE_LOG(LogTemp, Error, TEXT("%S(%u)> if (nullptr == BlockBoard)"), __FUNCTION__, __LINE__);
		return;
	}

	float StateTime = BlockBoard->GetValueAsFloat(TEXT("StateTime"));
	StateTime += DeltaSeconds;
	BlockBoard->SetValueAsFloat(TEXT("StateTime"), StateTime);
}

EBTNodeResult::Type UBTTask_Genichiro::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return EBTNodeResult::Type::InProgress;
}

AGlobalCharacter* UBTTask_Genichiro::GetGlobalCharacter(UBehaviorTreeComponent& OwnerComp)
{
	AAICon* AiCon = OwnerComp.GetOwner<AAICon>();
	if (nullptr == AiCon || false == AiCon->IsValidLowLevel())
	{
		UE_LOG(LogTemp, Error, TEXT("%S(%u)> if (nullptr == Character || false == Character->IsValidLowLevel())"), __FUNCTION__, __LINE__);
		// 정상적이지 않은 뭔가가 
		return nullptr;
	}

	// 컨트롤러에게 내가 조작하는 Pawn은 GetPawn을 사용한다.
	AGlobalCharacter* Character = AiCon->GetPawn<AGlobalCharacter>();

	if (nullptr == Character || false == Character->IsValidLowLevel())
	{
		UE_LOG(LogTemp, Error, TEXT("%S(%u)> if (nullptr == Character || false == Character->IsValidLowLevel())"), __FUNCTION__, __LINE__);
		// 정상적이지 않은 뭔가가 
		return nullptr;
	}

	return Character;
}

UBlackboardComponent* UBTTask_Genichiro::GetBlackboardComponent(UBehaviorTreeComponent& OwnerComp)
{
	UBlackboardComponent* BlockBoard = OwnerComp.GetBlackboardComponent();

	if (nullptr == BlockBoard)
	{
		UE_LOG(LogTemp, Error, TEXT("%S(%u)> if (nullptr == BlockBoard)"), __FUNCTION__, __LINE__);
		return nullptr;
	}

	return BlockBoard;
}

float UBTTask_Genichiro::GetStateTime(UBehaviorTreeComponent& OwnerComp)
{
	UBlackboardComponent* BlockBoard = OwnerComp.GetBlackboardComponent();

	if (nullptr == BlockBoard)
	{
		UE_LOG(LogTemp, Error, TEXT("if (nullptr == BlockBoard)"), __FUNCTION__, __LINE__);
		return 0.0f;
	}

	float StateTime = BlockBoard->GetValueAsFloat(TEXT("StateTime"));

	return StateTime;
}

void UBTTask_Genichiro::ResetStateTime(UBehaviorTreeComponent& OwnerComp)
{
	UBlackboardComponent* BlockBoard = OwnerComp.GetBlackboardComponent();

	if (nullptr == BlockBoard)
	{
		UE_LOG(LogTemp, Error, TEXT("%S(%u)> if (nullptr == BlockBoard)"), __FUNCTION__, __LINE__);
		return;
	}

	BlockBoard->SetValueAsFloat(TEXT("StateTime"), 0.0f);
}

GenichiroState UBTTask_Genichiro::GetGenichiroState(UBehaviorTreeComponent& OwnerComp)
{
	UBlackboardComponent* BlockBoard = OwnerComp.GetBlackboardComponent();

	if (nullptr == BlockBoard)
	{
		UE_LOG(LogTemp, Error, TEXT("if (nullptr == BlockBoard)"), __FUNCTION__, __LINE__);
		return GenichiroState::None;
	}

	uint8 Enum = BlockBoard->GetValueAsEnum(TEXT("GenichiroState"));

	return static_cast<GenichiroState>(Enum);
}

void UBTTask_Genichiro::SetStateChange(UBehaviorTreeComponent& OwnerComp, uint8 _State)
{
	UBlackboardComponent* BlockBoard = OwnerComp.GetBlackboardComponent();

	if (nullptr == BlockBoard)
	{
		UE_LOG(LogTemp, Error, TEXT("%S(%u)> if (nullptr == BlockBoard)"), __FUNCTION__, __LINE__);
		return;
	}

	BlockBoard->SetValueAsEnum(TEXT("GenichiroState"), _State);
	GetGlobalCharacter(OwnerComp)->SetAniState(_State);

	ResetStateTime(OwnerComp);

	FinishLatentTask(OwnerComp, EBTNodeResult::Type::Failed);
}

class AActor* UBTTask_Genichiro::GetTargetSearch(UBehaviorTreeComponent& OwnerComp)
{
	FString TargetTag = GetBlackboardComponent(OwnerComp)->GetValueAsString(TEXT("TargetTag"));
	TArray<AActor*> TargetActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), *TargetTag, TargetActors);

	AGlobalCharacter* Pawn = GetGlobalCharacter(OwnerComp);

	float SearchRange = GetBlackboardComponent(OwnerComp)->GetValueAsFloat(TEXT("SearchRange"));

	AActor* ResultActor = nullptr;

	if (0 != TargetActors.Num())
	{
		// 제일 가까운 대상 추적
		float Range = TNumericLimits<float>::Max();

		for (size_t i = 0; i < TargetActors.Num(); i++)
		{
			AGlobalCharacter* Target = Cast<AGlobalCharacter>(TargetActors[i]);
			
			float Dis = (Pawn->GetActorLocation() - Target->GetActorLocation()).Size();

			if (SearchRange <= Dis)
			{
				continue;
			}

			if (Range > Dis && Target->GetHP() > 0)
			{
				Range = Dis;
				ResultActor = Target;
			}
		}
	}

	return ResultActor;
}

bool UBTTask_Genichiro::AnimChangeCheck(UBehaviorTreeComponent& OwnerComp)
{
	uint8 Enum = GetBlackboardComponent(OwnerComp)->GetValueAsEnum(TEXT("GenichiroState"));

	if (GetGlobalCharacter(OwnerComp)->GetAniState() != Enum)
	{
		SetStateChange(OwnerComp, GetGlobalCharacter(OwnerComp)->GetAniState<GenichiroState>());

		return true;
	}

	return false;
}

UNavigationPath* UBTTask_Genichiro::PathFindNavPath(UBehaviorTreeComponent& OwnerComp, AActor* _Actor)
{
	return PathFindNavPath(OwnerComp, _Actor->GetActorLocation());
}

UNavigationPath* UBTTask_Genichiro::PathFindNavPath(UBehaviorTreeComponent& _OwnerComp, FVector _EndPos)
{
	UNavigationPath* PathObject = nullptr;
	FVector StartPos = GetGlobalCharacter(_OwnerComp)->GetActorLocation();
	FVector EndPos = _EndPos;

	PathObject = UNavigationSystemV1::FindPathToLocationSynchronously(
		GetWorld(),
		StartPos,
		EndPos
	);
	return PathObject;
}