# Managing Randomness

:::note

Randomness is an important part of any procedural generation algorithm.\
You **must not** take easily on it and should think about it seriously from the beginning of your project.\
This page will show you some features provided by the plugin to help you with tat task and gives you some best practices as well.

:::

## Overview

When prototyping your game, you will certainly use the basic `Get Random` nodes provided by the engine.\
But this is quite dangerous, because each time you will launch your game, you'll get different results.
When fixing bugs, this is a real nightmare.\
This is why you should always use **deterministic** randomness in your code.\
Unreal Engine provides the same random nodes as the basic ones but with the suffix `From Stream` at the end.
This means that they take an extra input, a `Random Stream` struct, which holds the current state of a random number generator.\
A random stream generates a new random number based a the previous one, and is initialized with a seed, which is just the first random number used to generate the subsequent ones.
That means the sequence of random numbers generated from the same seed will always be the same.
This is what is called a "deterministic" randomness.

## Best practices to handle randomness

Using one unique `Random Stream` for the whole dungeon may seems obvious if you want only one seed to generate the dungeon.\
However, this is a really bad design, because any small change in your code using some randomness will drastically change your whole dungeon.

For example, let's say you want your lights in your dungeon to have random colors.
When adding the code to choose a *deterministic* random color to your light actors, the random stream will now give a random number to your light instead of what other code that should have taken it after that.
So, after the light actor, the whole number stream slides, giving different numbers to what need random numbers afterward.

To prevent that and make a robust random system that will not break your reproducibility of a dungeon whenever you change your code, you should use multiple random streams, initialized with some deterministic seed.

## How the plugin can help you

When you generate a dungeon, there are many pieces that may need randomness: the generator algorithm, the room custom data, the actors and their components, etc.\
What you want to achieve is to generate the exact same dungeon from only one seed.

The [`Dungeon Generator`](../Dungeon-Generator) will generate a new seed for each generation depending on its settings.\
But how to use it in your actors placed in the room levels?

### The Deterministic Random Component

The plugin provides a [`Deterministic Random`](/api/Classes/DeterministicRandomComponent) component you can add to your actors.\
This component gives you a `Random Stream` initialized with a deterministic seed, computed by combining several sources:

- The dungeon generator's guid
- The dungeon generator's seed
- The owning room's ID
- The guid given through the [`Room Actor Guid`](/api/Classes/RoomActorGuid) interface if implemented (you can just add a [`Simple Guid`](/api/Classes/SimpleGuidComponent) component for that)

This makes the random stream of the actor completely local and independent to each others, but will return different results for each instance of your actor, and will stay deterministic from the dungeon seed as well.\
Also, this is a robust randomness, because when you place or remove new actor instances in your rooms, that will change nothing at all for the other actors.
