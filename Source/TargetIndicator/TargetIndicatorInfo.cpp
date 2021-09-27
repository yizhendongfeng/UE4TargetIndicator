// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetIndicatorInfo.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

bool UTargetIndicatorInfo::ProjectWorldToScreenAsIndicator(APlayerController const* Player, const FVector& WorldPosition, const int Offset, FVector2D& ScreenPosition, float& Angle, float& Distance)
{
	FVector Projected = FVector::ZeroVector;
	bool bTargetBehindCamera = false;

	// Custom Projection Function
	Distance = FVector::Distance(Player->GetPawn()->GetActorLocation(), WorldPosition) / 100.0f;	//cmת��Ϊm
	ULocalPlayer* const LP = Player ? Player->GetLocalPlayer() : nullptr;
	if (LP && LP->ViewportClient)
	{
		FSceneViewProjectionData NewProjectionData;
		if (LP->GetProjectionData(LP->ViewportClient->Viewport, EStereoscopicPass::eSSP_FULL, NewProjectionData))
		{
			const FMatrix ViewProjectionMatrix = NewProjectionData.ComputeViewProjectionMatrix();
			FIntRect ViewRectangle = NewProjectionData.GetConstrainedViewRect();
			const FVector2D  ViewportCenter = ViewRectangle.Max / 2;
			ViewRectangle.Min += FIntPoint(Offset, Offset);
			ViewRectangle.Max -= FIntPoint(Offset, Offset);

			FPlane Result = ViewProjectionMatrix.TransformFVector4(FVector4(WorldPosition, 1.f));
			if (Result.W < 0.f) { bTargetBehindCamera = true; }

			if (FMath::Abs(Result.W) < FLT_EPSILON) return false;
			const float RHW = 1.f / FMath::Abs(Result.W);
			Projected = FVector(Result.X * RHW, Result.Y * RHW, RHW);
			// Normalize to 0..1 UI Space
			const float NormX = (Projected.X / 2.f) + 0.5f;
			const float NormY = 1.f - (Projected.Y / 2.f) - 0.5f;

			Projected.X = (float)ViewRectangle.Min.X + (NormX * (float)ViewRectangle.Width());
			Projected.Y = (float)ViewRectangle.Min.Y + (NormY * (float)ViewRectangle.Height());
			FVector2D Projected2D = FVector2D(Projected.X, Projected.Y);
			//GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::White, bTargetBehindCamera ? "true," : "false," + Projected.ToString());
			//UE_LOG(LogTemp, Warning, TEXT("Projected.X:%f, Y:%f"), Projected.X, Projected.Y);

			if (Projected.X >= ViewRectangle.Min.X && Projected.X <= ViewRectangle.Max.X && Projected.Y >= ViewRectangle.Min.Y && Projected.Y <= ViewRectangle.Max.Y && !bTargetBehindCamera)
				ScreenPosition = Projected2D;
			else
			{
				//Projected2D = ViewportCenter + (Projected2D - ViewportCenter).GetSafeNormal();
				GetLineIntersectionPoint(ViewportCenter, Projected2D, ViewRectangle.Min, FVector2D(ViewRectangle.Max.X, ViewRectangle.Min.Y), ScreenPosition)
					|| GetLineIntersectionPoint(ViewportCenter, Projected2D, FVector2D(ViewRectangle.Min.X, ViewRectangle.Max.Y), ViewRectangle.Max, ScreenPosition)
					|| GetLineIntersectionPoint(ViewportCenter, Projected2D, ViewRectangle.Min, FVector2D(ViewRectangle.Min.X, ViewRectangle.Max.Y), ScreenPosition)
					|| GetLineIntersectionPoint(ViewportCenter, Projected2D, FVector2D(ViewRectangle.Max.X, ViewRectangle.Min.Y), ViewRectangle.Max, ScreenPosition);
				/* ����Ŀ��ͶӰ������Ļ���°벿�� */
				if (bTargetBehindCamera && ScreenPosition.Y < ViewportCenter.Y)	// FIXME: ��Ŀ��ͶӰ������Ļ�ϰ벿������������ӽ���ת�����������ʱ���������Ļ��ˮƽ�м�������
				{
					ScreenPosition.Y = ViewportCenter.Y * 2 - ScreenPosition.Y;
				}
				//UE_LOG(LogTemp, Warning, TEXT("behind:%d, out of screen X:%f, Y:%f, ScreenPosition.X:%f, ScreenPosition.Y:%f"), bTargetBehindCamera, Projected.X, Projected.Y, ScreenPosition.X, ScreenPosition.Y);
			}
			Angle = FMath::Atan2(ScreenPosition.Y - ViewportCenter.Y, ScreenPosition.X - ViewportCenter.X);
			Angle = FMath::RadiansToDegrees(Angle) + 90;	// ���Ժ��ִ˴�������90�ȣ���֪��Ϊʲô
		}
	}
	return bTargetBehindCamera;
}

bool UTargetIndicatorInfo::GetLineIntersectionPoint(FVector2D Line1Point1, FVector2D Line1Point2, FVector2D Line2Point1, FVector2D Line2Point2, FVector2D& IntersectedPoint)
{
	FVector2D R = Line1Point2 - Line1Point1;
	FVector2D S = Line2Point2 - Line2Point1;
	if (FMath::Abs(R ^ S) < FLT_EPSILON)
		return false;

	float t = (Line2Point1 - Line1Point1) ^ S / (R ^ S);
	float u = (Line1Point1 - Line2Point1) ^ R / (S ^ R);
	if (t > 0 && u >= 0 && u <= 1) // ������Line1���߷�������Lin2�м�
	{
		IntersectedPoint = Line1Point1 + t * R;
		return true;
	}
	else
	{
		return false;
	}
}