#pragma once

#include "HttpRouteHandle.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "IHttpRouter.h"
#include "KKHttpType.generated.h"


UENUM()
enum class EKKHttpVerb : uint8
{
	EGet,
	EPost,
	EPUT,
	EPATCH,
	EDELETE,
	EOPTIONS,
	EMAX
};

UENUM()
enum class EKKHttpResponseCodes : uint16
{
	Unknown = 0,
	Continue = 100,
	SwitchProtocol = 101,
	Ok = 200,
	Created = 201,
	Accepted = 202,
	Partial = 203,
	NoContent = 204,
	ResetContent = 205,
	PartialContent = 206,
	Ambiguous = 300,
	Moved = 301,
	Redirect = 302,
	RedirectMethod = 303,
	NotModified = 304,
	UseProxy = 305,
	RedirectKeepVerb = 307,
	BadRequest = 400,
	Denied = 401,
	PaymentReq = 402,
	Forbidden = 403,
	NotFound = 404,
	BadMethod = 405,
	NoneAcceptable = 406,
	ProxyAuthReq = 407,
	RequestTimeout = 408,
	Conflict = 409,
	Gone = 410,
	LengthRequired = 411,
	PrecondFailed = 412,
	RequestTooLarge = 413,
	UriTooLong = 414,
	UnsupportedMedia = 415,
	TooManyRequests = 429,
	RetryWith = 449,
	ServerError = 500,
	NotSupported = 501,
	BadGateway = 502,
	ServiceUnavail = 503,
	GatewayTimeout = 504,
	VersionNotSup = 505
};

// DECLARE_DELEGATE_ThreeParams(FHttpRequestCompleteDelegate, FHttpRequestPtr /*Request*/, FHttpResponsePtr /*Response*/, bool /*bConnectedSuccessfully*/);

USTRUCT(BlueprintType)
struct FHttpRequestPtrWrapper
{
	GENERATED_BODY()
	FHttpRequestPtrWrapper() = default;
	FHttpRequestPtr Request;
	FHttpRequestPtrWrapper(const FHttpRequestPtr& InRequest):Request(InRequest){};
};
USTRUCT(BlueprintType)
struct FHttpResponsePtrWrapper
{
	GENERATED_BODY()
	FHttpResponsePtrWrapper() = default;
	FHttpResponsePtr Response;
	FHttpResponsePtrWrapper(const FHttpResponsePtr& InResponse):Response(InResponse){};
};

USTRUCT(BlueprintType)
struct FRouterWrapper
{
	GENERATED_BODY()
	FRouterWrapper() = default;
	FRouterWrapper(const TSharedPtr<IHttpRouter>& InRouter):Router(InRouter){}
	TSharedPtr<IHttpRouter> Router;
};

USTRUCT(BlueprintType)
struct FHttpRouteHandleWrapper
{
	GENERATED_BODY()
	FHttpRouteHandle RouteHandle;
};

USTRUCT(BlueprintType)
struct FHttpServerRequestWrapper
{
	GENERATED_BODY()
	FHttpServerRequestWrapper() = default;
	FHttpServerRequestWrapper(const FHttpServerRequest& InServerRequest):ServerRequest(InServerRequest){}
	FHttpServerRequest ServerRequest;
};

USTRUCT(BlueprintType)
struct FHttpResultCallbackWrapper
{
	GENERATED_BODY()
	FHttpResultCallbackWrapper() = default;
	FHttpResultCallbackWrapper(const FHttpResultCallback& InResultCallback):HttpResultCallback(InResultCallback){}
	FHttpResultCallback HttpResultCallback;
};

USTRUCT(BlueprintType)
struct FStringList
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite)
	TArray<FString> List;
};

USTRUCT(BlueprintType)
struct FHttpServerResponseWrapper
{
	GENERATED_BODY()
	FHttpServerResponseWrapper() = default;
	TArray<uint8> Body;
	EHttpServerResponseCodes Code;
	TMap<FString, TArray<FString>> Headers;
};