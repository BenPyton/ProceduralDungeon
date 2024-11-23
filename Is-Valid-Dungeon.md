---
pagination_prev: guides/Continue-To-Add-Room
pagination_next: guides/Choose-Door
---

# Is Valid Dungeon

<!-- BEGIN IMPORTS -->

import Tabs from '@theme/Tabs';
import TabItem from '@theme/TabItem';

<!-- END IMPORTS -->

This function determines if the dungeon corresponds to your needs.\
Check all requirements you want here and return true if the dungeon meets all of them.\
If you return false, the generator will wipes the generated dungeon and begins a new generation starting at the `Generation Init` event.

In output, you have to return true to validate the dungeon, or false to regenerate.

Here an example where we check if the dungeon has an exit:

<!-- [BEGIN TABS] Blueprint | C++ --> <Tabs>
<!-- [BEGIN TAB ITEM] Blueprint --> <TabItem value="bp" label="Blueprint" default>

![](Images/IsValidDungeon.jpg)

<!-- [END TAB ITEM] Blueprint --> </TabItem>
<!-- [BEGIN TAB ITEM] C++ --> <TabItem value="cpp" label="C++">

```cpp title="MyDungeonGenerator.h"
UCLASS()
class AMyDungeonGenerator : public ADungeonGenerator
{
    GENERATED_BODY()
public:
    // ...

    virtual bool IsValidDungeon_Implementation() override;

    // This variable is just for example purpose
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "My Dungeon")
    URoomData* EndingRoom {nullptr};

    // ...
}
```

```cpp title="MyDungeonGenerator.cpp"

bool AMyDungeonGenerator::IsValidDungeon_Implementation()
{
    return GetRooms()->HasAlreadyRoomData(EndingRoom);
}

```

<!-- [END TAB ITEM] C++ --> </TabItem>
<!-- [END TABS] Blueprint | C++ --> </Tabs>
