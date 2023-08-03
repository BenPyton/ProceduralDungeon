The occlusion culling system of this plugin consists of showing only the necessary rooms to avoid a lots of mesh drawing and thus improving performances.

${\color{orange}\boxed{IMPORTANT}}$ Only the visibility of meshes is toggled. The ticking, animations, collisions, etc. are kept! So, the possible performance gain is on the rendering only!

You can toggle this system on or off in the [[Plugin Settings]] alongside some other occlusion settings:
- The `occlusion distance` determine how many rooms are visible from the player's room. 
- `Occlude dynamic actors` allows dynamic actors spawned after the dungeon generation and/or moving between rooms to be occluded with the rooms by the plugin. You'll also have to put a `Room Visibility Component` on those actors.

## Room Visibility Component

This actor component allows the actor to be tracked by the dungeon's rooms and so be occluded with them.

You need to toggle on `Occlude Dynamic Actors` in the [[Plugin Settings]] in order to make it works.

What is available from this component:
- `Is Visible` function returns true if the actor is inside a visible room.
- `Set Visibility` allows you to set the visibility mode
	- `Same As Room` will toggle actor's visibility based on `Is Visible` value.
	- `Force Hidden`and `Force Visible` will turn visibility on/off regardless the `Is Visible` value.
	- `Custom` will does nothing itself and lets you manage the actor's visibility. You can then use `Is Visible` and/or your own logic to toggle actor's visibility.
- `Get Visibility` will returns the current visibility mode set by `Set Visibility`.
- `On Room Visibility Changed` is a delegate called when the `Is Visible` value changes. Useful for example when you have a `Custom` visibility and you are changing your actor visibility by events instead of in `Tick`.
