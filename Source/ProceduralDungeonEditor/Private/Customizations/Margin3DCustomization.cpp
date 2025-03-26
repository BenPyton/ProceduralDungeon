/*
 * Copyright (c) 2024 Benoit Pelletier
 *
 * This software is available under different licenses depending on the source from which it was obtained:
 * - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
 * - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
 *
 * Please refer to the accompanying LICENSE file for further details.
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
