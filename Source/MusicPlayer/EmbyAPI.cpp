// Corey Wunderlich - WunderVision 2023 
// https://www.wundervisionengineering.com/

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
	FString FullURL = FString::Printf(TEXT("%s/emby/%s?api_key=%s%s"), *m_ServerURL, *Route, *m_ApiKey, *Params);
	UE_LOG(LogTemp, Warning, TEXT("%s"), *FullURL);
	FHttpModule& HttpModule = FHttpModule::Get();
	ResponseCallback.Execute(TEXT("WTF"));
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> pRequest = HttpModule.CreateRequest();
	pRequest->SetVerb(TEXT("GET"));
	pRequest->SetURL(FullURL);
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
	FWebRequestCompleteDelegate RequestCompleteCallback;
	RequestCompleteCallback.BindLambda([GenresReceivedCallback, this](const FString& Response)->void
		{
			TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(Response);
			TSharedPtr<FJsonValue> ParsedObject;
			TArray<FEmbyObject> GenreList;
			if (FJsonSerializer::Deserialize(JsonReader, ParsedObject))
			{
				UE_LOG(LogTemp, Error, TEXT("Success In Deserial"));
				auto GenreJsonArray = ParsedObject->AsObject()->GetArrayField(TEXT("Items"));
				for (auto JsonObject : GenreJsonArray)
				{
					FEmbyObject EmbyObject;
					EmbyObject.Name = JsonObject.Get()->AsObject()->GetStringField("Name");
					int32 ObjectID = JsonObject.Get()->AsObject()->GetIntegerField("Id");
					EmbyObject.PrimaryImageURL = FString::Printf(TEXT("%s/emby/Items/%d/Images/Primary?api_key=%s&maxHeight=220&maxWidth=220"), *m_ServerURL, ObjectID, *m_ApiKey);
					
					GenreList.Add(EmbyObject);
					// UE_LOG(LogTemp, Warning, TEXT("We Got Something %s"), *genreName);
				}
				if (GenreList.Num() > 0)
				{
					GenresReceivedCallback.Execute(GenreList);
				}
			}

		});
	MakeWebRequestAsync("MusicGenres", "&SortOrder=Ascending", RequestCompleteCallback);
}

void EmbyAPI::GetArtistsOfGenreAsync(const FString& Genre, FEmbyObjectListReceivedDelegate& ArtistsReceivedCallback)
{
	UE_LOG(LogTemp, Warning, TEXT("Getting Artists"));
	FWebRequestCompleteDelegate RequestCompleteCallback;
	RequestCompleteCallback.BindLambda([ArtistsReceivedCallback, this](const FString& Response)->void
		{
			TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(Response);
			TSharedPtr<FJsonValue> ParsedObject;
			TArray<FEmbyObject> ArtistList;
			if (FJsonSerializer::Deserialize(JsonReader, ParsedObject))
			{
				UE_LOG(LogTemp, Warning, TEXT("Success In Deserial"));
				auto GenreJsonArray = ParsedObject->AsObject()->GetArrayField(TEXT("Items"));
				for (auto JsonObject : GenreJsonArray)
				{
					FEmbyObject EmbyObject;
					EmbyObject.Name = JsonObject.Get()->AsObject()->GetStringField("Name");
					int32 ObjectID = JsonObject.Get()->AsObject()->GetIntegerField("Id");
					EmbyObject.PrimaryImageURL = FString::Printf(TEXT("%s/emby/Items/%d/Images/Primary?api_key=%s&maxHeight=220&maxWidth=220"), *m_ServerURL, ObjectID, *m_ApiKey);
					ArtistList.Add(EmbyObject);
				}
				if (ArtistList.Num() > 0)
				{
					ArtistsReceivedCallback.Execute(ArtistList);
				}
			}

		});
	FString UrlParameters = FString::Printf(TEXT("&SortOrder=Ascending&Recursive=true&IncludeItemTypes=MusicArtist&Genres=%s"), *FGenericPlatformHttp::UrlEncode(Genre));
	MakeWebRequestAsync("Items", UrlParameters, RequestCompleteCallback);
}

