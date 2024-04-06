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
