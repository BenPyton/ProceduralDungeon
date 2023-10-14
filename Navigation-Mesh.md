If you are using the default unreal navigation system ([UNavigationSystemV1](https://docs.unrealengine.com/5.0/en-US/basic-navigation-in-unreal-engine/)), then the navigation mesh is automatically rebuild after each dungeon generation.

If you are not, then you'll have to handle it yourself in the `Post Generation` event of the [[dungeon generator|Dungeon Generator]] and you can ignore the rest of this page.

### How to configure the default Unreal's navigation system?

The best way to manage navmesh bounding boxes and obstacle zones is to place them in each room level (not in the master level).

There is also a project setting you **must** change if you want the navmesh to be rebuild properly.\
Go to `File > Project Settings > Navigation Mesh` and set the `Runtime Generation` to `Dynamic`:

[[Images/NavMeshSettings.jpg]]