/*
 * Copyright (c) 2024 Benoit Pelletier
 *
 * This software is available under different licenses depending on the source from which it was obtained:
 * - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
 * - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
 *
 * Please refer to the accompanying LICENSE file for further details.
 */

#pragma once

#include "Utils/ReplicationDefines.h"
#include "Net/UnrealNetwork.h" // DOREPLIFETIME
#include "Net/Core/PushModel/PushModel.h"

#if UE_WITH_IRIS
#include "Iris/ReplicationSystem/ReplicationFragmentUtil.h"
#endif // UE_WITH_IRIS

// Should be called to modify the value of a replicated variable in a specific object.
// Use in place of Object->Property = Value;
#define SET_OBJECT_REPLICATED_PROPERTY_VALUE(Object, Property, Value) \
	Property = Value; \
	MARK_PROPERTY_DIRTY_FROM_NAME(std::remove_pointer_t<decltype(Object)>, Property, Object);

// Should be called in the owner class of a replicated variable.
// in place of Name = Value;
#define SET_REPLICATED_PROPERTY_VALUE(Property, Value) \
	SET_OBJECT_REPLICATED_PROPERTY_VALUE(this, Property, Value);

// Should be called in the actor class of a replicated variable.
// in place of Name = Value;
#define SET_ACTOR_REPLICATED_PROPERTY_VALUE(Property, Value) \
	FlushNetDormancy(); \
	SET_OBJECT_REPLICATED_PROPERTY_VALUE(this, Property, Value);

// Should be called in a UReplicableObject derived class
// in place of Name = Value;
#define SET_SUBOBJECT_REPLICATED_PROPERTY_VALUE(Property, Value) \
	WakeUpOwnerActor(); \
	SET_OBJECT_REPLICATED_PROPERTY_VALUE(this, Property, Value);
