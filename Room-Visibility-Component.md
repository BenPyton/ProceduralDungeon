# Room Visibility Component

This actor component allows the actor to be tracked by the dungeon's rooms and so be occluded by them.

You need to set `Occlude Dynamic Actors`in the [[Plugin Settings]] in order to make it works.

What is available from this component:
- `Is Visible` function returns true if the actor is inside a visible room.
- `Set Visibility` allows you to set the visibility mode
	- `Same As Room` will toggle actor's visibility based on `Is Visible` value.
	- `Force Hidden`and `Force Visible` will turn visibility on/off regardless the `Is Visible` value.
	- `Custom` will does nothing itself and lets you manage the actor's visibility. You can then use `Is Visible` and/or your own logic to toggle actor's visibility.
- `Get Visibility`will returns the current visibility mode set by `Set Visibility`.
- `On Room Visibility Changed` is a delegate called when the `Is Visible` value changes. Useful for example when you have a `Custom` visibility and you are changing your actor visibility by events instead of in `Tick`. 



