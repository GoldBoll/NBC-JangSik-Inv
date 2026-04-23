// Fill out your copyright notice in the Description page of Project Settings.


#include "InvGameMode.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"

AInvGameMode::AInvGameMode() :
		InventoryWidgetClass(nullptr),InventoryWidgetInstance(nullptr)
{
}


void AInvGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	if (InventoryWidgetClass)
	{
		APlayerController* PC = GetWorld()->GetFirstPlayerController();
		InventoryWidgetInstance = CreateWidget<UUserWidget>(PC, InventoryWidgetClass);
		if (InventoryWidgetInstance)
		{
			InventoryWidgetInstance->AddToViewport();
			PC->SetShowMouseCursor(true);
			PC->SetInputMode(FInputModeGameAndUI());
		}
	}
}

UUserWidget* AInvGameMode::GetInventoryWidget() const
{
	return InventoryWidgetInstance;
}



	