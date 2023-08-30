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

#include "DungeonBlueprintLibrary.h"
#include "Door.h"
#include "DoorType.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/Pawn.h"

bool UDungeonBlueprintLibrary::IsDoorOfType(const TSubclassOf<ADoor> DoorClass, const UDoorType* DoorType)
{
	ADoor* Door = DoorClass.GetDefaultObject();
	return Door && (Door->GetDoorType() == DoorType);
}

bool UDungeonBlueprintLibrary::CompareDataTableRows(const FDataTableRowHandle& A, const FDataTableRowHandle& B)
{
	return A == B;
}

void UDungeonBlueprintLibrary::Spectate(APlayerController* Controller, bool DestroyPawn)
{
	if (!Controller)
		return;

	if (!Controller->HasAuthority())
		return;

	APawn* PreviousPawn = Controller->GetPawn();

	Controller->PlayerState->SetIsSpectator(true);
	Controller->ChangeState(NAME_Spectating);
	Controller->bPlayerIsWaiting = true;
	Controller->ClientGotoState(NAME_Spectating);

	if (DestroyPawn && IsValid(PreviousPawn))
	{
		PreviousPawn->Destroy();
	}
}
