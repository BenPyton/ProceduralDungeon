# Console Commands

The plugin adds several console commands to control some plugin's settings and add some debug functions.

## Console Variables

All console variables begins with `pd` (for ***p***rocedural ***d***ungeon), then continue with categories (e.g. `Occlusion` or `Debug`), then sub-categories, etc. and finally the variable.

Here a list of *all* console variables added by the plugin:

Console Variable | Arguments | Description
---|---|---
`pd.Occlusion` | boolean | Enable/disable the plugin's [room culling](../Advanced-Features/Occlusion-Culling.md) system.
`pd.Occlusion.Distance` | integer | Change the number of visible rooms from the player's room (`1` means only the player's room).
`pd.Occlusion.DynamicActors` | boolean | Enable/disable the occlusion of actors with a `Room Visibility` component attached on them.
`pd.Debug.Draw` | boolean | Enable/disable the debug drawings of the rooms and doors.
`pd.Debug.Log.OnScreen` | boolean | Enable/disable the on-screen logging of the plugin.
`pd.Debug.Log.Duration` | float | Change the on-screen logging duration (in seconds) of the plugin.

<!-- markdownlint-disable MD024 -->
## Console Commands
<!-- markdownlint-enable MD024 -->

Here a list of *all* console commands added by the plugin:

Console Command | Arguments | Description
---|---|---
`pd.Generate` | string&#160;(optional) | Call `Generate` on [Dungeon Generator](../Getting-Started/Generating-Dungeon/Dungeon-Generator.md) actors with the name or tag provided, or all if nothing provided.
`pd.Unload` | string&#160;(optional) | Call `Unload` on [Dungeon Generator](../Getting-Started/Generating-Dungeon/Dungeon-Generator.md) actors with the name or tag provided, or all if nothing provided.
