/*
 * MIT License
 *
 * Copyright (c) 2025 Benoit Pelletier
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
#include "Components/ActorComponent.h"
#include "Interfaces/RoomActorGuid.h"
#include "SimpleGuidComponent.generated.h"

#define GUID_DEBUG_ENABLED 0

// A simple guid component that will retrieve the Editor's ActorGuid
// to save/load it in packaged games.
//
// :::warning
//
// This component will work only on placed actor, not actors spawned during runtime!!!
//
// :::
UCLASS(BlueprintType, Blueprintable, ClassGroup = "ProceduralDungeon", meta = (BlueprintSpawnableComponent))
class PROCEDURALDUNGEON_API USimpleGuidComponent : public UActorComponent, public IRoomActorGuid
{
	GENERATED_BODY()

public:	
	USimpleGuidComponent();

protected:
	//~ Begin UActorComponent Interface
	virtual void OnRegister() override;
	//~ End UActorComponent Interface

	//~ Begin IRoomActorGuid Interface
	virtual FGuid GetGuid_Implementation() const override;
	//~ End IRoomActorGuid Interface

	// Unfortunately I can't place them in the #if block below. UE will complain about it.
	virtual void Serialize(FArchive& Ar) override;
	virtual void Serialize(FStructuredArchive::FRecord Record) override;

#if GUID_DEBUG_ENABLED // Enable some logs to debug lifecycle of the component.
	//~ Begin UObject Interface
	virtual void PostInitProperties() override;
	virtual void PreSave(FObjectPreSaveContext SaveContext);
	virtual void PostLoad() override;
	//~ End UObject Interface

	//~ Begin UActorComponent Interface
	virtual void OnComponentCreated() override;
	virtual void InitializeComponent() override;
	virtual void BeginPlay() override;
	//~ End UActorComponent Interface
#endif

public:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, NonPIEDuplicateTransient, TextExportTransient, Category = "Guid")
	FGuid Guid;
};
