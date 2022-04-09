This function let you choose a door to spawn between 2 rooms.
If you don't want a door between the rooms you just have to return `none`.

One of the room class input can be null if there is no room connected to it. You certainly want to set an always closed door, or just a wall in this case.

For instance:\
[[Images/ChooseDoor.jpg]]

### **Next Step**
The next step is to [[actually generate the dungeon by calling the Generate function|Dungeon Generator]].