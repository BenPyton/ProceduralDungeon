/*
 * Copyright (c) 2025 Benoit Pelletier
 *
 * This software is available under different licenses depending on the source from which it was obtained:
 * - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
 * - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
 *
 * Please refer to the accompanying LICENSE file for further details.
 */

#include "Components/SimpleGuidComponent.h"
#include "GameFramework/Actor.h"
#include "ProceduralDungeonLog.h"

#if GUID_DEBUG_ENABLED
	#define LOG_GUID_INFO(...) DungeonLog_InfoSilent(##__VA_ARGS__)
#else
	#define LOG_GUID_INFO(...)
#endif

USimpleGuidComponent::USimpleGuidComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	LOG_GUID_INFO("[%s.SimpleGuidComponent] Construct Component", *GetNameSafe(GetOwner()));
	LOG_GUID_INFO("-- Guid: %s", *Guid.ToString());
}

void USimpleGuidComponent::OnRegister()
{
	LOG_GUID_INFO("[%s.SimpleGuidComponent] Register Component", *GetNameSafe(GetOwner()));
	Super::OnRegister();
	LOG_GUID_INFO("-- Guid: %s", *Guid.ToString());
	if (!Guid.IsValid() && IsValid(GetOwner()))
	{
#if WITH_EDITOR
		Guid = GetOwner()->GetActorGuid();
		LOG_GUID_INFO("-- Guid Not Valid! Retrieving Guid from actor: %s", *Guid.ToString());
#else
		LOG_GUID_INFO("-- Guid Not Valid! Can't retreive guid from actor!");
#endif
	}
}

FGuid USimpleGuidComponent::GetGuid_Implementation() const
{
	return Guid;
}

bool USimpleGuidComponent::ShouldSaveActor_Implementation() const
{
	return bSaveActorInDungeon;
}

void USimpleGuidComponent::Serialize(FArchive& Ar)
{
	LOG_GUID_INFO("[%s.SimpleGuidComponent] Serialize(Ar) Component (%s)", *GetNameSafe(GetOwner()), Ar.IsLoading() ? TEXT("load") : TEXT("save"));
	Super::Serialize(Ar);
	LOG_GUID_INFO("-- Guid: %s", *Guid.ToString());
}

void USimpleGuidComponent::Serialize(FStructuredArchive::FRecord Record)
{
	LOG_GUID_INFO("[%s.SimpleGuidComponent] Serialize(Record) Component", *GetNameSafe(GetOwner()));
	Super::Serialize(Record);
	LOG_GUID_INFO("-- Guid: %s", *Guid.ToString());
}

#if GUID_DEBUG_ENABLED
void USimpleGuidComponent::PostInitProperties()
{
	LOG_GUID_INFO("[%s.SimpleGuidComponent] PostInitProperties Component", *GetNameSafe(GetOwner()));
	Super::PostInitProperties();
	LOG_GUID_INFO("-- Guid: %s", *Guid.ToString());
}

void USimpleGuidComponent::PreSave(FObjectPreSaveContext SaveContext)
{
	LOG_GUID_INFO("[%s.SimpleGuidComponent] PreSave Component", *GetNameSafe(GetOwner()));
	Super::PreSave(SaveContext);
	LOG_GUID_INFO("-- Guid: %s", *Guid.ToString());
}

void USimpleGuidComponent::PostLoad()
{
	LOG_GUID_INFO("[%s.SimpleGuidComponent] PostLoad Component", *GetNameSafe(GetOwner()));
	Super::PostLoad();
	LOG_GUID_INFO("-- Guid: %s", *Guid.ToString());
}

void USimpleGuidComponent::OnComponentCreated()
{
	LOG_GUID_INFO("[%s.SimpleGuidComponent] Created Component", *GetNameSafe(GetOwner()));
	Super::OnComponentCreated();
	LOG_GUID_INFO("-- Guid: %s", *Guid.ToString());
}

void USimpleGuidComponent::InitializeComponent()
{
	LOG_GUID_INFO("[%s.SimpleGuidComponent] Initialize Component", *GetNameSafe(GetOwner()));
	Super::InitializeComponent();
	LOG_GUID_INFO("-- Guid: %s", *Guid.ToString());
}

void USimpleGuidComponent::BeginPlay()
{
	LOG_GUID_INFO("[%s.SimpleGuidComponent] BeginPlay Component", *GetNameSafe(GetOwner()));
	Super::BeginPlay();
	LOG_GUID_INFO("-- Guid: %s", *Guid.ToString());
}
#endif
