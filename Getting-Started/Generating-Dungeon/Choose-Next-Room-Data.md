---
slug: /guides/Choose-Next-Room-Data
pagination_prev: guides/Getting-Started/Generating-Dungeon/Choose-First-Room-Data
pagination_next: guides/Getting-Started/Generating-Dungeon/Continue-To-Add-Room
---

# Choose Next Room

<!-- BEGIN IMPORTS -->

import Tabs from '@theme/Tabs';
import TabItem from '@theme/TabItem';

<!-- END IMPORTS -->

This function is the main part of the procedural generation: it's where you set your own logic to generate your dungeon.\
This function is run each time the generator tries to add a room to the dungeon.

As input parameters:

- `Current Room` is the room from which the generator will try to add the next room.\
For example, the first time this function is called, the `Current Room` is the `RoomData` asset you have returned in the [`Choose First Room`](Choose-First-Room-Data.md) function.
[DEPRECATED: use [`Get Room Data`](api/Classes/ReadOnlyRoom/Nodes/GetRoomData/GetRoomData.md) of the `Current Room Instance` input instead]
- `Current Room Instance` is the room instance from which the generator will try to add the next room.\
For example, the first time this function is called, the `Current Room Instance` is the `Room` instance created from the room data you have returned in the [`Choose First Room`](Choose-First-Room-Data.md) function.
- `Door Data` is the door on which the next room will be connected to.\
You have access to its location (expressed in room units, not in unreal units!), its orientation and its `DoorType`.

As output:

- `Return Value` is the `Room Data` asset you want to add as the next room. The value must be non-null and have at least one compatible door with `Door Data`.
- `Door Index` is the index of the door (from the `Room Data` asset your return) used to connect with the `Current Room`.\
If the index is negative, it will choose a random compatible door.\
If you set a value, it will take the door at the same index in your `Room Data` asset (*make sure the door is compatible!*)

:::warning[How to code my specific use case?]

If you struggle to write the code in this function, I would suggest you to read [this page](../../Best-Practices/Workflows/Dungeon-Generation-Algorithm.md) to better understand the process of writing the code for this function.

:::

Here an example of a simple dungeon made by `NbRoom` randomly chosen standard rooms before setting an exit room:

<!-- [BEGIN TABS] Blueprint | C++ --> <Tabs>
<!-- [BEGIN TAB ITEM] Blueprint --> <TabItem value="bp" label="Blueprint" default>

![](../../Images/ChooseNextRoomData.jpg)

<!-- [END TAB ITEM] Blueprint --> </TabItem>
<!-- [BEGIN TAB ITEM] C++ --> <TabItem value="cpp" label="C++">

```cpp title="MyDungeonGenerator.h"
UCLASS()
class AMyDungeonGenerator : public ADungeonGenerator
{
    GENERATED_BODY()
public:
    // ...

    virtual URoomData* ChooseNextRoomData_Implementation(const URoomData* CurrentRoom, const TScriptInterface<IReadOnlyRoom>& CurrentRoomInstance, const FDoorDef& DoorData, int& DoorIndex) override;

    // The following variables are just for example purpose
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "My Dungeon")
    TArray<URoomData*> NormalRooms;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "My Dungeon")
    URoomData* EndingRoom;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "My Dungeon")
    int32 MinNbRoom;

    // ...
}
```

```cpp title="MyDungeonGenerator.cpp"

URoomData* AMyDungeonGenerator::ChooseNextRoomData_Implementation(const URoomData* CurrentRoom, const TScriptInterface<IReadOnlyRoom>& CurrentRoomInstance, const FDoorDef& DoorData, int& DoorIndex)
{
    // After reaching the minimum number of room, place the end room
    if (GetRooms()->Count() >= MinNbRoom)
    {
        return EndingRoom;
    }

    // Otherwise, place a random normal room
    return GetRandomRoomData(NormalRooms);
}

```

<!-- [END TAB ITEM] C++ --> </TabItem>
<!-- [END TABS] Blueprint | C++ --> </Tabs>
