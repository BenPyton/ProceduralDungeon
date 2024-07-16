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
