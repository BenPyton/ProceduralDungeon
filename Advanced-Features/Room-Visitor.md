---
slug: /guides/Room-Visitor
---

# Room Visitor

The `Room Visitor` is an [interface](https://dev.epicgames.com/community/learning/tutorials/bLXe/interfaces-bp-c) used to add behaviors to your actors or components when they enter or exit a room.

For example, you can use this interface to keep track of the rooms the actor is in to be able to call functions on them, change some [Room Custom Data](Room-Custom-Data.md) variables, display various details on player's HUD, etc.

The [Room Visibility Component](Occlusion-Culling.md) (used to show/hide an actor with the rooms it is in) is implementing the `Room Visitor` interface to bind/unbind to the room's visibility event dispatcher when the actor enters or exits any room, and thus can change its visibility according to the room visibility state.

## How to use it

Just add the interface like any other interface in Blueprint or C++, and implement its functions.

For example, this is a simple implementation in Blueprint to keep a reference of the rooms an actor is in (stored in an array called `Cached Room Array`).

![](../Images/RoomVisitor.png)

## Differences with [`Room Observer`](Room-Observer.md) component

As the name implies, the `Room Visitor` interface is used to create game logic when the actor "**visits**" a new room.

The implemented events are called only when the implementing actor enters/exits a room, **not** when other actors enter/exit rooms.

On the other hand, the `Room Observer` dispatch its events when **other** actors enter/leave the room the observer is in.

## Differences with [`Room Visibility`](Occlusion-Culling.md) component

The `Room Visibility` component **implements** the `Room Visitor` interface, so it can bind/unbind its internal functions to the visibility event of the rooms the actor enters/exits.

This is a simple example of what you can do with this interface.