void EmbyAPI::GetAlbumsOfArtistsAsync(const FString& Artist, FEmbyObjectListReceivedDelegate& AlbumsReceivedCallback)
{
	UE_LOG(LogTemp, Warning, TEXT("Getting Albums"));
	FWebRequestCompleteDelegate RequestCompleteCallback;
	RequestCompleteCallback.BindLambda([AlbumsReceivedCallback, this](const FString& Response)->void {
		TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(Response);
		TSharedPtr<FJsonValue> ParsedObject;
		TArray<FEmbyObject> AlbumList;
		if (FJsonSerializer::Deserialize(JsonReader, ParsedObject))
		{
			UE_LOG(LogTemp, Warning, TEXT("Success In Deserial"));
			auto GenreJsonArray = ParsedObject->AsObject()->GetArrayField(TEXT("Items"));
			for (auto JsonObject : GenreJsonArray)
			{
				FEmbyObject EmbyObject;
				EmbyObject.Name = JsonObject.Get()->AsObject()->GetStringField("Name");
				int32 ObjectID = JsonObject.Get()->AsObject()->GetIntegerField("Id");
				EmbyObject.PrimaryImageURL = FString::Printf(TEXT("%s/emby/Items/%d/Images/Primary?api_key=%s&maxHeight=220&maxWidth=220"), *m_ServerURL, ObjectID, *m_ApiKey);
				AlbumList.Add(EmbyObject);
			}
			if (AlbumList.Num() > 0)
			{
				AlbumsReceivedCallback.Execute(AlbumList);
			}
		}

		});
	FString UrlParameters = FString::Printf(TEXT("&SortOrder=Ascending&Recursive=true&IncludeItemTypes=MusicAlbum&Artists=%s"), *FGenericPlatformHttp::UrlEncode(Artist));
	MakeWebRequestAsync("Items", UrlParameters, RequestCompleteCallback);
}

void EmbyAPI::GetSongsOfAlbumsAsync(const FString& Artist, const FString& Album, FEmbySongListReceivedDelegate& SongsReceivedCallback)
{
	UE_LOG(LogTemp, Warning, TEXT("Getting Songs"));
	FWebRequestCompleteDelegate RequestCompleteCallback;
	RequestCompleteCallback.BindLambda([SongsReceivedCallback, this](const FString& Response)->void {
		TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(Response);
		TSharedPtr<FJsonValue> ParsedObject;
		TArray<FEmbySong> SongList;
		if (FJsonSerializer::Deserialize(JsonReader, ParsedObject))
		{
			UE_LOG(LogTemp, Warning, TEXT("Success In Deserial"));
			auto GenreJsonArray = ParsedObject->AsObject()->GetArrayField(TEXT("Items"));
			for (auto JsonObject : GenreJsonArray)
			{
				int32 ObjectID = JsonObject.Get()->AsObject()->GetIntegerField("Id");
				int32 AlbumID = JsonObject.Get()->AsObject()->GetIntegerField("AlbumID");
				FEmbySong SongStruct;
				SongStruct.URL = FString::Printf(TEXT("%s/emby/Audio/%d/stream?static=true&api_key=%s"), *m_ServerURL, ObjectID, *m_ApiKey);
				SongStruct.PrimaryImageURL = FString::Printf(TEXT("%s/emby/Items/%d/Images/Primary?api_key=%s&maxHeight=220&maxWidth=220"), *m_ServerURL, AlbumID, *m_ApiKey);
				SongStruct.Name = JsonObject.Get()->AsObject()->GetStringField("Name");
				SongList.Add(SongStruct);
			}
			if (SongList.Num() > 0)
			{
				SongsReceivedCallback.Execute(SongList);
			}
		}

		});

	FString UrlParameters = FString::Printf(TEXT("&Type=Audio&Recursive=True&Artists=%s&Albums=%s"), *FGenericPlatformHttp::UrlEncode(Artist), *FGenericPlatformHttp::UrlEncode(Album));
	MakeWebRequestAsync("Items", UrlParameters, RequestCompleteCallback);
}
