/*
 * MIT License
 *
 * Copyright (c) 2023 Benoit Pelletier
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

#include "RoomDataFactory.h"
#include "RoomData.h"
#include "AssetTypeCategories.h"
#include "ProceduralDungeonEditor.h"
#include "ClassViewerModule.h"
#include "ClassViewerFilter.h"
#include "Kismet2/SClassPickerDialog.h"
#include "ProceduralDungeonEditorSettings.h"
#include "Misc/EngineVersionComparison.h"

#define LOCTEXT_NAMESPACE "RoomDataFactory"

// Copied and a little modified from the EditorFactories.cpp of UnrealEd module (not public)
class FRoomDataClassFilter : public IClassViewerFilter
{
public:
	FRoomDataClassFilter()
		: DisallowedClassFlags(CLASS_Abstract | CLASS_Deprecated | CLASS_NewerVersionExists | CLASS_HideDropDown)
	{
	}

   /** Disallowed class flags. */
	EClassFlags DisallowedClassFlags;

	virtual bool IsClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const UClass* InClass, TSharedRef< FClassViewerFilterFuncs > InFilterFuncs) override
	{
		bool bAllowed = !InClass->HasAnyClassFlags(DisallowedClassFlags)
			&& InFilterFuncs->IfInChildOfClassesSet(ParentClasses, InClass) != EFilterReturn::Failed;

		return bAllowed;
	}

	virtual bool IsUnloadedClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const TSharedRef< const IUnloadedBlueprintData > InUnloadedClassData, TSharedRef< FClassViewerFilterFuncs > InFilterFuncs) override
	{
		return !InUnloadedClassData->HasAnyClassFlags(DisallowedClassFlags)
			&& InFilterFuncs->IfInChildOfClassesSet(ParentClasses, InUnloadedClassData) != EFilterReturn::Failed;
	}

private:
	/** All children of these classes will be included unless filtered out by another setting. */
	static TSet<const UClass*> ParentClasses;
};

TSet<const UClass*> FRoomDataClassFilter::ParentClasses {URoomData::StaticClass()};

URoomDataFactory::URoomDataFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = URoomData::StaticClass();
}

uint32 URoomDataFactory::GetMenuCategories() const
{
	FProceduralDungeonEditorModule& EditorModule = FModuleManager::LoadModuleChecked<FProceduralDungeonEditorModule>("ProceduralDungeonEditor");
	return EditorModule.GetAssetTypeCategory();
}

bool URoomDataFactory::ConfigureProperties()
{
	// Get the default room data class from the editor settings
	const UProceduralDungeonEditorSettings* EditorSettings = GetDefault<UProceduralDungeonEditorSettings>();
	UClass* DefaultClass = EditorSettings->DefaultRoomDataClass.Get();
	if (!DefaultClass)
		DefaultClass = URoomData::StaticClass();

	// TODO: want to show only RootClass and children with a plugin setting
	// However the SClassPickerDialog will always show all parent classes of the RootClass
	// (see SClassPickerDialog.cpp line 49)
	UClass* RootClass = URoomData::StaticClass();
	//if (EditorSettings->bShowOnlyDefaultAndChildren)
	//	RootClass = DefaultClass;

	// Bypass the class picker if there is no derived class of the default class
	if (EditorSettings->bUseDefaultIfNoChild)
	{
		TArray<UClass*> DerivedClasses;
		GetDerivedClasses(DefaultClass, DerivedClasses, false);
		if (DerivedClasses.Num() <= 0)
		{
			RoomDataClass = DefaultClass;
			return true;
		}
	}

	// nullptr the RoomDataClass so we can check for selection
	RoomDataClass = nullptr;

	// Load the classviewer module to display a class picker
	FClassViewerModule& ClassViewerModule = FModuleManager::LoadModuleChecked<FClassViewerModule>("ClassViewer");

	// The class viewer filter to show only RoomData class and its child classes
	TSharedPtr<FRoomDataClassFilter> Filter = MakeShareable(new FRoomDataClassFilter);

	// Fill in options
	FClassViewerInitializationOptions Options;
	Options.Mode = EClassViewerMode::ClassPicker;
	Options.InitiallySelectedClass = DefaultClass;
#if UE_VERSION_OLDER_THAN(5, 0, 0)
	Options.ClassFilter = Filter;
#else
	Options.ClassFilters.Add(Filter.ToSharedRef());
#endif

	const FText TitleText = LOCTEXT("CreateRoomDataOptions", "Pick Room Data Class");
	UClass* ChosenClass = nullptr;
	const bool bPressedOk = SClassPickerDialog::PickClass(TitleText, Options, ChosenClass, RootClass);
	if (bPressedOk)
	{
		RoomDataClass = ChosenClass;
	}

	return bPressedOk;
}

UObject* URoomDataFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	if (RoomDataClass != nullptr)
	{
		return NewObject<URoomData>(InParent, RoomDataClass, InName, Flags | RF_Transactional);
	}
	else
	{
		// if we have no data asset class, use the passed-in class instead
		check(InClass->IsChildOf(URoomData::StaticClass()));
		return NewObject<URoomData>(InParent, InClass, InName, Flags);
	}
}

#undef LOCTEXT_NAMESPACE
