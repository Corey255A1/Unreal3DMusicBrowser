// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MusicGameBase.generated.h"

/**
 * 
 */
UCLASS()
class MUSICPLAYER_API AMusicGameBase : public AGameModeBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly)
	FString APIKey;

	UPROPERTY(EditDefaultsOnly)
	FString ServerURL;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;
public:
	UFUNCTION(BlueprintImplementableEvent)
	void GenreAdded(const FString& Name);
};
