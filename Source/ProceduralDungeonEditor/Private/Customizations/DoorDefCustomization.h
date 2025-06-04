// Copyright Benoit Pelletier 2023 - 2025 All Rights Reserved.
//
// This software is available under different licenses depending on the source from which it was obtained:
// - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
// - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
// Please refer to the accompanying LICENSE file for further details.

#pragma once

#include "IPropertyTypeCustomization.h"
#include "PropertyHandle.h"
#include "Customizations/MathStructCustomizations.h"

class FDoorDefCustomization : public FMathStructCustomization
{
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();

protected:
	// ~BEGIN FMathStructCustomization
	virtual void GetSortedChildren(TSharedRef<IPropertyHandle> StructPropertyHandle, TArray<TSharedRef<IPropertyHandle>>& OutChildren) override;
	virtual TSharedRef<SWidget> MakeChildWidget(TSharedRef<IPropertyHandle>& StructurePropertyHandle, TSharedRef<IPropertyHandle>& PropertyHandle) override;
	// ~END FMathStructCustomization
};
