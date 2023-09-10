// Fill out your copyright notice in the Description page of Project Settings.


#include "MusicGameBase.h"
#include "EmbyAPI.h"

void AMusicGameBase::BeginPlay() {
	Super::BeginPlay();
	EmbyConnection = MakeUnique<EmbyAPI>(ServerURL, APIKey);
	FEmbyStringListReceivedDelegate genreListReceived;
	genreListReceived.BindUObject(this, &AMusicGameBase::GenresReceived);
	EmbyConnection->GetGenresAsync(genreListReceived);

	FEmbyStringListReceivedDelegate artistListReceived;
	artistListReceived.BindUObject(this, &AMusicGameBase::ArtistsReceived);
	EmbyConnection->GetArtistsOfGenreAsync("Pop", artistListReceived);

	FEmbyStringListReceivedDelegate albumListReceived;
	albumListReceived.BindUObject(this, &AMusicGameBase::AlbumsReceived);
	EmbyConnection->GetAlbumsOfArtistsAsync("The Killers", albumListReceived);

	FEmbyStringListReceivedDelegate songListReceived;
	songListReceived.BindUObject(this, &AMusicGameBase::SongsReceived);
	EmbyConnection->GetSongsOfAlbumsAsync("The Killers", "Hot Fuss", songListReceived);

}

void AMusicGameBase::Tick(float DeltaTime) {


}