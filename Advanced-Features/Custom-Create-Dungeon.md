# Custom `Create Dungeon` Function

:::warning

As of now, overriding the `Create Dungeon` function is only available in C++ because I didn't found out in blueprint how to prevent overriding functions in child classes while allowing them in parent class.

If you know how to do that, please let me know on [**Github**](https://github.com/BenPyton/ProceduralDungeon/issues) or [**Discord**](https://discord.gg/YE2dPda2CC)!

:::

## Introduction

The provided `Dungeon Generator` class in the plugin has a basic default generation algorithm.\
Its behavior is to create a first room and then add new rooms to existing doors.

See the `Create Dungeon` function in the diagram below:

![](../Images/Flowchart_Dark_v4.svg)

If you are not satisfied with this default behavior, you can write your own `Create Dungeon` function while keeping the core features of the generator.

## The `Dungeon Generator Base` class

To write your own custom `Create Dungeon`, you'll need to create a new class deriving from **`Dungeon Generator Base`**.\
This class has the core features of a dungeon generator: the state machine to load/unload the level instances (shown in the diagram above), the network replication, the room culling system, etc.

:::note

You *can* derive from the `Dungeon Generator` class and override the `Create Dungeon` function, but I would strongly discourage that as it will embed some generator's settings and overridable functions that you will certainly not using anymore.

:::

:::tip

You can look into the `Dungeon Generator` class as an example to help you writing your own `Create Function`.

:::

Below is a template to start your class.
You can then add any variables and functions you need for your generation.

```cpp title="MyCustomDungeonGenerator.h"
UCLASS()
class AMyCustomDungeonGenerator : public ADungeonGeneratorBase
{
    GENERATED_BODY()

protected:
	//~ Begin ADungeonGeneratorBase Interface
	virtual bool CreateDungeon_Implementation() override;
	//~ End ADungeonGeneratorBase Interface
}
```

```cpp title="MyCustomDungeonGenerator.cpp"
URoomData* AMyCustomDungeonGenerator::CreateDungeon_Implementation()
{
    // Must be called whenever you start a new generation.
    // It will call the `Generation Init` event to allow resetting variables used for the generation.
    StartNewDungeon();

    // ... Do your generation logic here ...
    // Here the important functions to create and place a new room in the dungeon:
    {
        // Create a new room instance from a room data
        URoom* NewRoom = CreateRoomInstance(RoomData);

        // Will attempt to place the room in a way to connect the NewRoom's door at specified index to the target door.
        // The GetWorld() is used to also check is the room is colliding with actors in the world (when the bUseWorldCollisionCheck is true).
        if (!TryPlaceRoom(NewRoom, NewRoomDoorIndex, TargetDoor, GetWorld())
        {
            // The room could not be placed.
            NewRoom = nullptr;
        }

        // Will actually adds the room into the dungeon and connects the provided doors if possible.
        // This function will call `OnRoomAdded` and return true if the room has been successfully added to the dungeon.
        // You can pass an empty array as `DoorsToConnect` to try connecting all the doors.
        if (!AddRoomToDungeon(NewRoom, /*DoorsToConnect = */TArray<int>{NewRoomDoorIndex}, /*bFailIfNotConnected = */true))
        {
            // The room was not added to the dungeon, because it was invalid (nullptr) or not connected.
            OnFailedToAddRoom(ParentRoomData, TargetDoor);
        }
    }
    // ...

    // Must be called when the dungeon has finished the generation.
    // Will initialize the room instances and call the `Initialize Dungeon` event.
    FinalizeDungeon();

    // You should return true when the dungeon is generated properly.
    // If returning false, the dungeon will be erased and an error will be displayed.
    return true;
}
```
