// Corey Wunderlich - WunderVision 2023 
// https://www.wundervisionengineering.com/

#include "MusicGameBase.h"
#include "EmbyAPI.h"

void AMusicGameBase::BeginPlay() {
	Super::BeginPlay();
	EmbyConnection = MakeUnique<EmbyAPI>(ServerURL, APIKey);
	FEmbyObjectListReceivedDelegate GenreListReceived;
	GenreListReceived.BindUObject(this, &AMusicGameBase::GenresReceived);
	EmbyConnection->GetGenresAsync(GenreListReceived);
}

void AMusicGameBase::Tick(float DeltaTime) {


}

void AMusicGameBase::GetArtistsOfGenreAsync(const FString& Genre) {
	FEmbyObjectListReceivedDelegate ArtistListReceived;
	ArtistListReceived.BindUObject(this, &AMusicGameBase::ArtistsReceived);
	EmbyConnection->GetArtistsOfGenreAsync(Genre, ArtistListReceived);
}

void AMusicGameBase::GetAlbumsOfArtistsAsync(const FString& Artist) {
	FEmbyObjectListReceivedDelegate AlbumListReceived;
	AlbumListReceived.BindUObject(this, &AMusicGameBase::AlbumsReceived);
	EmbyConnection->GetAlbumsOfArtistsAsync(Artist, AlbumListReceived);
}

void AMusicGameBase::GetSongsOfAlbumsAsync(const FString& Artist, const FString& Album) {
	FEmbySongListReceivedDelegate SongListReceived;
	SongListReceived.BindUObject(this, &AMusicGameBase::SongsReceived);
	EmbyConnection->GetSongsOfAlbumsAsync(Artist, Album, SongListReceived);

}