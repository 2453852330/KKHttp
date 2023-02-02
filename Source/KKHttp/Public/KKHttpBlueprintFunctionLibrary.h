// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "KKHttpType.h"
#include "KKHttpBlueprintFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class KKHTTP_API UKKHttpBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	// The HTTP request function 
	UFUNCTION(BlueprintCallable,meta=(AutoCreateRefTerm="Header,FormData"))
	static void KK_Http(
		EKKHttpVerb Verb,
		FString URL,
		float TimeOut,
		TMap<FString,FString> Header,
		TMap<FString,FString> FormData,
		FString PostContent,
		FHttpHelperRequestCompleteDelegate OnRequestComplete,
		FHttpHelperRequestProgressDelegate OnRequestProgress);

	// helper function to get response string
	UFUNCTION(BlueprintCallable,BlueprintPure)
	static FString KK_GetContentAsString(FKKHttp InHttpHelperPtr);
};
