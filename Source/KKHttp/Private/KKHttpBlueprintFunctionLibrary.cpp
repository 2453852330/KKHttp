// Fill out your copyright notice in the Description page of Project Settings.


#include "KKHttpBlueprintFunctionLibrary.h"

#include "CookOnTheFly.h"
#include "HttpModule.h"
#include "HttpServerModule.h"

#include "Interfaces/IHttpResponse.h"


void UKKHttpBlueprintFunctionLibrary::BPL_HttpRequest(
	EKKHttpVerb Verb,
	FString URL,
	float TimeOut,
	TMap<FString, FString> Header,
	TMap<FString, FString> FormData,
	FString PostContent,
	FOnRequestProgress64_KK OnRequestProgress64,
	FOnStatusCodeReceived_KK OnStatusCodeReceived,
	FOnProcessRequestComplete_KK OnProcessRequestComplete)
{
	TSharedRef<IHttpRequest> http_request = FHttpModule::Get().CreateRequest();
	// verb
	switch (Verb)
	{
	case EKKHttpVerb::EGet:
		http_request->SetVerb(TEXT("GET"));
		break;
	case EKKHttpVerb::EPost:
		http_request->SetVerb(TEXT("POST"));
		http_request->SetContentAsString(PostContent);
		break;
	}
	// time_out
	http_request->SetTimeout(TimeOut);
	// header
	for (const TTuple<FString,FString>& it : Header)
	{
		http_request->SetHeader(it.Key,it.Value);
	}
	// URL
	FString url;
	url += URL;
	if (FormData.Num())
	{
		url+=TEXT("?");
		for (const TTuple<FString,FString> & it : FormData)
		{
			// is last ?
			url += (it.Key + TEXT("=") + it.Value + TEXT("&"));
		}
	}
	http_request->SetURL(url);
	// delegate
	http_request->OnRequestProgress64().BindLambda([OnRequestProgress64](FHttpRequestPtr Request, uint64 BytesSent, uint64 BytesReceived)
	{
		OnRequestProgress64.ExecuteIfBound(FHttpRequestPtrWrapper(Request),BytesSent,BytesReceived);
	});
	http_request->OnStatusCodeReceived().BindLambda([OnStatusCodeReceived](FHttpRequestPtr Request, int32 StatusCode)
	{
		OnStatusCodeReceived.ExecuteIfBound(FHttpRequestPtrWrapper(Request),StatusCode);
	});
	http_request->OnProcessRequestComplete().BindLambda([OnProcessRequestComplete](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
	{
		OnProcessRequestComplete.ExecuteIfBound(FHttpRequestPtrWrapper(Request),FHttpResponsePtrWrapper(Response),bConnectedSuccessfully);
	});

	http_request->ProcessRequest();
}

void UKKHttpBlueprintFunctionLibrary::BPL_HttpRequest_Stream(EKKHttpVerb Verb, FString URL, float TimeOut, TMap<FString, FString> Header, TMap<FString, FString> FormData,
	FString PostContent,
	FOnRequestProgress64_KK OnRequestProgress64,
	FOnStatusCodeReceived_KK OnStatusCodeReceived,
	FOnProcessRequestComplete_KK OnProcessRequestComplete,
	FResponseBodyReceiveStream_KK OnResponseBodyReceiveStream)
{
	TSharedRef<IHttpRequest> http_request = FHttpModule::Get().CreateRequest();
	// verb
	switch (Verb)
	{
	case EKKHttpVerb::EGet:
		http_request->SetVerb(TEXT("GET"));
		break;
	case EKKHttpVerb::EPost:
		http_request->SetVerb(TEXT("POST"));
		http_request->SetContentAsString(PostContent);
		break;
	}
	// time_out
	http_request->SetTimeout(TimeOut);
	// header
	for (const TTuple<FString,FString>& it : Header)
	{
		http_request->SetHeader(it.Key,it.Value);
	}
	// URL
	FString url;
	url += URL;
	if (FormData.Num())
	{
		url+=TEXT("?");
		for (const TTuple<FString,FString> & it : FormData)
		{
			// is last ?
			url += (it.Key + TEXT("=") + it.Value + TEXT("&"));
		}
	}
	http_request->SetURL(url);
	// delegate
	http_request->OnRequestProgress64().BindLambda([OnRequestProgress64](FHttpRequestPtr Request, uint64 BytesSent, uint64 BytesReceived)
	{
		OnRequestProgress64.ExecuteIfBound(FHttpRequestPtrWrapper(Request),BytesSent,BytesReceived);
	});
	http_request->OnStatusCodeReceived().BindLambda([OnStatusCodeReceived](FHttpRequestPtr Request, int32 StatusCode)
	{
		OnStatusCodeReceived.ExecuteIfBound(FHttpRequestPtrWrapper(Request),StatusCode);
	});
	http_request->OnProcessRequestComplete().BindLambda([OnProcessRequestComplete](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
	{
		OnProcessRequestComplete.ExecuteIfBound(FHttpRequestPtrWrapper(Request),FHttpResponsePtrWrapper(Response),bConnectedSuccessfully);
	});
	http_request->SetResponseBodyReceiveStreamDelegate(FHttpRequestStreamDelegate::CreateLambda([OnResponseBodyReceiveStream](void* Ptr, int64 Length)
	{
		AsyncTask(ENamedThreads::GameThread, [OnResponseBodyReceiveStream,Ptr,Length]()
		{
			TArray<uint8> Data(reinterpret_cast<uint8*>(Ptr),Length);
			OnResponseBodyReceiveStream.ExecuteIfBound(Data);
		});
		return true;
	}));
	
	http_request->ProcessRequest();
}


FString UKKHttpBlueprintFunctionLibrary::BPL_GetResponseContentAsString(const FHttpResponsePtrWrapper& Response)
{
	if (Response.Response.IsValid())
	{
		return Response.Response->GetContentAsString();
	}
	return TEXT("Response is nullptr");
}

TArray<uint8> UKKHttpBlueprintFunctionLibrary::BPL_GetResponseContent(const FHttpResponsePtrWrapper& Response)
{
	if (Response.Response.IsValid())
	{
		return Response.Response->GetContent();
	}
	return {};
}

FString UKKHttpBlueprintFunctionLibrary::BPL_BytesToString(const TArray<uint8>& Bytes)
{
	return FString(Bytes.Num(),reinterpret_cast<const char*>(Bytes.GetData()));
}

TArray<uint8> UKKHttpBlueprintFunctionLibrary::BPL_StringToBytes(FString String)
{
	FTCHARToUTF8 Converter(String);
	TArray<uint8> Tmp(reinterpret_cast<const uint8*>(Converter.Get()),Converter.Length());
	return Tmp;
}

bool UKKHttpBlueprintFunctionLibrary::BPL_CreateServerRouter(int32 BindPort, FRouterWrapper& Router)
{
	Router.Router = FHttpServerModule::Get().GetHttpRouter(BindPort,true);
	return Router.Router.IsValid();
}

FHttpRouteHandleWrapper UKKHttpBlueprintFunctionLibrary::BPL_Bind(FRouterWrapper& Router,FString RoutePath,EKKHttpVerb Verb,FServerHttpHandler HttpHandler)
{
	FHttpRouteHandleWrapper handle;
	// const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete
	handle.RouteHandle = Router.Router->BindRoute(FHttpPath(RoutePath),ConvertToHttpVerb(Verb),FHttpRequestHandler::CreateLambda(
		[HttpHandler](const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete)
		{
			return HttpHandler.Execute(FHttpServerRequestWrapper(Request),FHttpResultCallbackWrapper(OnComplete));
		}));
	return handle;
}

void UKKHttpBlueprintFunctionLibrary::BPL_UnBind(FRouterWrapper& Router, const FHttpRouteHandleWrapper& Handle)
{
	Router.Router->UnbindRoute(Handle.RouteHandle);
}

void UKKHttpBlueprintFunctionLibrary::BPL_StartAllListeners()
{
	FHttpServerModule::Get().StartAllListeners();
}

void UKKHttpBlueprintFunctionLibrary::BPL_StopAllListeners()
{
	FHttpServerModule::Get().StopAllListeners();
}

TArray<uint8> UKKHttpBlueprintFunctionLibrary::BPL_GetBody(const FHttpServerRequestWrapper& Request)
{
	return Request.ServerRequest.Body;
}

TMap<FString, FStringList> UKKHttpBlueprintFunctionLibrary::BPL_GetHeader(const FHttpServerRequestWrapper& Request)
{
	TMap<FString, FStringList> Tmp;
	for (const TTuple<FString, TArray<FString>>& Header : Request.ServerRequest.Headers)
	{
		FStringList& StringList = Tmp.Add(Header.Key);
		for (FString Value : Header.Value)
		{
			StringList.List.Add(Value);;
		}
	}
	return Tmp;
}

TMap<FString, FString> UKKHttpBlueprintFunctionLibrary::BPL_GetPathParams(const FHttpServerRequestWrapper& Request)
{
	return Request.ServerRequest.PathParams;
}

TMap<FString, FString> UKKHttpBlueprintFunctionLibrary::BPL_GetQueryParams(const FHttpServerRequestWrapper& Request)
{
	return Request.ServerRequest.QueryParams;
}


void UKKHttpBlueprintFunctionLibrary::BPL_PushComplete(const FHttpResultCallbackWrapper& OnComplete,const FHttpServerResponseWrapper& ServerResponse)
{
	TUniquePtr<FHttpServerResponse> Response = MakeUnique<FHttpServerResponse>();
	Response->Body = ServerResponse.Body;
	Response->Code = ServerResponse.Code;
	Response->Headers = ServerResponse.Headers;
	OnComplete.HttpResultCallback(MoveTemp(Response));
}

FHttpServerResponseWrapper UKKHttpBlueprintFunctionLibrary::BPL_CreateServerResponse(TArray<uint8> Body, int32 ResponseCodes, TMap<FString, FStringList> Header)
{
	FHttpServerResponseWrapper ServerResponseWrapper;
	ServerResponseWrapper.Body = Body;
	ServerResponseWrapper.Code = ConvertToHttpResponseCodes(ResponseCodes);
	for (const TTuple<FString, FStringList> & it : Header)
	{
		TArray<FString> & List = ServerResponseWrapper.Headers.Add(it.Key);
		List.Append(it.Value.List);
	}
	return ServerResponseWrapper;
}


EHttpServerRequestVerbs UKKHttpBlueprintFunctionLibrary::ConvertToHttpVerb(EKKHttpVerb InVerb)
{
	switch (InVerb)
	{
	case EKKHttpVerb::EGet:
		return EHttpServerRequestVerbs::VERB_GET;
	case EKKHttpVerb::EPost:
		return EHttpServerRequestVerbs::VERB_POST;
	case EKKHttpVerb::EPUT:
		return EHttpServerRequestVerbs::VERB_PUT;
	case EKKHttpVerb::EPATCH:
		return EHttpServerRequestVerbs::VERB_PATCH;
	case EKKHttpVerb::EDELETE:
		return EHttpServerRequestVerbs::VERB_DELETE;
	case EKKHttpVerb::EOPTIONS:
		return EHttpServerRequestVerbs::VERB_OPTIONS;
	case EKKHttpVerb::EMAX:
		return EHttpServerRequestVerbs::VERB_NONE;
	}
	return EHttpServerRequestVerbs::VERB_NONE;
}

EHttpServerResponseCodes UKKHttpBlueprintFunctionLibrary::ConvertToHttpResponseCodes(int32 ResponseCodes)
{
	switch (ResponseCodes) {
        case 0:
            return EHttpServerResponseCodes::Unknown;
        case 100:
            return EHttpServerResponseCodes::Continue;
        case 101:
            return EHttpServerResponseCodes::SwitchProtocol;
        case 200:
            return EHttpServerResponseCodes::Ok;
        case 201:
            return EHttpServerResponseCodes::Created;
        case 202:
            return EHttpServerResponseCodes::Accepted;
        case 203:
            return EHttpServerResponseCodes::Partial;
        case 204:
            return EHttpServerResponseCodes::NoContent;
        case 205:
            return EHttpServerResponseCodes::ResetContent;
        case 206:
            return EHttpServerResponseCodes::PartialContent;
        case 300:
            return EHttpServerResponseCodes::Ambiguous;
        case 301:
            return EHttpServerResponseCodes::Moved;
        case 302:
            return EHttpServerResponseCodes::Redirect;
        case 303:
            return EHttpServerResponseCodes::RedirectMethod;
        case 304:
            return EHttpServerResponseCodes::NotModified;
        case 305:
            return EHttpServerResponseCodes::UseProxy;
        case 307:
            return EHttpServerResponseCodes::RedirectKeepVerb;
        case 400:
            return EHttpServerResponseCodes::BadRequest;
        case 401:
            return EHttpServerResponseCodes::Denied;
        case 402:
            return EHttpServerResponseCodes::PaymentReq;
        case 403:
            return EHttpServerResponseCodes::Forbidden;
        case 404:
            return EHttpServerResponseCodes::NotFound;
        case 405:
            return EHttpServerResponseCodes::BadMethod;
        case 406:
            return EHttpServerResponseCodes::NoneAcceptable;
        case 407:
            return EHttpServerResponseCodes::ProxyAuthReq;
        case 408:
            return EHttpServerResponseCodes::RequestTimeout;
        case 409:
            return EHttpServerResponseCodes::Conflict;
        case 410:
            return EHttpServerResponseCodes::Gone;
        case 411:
            return EHttpServerResponseCodes::LengthRequired;
        case 412:
            return EHttpServerResponseCodes::PrecondFailed;
        case 413:
            return EHttpServerResponseCodes::RequestTooLarge;
        case 414:
            return EHttpServerResponseCodes::UriTooLong;
        case 415:
            return EHttpServerResponseCodes::UnsupportedMedia;
        case 429:
            return EHttpServerResponseCodes::TooManyRequests;
        case 449:
            return EHttpServerResponseCodes::RetryWith;
        case 500:
            return EHttpServerResponseCodes::ServerError;
        case 501:
            return EHttpServerResponseCodes::NotSupported;
        case 502:
            return EHttpServerResponseCodes::BadGateway;
        case 503:
            return EHttpServerResponseCodes::ServiceUnavail;
        case 504:
            return EHttpServerResponseCodes::GatewayTimeout;
        case 505:
            return EHttpServerResponseCodes::VersionNotSup;
        default:
            UE_LOG(LogTemp,Error, TEXT("UKKHttpBlueprintFunctionLibrary::ConvertToHttpResponseCodes: Cant Convert Response Code {%d} To EHttpServerResponseCodes"),ResponseCodes)
            return EHttpServerResponseCodes::Unknown;
    }
}
