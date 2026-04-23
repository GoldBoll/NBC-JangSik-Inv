// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"
#include "InvGameMode.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"

// Sets default values
AMyCharacter::AMyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	FItem Sword  = { TEXT("검"),  TEXT("강력한 검"),  TEXT("전사") };
	FItem Potion = { TEXT("물약"), TEXT("체력 회복"), NAME_None   };
	ItemDB.Add(Sword.Name,  Sword);
	ItemDB.Add(Potion.Name, Potion);

	// 필수 1 — TArray 가방에 추가
	AddItem(Sword);
	AddItem(Potion);
	PrintBag();

	// 필수 2 — TMap Key로 아이템 정보 조회
	if (FItem* Info = ItemDB.Find(TEXT("검")))
		UE_LOG(LogTemp, Warning, TEXT("[ItemDB] 검: %s"), *Info->Desc);

	// 필수 3·4 — 칭호 없이 시도 → 실패
	UseItem(TEXT("검"));

	// 칭호 획득 후 재시도 → 성공
	AcquireTitle(TEXT("전사"));
	UseItem(TEXT("검"));

	// 칭호 불필요 아이템 → 성공
	UseItem(TEXT("물약"));
}

// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
}

void AMyCharacter::AddItem(FItem Item)
{
	Bag.Add(Item);
	UpdateInventoryUI();
}

void AMyCharacter::AcquireTitle(FName Title)
{
	OwnedTitles.Add(Title);
	UE_LOG(LogTemp, Warning, TEXT("[AcquireTitle] 칭호 획득: %s"), *Title.ToString());
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green,
			FString::Printf(TEXT("[AcquireTitle] 칭호 획득: %s"), *Title.ToString()));
	UpdateInventoryUI();
}

bool AMyCharacter::HasTitle(FName Title) const
{
	return OwnedTitles.Contains(Title);
}

void AMyCharacter::UseItem(FString ItemName)
{
	FItem* Item = ItemDB.Find(ItemName);
	if (!Item) return;

	if (Item->RequiredTitle != NAME_None && !HasTitle(Item->RequiredTitle))
	{
		UE_LOG(LogTemp, Warning, TEXT("[UseItem] %s 불가 — 칭호 '%s' 필요"),
			*ItemName, *Item->RequiredTitle.ToString());
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,
				FString::Printf(TEXT("[UseItem] %s 불가 — 칭호 '%s' 필요"),
					*ItemName, *Item->RequiredTitle.ToString()));
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("[UseItem] %s 사용 완료"), *ItemName);
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow,
			FString::Printf(TEXT("[UseItem] %s 사용 완료"), *ItemName));
}

void AMyCharacter::PrintBag() const
{
	UE_LOG(LogTemp, Warning, TEXT("[PrintBag] 가방 목록 (%d개):"), Bag.Num());
	for (const FItem& Item : Bag)
	{
		UE_LOG(LogTemp, Warning, TEXT("  - %s: %s"), *Item.Name, *Item.Desc);
	}
}

// Character에서 아이템 추가/사용 후 호출
void AMyCharacter::UpdateInventoryUI()
{
	AInvGameMode* InvGM = Cast<AInvGameMode>(GetWorld()->GetAuthGameMode());
	if (!InvGM) return;

	UUserWidget* Widget = InvGM->GetInventoryWidget();
	if (!Widget) return;

	// 챕터4-1 GetWidgetFromName + SetText 패턴
	if (UTextBlock* BagText = Cast<UTextBlock>(Widget->GetWidgetFromName(TEXT("BagList"))))
	{
		FString BagStr;
		for (const FItem& Item : Bag)
			BagStr += Item.Name + TEXT("\n");
		BagText->SetText(FText::FromString(BagStr));
	}

	if (UTextBlock* TitleText = Cast<UTextBlock>(Widget->GetWidgetFromName(TEXT("TitleList"))))
	{
		FString TitleStr;
		for (const FName& Title : OwnedTitles)
			TitleStr += Title.ToString() + TEXT("\n");
		TitleText->SetText(FText::FromString(TitleStr));
	}
}