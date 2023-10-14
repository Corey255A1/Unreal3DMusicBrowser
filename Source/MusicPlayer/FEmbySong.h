// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FEmbyObject.h"
#include "FEmbySong.generated.h"
/**
 * 
 */
USTRUCT(BlueprintType)
struct MUSICPLAYER_API FEmbySong: public FEmbyObject
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString URL;
};
