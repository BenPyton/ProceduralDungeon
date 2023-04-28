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

#include "DoorDefCustomization.h"
#include "ProceduralDungeonTypes.h"
#include "PropertyCustomizationHelpers.h"

#define LOCTEXT_NAMESPACE "FDoorDefCustomization"

TSharedRef<IPropertyTypeCustomization> FDoorDefCustomization::MakeInstance()
{
	return MakeShareable(new FDoorDefCustomization());
}

void FDoorDefCustomization::GetSortedChildren(TSharedRef<IPropertyHandle> StructPropertyHandle, TArray< TSharedRef<IPropertyHandle> >& OutChildren)
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
