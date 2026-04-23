// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyCharacter.generated.h"

class APlayerController;

USTRUCT(BlueprintType)
struct FItem
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FString Desc;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FName RequiredTitle;
};



UCLASS()
class NBC_JANGSIK_INV_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// 필수 1 — 가방
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TArray<FItem> Bag;

	// 필수 2 — 아이템 정보 DB (Key: 이름, Value: FItem)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TMap<FString, FItem> ItemDB;

	// 필수 3 — 보유 칭호 (FName 사용)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TSet<FName> OwnedTitles;

	// 함수 — 챕터1-7 UFUNCTION 패턴 적용
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void AddItem(FItem Item);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void AcquireTitle(FName Title);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool HasTitle(FName Title) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void UseItem(FString ItemName);  // 챕터3-1 ActivateItem 패턴 차용

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void PrintBag() const;
	
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void UpdateInventoryUI();


};
