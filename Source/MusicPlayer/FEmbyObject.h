// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FEmbyObject.generated.h"
/**
 *
 */
USTRUCT(BlueprintType)
struct MUSICPLAYER_API FEmbyObject
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString Name;

	UPROPERTY(BlueprintReadWrite)
	FString PrimaryImageURL;
};