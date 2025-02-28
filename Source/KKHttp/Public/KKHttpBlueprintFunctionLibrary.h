// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "KKHttpType.h"
#include "HttpFwd.h"
#include "IHttpRouter.h"
#include "KKHttpBlueprintFunctionLibrary.generated.h"

DECLARE_DYNAMIC_DELEGATE_ThreeParams(FOnRequestProgress64_KK,FHttpRequestPtrWrapper,Request,uint64,BytesSend,uint64,BytesReceived);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnStatusCodeReceived_KK,FHttpRequestPtrWrapper,Reques,int32,StatusCode);
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FOnProcessRequestComplete_KK,FHttpRequestPtrWrapper,Request,FHttpResponsePtrWrapper,Response,bool,bConnectedSuccessfully);
DECLARE_DYNAMIC_DELEGATE_OneParam(FResponseBodyReceiveStream_KK,const TArray<uint8> & ,StreamData);

// server
DECLARE_DYNAMIC_DELEGATE_RetVal_TwoParams(bool,FServerHttpHandler,const FHttpServerRequestWrapper&,Request,const FHttpResultCallbackWrapper&,OnComplete);


UCLASS()
class KKHTTP_API UKKHttpBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	
	// request
	UFUNCTION(BlueprintCallable,Category="KKHttp|Request",meta=(AutoCreateRefTerm="Header,FormData"))
	static void BPL_HttpRequest(
		EKKHttpVerb Verb,
		FString URL,
		float TimeOut,
		TMap<FString,FString> Header,
		TMap<FString,FString> FormData,
		FString PostContent,
		FOnRequestProgress64_KK OnRequestProgress64,
		FOnStatusCodeReceived_KK OnStatusCodeReceived,
		FOnProcessRequestComplete_KK OnProcessRequestComplete);

	UFUNCTION(BlueprintCallable,Category="KKHttp|Request",meta=(AutoCreateRefTerm="Header,FormData"))
	static void BPL_HttpRequest_Stream(
		EKKHttpVerb Verb,
		FString URL,
		float TimeOut,
		TMap<FString,FString> Header,
		TMap<FString,FString> FormData,
		FString PostContent,
		FOnRequestProgress64_KK OnRequestProgress64,
		FOnStatusCodeReceived_KK OnStatusCodeReceived,
		FOnProcessRequestComplete_KK OnProcessRequestComplete,
		FResponseBodyReceiveStream_KK OnResponseBodyReceiveStream);
	
	// get
	UFUNCTION(BlueprintPure,Category="KKHttp|Helper")
	static FString BPL_GetResponseContentAsString(const FHttpResponsePtrWrapper & Response);
	UFUNCTION(BlueprintPure,Category="KKHttp|Helper")
	static TArray<uint8> BPL_GetResponseContent(const FHttpResponsePtrWrapper & Response);
	UFUNCTION(BlueprintPure,Category="KKHttp|Helper")
	static FString BPL_BytesToString(const TArray<uint8> & Bytes);
	UFUNCTION(BlueprintPure,Category="KKHttp|Helper")
	static TArray<uint8> BPL_StringToBytes(FString String);

	// server
	UFUNCTION(BlueprintPure,Category="KKHttp|Server")
	static bool BPL_CreateServerRouter(int32 BindPort,FRouterWrapper& Router);
	UFUNCTION(BlueprintPure,Category="KKHttp|Server")
	static FHttpRouteHandleWrapper BPL_Bind(UPARAM(ref) FRouterWrapper& Router,FString RoutePath,EKKHttpVerb Verb,FServerHttpHandler HttpHandler);
	UFUNCTION(BlueprintPure,Category="KKHttp|Server")
	static void BPL_UnBind(UPARAM(ref) FRouterWrapper& Router,const FHttpRouteHandleWrapper& Handle);
	UFUNCTION(BlueprintCallable,Category="KKHttp|Server")
	static void BPL_StartAllListeners();
	UFUNCTION(BlueprintCallable,Category="KKHttp|Server")
	static void BPL_StopAllListeners();

	// FHttpServerRequestWrapper
	UFUNCTION(BlueprintPure,Category="KKHttp|Server")
	static TArray<uint8> BPL_GetBody(const FHttpServerRequestWrapper& Request);
	UFUNCTION(BlueprintPure,Category="KKHttp|Server")
	static TMap<FString, FStringList> BPL_GetHeader(const FHttpServerRequestWrapper& Request);
	UFUNCTION(BlueprintPure,Category="KKHttp|Server")
	static TMap<FString,FString> BPL_GetPathParams(const FHttpServerRequestWrapper& Request);
	UFUNCTION(BlueprintPure,Category="KKHttp|Server")
	static TMap<FString,FString> BPL_GetQueryParams(const FHttpServerRequestWrapper& Request);
	// FHttpResultCallbackWrapper
	UFUNCTION(BlueprintCallable,Category="KKHttp|Server")
	static void BPL_PushComplete(const FHttpResultCallbackWrapper& OnComplete,const FHttpServerResponseWrapper& ServerResponse);
	UFUNCTION(BlueprintPure,Category="KKHttp|Server",meta=(AutoCreateRefTerm="Header"))
	static FHttpServerResponseWrapper BPL_CreateServerResponse(TArray<uint8> Body,int32 ResponseCodes,TMap<FString,FStringList> Header);
	
private:
	static EHttpServerRequestVerbs ConvertToHttpVerb(EKKHttpVerb InVerb);
	static EHttpServerResponseCodes ConvertToHttpResponseCodes(int32 ResponseCodes);
};

