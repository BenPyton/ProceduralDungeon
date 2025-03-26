/*
 * Copyright (c) 2024 Benoit Pelletier
 *
 * This software is available under different licenses depending on the source from which it was obtained:
 * - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
 * - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
 *
 * Please refer to the accompanying LICENSE file for further details.
 */

#include "ProceduralDungeonEdTypes.h"

EAxisList::Type operator~(const EAxisList::Type& This)
{
	return static_cast<EAxisList::Type>(EAxisList::All - This);
}

EAxisList::Type& operator&=(EAxisList::Type& This, const EAxisList::Type& Other)
{
	This = static_cast<EAxisList::Type>(This & Other);
	return This;
}

FBoxCenterAndExtent FMargin3D::Apply(const FBoxCenterAndExtent& Bounds) const
{
	FBoxCenterAndExtent NewBounds(Bounds);
	NewBounds.Extent += 0.5f * FVector(XAxis.X + XAxis.Y, YAxis.X + YAxis.Y, ZAxis.X + ZAxis.Y);
	NewBounds.Center += 0.5f * FVector(XAxis.X - XAxis.Y, YAxis.X - YAxis.Y, ZAxis.X - ZAxis.Y);
	return NewBounds;
}
