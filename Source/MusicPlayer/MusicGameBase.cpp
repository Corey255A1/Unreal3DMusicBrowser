// Fill out your copyright notice in the Description page of Project Settings.


#include "MusicGameBase.h"
#include "EmbyAPI.h"

void AMusicGameBase::BeginPlay() {
	Super::BeginPlay();
	EmbyConnection = MakeUnique<EmbyAPI>(ServerURL, APIKey);
	FEmbyStringListReceivedDelegate genreListReceived;
	genreListReceived.BindUObject(this, &AMusicGameBase::GenresReceived);
	EmbyConnection->GetGenresAsync(genreListReceived);
}

void AMusicGameBase::Tick(float DeltaTime) {


}

void AMusicGameBase::GetArtistsOfGenreAsync(const FString& genre) {
	FEmbyStringListReceivedDelegate artistListReceived;
	artistListReceived.BindUObject(this, &AMusicGameBase::ArtistsReceived);
	EmbyConnection->GetArtistsOfGenreAsync(genre, artistListReceived);
}

void AMusicGameBase::GetAlbumsOfArtistsAsync(const FString& artist) {
	FEmbyStringListReceivedDelegate albumListReceived;
	albumListReceived.BindUObject(this, &AMusicGameBase::AlbumsReceived);
	EmbyConnection->GetAlbumsOfArtistsAsync(artist, albumListReceived);
}

void AMusicGameBase::GetSongsOfAlbumsAsync(const FString& artist, const FString& album) {
	FEmbyStringListReceivedDelegate songListReceived;
	songListReceived.BindUObject(this, &AMusicGameBase::SongsReceived);
	EmbyConnection->GetSongsOfAlbumsAsync(artist, album, songListReceived);

}
