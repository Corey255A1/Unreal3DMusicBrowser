// Fill out your copyright notice in the Description page of Project Settings.


#include "EmbyAPI.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Json.h"
#include "GenericPlatform/GenericPlatformHttp.h"

EmbyAPI::EmbyAPI(const FString& serverURL, const FString& apiKey) :
	m_serverURL{ serverURL },
	m_apiKey{ apiKey }
{
}

EmbyAPI::~EmbyAPI()
{
}

void EmbyAPI::MakeWebRequestAsync(const FString& route, const FString& params, FWebRequestCompleteDelegate& responseCallback)
{

	//FString encodedParams = FGenericPlatformHttp::UrlEncode(params);

	FString fullURL = FString::Printf(TEXT("%s/emby/%s?api_key=%s%s"), *m_serverURL, *route, *m_apiKey, *params);
	UE_LOG(LogTemp, Warning, TEXT("%s"), *fullURL);
	FHttpModule& httpModule = FHttpModule::Get();
	responseCallback.Execute(TEXT("WTF"));
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> pRequest = httpModule.CreateRequest();
	pRequest->SetVerb(TEXT("GET"));
	pRequest->SetURL(fullURL);
	pRequest->OnProcessRequestComplete().BindLambda(
		[responseCallback](
			FHttpRequestPtr pRequest,
			FHttpResponsePtr pResponse,
			bool connectedSuccessfully) mutable
			{
				check(IsInGameThread());
				if (connectedSuccessfully)
				{
					responseCallback.Execute(*pResponse->GetContentAsString());
				}
				else
				{
					switch (pRequest->GetStatus())
					{
					case EHttpRequestStatus::Failed_ConnectionError:
						UE_LOG(LogTemp, Error, TEXT("Connection failed."));
					default:
						UE_LOG(LogTemp, Error, TEXT("Request failed."));
					}
				}
			});
	pRequest->ProcessRequest();
}

void EmbyAPI::GetGenresAsync(FEmbyStringListReceivedDelegate& genresReceivedCallback)
{
	UE_LOG(LogTemp, Error, TEXT("Getting Genres"));
	FWebRequestCompleteDelegate requestCompleteCallback;
	requestCompleteCallback.BindLambda([genresReceivedCallback](const FString& response)->void
		{
			TSharedRef<TJsonReader<TCHAR>> jsonReader = TJsonReaderFactory<TCHAR>::Create(response);
			TSharedPtr<FJsonValue> parsedObject;
			TArray<FString> genreList;
			if (FJsonSerializer::Deserialize(jsonReader, parsedObject))
			{
				UE_LOG(LogTemp, Error, TEXT("Success In Deserial"));
				auto genreJsonArray = parsedObject->AsObject()->GetArrayField(TEXT("Items"));
				for (auto jsonObject : genreJsonArray)
				{
					FString genreName = jsonObject.Get()->AsObject()->GetStringField("Name");
					genreList.Add(genreName);
					// UE_LOG(LogTemp, Warning, TEXT("We Got Something %s"), *genreName);
				}
				if (genreList.Num() > 0)
				{
					genresReceivedCallback.Execute(genreList);
				}
			}

		});
	MakeWebRequestAsync("MusicGenres", "&SortOrder=Ascending", requestCompleteCallback);
}

void EmbyAPI::GetArtistsOfGenreAsync(const FString& genre, FEmbyStringListReceivedDelegate& artistsReceivedCallback)
{
	UE_LOG(LogTemp, Warning, TEXT("Getting Artists"));
	FWebRequestCompleteDelegate requestCompleteCallback;
	requestCompleteCallback.BindLambda([artistsReceivedCallback](const FString& response)->void
		{
			TSharedRef<TJsonReader<TCHAR>> jsonReader = TJsonReaderFactory<TCHAR>::Create(response);
			TSharedPtr<FJsonValue> parsedObject;
			TArray<FString> artistList;
			if (FJsonSerializer::Deserialize(jsonReader, parsedObject))
			{
				UE_LOG(LogTemp, Warning, TEXT("Success In Deserial"));
				auto genreJsonArray = parsedObject->AsObject()->GetArrayField(TEXT("Items"));
				for (auto jsonObject : genreJsonArray)
				{
					FString genreName = jsonObject.Get()->AsObject()->GetStringField("Name");
					artistList.Add(genreName);
					// UE_LOG(LogTemp, Warning, TEXT("We Got Something %s"), *genreName);
				}
				if (artistList.Num() > 0)
				{
					artistsReceivedCallback.Execute(artistList);
				}
			}

		});
	FString urlParameters = FString::Printf(TEXT("&SortOrder=Ascending&Recursive=true&IncludeItemTypes=MusicArtist&Genres=%s"), *FGenericPlatformHttp::UrlEncode(genre));
	MakeWebRequestAsync("Items", urlParameters, requestCompleteCallback);
}

