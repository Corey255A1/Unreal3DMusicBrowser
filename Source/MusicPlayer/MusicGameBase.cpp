// Fill out your copyright notice in the Description page of Project Settings.


#include "MusicGameBase.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Json.h"


void AMusicGameBase::BeginPlay() {
	Super::BeginPlay();
    UE_LOG(LogTemp, Error, TEXT("BLERG"));
	FString testURL = TEXT("http://192.168.1.125:8096/emby/MusicGenres?api_key=0edb611ccc2c4522b19c30bcc1751614");
	FHttpModule& httpModule = FHttpModule::Get();

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> pRequest = httpModule.CreateRequest();
    pRequest->SetVerb(TEXT("GET"));
	pRequest->SetURL(testURL);

    pRequest->OnProcessRequestComplete().BindLambda(
        // Here, we "capture" the 'this' pointer (the "&"), so our lambda can call this
        // class's methods in the callback.
        [&](
            FHttpRequestPtr pRequest,
            FHttpResponsePtr pResponse,
            bool connectedSuccessfully) mutable {

                if (connectedSuccessfully) {

                    // We should have a JSON response - attempt to process it.
                    TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(pResponse->GetContentAsString());
                    TSharedPtr<FJsonValue> ParsedObject;

                    UE_LOG(LogTemp, Error, TEXT("Got Some Stuff."));
                    UE_LOG(LogTemp, Error, TEXT("%s"), *pResponse->GetContentAsString());
                    if (FJsonSerializer::Deserialize(JsonReader, ParsedObject))
                    {
                        UE_LOG(LogTemp, Error, TEXT("Success In Deserial"));
                        auto GenreArray = ParsedObject->AsObject()->GetArrayField(TEXT("Items"));
                        for (auto jsonObject : GenreArray) {
                            UE_LOG(LogTemp, Warning, TEXT("We Got Something %s"), *jsonObject.Get()->AsObject()->GetStringField("Name"));
                        }
                    }
                }
                else {
                    switch (pRequest->GetStatus()) {
                    case EHttpRequestStatus::Failed_ConnectionError:
                        UE_LOG(LogTemp, Error, TEXT("Connection failed."));
                    default:
                        UE_LOG(LogTemp, Error, TEXT("Request failed."));
                    }
                }
        });

    pRequest->ProcessRequest();

}

void AMusicGameBase::Tick(float DeltaTime) {


}