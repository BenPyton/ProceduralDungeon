/*
 * MIT License
 *
 * Copyright (c) 2019-2023 Benoit Pelletier
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

#include "ProceduralDungeonTypes.h"

bool operator!(const EDoorDirection& Direction)
{
	return Direction == EDoorDirection::NbDirection;
}

EDoorDirection operator+(const EDoorDirection& A, const EDoorDirection& B)
{
	check(!!A && !!B);
	return static_cast<EDoorDirection>(static_cast<uint8>((static_cast<uint8>(A) + static_cast<uint8>(B))) % static_cast<uint8>(EDoorDirection::NbDirection));
}

EDoorDirection operator-(const EDoorDirection& A, const EDoorDirection& B)
{
	check(!!A && !!B);
	return static_cast<EDoorDirection>(static_cast<uint8>((static_cast<uint8>(A) - static_cast<uint8>(B))) % static_cast<uint8>(EDoorDirection::NbDirection));
}

EDoorDirection& operator+=(EDoorDirection& A, const EDoorDirection& B)
{
	A = A + B;
	return A;
}

EDoorDirection& operator-=(EDoorDirection& A, const EDoorDirection& B)
{
	A = A - B;
	return A;
}

EDoorDirection& operator++(EDoorDirection& Direction)
{
	Direction = Direction + EDoorDirection::East;
	return Direction;
}

EDoorDirection& operator--(EDoorDirection& Direction)
{
	Direction = Direction - EDoorDirection::East;
	return Direction;
}

EDoorDirection operator++(EDoorDirection& Direction, int)
{
	EDoorDirection old {Direction};
	++Direction;
	return old;
}

EDoorDirection operator--(EDoorDirection& Direction, int)
{
	EDoorDirection old {Direction};
	--Direction;
	return old;
}

EDoorDirection operator-(const EDoorDirection& Direction)
{
	return EDoorDirection::North - Direction;
}

EDoorDirection operator~(const EDoorDirection& Direction)
{
	return Direction + EDoorDirection::South;
}

FIntVector ToIntVector(const EDoorDirection& Direction)
{
	FIntVector Dir = FIntVector::ZeroValue;
	switch (Direction)
	{
	case EDoorDirection::North:
		Dir.X = 1;
		break;
	case EDoorDirection::East:
		Dir.Y = 1;
		break;
	case EDoorDirection::West:
		Dir.Y = -1;
		break;
	case EDoorDirection::South:
		Dir.X = -1;
		break;
	default:
		checkNoEntry();
	}
	return Dir;
}

FVector ToVector(const EDoorDirection& Direction)
{
	FVector Dir = FVector::ZeroVector;
	switch (Direction)
	{
	case EDoorDirection::North:
		Dir.X = 1.0f;
		break;
	case EDoorDirection::East:
		Dir.Y = 1.0f;
		break;
	case EDoorDirection::West:
		Dir.Y = -1.0f;
		break;
	case EDoorDirection::South:
		Dir.X = -1.0f;
		break;
	default:
		checkNoEntry();
	}
	return Dir;
}

FQuat ToQuaternion(const EDoorDirection& Direction)
{
	check(!!Direction);
	return FRotator(0.0f, 90.0f * static_cast<uint8>(Direction), 0.0f).Quaternion();
}

FIntVector Rotate(const FIntVector& Pos, const EDoorDirection& Rot)
{
	FIntVector NewPos = Pos;
	switch (Rot)
	{
	case EDoorDirection::North:
		NewPos = Pos;
		break;
	case EDoorDirection::West:
		NewPos.Y = -Pos.X;
		NewPos.X = Pos.Y;
		break;
	case EDoorDirection::East:
		NewPos.Y = Pos.X;
		NewPos.X = -Pos.Y;
		break;
	case EDoorDirection::South:
		NewPos.Y = -Pos.Y;
		NewPos.X = -Pos.X;
		break;
	default:
		checkNoEntry();
	}
	return NewPos;
}
