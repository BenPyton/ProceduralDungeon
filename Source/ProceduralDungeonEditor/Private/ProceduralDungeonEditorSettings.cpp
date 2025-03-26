/*
 * Copyright (c) 2023-2024 Benoit Pelletier
 *
 * This software is available under different licenses depending on the source from which it was obtained:
 * - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
 * - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
 *
 * Please refer to the accompanying LICENSE file for further details.
 */

#include "ProceduralDungeonEditorSettings.h"

UProceduralDungeonEditorSettings::UProceduralDungeonEditorSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DefaultRoomDataClass = URoomData::StaticClass();
	bUseDefaultIfNoChild = true;
	//bShowOnlyDefaultAndChildren = false;

	DefaultMargins.XAxis.X = 10.0f;
	DefaultMargins.XAxis.Y = 10.0f;
	DefaultMargins.YAxis.X = 10.0f;
	DefaultMargins.YAxis.Y = 10.0f;
	DefaultMargins.ZAxis.X = 10.0f;
	DefaultMargins.ZAxis.Y = 10.0f;
}
