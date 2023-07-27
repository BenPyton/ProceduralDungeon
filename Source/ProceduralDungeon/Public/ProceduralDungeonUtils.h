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

#pragma once

#include "Math/Vector.h"
#include "Math/IntVector.h"

namespace IntVector
{
	// Returns the component-wise minimum of A and B
	FIntVector PROCEDURALDUNGEON_API Min(const FIntVector& A, const FIntVector& B);

	// Returns the component-wise maximum of A and B
	FIntVector PROCEDURALDUNGEON_API Max(const FIntVector& A, const FIntVector& B);

	// Returns the component-wise minimum and maximum of A and B
	void PROCEDURALDUNGEON_API MinMax(const FIntVector& A, const FIntVector& B, FIntVector& OutMin, FIntVector& OutMax);
}

namespace Dungeon
{
	// Returns the real world location of a point in room units
	FVector PROCEDURALDUNGEON_API ToWorldLocation(FIntVector RoomPoint);

	// Returns the real world vector (no offset) of a vector in room units
	FVector PROCEDURALDUNGEON_API ToWorldVector(FIntVector RoomVector);

	// Returns the location in room units from a point in real world
	FIntVector PROCEDURALDUNGEON_API ToRoomLocation(FVector WorldPoint);

	// Returns the vector (no offset) in room units from a vector in real world
	FIntVector PROCEDURALDUNGEON_API ToRoomVector(FVector WorldVector);

	// Returns the real world snapped location to the nearest point in room units from a real world point
	FVector PROCEDURALDUNGEON_API SnapPoint(FVector Point);
}
