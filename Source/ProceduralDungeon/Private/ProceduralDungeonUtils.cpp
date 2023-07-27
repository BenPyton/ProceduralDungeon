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

#include "ProceduralDungeonUtils.h"
#include "Room.h"

FIntVector IntVector::Min(const FIntVector& A, const FIntVector& B)
{
	return FIntVector(FMath::Min(A.X, B.X), FMath::Min(A.Y, B.Y), FMath::Min(A.Z, B.Z));
}

FIntVector IntVector::Max(const FIntVector& A, const FIntVector& B)
{
	return FIntVector(FMath::Max(A.X, B.X), FMath::Max(A.Y, B.Y), FMath::Max(A.Z, B.Z));
}

void IntVector::MinMax(const FIntVector& A, const FIntVector& B, FIntVector& OutMin, FIntVector& OutMax)
{
	OutMin = Min(A, B);
	OutMax = Max(A, B);
}

FVector Dungeon::ToWorldLocation(FIntVector RoomPoint)
{
    return URoom::Unit() * (FVector(RoomPoint) - FVector(0.5f, 0.5f, 0.0f));
}

FIntVector Dungeon::ToRoomLocation(FVector WorldPoint)
{
    const FVector Unit = URoom::Unit();
    const int X = FMath::RoundToInt(0.5f + (WorldPoint.X) / Unit.X);
    const int Y = FMath::RoundToInt(0.5f + (WorldPoint.Y) / Unit.Y);
    const int Z = FMath::RoundToInt((WorldPoint.Z) / Unit.Z);
    return FIntVector(X, Y, Z);
}

FIntVector Dungeon::ToRoomVector(FVector WorldVector)
{
    const FVector Unit = URoom::Unit();
    const int X = FMath::RoundToInt(WorldVector.X / Unit.X);
    const int Y = FMath::RoundToInt(WorldVector.Y / Unit.Y);
    const int Z = FMath::RoundToInt(WorldVector.Z / Unit.Z);
    return FIntVector(X, Y, Z);
}

FVector Dungeon::SnapPoint(FVector Point)
{
    return ToWorldLocation(ToRoomLocation(Point));
}
