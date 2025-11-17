# Room Constraints

Room constraints are useful if you want to add some constraints to the placement of your rooms.

There are 2 built-in constraints in the plugin: the `Location Constraint` and `Direction Constraint`.
But you can create your own constraints by creating child classes of `Room Constraint`.

You set up your constraints in your `Room Data` assets, and then you can use some nodes to check those constraints.

The node `Does Pass All Constraints` will tell you if a specific `Room Data` asset is passing all its constraints in a specific case.

You can use the [`Filter and Sort Rooms`](./Filter-Sort.md) node to filter a room data list, discarding the ones that do not pass the constraints.
