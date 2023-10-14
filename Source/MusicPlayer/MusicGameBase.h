// Corey Wunderlich - WunderVision 2023 
// https://www.wundervisionengineering.com/
#pragma once
#include "EmbyAPI.h"
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FEmbySong.h"
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
	void GenresReceived(const TArray<FEmbyObject>& GenreList);

	UFUNCTION(BlueprintImplementableEvent)
	void ArtistsReceived(const TArray<FEmbyObject>& ArtistList);

	UFUNCTION(BlueprintImplementableEvent)
	void AlbumsReceived(const TArray<FEmbyObject>& AlbumList);

	UFUNCTION(BlueprintImplementableEvent)
	void SongsReceived(const TArray<FEmbySong>& SongList);

	UFUNCTION(BlueprintCallable)
	void GetArtistsOfGenreAsync(const FString& Genre);

	UFUNCTION(BlueprintCallable)
	void GetAlbumsOfArtistsAsync(const FString& Artist);

	UFUNCTION(BlueprintCallable)
	void GetSongsOfAlbumsAsync(const FString& Artist, const FString& Album);
};
