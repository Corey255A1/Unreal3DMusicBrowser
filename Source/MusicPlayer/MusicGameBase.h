// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "EmbyAPI.h"
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
	TUniquePtr<EmbyAPI> EmbyConnection;


	UPROPERTY(EditDefaultsOnly)
	FString APIKey;

	UPROPERTY(EditDefaultsOnly)
	FString ServerURL;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;
public:
	UFUNCTION(BlueprintImplementableEvent)
	void GenresReceived(const TArray<FString>& GenreList);

	UFUNCTION(BlueprintImplementableEvent)
	void ArtistsReceived(const TArray<FString>& ArtistList);

	UFUNCTION(BlueprintImplementableEvent)
	void AlbumsReceived(const TArray<FString>& AlbumList);

	UFUNCTION(BlueprintImplementableEvent)
	void SongsReceived(const TArray<FString>& SongList);

	UFUNCTION(BlueprintCallable)
	void GetArtistsOfGenreAsync(const FString& genre);

	UFUNCTION(BlueprintCallable)
	void GetAlbumsOfArtistsAsync(const FString& artist);

	UFUNCTION(BlueprintCallable)
	void GetSongsOfAlbumsAsync(const FString& artist, const FString& album);
};
