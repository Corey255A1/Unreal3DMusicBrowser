// Fill out your copyright notice in the Description page of Project Settings.


#include "MusicGameBase.h"
#include "EmbyAPI.h"

void AMusicGameBase::BeginPlay() {
	Super::BeginPlay();
	EmbyConnection = MakeUnique<EmbyAPI>(ServerURL, APIKey);
	FEmbyObjectListReceivedDelegate genreListReceived;
	genreListReceived.BindUObject(this, &AMusicGameBase::GenresReceived);
	EmbyConnection->GetGenresAsync(genreListReceived);
}

void AMusicGameBase::Tick(float DeltaTime) {


}

void AMusicGameBase::GetArtistsOfGenreAsync(const FString& Genre) {
	FEmbyObjectListReceivedDelegate artistListReceived;
	artistListReceived.BindUObject(this, &AMusicGameBase::ArtistsReceived);
	EmbyConnection->GetArtistsOfGenreAsync(Genre, artistListReceived);
}

void AMusicGameBase::GetAlbumsOfArtistsAsync(const FString& Artist) {
	FEmbyObjectListReceivedDelegate albumListReceived;
	albumListReceived.BindUObject(this, &AMusicGameBase::AlbumsReceived);
	EmbyConnection->GetAlbumsOfArtistsAsync(Artist, albumListReceived);
}

void AMusicGameBase::GetSongsOfAlbumsAsync(const FString& Artist, const FString& Album) {
	FEmbySongListReceivedDelegate songListReceived;
	songListReceived.BindUObject(this, &AMusicGameBase::SongsReceived);
	EmbyConnection->GetSongsOfAlbumsAsync(Artist, Album, songListReceived);

}