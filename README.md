# 언리얼 마스터 과제 — 인벤토리 시스템 구현계획

https://github.com/user-attachments/assets/68242a82-a327-4551-8769-24609605deb2

**프로젝트 경로:** `D:\Unreal\NBC_JangSik_Inv`  
**구조:** Character 직접 구현 (Component 분리 없음)  
**참고 챕터:** 1-7(리플렉션) · 3-1(아이템 설계) · 3-3(데이터 관리) · 4-1(UMG)

---

## 필수 과제

- 가방은 `TArray`를 통해서 만드세요
- `TMap`은 TArray에 담긴 아이템들을 확인하게 되면 그 아이템 정보를 Key값으로 아이템 정보들이 뜨도록 만들어주세요
- `TSet`은 칭호 획득을 사용하시면 됩니다
- 해당 칭호가 있어야 아이템을 사용할 수 있도록 만들어주세요

## 도전 과제

- UI를 통해서 비주얼로 확인할 수 있도록 만들어주세요

---

## STEP 1 — FItem 구조체 + Character 헤더 선언

> 챕터1-7 USTRUCT/UPROPERTY 패턴, 챕터3-1 ItemType(FName) 패턴 적용

```cpp
// InvCharacter.h 상단 (generated.h 직전)
USTRUCT(BlueprintType)
struct FItem
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    FString Name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    FString Desc;

    // FName 사용 (챕터3-1 ItemType 패턴) — TSet<FName> OwnedTitles와 타입 일치
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    FName RequiredTitle;  // NAME_None = 칭호 조건 없음
};
```

Character 헤더 멤버 선언:

```cpp
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
void UseItem(FString ItemName);

UFUNCTION(BlueprintCallable, Category = "Inventory")
void PrintBag() const;
```

---

## STEP 2 — CPP 구현 + BeginPlay 통합 테스트 (필수 1~4)

> 챕터3-3 TArray 순회·누적 패턴 참고

```cpp
void AInvCharacter::BeginPlay()
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
```

`UseItem` 핵심 로직:

```cpp
void AInvCharacter::UseItem(FString ItemName)
{
    FItem* Item = ItemDB.Find(ItemName);
    if (!Item) return;

    // 필수 4 — 칭호 조건 체크
    if (Item->RequiredTitle != NAME_None && !HasTitle(Item->RequiredTitle))
    {
        UE_LOG(LogTemp, Warning, TEXT("[UseItem] %s 불가 — 칭호 '%s' 필요"),
            *ItemName, *Item->RequiredTitle.ToString());
        return;
    }
    UE_LOG(LogTemp, Warning, TEXT("[UseItem] %s 사용 완료"), *ItemName);
}

bool AInvCharacter::HasTitle(FName Title) const
{
    return OwnedTitles.Contains(Title);
}
```

---

## STEP 3 — WBP_Inventory 위젯 (도전)

> 챕터4-1 UMG 패턴 그대로 적용

### Build.cs에 UMG 모듈 추가

```csharp
PublicDependencyModuleNames.AddRange(new string[] {
    "Core", "CoreUObject", "Engine", "InputCore",
    "UMG"
});
```

### 위젯 생성 — PlayerController BeginPlay

```cpp
// InvPlayerController.h
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
TSubclassOf<UUserWidget> InventoryWidgetClass;

UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
UUserWidget* InventoryWidgetInstance;

UFUNCTION(BlueprintPure, Category = "UI")
UUserWidget* GetInventoryWidget() const;
```

```cpp
// InvPlayerController.cpp — BeginPlay
if (InventoryWidgetClass)
{
    InventoryWidgetInstance = CreateWidget<UUserWidget>(this, InventoryWidgetClass);
    if (InventoryWidgetInstance)
        InventoryWidgetInstance->AddToViewport();
}
```

### 위젯 갱신 — SetText 방식

```cpp
void AInvCharacter::UpdateInventoryUI()
{
    AInvPlayerController* InvPC = Cast<AInvPlayerController>(GetController());
    if (!InvPC) return;

    UUserWidget* Widget = InvPC->GetInventoryWidget();
    if (!Widget) return;

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
```

### WBP_Inventory 위젯 구조

```
WBP_Inventory (Canvas Panel)
├── TextBlock "BagList"   — Bag TArray 아이템 목록
├── TextBlock "TitleList" — OwnedTitles TSet 칭호 목록
└── Button "UseBtn"       → OnClicked → UseItem(선택된 아이템)
```

### 에디터 설정 순서

1. GameMode → Default Controller Class를 `AInvPlayerController`로 설정
2. WBP_Inventory 생성 (Content Browser → Blueprint → Widget Blueprint)
   - TextBlock 두 개: 이름 `BagList`, `TitleList`
   - Button 하나: 이름 `UseBtn`
3. InvPlayerController BP → InventoryWidgetClass에 WBP_Inventory 할당
4. UseBtn OnClicked 이벤트:
   `OnClicked → Get Owning Player Pawn → Cast To AMyCharacter → UseItem("검")`
