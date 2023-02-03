/*
 * MIT License
 *
 * Copyright (c) 2019-2023 Benoit Pelletier
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "RoomLevel.generated.h"

class URoom;

UCLASS(Blueprintable, ClassGroup="Procedural Dungeon")
class PROCEDURALDUNGEON_API ARoomLevel : public ALevelScriptActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Data")
	class URoomData* Data;

public:
	UPROPERTY()
	URoom* Room = nullptr;
	bool IsInit = false;
	bool PendingInit = false;

	UPROPERTY(EditAnywhere, Category = "Room Level")
	bool AlwaysVisible = false;

public:
	ARoomLevel(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;
	virtual bool ShouldTickIfViewportsOnly() const override { return true; }

	void Init(URoom* Room);
	void SetActorsVisible(bool Visible);

	UFUNCTION(BlueprintPure, Category = "Procedural Dungeon", meta = (CompactNodeTitle = "Is Player Inside"))
	bool IsPlayerInside() const { return IsValid(Room) ? Room->IsPlayerInside() : false; }

	UFUNCTION(BlueprintPure, Category = "Procedural Dungeon", meta = (CompactNodeTitle = "Is Visible"))
	bool IsVisible() const { return IsValid(Room) ? Room->IsVisible() : true; }

	UFUNCTION(BlueprintPure, Category = "Procedural Dungeon", meta = (CompactNodeTitle = "Is Locked"))
	bool IsLocked() const { return IsValid(Room) ? Room->IsLocked() : false; }

	UFUNCTION(BlueprintCallable, Category = "Procedural Dungeon")
	void Lock(bool lock) { if(IsValid(Room)) Room->Lock(lock); }

private:
	UPROPERTY()
	TArray<AActor*> ActorsInLevel;
	FTransform Transform;
	FBoxCenterAndExtent Bounds;

private:
	void UpdateBounds();
	virtual void PostInitProperties() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
