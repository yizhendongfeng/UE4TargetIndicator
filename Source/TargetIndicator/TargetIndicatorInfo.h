// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TargetIndicatorInfo.generated.h"


/**
 *
 */
UCLASS()
class TARGETINDICATOR_API UTargetIndicatorInfo : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()


	UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject", CallableWithoutWorldContext), Category = "HUD|Util")
	static bool ProjectWorldToScreenAsIndicator(APlayerController const* Player, const FVector& WorldPosition, const int Offset, FVector2D& ScreenPosition, float& Angle, float& Distance);

	static bool GetLineIntersectionPoint(FVector2D Line1Point1, FVector2D Line1Point2, FVector2D Line2Point1, FVector2D Line2Point2, FVector2D& IntersectedPoint);
};
