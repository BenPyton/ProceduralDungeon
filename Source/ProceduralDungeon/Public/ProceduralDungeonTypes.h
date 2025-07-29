// Copyright Benoit Pelletier 2019 - 2025 All Rights Reserved.
//
// This software is available under different licenses depending on the source from which it was obtained:
// - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
// - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
// Please refer to the accompanying LICENSE file for further details.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Math/GenericOctree.h" // FBoxCenterAndExtent
#include "ProceduralDungeonTypes.generated.h"

UENUM()
enum class EGenerationState : uint8
{
	Idle					UMETA(DisplayName = "Idle"),
	Generation 				UMETA(DisplayName = "Generation"),
	Load 					UMETA(DisplayName = "Load"),
	Initialization 			UMETA(DisplayName = "Initialization"),
	Unload 					UMETA(DisplayName = "Unload"),
	Play					UMETA(DisplayName = "Play"),
	NbState					UMETA(Hidden)
};

// The different directions a door can face.
UENUM(BlueprintType, meta = (DisplayName = "Door Direction"))
enum class EDoorDirection : uint8
{
	North		= 0 		UMETA(DisplayName = "North", ToolTip = "rotation = 0 | positive X (world forward)"),
	East		= 1 		UMETA(DisplayName = "East", ToolTip = "rotation = 90 | positive Y (world right)"),
	South		= 2 		UMETA(DisplayName = "South", ToolTip = "rotation = 180 | negative X (world backward)"),
	West		= 3 		UMETA(DisplayName = "West", ToolTip = "rotation = 270 | negative Y (world left)"),
	NbDirection	= 4 		UMETA(Hidden)
};

bool PROCEDURALDUNGEON_API operator!(const EDoorDirection& Direction);
EDoorDirection PROCEDURALDUNGEON_API operator+(const EDoorDirection& A, const EDoorDirection& B);
EDoorDirection PROCEDURALDUNGEON_API operator-(const EDoorDirection& A, const EDoorDirection& B);
// TODO: Don't know how to export these...
EDoorDirection& operator+=(EDoorDirection& A, const EDoorDirection& B);
EDoorDirection& operator-=(EDoorDirection& A, const EDoorDirection& B);
EDoorDirection& operator++(EDoorDirection& Direction);
EDoorDirection& operator--(EDoorDirection& Direction);
EDoorDirection PROCEDURALDUNGEON_API operator++(EDoorDirection& Direction, int);
EDoorDirection PROCEDURALDUNGEON_API operator--(EDoorDirection& Direction, int);
EDoorDirection PROCEDURALDUNGEON_API operator-(const EDoorDirection& Direction);
EDoorDirection PROCEDURALDUNGEON_API operator~(const EDoorDirection& Direction);
inline EDoorDirection PROCEDURALDUNGEON_API Opposite(const EDoorDirection& Direction) { return ~Direction; }
FIntVector PROCEDURALDUNGEON_API ToIntVector(const EDoorDirection& Direction);
FVector PROCEDURALDUNGEON_API ToVector(const EDoorDirection& Direction);
FQuat PROCEDURALDUNGEON_API ToQuaternion(const EDoorDirection& Direction);
float PROCEDURALDUNGEON_API ToAngle(const EDoorDirection& Direction);
FIntVector PROCEDURALDUNGEON_API Rotate(const FIntVector& Pos, const EDoorDirection& Rot);
FVector PROCEDURALDUNGEON_API Rotate(const FVector& Pos, const EDoorDirection& Rot);

FIntVector PROCEDURALDUNGEON_API Transform(const FIntVector& Pos, const FIntVector& Translation, const EDoorDirection& Rotation);
FIntVector PROCEDURALDUNGEON_API InverseTransform(const FIntVector& Pos, const FIntVector& Translation, const EDoorDirection& Rotation);

// Those ones are just for consistent naming and centralized code
EDoorDirection PROCEDURALDUNGEON_API Transform(const EDoorDirection& Direction, const EDoorDirection& Rotation);
EDoorDirection PROCEDURALDUNGEON_API InverseTransform(const EDoorDirection& Direction, const EDoorDirection& Rotation);

//The different types of generation algorithms.
UENUM(BlueprintType, meta = (DisplayName = "Generation Type"))
enum class EGenerationType : uint8
{
	DFS = 0 				UMETA(DisplayName = "Depth First", Tooltip = "Make the dungeon more linear"),
	BFS = 1 				UMETA(DisplayName = "Breadth First", Tooltip = "Make the dungeon less linear"),
	NbType = 2 				UMETA(Hidden)
};

// The different types of seed update at each generation.
UENUM(BlueprintType, meta = (DisplayName = "Seed Type"))
enum class ESeedType : uint8
{
	Random = 0 				UMETA(DisplayName = "Random", Tooltip = "Random seed at each generation"),
	AutoIncrement = 1 		UMETA(DisplayName = "Auto Increment", Tooltip = "Get the initial seed and increment at each generation"),
	Fixed = 2 				UMETA(DisplayName = "Fixed", Tooltip = "Always use initial seed (or you can set it manually via blueprint)"),
	NbType = 3 				UMETA(Hidden)
};