void EmbyAPI::GetAlbumsOfArtistsAsync(const FString& artist, FEmbyStringListReceivedDelegate& albumsReceivedCallback)
{
	UE_LOG(LogTemp, Warning, TEXT("Getting Albums"));
	FWebRequestCompleteDelegate requestCompleteCallback;
	requestCompleteCallback.BindLambda([albumsReceivedCallback](const FString& response)->void {
		TSharedRef<TJsonReader<TCHAR>> jsonReader = TJsonReaderFactory<TCHAR>::Create(response);
		TSharedPtr<FJsonValue> parsedObject;
		TArray<FString> albumList;
		if (FJsonSerializer::Deserialize(jsonReader, parsedObject))
		{
			UE_LOG(LogTemp, Warning, TEXT("Success In Deserial"));
			auto genreJsonArray = parsedObject->AsObject()->GetArrayField(TEXT("Items"));
			for (auto jsonObject : genreJsonArray)
			{
				FString albumName = jsonObject.Get()->AsObject()->GetStringField("Name");
				albumList.Add(albumName);
				// UE_LOG(LogTemp, Warning, TEXT("We Got Something %s"), *genreName);
			}
			if (albumList.Num() > 0)
			{
				albumsReceivedCallback.Execute(albumList);
			}
		}

		});
	FString urlParameters = FString::Printf(TEXT("&SortOrder=Ascending&Recursive=true&IncludeItemTypes=MusicAlbum&Artists=%s"), *FGenericPlatformHttp::UrlEncode(artist));
	MakeWebRequestAsync("Items", urlParameters, requestCompleteCallback);
}

void EmbyAPI::GetSongsOfAlbumsAsync(const FString& artist, const FString& album, FEmbyStringIntListReceivedDelegate& songsReceivedCallback)
{
	UE_LOG(LogTemp, Warning, TEXT("Getting Songs"));
	FWebRequestCompleteDelegate requestCompleteCallback;
	requestCompleteCallback.BindLambda([songsReceivedCallback, this](const FString& response)->void {
		TSharedRef<TJsonReader<TCHAR>> jsonReader = TJsonReaderFactory<TCHAR>::Create(response);
		TSharedPtr<FJsonValue> parsedObject;
		TArray<FEmbySong> songList;
		if (FJsonSerializer::Deserialize(jsonReader, parsedObject))
		{
			UE_LOG(LogTemp, Warning, TEXT("Success In Deserial"));
			auto genreJsonArray = parsedObject->AsObject()->GetArrayField(TEXT("Items"));
			for (auto jsonObject : genreJsonArray)
			{
				int32 songID = jsonObject.Get()->AsObject()->GetIntegerField("Id");
				FEmbySong songStruct;
				songStruct.URL = FString::Printf(TEXT("%s/emby/Audio/%d/stream?static=true&api_key=%s"), *m_serverURL, songID, *m_apiKey);
				songStruct.Name = jsonObject.Get()->AsObject()->GetStringField("Name");
				songList.Add(songStruct);
				// UE_LOG(LogTemp, Warning, TEXT("We Got Something %s"), *genreName);
			}
			if (songList.Num() > 0)
			{
				songsReceivedCallback.Execute(songList);
			}
		}

		});

	FString urlParameters = FString::Printf(TEXT("&Type=Audio&Recursive=True&Artists=%s&Albums=%s"), *FGenericPlatformHttp::UrlEncode(artist), *FGenericPlatformHttp::UrlEncode(album));
	MakeWebRequestAsync("Items", urlParameters, requestCompleteCallback);
}
