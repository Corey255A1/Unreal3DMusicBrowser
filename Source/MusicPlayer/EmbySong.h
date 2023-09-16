// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EmbySong.generated.h"
/**
 * 
 */
USTRUCT(BlueprintType)
struct MUSICPLAYER_API FEmbySong
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString Name;

	UPROPERTY(BlueprintReadWrite)
	FString URL;
};
