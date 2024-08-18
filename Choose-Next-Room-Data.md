---
pagination_prev: guides/Choose-First-Room-Data
pagination_next: guides/Continue-To-Add-Room
---

# Choose Next Room

This function is the main part of the procedural generation: it's where you set your own logic to generate your dungeon.\
This function is run each time the generator tries to add a room to the dungeon.

As input parameters:

- `Current Room` is the room from which the generator will try to add the next room.\
For example, the first time this function is called, the `Current Room` is the `RoomData` asset you have returned in the [`Choose First Room`](Choose-First-Room-Data.md) function.
- `Door Data` is the door on which the next room will be connected to.\
You have access to its location (expressed in room units, not in unreal units!), its orientation and its `DoorType`.

As output:

- `Return Value` is the `Room Data` asset you want to add as the next room. The value must be non-null and have at least one compatible door with `Door Data`.
- `Door Index` is the index of the door (from the `Room Data` asset your return) used to connect with the `Current Room`.\
If the index is negative, it will choose a random compatible door.\
If you set a value, it will take the door at the same index in your `Room Data` asset (*make sure the door is compatible!*)

Here an example of a simple dungeon made by `NbRoom` randomly chosen standard rooms before setting an exit room:\
![](Images/ChooseNextRoomData.jpg)

### Tips on how you can do it

Since it is one of the most important function of the plugin, I will give you some general steps on how you can effectively create the code of this function.

1. First, you have to define the goal of your dungeon. You should ask yourself: 'What the player have to do to clear the dungeon?'.\
In the [example project][1], I want the player to reach a specific room (called the end room) in order to clear the dungeon.

2. Secondly, you have to design how you want your dungeon to look like.\
Do you want secret rooms? a boss room? an increasing room difficulty between start and end? etc.
In the [example project][1], I want to have something simple simple.
The end room is locked and the player needs to get a key placed in a specific room (called the key room) to unlock the end room.
The dungeon will thus have only one key room and only one end room.\
Since the player is spawned at the start room in my example project, the dungeon needs to look like that in order for the player to complete the dungeon: `Start Room` -> `Key Room` -> `End Room`.
I can then add whatever optional rooms I want between them (corridors, dead ends, etc.).

3. Thirdly, once you have designed your dungeon on paper, you have to convert it to code in the `ChooseNextRoomData` function.
The first thing would be to determine which of your rules are more specific to the others.\
If I am taking the [example][1] I have described above, I want *one* end room and *one* key room.
These 2 rules are more specific than the "adding whatever corridors, dead ends, etc. between those rooms".\
Since I need a key room before the end room, I first check that I have a key room.
If not, I check if I have the conditions fulfilled to add one, and return the key room if it is.\
If a have a key room, I do the same kind of checks with the end room (*do I have one? are the conditions fulfilled to add one?*).\
If the conditions are not fulfilled for either of rooms, then I do my generic rule: adding a random room from a list of simple "corridor" rooms.

The rules I explained as an example in these steps are extremely simple.\
Remember that you can do whatever you want!\
You don't need an end room if you want another goal for your dungeon (e.g. killing all the mobs).\
You don't need the player to spawn in the start room (or even have a specific room as a first room).
You could for example have multiple rooms with a player start actor you choose from to spawn the player.\
You can add any variables you want in your room data to add constraints on your generation rules (e.g. add a room difficulty value so you can choose a hard room after an easy room).

[1]: https://github.com/BenPyton/DungeonExample
