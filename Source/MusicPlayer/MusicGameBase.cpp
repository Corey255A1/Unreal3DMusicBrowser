// Fill out your copyright notice in the Description page of Project Settings.


#include "MusicGameBase.h"
#include "EmbyAPI.h"

void AMusicGameBase::BeginPlay() {
	Super::BeginPlay();
	EmbyConnection = std::make_unique<EmbyAPI>(ServerURL, APIKey);

	EmbyConnection->GetGenresAsync(std::bind(&AMusicGameBase::GenresReceived, this, std::placeholders::_1));

}

void AMusicGameBase::Tick(float DeltaTime) {


}