// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "InvGameMode.generated.h"

UCLASS()
class NBC_JANGSIK_INV_API AInvGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
public:
	AInvGameMode();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> InventoryWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	UUserWidget* InventoryWidgetInstance;

	UFUNCTION(BlueprintPure, Category = "UI")
	UUserWidget* GetInventoryWidget() const;
	
	
};
