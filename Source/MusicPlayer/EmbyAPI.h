// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <functional>
/**
 * 
 */
class MUSICPLAYER_API EmbyAPI
{
private:
	FString m_serverURL;
	FString m_apiKey;
	void MakeWebRequestAsync(const FString& route, const FString& params, std::function<void(const FString&)> responseCallback);
	void ProcessGenreResponse(const FString& response);
public:
	EmbyAPI(const FString& serverURL, const FString& apiKey);
	~EmbyAPI();
	void GetGenresAsync(std::function<void(const TArray<FString>&)> genresReceivedCallback);

};
