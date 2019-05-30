# Network

### Specificities

Since you can have thousand of actor in your dungeon, you cannot send all of them throught the network.\
So the generation is deterministic and is based on a seed which is send to all clients and thus they generate all the same dungeon.

However be careful, all the actors spawned by the generation are not linked throught the network and thus can't be replicated.\
So each of them has to be deterministic too, and they can spawn afterward from the server actors you want to be replicated.

Also, all players must be connected and loaded when you call the Generate function, otherwise, clients which arrives after won't trigger the Generate event.

### Multiplayer in Editor

You cannot play in multiplayer directly from the viewport, you have to play in standalone and not in single process.

To do that, you have to click on the arrow next to the play button and go to "Advance Settings".\
[[Images/AdvanceSettings.jpg]]

Then in Multiplayer Options, set number of player to 2, uncheck the single process box and set the play mode to "Listen Server" or "Client".\
[[Images/MultiOptions.jpg]]

Finally, close the settings window and click again on the arrow next to the Play button and choose "Standalone Game".
[[Images/Standalone.jpg]]