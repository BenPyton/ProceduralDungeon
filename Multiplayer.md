First off, be sure to have minimum knowledge of how multiplayer games work in Unreal Engine.\
I would suggest you to read this [compendium](https://cedric-neukirchen.net/Downloads/Compendium/UE4_Network_Compendium_by_Cedric_eXi_Neukirchen.pdf) if not (it was written for UE4 but it's the same for UE5).

### Overview

Your dungeons can have lots of actors, thousands of actors.\
So, to avoid sending all those actors over the network, the dungeon generation has been made deterministic.\
A seed is generated on server-side, and then sent to all clients, thus they can generate the exact same dungeon.

However, there are some caveats with that, and the main one is that the actors placed in the room levels are not replicated over the network.

Also, all players must be connected and loaded when you call the `Generate` function of the `DungeonGenerator`.
Otherwise, clients which arrives after won't trigger the `Generate` event.\
*(this will eventually be fixed in a later release of the plugin)*

### How to do replication with this plugin

- **Doors** are the only replicated actors spawned by the dungeon generation.\
However, you still need to do some logic correctly on your project in order to make it work properly.\
See the multiplayer section of the [[door wiki page|Door]] for details on it.

- Actors directly placed in room levels are not replicated. If you want to have some actors (like enemies, chests, NPCs, etc.) and want them replicated, you need to make some workarounds.\
My suggestion: 
	- In your actor you want replicated, check the `Replicates` field.
	- Create and place a spawner actor in your room level. This spawner is not replicated.
	- In the `BeginPlay` of the spawner, check if it's server-side (with the `HasAuthority` function) before spawning the real actor you want.
	- Spawn the actor if previous point is true. The spawned actor will spawns on all clients too and will be replicated properly.

[[Images/MultiSpawner.jpg]]

### Multiplayer in Editor

You cannot play in multiplayer directly from the editor viewport.\
You have to change some settings in the editor in order to test your game in multiplayer.

Here is how to setup the editor.\
*(Some options could be in another place on newer version, but they are all here)*

- First, you have to click on the arrow next to the play button and go to `Advance Settings`.\
[[Images/AdvanceSettings.jpg]]

- Then in `Multiplayer Options`, set `Number of Players` to 2, uncheck the `Use Single Process` box and set the `Editor Multiplyer Mode` to `Listen Server` or `Client`.\
[[Images/MultiOptions.jpg]]

- Finally, close the settings window and click again on the arrow next to the Play button and choose `Standalone Game`.\
[[Images/Standalone.jpg]]