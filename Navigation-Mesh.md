
The best way to manage navmesh bounding boxes and obstacle zones is to place them in each room level (not in the master level).

There is also a project setting you **must** change if you want to the navmesh to be rebuild after the generation of the dungeon.\
Go to `File > Project Settings > Navigation Mesh` and set the `Runtime Generation` to `Dynamic`:

[[Images/NavMeshSettings.jpg]]