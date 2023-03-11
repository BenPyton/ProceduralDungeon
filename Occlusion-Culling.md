# Occlusion Culling

The occlusion culling system of this plugin consists of showing only the necessary rooms to avoid a lots of mesh drawing and thus improving performances.

**/!\ Important /!\\** Only the visibility of meshes is toggled. The ticking, animations, collisions, etc. are kept! So, the possible performance gain is on the rendering only!

You can toggle this system on or off in the [[Plugin Settings]] alongside some other occlusion settings:
- The `occlusion distance` determine how many rooms are visible from the player's room. 
- `Occlude dynamic actors` allows dynamic actors spawned after the dungeon generation and/or moving between rooms to be occluded with the rooms by the plugin. You'll also have to put the [[Room Visibility Component]] on those actors.