# Overriding the Dungeon Settings

In some cases, you may want to create multiple dungeons with different room sizes.  
You can do so by overriding the default [plugin settings](../Getting-Started/Plugin-Settings).

To override that, you have to create a new `Dungeon Settings` data asset from the editor.

This data asset can then be set to the `Dungeon Generator`'s `Settings Override` field in the advanced section.

You'll also have to set this data asset to all the `Room Data` assets.

:::info[CAUTION]

The generator can only use `Room Data` with the same `Dungeon Settings` asset override!  
If you try to place a `Room Data` with a different override, the generator will complain!

:::
