/*
* MIT License
*
* Copyright (c) 2024 Benoit Pelletier
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

#include "Margin3DCustomization.h"
#include "ProceduralDungeonEditorSettings.h"
#include "PropertyCustomizationHelpers.h"

#define LOCTEXT_NAMESPACE "FMargin3DCustomization"

TSharedRef<IPropertyTypeCustomization> FMargin3DCustomization::MakeInstance()
{
	return MakeShareable(new FMargin3DCustomization());
}

void FMargin3DCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	TSharedPtr<IPropertyHandle> AxisProps[3] = {
		StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FMargin3D, XAxis)),
		StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FMargin3D, YAxis)),
		StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FMargin3D, ZAxis))
	};

	TSharedPtr<SHorizontalBox> ValueRow;

	HeaderRow
	.NameContent()
	[
		StructPropertyHandle->CreatePropertyNameWidget()
	]
	.ValueContent()[
		SAssignNew(ValueRow, SHorizontalBox)
	];

	for (const auto& AxisProp : AxisProps)
	{
		auto PositiveProp = AxisProp->GetChildHandle(GET_MEMBER_NAME_CHECKED(FVector2D, X));
		auto NegativeProp = AxisProp->GetChildHandle(GET_MEMBER_NAME_CHECKED(FVector2D, Y));

		// Axis name
		ValueRow->AddSlot()
			.AutoWidth()
			[
				AxisProp->CreatePropertyNameWidget()
			];

		if (PositiveProp)
		{
			// Margin along positive axis
			ValueRow->AddSlot()
				.AutoWidth()
				.Padding(3.0f, 0.0f)
				[
					PositiveProp->CreatePropertyValueWidget()
				];
		}

		if (NegativeProp)
		{
			// Margin along negative axis
			ValueRow->AddSlot()
				.AutoWidth()
				.Padding(0.0f, 0.0f, 20.0f, 0.0f)
				[
					NegativeProp->CreatePropertyValueWidget()
				];
		}
	}
}

void FMargin3DCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	// TODO?
}


#undef LOCTEXT_NAMESPACE
