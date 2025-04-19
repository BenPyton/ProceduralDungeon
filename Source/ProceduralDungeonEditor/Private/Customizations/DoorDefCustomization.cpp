/*
 * Copyright (c) 2023 Benoit Pelletier
 *
 * This software is available under different licenses depending on the source from which it was obtained:
 * - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
 * - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
 *
 * Please refer to the accompanying LICENSE file for further details.
 */

#include "DoorDefCustomization.h"
#include "ProceduralDungeonTypes.h"
#include "PropertyCustomizationHelpers.h"

#define LOCTEXT_NAMESPACE "FDoorDefCustomization"

TSharedRef<IPropertyTypeCustomization> FDoorDefCustomization::MakeInstance()
{
	return MakeShareable(new FDoorDefCustomization());
}

void FDoorDefCustomization::GetSortedChildren(TSharedRef<IPropertyHandle> StructPropertyHandle, TArray<TSharedRef<IPropertyHandle>>& OutChildren)
{
	TSharedPtr<IPropertyHandle> PositionProp = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FDoorDef, Position));
	TSharedPtr<IPropertyHandle> DirectionProp = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FDoorDef, Direction));
	TSharedPtr<IPropertyHandle> TypeProp = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FDoorDef, Type));

	// ============= Copied from VectorStructCustomization class (private in engine so can't inherit from it) ============================
	// Only replaced the StructPropertyHandle to the PositionProp

	static const FName X("X");
	static const FName Y("Y");
	static const FName Z("Z");

	TSharedPtr<IPropertyHandle> VectorChildren[3];

	uint32 NumChildren;
	PositionProp->GetNumChildren(NumChildren);

	for (uint32 ChildIndex = 0; ChildIndex < NumChildren; ++ChildIndex)
	{
		TSharedRef<IPropertyHandle> ChildHandle = PositionProp->GetChildHandle(ChildIndex).ToSharedRef();
		const FName PropertyName = ChildHandle->GetProperty()->GetFName();

		if (PropertyName == X)
		{
			VectorChildren[0] = ChildHandle;
		}
		else if (PropertyName == Y)
		{
			VectorChildren[1] = ChildHandle;
		}
		else
		{
			check(PropertyName == Z);
			VectorChildren[2] = ChildHandle;
		}
	}

	OutChildren.Add(VectorChildren[0].ToSharedRef());
	OutChildren.Add(VectorChildren[1].ToSharedRef());
	OutChildren.Add(VectorChildren[2].ToSharedRef());

	// ================ Add Direction property in the children ===================

	OutChildren.Add(DirectionProp.ToSharedRef());
	OutChildren.Add(TypeProp.ToSharedRef());
}

TSharedRef<SWidget> FDoorDefCustomization::MakeChildWidget(TSharedRef<IPropertyHandle>& StructurePropertyHandle, TSharedRef<IPropertyHandle>& PropertyHandle)
{
	const FFieldClass* PropertyClass = PropertyHandle->GetPropertyClass();
	if (PropertyClass == FEnumProperty::StaticClass())
		return PropertyHandle->CreatePropertyValueWidget();

	if (PropertyClass == FStructProperty::StaticClass())
		return PropertyHandle->CreatePropertyValueWidget();

	if (PropertyClass == FObjectProperty::StaticClass())
		return PropertyHandle->CreatePropertyValueWidget();

	return FMathStructCustomization::MakeChildWidget(StructurePropertyHandle, PropertyHandle);
}

#undef LOCTEXT_NAMESPACE
