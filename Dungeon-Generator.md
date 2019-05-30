# Dungeon Generator

### Create a dungeon generator

The dungeon generator is the actor which generate the dungeon.\
To create one, create a new blueprint, expand "All Classes" and type "DungeonGenerator".

The dungeon generator don't work directly by itself, you have to [[create rooms data and rooms levels first|Room Architecture]].

To be able to generate a dungeon, you also have to override some minimal functions:
- [[Choose First Room Data]]
- [[Choose Next Room Data]]
- [[Is Valid Dungeon]]
- [[Continue Adding Room]]

There are some other functions you can override:
- **Generation Init**: Called to reset all generation data
- **On Room Added**: Called when a room is successfully added to the dungeon
- **Pre Generation**: Called after unloading previous dungeon but before generating new dungeon
- **Post Generation**: Called after loading and initializing all rooms of the dungeon

### Methods you can use

**Generate**\
Launch the generation on all clients when called by the server, do nothing when called by clients.

**Has Already Room Data**\
This function take a RoomData Class Reference and return true if the dungeon contain a room of that class.

**Get Random Room Data**\
Take an array of RoomData Class Reference and return deterministically a random RoomData in it.

**Get Nb Room**\
Return the current number of room during the generation, or total number of room after generation.

### Tips
The dungeon generator use RoomData Class References.\
To create variables of this type, in the Variable Type field search "RoomData", hever it and click on "Class Reference".
[Insert image here]
