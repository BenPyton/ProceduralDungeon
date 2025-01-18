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

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GeneratorProvider.generated.h"

UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UGeneratorProvider : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for classes that give access to a ADungeonGeneratorBase instance.
 * @TODO: Currently only used to resolve URoom::GeneratorOwner references when loading a saved dungeon.
 * It would be better in a future version to decouple the URoom from the DungeonGenerator and instead
 * pass some Interface references to the functions needed (currently a Transform and a RandomStream).
 * I just want to say that this interface is just temporary and must not be used by users of the plugin,
 * as it will certainly be removed in a near future version of the plugin.
 */
class IGeneratorProvider
{
	GENERATED_BODY()

public:
	virtual class ADungeonGeneratorBase* GetGenerator() const = 0;
};
