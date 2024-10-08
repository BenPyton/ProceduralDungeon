# The 3Cs of a Game

## What's that thing?

The abbreviation "3Cs" (read as "three seas") stands for "Character, Camera, Controls".\
These are the three foundations of the gameplay of any game (even with no characters).\
You should (and must) have to define and implement them early on before anything else if you want a good game experience for the players.

Some links [here](https://www.reddit.com/r/gamedesign/comments/jn089r/the_3cs_of_game_design_and_how_they_affect_the/) and [here](https://www.pluralsight.com/blog/film-games/character-controls-camera-3cs-game-development).

## The Character

I will not speak about the art part (character design, color palette, etc.) or writing part (personality, background story, etc.) of it because it is not relevant to the use of the plugin, even though those parts fall into this category.

However, the gameplay of the character must be discussed and defined here, and preferably implemented before using the plugin.

Here are some important gameplay designs to define for the plugin:

- The character's locomotion: the speed of movement, if it can move on 2D or 3D, if it's a grid base movement or a free movement.
- The character's movement abilities: Can it sprint? Can it jump? (if yes, how high/far); Can it dash? (if yes how far/quick); Can it fly? Can it teleport? Can it swim? Can it use a grapple? etc.
- The character's actions: Can it attack? if yes how? (melee with swords etc., ranged with bows or guns, etc.) Can it interacts with other things (like doors, buttons, levers, containers, etc.)?

## The Camera

This is simply the point of view, but it's really a crucial part to define before using the plugin:

- You can go in first person view if you want a very immersive experience for the player.
- You can also use a third person view and in that case you must define a more specific view: top-down (like in old Zelda games), shoulder (like most mainstream games nowadays), fixed (like the first Resident Evil game), etc. You must also define the abilities of the camera, for example you can have a top-down camera the player could rotate or not.

## The Controls

This part is not relevant for the use of the plugin, but is really important for the player's experience.

This is just assigning buttons to the actions the player can perform (moving camera, moving character, jumping, attacking, interacting, etc.).\
It is strongly recommended to think about the different input devices (keyboard/mouse, gamepad, joystick, etc.).

Nowadays all players are used to some basic inputs for common gameplay (e.g. left stick/WASD to move, right stick/mouse for camera, button A/O or key E/F to interact, etc.) so it is not really advised to set less common inputs for basic actions like these.\
However, if you have an atypic gameplay (for example, no character at all, or an UI-only game) then you will have to assign controls so that it's easy to play your game.
