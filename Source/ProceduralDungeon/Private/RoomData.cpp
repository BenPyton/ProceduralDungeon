// Fill out your copyright notice in the Description page of Project Settings.

#include "RoomData.h"
#include "RoomLevel.h"
#include "ProceduralDungeonTypes.h"

URoomData::URoomData()
	: Super()
{
	Doors.Add(FDoorDef());
	Size = FIntVector(1, 1, 1);
	RandomDoor = true;
}