#pragma once

#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"

#include "KKHttpType.generated.h"

UENUM()
enum class EKKHttpVerb : uint8
{
	EGet,
	EPost
};

// DECLARE_DELEGATE_ThreeParams(FHttpRequestCompleteDelegate, FHttpRequestPtr /*Request*/, FHttpResponsePtr /*Response*/, bool /*bConnectedSuccessfully*/);

USTRUCT(BlueprintType)
struct FKKHttp
{
	GENERATED_USTRUCT_BODY()

	FKKHttp()
	{
	}

	FKKHttp(FHttpRequestPtr InHttpRequestPtr, FHttpResponsePtr InHttpResponsePtr): HttpRequest(InHttpRequestPtr),
		HttpResponse(InHttpResponsePtr)
	{
	}

	FKKHttp(FHttpRequestPtr InHttpRequestPtr): HttpRequest(InHttpRequestPtr)
	{
	}

	FHttpRequestPtr HttpRequest;
	FHttpResponsePtr HttpResponse;
};

DECLARE_DYNAMIC_DELEGATE_TwoParams(FHttpHelperRequestCompleteDelegate, FKKHttp , HttpHelperPtr, bool, bSuccess);

DECLARE_DYNAMIC_DELEGATE_ThreeParams(FHttpHelperRequestProgressDelegate, FKKHttp , HttpHelperPtr, int32,
                                     BytesSend, int32, BytesReceived);

