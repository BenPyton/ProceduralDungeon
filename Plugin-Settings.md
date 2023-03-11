# Plugin Settings

Here you can change some settings for all the plugin.

[[Images/PluginSettings.jpg]]

### General
- **Room Unit**: This is the size of a 1 unit room. Each room have a size made of a multiple of this unit. For instance, if you have a room unit of (1000, 1000, 400) and a room of size (3, 1, 2), this room will have in world unit a size of (3000, 1000, 800).
- **Door Size**: This is the size used by the debug draw to draw the doors. THis is just a visual indication for you, it is not used by the generation algorithm.
- **Door Offset**: You can add an offset on the Z-axis for the doors (eg. if you want to place a ground with a non-zero depth). This offset is a percentage between 0 and 1 of the room unit height.
- **Can Loop**: Enable possibility to link doors when they can.

[[Images/PluginRoomSettings.jpg]]

### Occlusion Culling
- **Enable Occlusion Culling**: This enables the occlusion culling system (see [[Occlusion Culling]] for more details).
- **Occlusion Distance**: Define the distance (in number of connected rooms) of visible rooms from the player's room.
- **Occlude Dynamic Actors**: **[EXPERIMENTAL]** Set it if you want to have dynamic actors tracked in the rooms to show/hide them with the room occlusions (see [[Occlusion Culling]] for more details).

### Debug
- **Draw Debug**: This will display the debug draw of room bounding volume and doors volume in editor and development builds.
- **On Screen Print Debug**: This will prints some debug messages on the player screen in addition to the logs (useful to compare seeds when testing multiplayer).
- **Print Debug Duration**: The duration in seconds of the on screen debug messages.