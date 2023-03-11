#include "ProceduralLevelStreaming.h"
#include "Engine/World.h"
#include "Engine/Level.h"
#include "Engine/Engine.h" // GEngine
#include "Misc/PackageName.h"
#include "RoomData.h"
#include "ProceduralDungeon.h"

#define LOCTEXT_NAMESPACE "World"

UProceduralLevelStreaming::UProceduralLevelStreaming(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UProceduralLevelStreaming::PostLoad()
{
	Super::PostLoad();

	// Initialize startup state of the streaming level
	if (GetWorld()->IsGameWorld())
	{
		bShouldBeLoaded = bInitiallyLoaded;
		SetShouldBeVisible(bInitiallyVisible);
	}
}

void UProceduralLevelStreaming::SetShouldBeLoaded(const bool bInShouldBeLoaded)
{
	if (bInShouldBeLoaded != bShouldBeLoaded)
	{
		bShouldBeLoaded = bInShouldBeLoaded;
		if (UWorld* World = GetWorld())
		{
			World->UpdateStreamingLevelShouldBeConsidered(this);
		}
	}
}


void UProceduralLevelStreaming::OnLevelDynamicUnloaded()
{
	//UE_LOG(LogProceduralDungeon, Warning, TEXT("End unload level: %s"), *GetWorldAssetPackageName());
	UWorld* World = GetWorld();
	if (nullptr != World)
	{
		//UE_LOG(LogProceduralDungeon, Warning, TEXT("Remove instance from world"));
		World->RemoveStreamingLevel(this);
	}
	bIsUnloaded = true;
}

UProceduralLevelStreaming* UProceduralLevelStreaming::LoadLevelInstance(UObject* WorldContextObject, const FString LevelName, const FString& InstanceNameSuffix, const FVector Location, const FRotator Rotation, bool& bOutSuccess)
{
	bOutSuccess = false;
	UWorld* const World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		return nullptr;
	}

	// Check whether requested map exists, this could be very slow if LevelName is a short package name
	FString LongPackageName;
	bOutSuccess = FPackageName::SearchForPackageOnDisk(LevelName, &LongPackageName);
	if (!bOutSuccess)
	{
		return nullptr;
	}

	return LoadLevelInstance_Internal(World, LongPackageName, InstanceNameSuffix, Location, Rotation, bOutSuccess);
}

UProceduralLevelStreaming* UProceduralLevelStreaming::LoadLevelInstanceBySoftObjectPtr(UObject* WorldContextObject, const TSoftObjectPtr<UWorld> Level, const FString& InstanceNameSuffix, const FVector Location, const FRotator Rotation, bool& bOutSuccess)
{
	bOutSuccess = false;
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		return nullptr;
	}

	// Check whether requested map exists, this could be very slow if LevelName is a short package name
	if (Level.IsNull())
	{
		return nullptr;
	}

	return LoadLevelInstance_Internal(World, Level.GetLongPackageName(), InstanceNameSuffix, Location, Rotation, bOutSuccess);
}

UProceduralLevelStreaming * UProceduralLevelStreaming::Load(UObject* WorldContextObject, URoomData* Data, const FString& InstanceNameSuffix, FVector Location, FRotator Rotation)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (nullptr == World)
	{
		UE_LOG(LogProceduralDungeon, Error, TEXT("Failed to load LevelStreamingDynamic: World is null"));
		return nullptr;
	}
	if (nullptr == Data)
	{
		UE_LOG(LogProceduralDungeon, Error, TEXT("Failed to load LevelStreamingDynamic: Data is null"));
		return nullptr;
	}

	bool success = false;
	UProceduralLevelStreaming* Instance = UProceduralLevelStreaming::LoadLevelInstanceBySoftObjectPtr(World, Data->Level, InstanceNameSuffix, Location, Rotation, success);

	if (!success)
	{
		UE_LOG(LogProceduralDungeon, Error, TEXT("Failed to load LevelStreamingDynamic: Unknown reason"));
		return nullptr;
	}

	return Instance;
}

void UProceduralLevelStreaming::Unload(UObject * WorldContextObject, UProceduralLevelStreaming * Instance)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (nullptr == World)
	{
		UE_LOG(LogProceduralDungeon, Error, TEXT("Failed to unload LevelStreamingDynamic: World is null"));
		return;
	}
	if (nullptr == Instance)
	{
		UE_LOG(LogProceduralDungeon, Error, TEXT("Failed to unload LevelStreamingDynamic: Instance is null"));
		return;
	}

	// Prefix to remove
	FString LevelName = Instance->GetWorldAssetPackageName();
	const FString PackagePath = FPackageName::GetLongPackagePath(LevelName);
	FString LevelPackageName = PackagePath + TEXT("/") + World->StreamingLevelsPrefix;

	LevelName.RemoveFromStart(LevelPackageName, ESearchCase::IgnoreCase);
	FLatentActionInfo LatentInfo(0, 2222, TEXT("OnLevelDynamicUnloaded"), Instance);
	UGameplayStatics::UnloadStreamLevel(World, *LevelName, LatentInfo, false);
}

UProceduralLevelStreaming* UProceduralLevelStreaming::LoadLevelInstance_Internal(UWorld* World, const FString& LongPackageName, const FString& InstanceNameSuffix, const FVector Location, const FRotator Rotation, bool& bOutSuccess)
{
	// Create Unique Name for sub-level package
	const FString ShortPackageName = FPackageName::GetShortName(LongPackageName);
	const FString PackagePath = FPackageName::GetLongPackagePath(LongPackageName);
	FString UniqueLevelPackageName = PackagePath + TEXT("/") + World->StreamingLevelsPrefix + ShortPackageName;

	//UE_LOG(LogProceduralDungeon, Warning, TEXT("Unique Id: %d"), UniqueLevelInstanceId);

	UniqueLevelPackageName += TEXT("_LevelInstance_") + InstanceNameSuffix;

	// Setup streaming level object that will load specified map
	UProceduralLevelStreaming* StreamingLevel = NewObject<UProceduralLevelStreaming>(World, UProceduralLevelStreaming::StaticClass(), NAME_None, RF_Transient, NULL);
	StreamingLevel->SetWorldAssetByPackageName(FName(*UniqueLevelPackageName));
	StreamingLevel->LevelColor = FColor::MakeRandomColor();
	StreamingLevel->SetShouldBeLoaded(true);
	StreamingLevel->SetShouldBeVisible(true);
	StreamingLevel->bShouldBlockOnLoad = false;
	StreamingLevel->bInitiallyLoaded = true;
	StreamingLevel->bInitiallyVisible = true;
	StreamingLevel->bDisableDistanceStreaming = true;
	// Transform
	StreamingLevel->LevelTransform = FTransform(Rotation, Location);
	// Map to Load
	StreamingLevel->PackageNameToLoad = FName(*LongPackageName);

	// Add the new level to world.
	World->AddStreamingLevel(StreamingLevel);

	bOutSuccess = true;
	return StreamingLevel;
}

#undef LOCTEXT_NAMESPACE