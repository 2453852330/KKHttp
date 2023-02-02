// Fill out your copyright notice in the Description page of Project Settings.


#include "KKHttpBlueprintFunctionLibrary.h"
#include "HttpModule.h"

void UKKHttpBlueprintFunctionLibrary::KK_Http(EKKHttpVerb Verb, FString URL, float TimeOut,
                                              TMap<FString, FString> Header, TMap<FString, FString> FormData, FString PostContent,
                                              FHttpHelperRequestCompleteDelegate OnRequestComplete, FHttpHelperRequestProgressDelegate OnRequestProgress)
{
	TSharedRef<IHttpRequest,ESPMode::ThreadSafe> http_request = FHttpModule::Get().CreateRequest();

	switch (Verb)
	{
	case EKKHttpVerb::EGet:
		http_request->SetVerb(TEXT("GET"));
		break;
	case EKKHttpVerb::EPost:
		http_request->SetVerb(TEXT("POST"));
		// set payload
		http_request->SetContentAsString(PostContent);
		break;
	}

	http_request->SetTimeout(TimeOut);

	for (TTuple<FString,FString> it : Header)
	{
		http_request->SetHeader(it.Key,it.Value);
	}
	// http://iamgateway.dev.jwis.cn/pdm-standalone/container/folder/searchTree?containerOid=2b1cefc6-6975-49c6-87e1-af8b3f143a18&containerModel=ProductContainer

	FString url;
	url += URL;
	if (FormData.Num())
	{
		url+=TEXT("?");
		for (TTuple<FString,FString> it : FormData)
		{
			// is last ?
			url += (it.Key + TEXT("=") + it.Value + TEXT("&"));
		}
	}

	http_request->SetURL(url);


	// 不能使用CreateLambda,只能使用BindLambda;
	// Create只是创建了一个代理函数,并没有进行绑定
	
	// FHttpRequestCompleteDelegate, FHttpRequestPtr /*Request*/, FHttpResponsePtr /*Response*/, bool /*bConnectedSuccessfully*/
	http_request->OnProcessRequestComplete().BindLambda([OnRequestComplete](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
	{
		// UE_LOG(LogTemp,Warning,TEXT("[HttpHelperFunctionLibrary.cpp|%d][%s]"),__LINE__,*Response->GetContentAsString());
		OnRequestComplete.ExecuteIfBound(FKKHttp(Request,Response),bConnectedSuccessfully);
	});

	// FHttpRequestProgressDelegate, FHttpRequestPtr /*Request*/, int32 /*BytesSent*/, int32 /*BytesReceived*/
	http_request->OnRequestProgress().BindLambda([OnRequestProgress](FHttpRequestPtr Request, int32 BytesSent, int32 BytesReceived)
	{
		OnRequestProgress.ExecuteIfBound(FKKHttp(Request),BytesSent,BytesReceived);
	});
	
	http_request->ProcessRequest();
}

FString UKKHttpBlueprintFunctionLibrary::KK_GetContentAsString(FKKHttp InHttpHelperPtr)
{
	return InHttpHelperPtr.HttpResponse?InHttpHelperPtr.HttpResponse->GetContentAsString():TEXT("");
}
