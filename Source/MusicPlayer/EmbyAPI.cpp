// Fill out your copyright notice in the Description page of Project Settings.


#include "EmbyAPI.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Json.h"
#include "GenericPlatform/GenericPlatformHttp.h"

EmbyAPI::EmbyAPI(const FString& ServerURL, const FString& ApiKey) :
	m_ServerURL{ ServerURL },
	m_ApiKey{ ApiKey }
{
}

EmbyAPI::~EmbyAPI()
{
}

void EmbyAPI::MakeWebRequestAsync(const FString& Route, const FString& Params, FWebRequestCompleteDelegate& ResponseCallback)
{

	//FString encodedParams = FGenericPlatformHttp::UrlEncode(params);

	FString fullURL = FString::Printf(TEXT("%s/emby/%s?api_key=%s%s"), *m_ServerURL, *Route, *m_ApiKey, *Params);
	UE_LOG(LogTemp, Warning, TEXT("%s"), *fullURL);
	FHttpModule& httpModule = FHttpModule::Get();
	ResponseCallback.Execute(TEXT("WTF"));
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> pRequest = httpModule.CreateRequest();
	pRequest->SetVerb(TEXT("GET"));
	pRequest->SetURL(fullURL);
	pRequest->OnProcessRequestComplete().BindLambda(
		[ResponseCallback](
			FHttpRequestPtr pRequest,
			FHttpResponsePtr pResponse,
			bool connectedSuccessfully) mutable
			{
				check(IsInGameThread());
				if (connectedSuccessfully)
				{
					ResponseCallback.Execute(*pResponse->GetContentAsString());
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

void EmbyAPI::GetGenresAsync(FEmbyObjectListReceivedDelegate& GenresReceivedCallback)
{
	UE_LOG(LogTemp, Error, TEXT("Getting Genres"));
	FWebRequestCompleteDelegate requestCompleteCallback;
	requestCompleteCallback.BindLambda([GenresReceivedCallback, this](const FString& response)->void
		{
			TSharedRef<TJsonReader<TCHAR>> jsonReader = TJsonReaderFactory<TCHAR>::Create(response);
			TSharedPtr<FJsonValue> parsedObject;
			TArray<FEmbyObject> genreList;
			if (FJsonSerializer::Deserialize(jsonReader, parsedObject))
			{
				UE_LOG(LogTemp, Error, TEXT("Success In Deserial"));
				auto genreJsonArray = parsedObject->AsObject()->GetArrayField(TEXT("Items"));
				for (auto jsonObject : genreJsonArray)
				{
					FEmbyObject embyObject;
					embyObject.Name = jsonObject.Get()->AsObject()->GetStringField("Name");
					int32 objectID = jsonObject.Get()->AsObject()->GetIntegerField("Id");
					embyObject.PrimaryImageURL = FString::Printf(TEXT("%s/emby/Items/%d/Images/Primary?api_key=%s"), *m_ServerURL, objectID, *m_ApiKey);
					
					genreList.Add(embyObject);
					// UE_LOG(LogTemp, Warning, TEXT("We Got Something %s"), *genreName);
				}
				if (genreList.Num() > 0)
				{
					GenresReceivedCallback.Execute(genreList);
				}
			}

		});
	MakeWebRequestAsync("MusicGenres", "&SortOrder=Ascending", requestCompleteCallback);
}

void EmbyAPI::GetArtistsOfGenreAsync(const FString& Genre, FEmbyObjectListReceivedDelegate& ArtistsReceivedCallback)
{
	UE_LOG(LogTemp, Warning, TEXT("Getting Artists"));
	FWebRequestCompleteDelegate requestCompleteCallback;
	requestCompleteCallback.BindLambda([ArtistsReceivedCallback, this](const FString& response)->void
		{
			TSharedRef<TJsonReader<TCHAR>> jsonReader = TJsonReaderFactory<TCHAR>::Create(response);
			TSharedPtr<FJsonValue> parsedObject;
			TArray<FEmbyObject> artistList;
			if (FJsonSerializer::Deserialize(jsonReader, parsedObject))
			{
				UE_LOG(LogTemp, Warning, TEXT("Success In Deserial"));
				auto genreJsonArray = parsedObject->AsObject()->GetArrayField(TEXT("Items"));
				for (auto jsonObject : genreJsonArray)
				{
					FEmbyObject embyObject;
					embyObject.Name = jsonObject.Get()->AsObject()->GetStringField("Name");
					int32 objectID = jsonObject.Get()->AsObject()->GetIntegerField("Id");
					embyObject.PrimaryImageURL = FString::Printf(TEXT("%s/emby/Items/%d/Images/Primary?api_key=%s"), *m_ServerURL, objectID, *m_ApiKey);
					artistList.Add(embyObject);
					// UE_LOG(LogTemp, Warning, TEXT("We Got Something %s"), *genreName);
				}
				if (artistList.Num() > 0)
				{
					ArtistsReceivedCallback.Execute(artistList);
				}
			}

		});
	FString urlParameters = FString::Printf(TEXT("&SortOrder=Ascending&Recursive=true&IncludeItemTypes=MusicArtist&Genres=%s"), *FGenericPlatformHttp::UrlEncode(Genre));
	MakeWebRequestAsync("Items", urlParameters, requestCompleteCallback);
}

void EmbyAPI::GetAlbumsOfArtistsAsync(const FString& Artist, FEmbyObjectListReceivedDelegate& AlbumsReceivedCallback)
{
	UE_LOG(LogTemp, Warning, TEXT("Getting Albums"));
	FWebRequestCompleteDelegate requestCompleteCallback;
	requestCompleteCallback.BindLambda([AlbumsReceivedCallback, this](const FString& response)->void {
		TSharedRef<TJsonReader<TCHAR>> jsonReader = TJsonReaderFactory<TCHAR>::Create(response);
		TSharedPtr<FJsonValue> parsedObject;
		TArray<FEmbyObject> albumList;
		if (FJsonSerializer::Deserialize(jsonReader, parsedObject))
		{
			UE_LOG(LogTemp, Warning, TEXT("Success In Deserial"));
			auto genreJsonArray = parsedObject->AsObject()->GetArrayField(TEXT("Items"));
			for (auto jsonObject : genreJsonArray)
			{
				FEmbyObject embyObject;
				embyObject.Name = jsonObject.Get()->AsObject()->GetStringField("Name");
				int32 objectID = jsonObject.Get()->AsObject()->GetIntegerField("Id");
				embyObject.PrimaryImageURL = FString::Printf(TEXT("%s/emby/Items/%d/Images/Primary?api_key=%s"), *m_ServerURL, objectID, *m_ApiKey);
				albumList.Add(embyObject);
				// UE_LOG(LogTemp, Warning, TEXT("We Got Something %s"), *genreName);
			}
			if (albumList.Num() > 0)
			{
				AlbumsReceivedCallback.Execute(albumList);
			}
		}

		});
	FString urlParameters = FString::Printf(TEXT("&SortOrder=Ascending&Recursive=true&IncludeItemTypes=MusicAlbum&Artists=%s"), *FGenericPlatformHttp::UrlEncode(Artist));
	MakeWebRequestAsync("Items", urlParameters, requestCompleteCallback);
}

void EmbyAPI::GetSongsOfAlbumsAsync(const FString& Artist, const FString& Album, FEmbySongListReceivedDelegate& SongsReceivedCallback)
{
	UE_LOG(LogTemp, Warning, TEXT("Getting Songs"));
	FWebRequestCompleteDelegate requestCompleteCallback;
	requestCompleteCallback.BindLambda([SongsReceivedCallback, this](const FString& response)->void {
		TSharedRef<TJsonReader<TCHAR>> jsonReader = TJsonReaderFactory<TCHAR>::Create(response);
		TSharedPtr<FJsonValue> parsedObject;
		TArray<FEmbySong> songList;
		if (FJsonSerializer::Deserialize(jsonReader, parsedObject))
		{
			UE_LOG(LogTemp, Warning, TEXT("Success In Deserial"));
			auto genreJsonArray = parsedObject->AsObject()->GetArrayField(TEXT("Items"));
			for (auto jsonObject : genreJsonArray)
			{
				int32 objectID = jsonObject.Get()->AsObject()->GetIntegerField("Id");
				FEmbySong songStruct;
				songStruct.URL = FString::Printf(TEXT("%s/emby/Audio/%d/stream?static=true&api_key=%s"), *m_ServerURL, objectID, *m_ApiKey);
				songStruct.PrimaryImageURL = FString::Printf(TEXT("%s/emby/Items/%d/Images/Primary?api_key=%s"), *m_ServerURL, objectID, *m_ApiKey);
				songStruct.Name = jsonObject.Get()->AsObject()->GetStringField("Name");
				songList.Add(songStruct);
				// UE_LOG(LogTemp, Warning, TEXT("We Got Something %s"), *genreName);
			}
			if (songList.Num() > 0)
			{
				SongsReceivedCallback.Execute(songList);
			}
		}

		});

	FString urlParameters = FString::Printf(TEXT("&Type=Audio&Recursive=True&Artists=%s&Albums=%s"), *FGenericPlatformHttp::UrlEncode(Artist), *FGenericPlatformHttp::UrlEncode(Album));
	MakeWebRequestAsync("Items", urlParameters, requestCompleteCallback);
}
