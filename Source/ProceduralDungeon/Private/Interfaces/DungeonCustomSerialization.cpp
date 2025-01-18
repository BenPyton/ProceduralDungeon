/*
 * MIT License
 *
 * Copyright (c) 2025 Benoit Pelletier
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

#include "Interfaces/DungeonCustomSerialization.h"
#include "ProceduralDungeonLog.h"
#include "ProceduralDungeonUtils.h"

bool IDungeonCustomSerialization::DispatchFixupReferences(UObject* Obj, UObject* Context)
{
	check(IsValid(Obj));
	DungeonLog_Debug("[BEGIN] Dispatch 'Fixup References' function from object '%s'.", *GetNameSafe(Obj));

	ObjectUtils::DispatchToObjectAndSubobjects(Obj, [Context](UObject* O) {
		auto* Custom = Cast<IDungeonCustomSerialization>(O);
		if (nullptr != Custom)
		{
			Custom->FixupReferences(Context);
		}
	});

	DungeonLog_Debug("[END] Dispatch 'Fixup References' function from object '%s'.", *GetNameSafe(Obj));
	return true;
}
