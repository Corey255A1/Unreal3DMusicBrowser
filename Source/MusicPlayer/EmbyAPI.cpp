// Fill out your copyright notice in the Description page of Project Settings.


#include "EmbyAPI.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Json.h"
EmbyAPI::EmbyAPI(const FString& serverURL, const FString& apiKey):
	m_serverURL{serverURL},
	m_apiKey{apiKey}
{
}

EmbyAPI::~EmbyAPI()
{
}

void EmbyAPI::MakeWebRequestAsync(const FString& route, const FString& params, std::function<void(const FString&)> responseCallback) {
    FString fullURL = FString::Printf(TEXT("%s/emby/%s?api_key=%s"), *m_serverURL, *route, *m_apiKey);
    UE_LOG(LogTemp, Warning, TEXT("%s"), *fullURL);
    FHttpModule& httpModule = FHttpModule::Get();
    responseCallback(TEXT("WTF"));
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> pRequest = httpModule.CreateRequest();
    pRequest->SetVerb(TEXT("GET"));
    pRequest->SetURL(fullURL);
    pRequest->OnProcessRequestComplete().BindLambda(
        [responseCallback](
            FHttpRequestPtr pRequest,
            FHttpResponsePtr pResponse,
            bool connectedSuccessfully) mutable {
                check(IsInGameThread());
                if (connectedSuccessfully) {
                    if (responseCallback) {
                        responseCallback(*pResponse->GetContentAsString());
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

void EmbyAPI::ProcessGenreResponse(const FString& response) {
    UE_LOG(LogTemp, Error, TEXT("PROCE"));
}

void EmbyAPI::GetGenresAsync(std::function<void(const TArray<FString>&)> genresReceivedCallback) {
    UE_LOG(LogTemp, Error, TEXT("Getting Genres"));
    MakeWebRequestAsync("MusicGenres", "", [genresReceivedCallback](const FString& response)->void {
        TSharedRef<TJsonReader<TCHAR>> jsonReader = TJsonReaderFactory<TCHAR>::Create(response);
        TSharedPtr<FJsonValue> parsedObject;
        TArray<FString> genreList;
        if (FJsonSerializer::Deserialize(jsonReader, parsedObject))
        {
            UE_LOG(LogTemp, Error, TEXT("Success In Deserial"));
            auto genreJsonArray = parsedObject->AsObject()->GetArrayField(TEXT("Items"));
            for (auto jsonObject : genreJsonArray) {
                FString genreName = jsonObject.Get()->AsObject()->GetStringField("Name");
                genreList.Add(genreName);
               // UE_LOG(LogTemp, Warning, TEXT("We Got Something %s"), *genreName);
            }
            if (genreList.Num() > 0) {
                genresReceivedCallback(genreList);
            }
        }

    });
}
