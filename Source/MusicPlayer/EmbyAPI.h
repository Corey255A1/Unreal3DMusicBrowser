// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FEmbySong.h"
/**
 * 
 */

DECLARE_DELEGATE_OneParam(FWebRequestCompleteDelegate, const FString&);
DECLARE_DELEGATE_OneParam(FEmbyStringListReceivedDelegate, const TArray<FString>&);
DECLARE_DELEGATE_OneParam(FEmbySongListReceivedDelegate, const TArray<FEmbySong>&);
DECLARE_DELEGATE_OneParam(FEmbyObjectListReceivedDelegate, const TArray<FEmbyObject>&);

class MUSICPLAYER_API EmbyAPI
{
private:
	FString m_ServerURL;
	FString m_ApiKey;
	void MakeWebRequestAsync(const FString& Route, const FString& Params, FWebRequestCompleteDelegate& ResponseCallback);
public:
	EmbyAPI(const FString& ServerURL, const FString& ApiKey);
	~EmbyAPI();
	void GetGenresAsync(FEmbyObjectListReceivedDelegate& GenresReceivedCallback);
	void GetArtistsOfGenreAsync(const FString& Genre, FEmbyObjectListReceivedDelegate& ArtistsReceivedCallback);
	void GetAlbumsOfArtistsAsync(const FString& Artist, FEmbyObjectListReceivedDelegate& AlbumsReceivedCallback);
	void GetSongsOfAlbumsAsync(const FString& Artist, const FString& Album, FEmbySongListReceivedDelegate& SongsReceivedCallback);
};
