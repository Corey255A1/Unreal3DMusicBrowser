// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EmbySong.h"
/**
 * 
 */

DECLARE_DELEGATE_OneParam(FWebRequestCompleteDelegate, const FString&);
DECLARE_DELEGATE_OneParam(FEmbyStringListReceivedDelegate, const TArray<FString>&);
DECLARE_DELEGATE_OneParam(FEmbyStringIntListReceivedDelegate, const TArray<FEmbySong>&);

class MUSICPLAYER_API EmbyAPI
{
private:
	FString m_serverURL;
	FString m_apiKey;
	void MakeWebRequestAsync(const FString& route, const FString& params, FWebRequestCompleteDelegate& responseCallback);
public:
	EmbyAPI(const FString& serverURL, const FString& apiKey);
	~EmbyAPI();
	void GetGenresAsync(FEmbyStringListReceivedDelegate& genresReceivedCallback);
	void GetArtistsOfGenreAsync(const FString& genre, FEmbyStringListReceivedDelegate& artistsReceivedCallback);
	void GetAlbumsOfArtistsAsync(const FString& artist, FEmbyStringListReceivedDelegate& albumsReceivedCallback);
	void GetSongsOfAlbumsAsync(const FString& artist, const FString& album, FEmbyStringIntListReceivedDelegate& songsReceivedCallback);
};