// Visibility mode for Room Visibilty Components.
UENUM(BlueprintType, meta = (DisplayName = "Room Visibility"))
enum class EVisibilityMode : uint8
{
	Default			UMETA(DisplayName = "Same As Room"),
	ForceVisible	UMETA(DisplayName = "Force Visible"),
	ForceHidden		UMETA(DisplayName = "Force Hidden"),
	Custom			UMETA(DisplayName = "Custom"),
	NbMode			UMETA(Hidden)
};

// Structure that defines a door.
// A door is defined by its position, its direction, and its type.
USTRUCT(BlueprintType)
struct PROCEDURALDUNGEON_API FDoorDef
{
	GENERATED_BODY()

public:
	static const FDoorDef Invalid;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DoorDef")
	FIntVector Position {FIntVector::ZeroValue};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DoorDef")
	EDoorDirection Direction {EDoorDirection::North};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DoorDef", meta = (DisplayThumbnail = false))
	class UDoorType* Type {nullptr};

public:
	FDoorDef() = default;
	FDoorDef(const FIntVector& InPosition, EDoorDirection InDirection, class UDoorType* InType = nullptr);

	bool IsValid() const;
	operator bool() const { return IsValid(); }
	bool operator==(const FDoorDef& Other) const;

	static bool AreCompatible(const FDoorDef& A, const FDoorDef& B);

	FVector GetDoorSize() const;
	float GetDoorOffset() const;
	FColor GetDoorColor() const;
	FString GetTypeName() const;
	FString ToString() const;
	FDoorDef GetOpposite() const;
	FBoxCenterAndExtent GetBounds(const FVector RoomUnit, bool bIncludeOffset = true) const;

	static FVector GetRealDoorPosition(const FDoorDef& DoorDef, const FVector RoomUnit, bool bIncludeOffset = true);
	static FVector GetRealDoorPosition(FIntVector DoorCell, EDoorDirection DoorRot, const FVector RoomUnit, float DoorOffset = 0.0f);
	static FQuat GetRealDoorRotation(const FDoorDef& DoorDef, bool bFlipped = false);

	static FDoorDef Transform(const FDoorDef& DoorDef, FIntVector Translation, EDoorDirection Rotation);
	static FDoorDef InverseTransform(const FDoorDef& DoorDef, FIntVector Translation, EDoorDirection Rotation);

#if !UE_BUILD_SHIPPING
	static void DrawDebug(const class UWorld* World, const FDoorDef& DoorDef, const FVector RoomUnit, const FTransform& Transform = FTransform::Identity, bool bIncludeOffset = false, bool bIsConnected = true);
	static void DrawDebug(const class UWorld* World, const FColor& Color, const FVector& DoorSize, const FVector RoomUnit, const FIntVector& DoorCell = FIntVector::ZeroValue, const EDoorDirection& DoorRot = EDoorDirection::NbDirection, const FTransform& Transform = FTransform::Identity, float DoorOffset = 0.0f, bool bIsConnected = true);
#endif // !UE_BUILD_SHIPPING
};

// TODO: Use UE built-in TBox<FIntVector> instead?
// The downside of doing that would be the Center and Extent computation that is slightly different...
// Also, the IsInside with another box does not consider coincident faces as inside...
// Also, operators + and += don't mean the same (extending box to include a point instead of shifting the box)...
struct PROCEDURALDUNGEON_API FBoxMinAndMax
{
public:
	FIntVector Min {0};
	FIntVector Max {0};

public:
	FBoxMinAndMax() = default;
	FBoxMinAndMax(const FIntVector& A, const FIntVector& B);

	FIntVector GetSize() const;
	FBoxCenterAndExtent ToCenterAndExtent() const;
	bool IsInside(const FIntVector& Cell) const;
	bool IsInside(const FBoxMinAndMax& Other) const;
	void Rotate(const EDoorDirection& Rot);
	void Extend(const FBoxMinAndMax& Other);
	FString ToString() const;
	FIntVector GetClosestPoint(const FIntVector& Point) const;

	static bool Overlap(const FBoxMinAndMax& A, const FBoxMinAndMax& B);

	FBoxMinAndMax& operator+=(const FIntVector& X);
	FBoxMinAndMax& operator-=(const FIntVector& X);
	FBoxMinAndMax operator+(const FIntVector& X) const;
	FBoxMinAndMax operator-(const FIntVector& X) const;
	bool operator==(const FBoxMinAndMax& Other) const;
	bool operator!=(const FBoxMinAndMax& Other) const;
};

FBoxMinAndMax PROCEDURALDUNGEON_API Rotate(const FBoxMinAndMax& Box, const EDoorDirection& Rot);
