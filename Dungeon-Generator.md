## Create a dungeon generator

The dungeon generator is an actor placed in your main level. It is responsible to generate the dungeon and spawn the room levels.\
To create one, create a new blueprint, expand "All Classes" and type `DungeonGenerator`.

The dungeon generator don't work directly by itself, you have to [[create data and levels for your rooms|Getting Started]] first.

To be able to generate a dungeon, you also have to override some minimal functions:
- [[Choose First Room Data]]
- [[Choose Next Room Data]]
- [[Continue To Add Room]]
- [[Is Valid Dungeon]]
- [[Choose Door]]

There are some other functions you can override:
- **`Pre Generation`**: Called before anything else when the generator is about generating a new dungeon.
- **`Post Generation`**: Called after loading and initializing all rooms of the dungeon. Use it to spawn or teleport players in the first room.
- **`Generation Init`**: Used to reset all your custom generation variables. It is called each time the generation start over when *IsValidDungeon* returns false.
- **`Generation Failed`**: Called when *IsValidDungeon* returned false after too many generation attempts. No dungeon is generated when this is called.
- **`On Room Added`**: Called when a room is successfully added to the dungeon (but not spawned yet). The added room is destroyed when *IsValidDungeon* returns false (you may have to use *GenerationInit* to reset variables changed from this call).
- **`Initialize Dungeon`**: Called after *ContinueToAddRoom* returned false. Allows you to initialize room instances before validating the dungeon with *IsValidDungeon* (useful if you want for example locking some rooms and putting keys in other ones).

## Functions you can call on the Dungeon Generator

**`Generate`**\
Launch the generation of the dungeon (for multiplayer games, it starts the generation when called on the server, and does nothing when called on clients).

**`Get Random Room Data`**\
Returns a room data chosen randomly from the provided array (deterministic with the dungeon generator seed).

**`Get Compatible Room Data`**\
Returns all room data (from the provided list) having at least one door compatible with the provided door type.

**`Get Random Stream`**\
Returns the dungeon random stream (useful to generate deterministically other random stuff).

## Functions you can call from the `Rooms` variable of the dungeon generator

**`Count`**\
Return the current number of room instances during the generation, or total number of room instances after generation.

**`Has Already Room Data`**\
Returns whether the dungeon contains at least one room of the specified room data.

**`Has Already One Room Data From`**\
Returns whether the dungeon contains at least one room of one of the specified room data.

**`Count Room Data`**\
Returns the number of room instances of the specified room data.

**`Count Total Room Data`**\
Returns the total number of room instances of all the specified room data.

**`Has Already Room Type`**\
Returns whether the dungeon contains at least one room of the specified room data ***class***.

**`Has Already One Room Type From`**\
Returns whether the dungeon contains at least one room of one of the specified room data ***classes***.

**`Count Room Type`**\
Returns the number of room instances of the specified room data ***class***.

**`Count Total Room Type`**\
Returns the total number of room instances of all the specified room data ***classes***.

**`Get All Rooms`**\
Returns an array containing all room instances of the generated dungeon.

**`Get All Rooms From Data`**\
Returns an array containing all room instances of a specific room data.

**`Get All Rooms From Data List`**\
Returns an array containing all room instances of any of the provided room data.

**`Get First Room From Data`**\
Returns the first found room instances of a specific room data (no specific order, can be any room in the dungeon).

**`Get All Rooms With Custom Data`**\
Returns an array containing all the room instances having a specific custom room data.

**`Get All Rooms With All Custom Data`**\
Returns an array containing all the rooms instances having all custom data provided. 

**`Get All Rooms With Any Custom Data`**\
Returns an array containing all the rooms instances having at least one of the custom data provided. 

**`Get Random Room`**\
Returns a random room instance from a list of room instances (deterministic with the dungeon generator seed).

**`Has Valid Path`**\
Returns whether a path exists between 2 room instances.

## Generator settings
The DungeonGenerator has some settings:\
[[Images/GeneratorSettings.jpg]]

#### Generation Type
- The `depth first` will prioritize the last added room to continue the generation. 
This will result in a dungeon more linear with shorter dead ends.
- The `breadth first` will prioritize the first added room to continue the generation. This will result in a more spread dungeon.

This is an example using the same very basic generation rules defined in this wiki. We just set a counter of 20 rooms to add the end room. So with depth first we have a lot of rooms to reach the end but dead-ends of only 1 room each. And with the breadth first, we have less rooms until the end, but the dead-ends are the same length of the path to reach the end (some dead-ends are shorter because the algorithm can't place a room after).\
[[Images/DepthBreadth.jpg]]

#### Seed Type
- `Random`: at each generation, the seed is randomly chosen.
- `Auto Increment`: the initial seed is used the first time, then a fixed number will be added at each generation. This will produce different dungeon at each generation, but you will have the same generation sequence each time you play.
- `Fixed`: the initial seed is used each time you generate, so you will always get the same dungeon.

#### Seed
The initial seed you want to use if the `Seed Type` is not `Random`

#### Seed Increment
The number used to increment the seed if the `Seed Type` is `Auto Increment`

#### Use Generator Transform
If checked, the dungeon will be offset and rotated using the dungeon generator actor's transform.\
If unchecked, the first room will be placed at world's (0,0,0) and no rotation will be applied.
