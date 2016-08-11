<!--
/**-->

# DALi Fundamentals  {#fundamentals}

## Actors and the Stage {#actors-and-stage}

Actor is the primary object with which DALi applications interact.
A DALi application uses a hierarchy of Dali::Actor objects to position visible content.
An actor inherits a position relative to its parent, and can be moved relative to this point.
UI controls can be built by combining multiple actors.

The Stage is a top-level object used for displaying a tree of Actors.
To display the contents of an actor, it must be added to the Dali::Stage,

The following example shows how to connect a new actor to the stage:

~~~{.cpp}
Actor actor = Actor::New();
Stage::GetCurrent().Add(actor);
~~~

~~~{.js}
var actor = new dali.Actor();
dali.stage.add( actor );
~~~

## The Coordinate System {#coordinate-system}

The Stage has a 2D size, which matches the size of the application window.
The default **unit 1 is 1 pixel with default camera and** the default coordinate system in DALi has the **origin at the top-left corner, with positive X to right, and position Y going
downwards**.  This is intended to be convenient when laying-out 2D views.

![ ](../assets/img/coordinate-system-and-stage.png)
![ ](coordinate-system-and-stage.png)


## Positioning Actors {#positioning-actors}

An actor inherits its parent's position.  The relative position between the actor & parent is determined by 3 properties:
1) ParentOrigin.  This Vector3 property defines a point within the parent actor's area.

![ ](../assets/img/parent-origin.png)
![ ](parent-origin.png)

The default is "top-left", which can be visualized in 2D as (0, 0), but is actually Vector3(0, 0, 0.5) in the 3D DALi world.  The actor's position is relative to this point.

2) AnchorPoint.  This Vector3 property defines a point within the child actor's area.

![ ](../assets/img/anchor-point.png)
![ ](anchor-point.png)

The default is "center", which can be visualized in 2D as (0.5, 0.5), but is actually Vector3(0.5, 0.5, 0.5) in the 3D DALi world.  The actor's position is also relative to this point.

3) Position.  This is the position vector between the parent-origin and anchor-point.

![ ](../assets/img/actor-position.png)
![ ](actor-position.png)

Therefore by default, an actors position is the distance between its center and the top-left corner of its parent.

An actor added directly to the stage with position (X = stageWidth*0.5, Y = stageHeight*0.5), would appear in the center of the screen.  Likewise an actor with position (X = actorWidth*0.5, Y = actorWidth*0.5), would appear at the top-left of the screen.

Note that since DALi is a 3D toolkit, this behaviour is the result of a default perspective camera setup.

## Scene Graph {#scene-graph}

From Wikipedia...
  
A scene graph is a collection of nodes in a graph or tree structure.
A node may have many children but often only a single parent,
with the effect of a parent applied to all its child nodes;
an operation performed on a group automatically propagates
its effect to all of its members. In many programs, associating
a geometrical transformation matrix (see also transformation and matrix)
at each group level and concatenating such matrices together is an
efficient and natural way to process such operations. A common feature,
for instance, is the ability to group related shapes/objects into a
compound object that can then be moved, transformed, selected,
etc. as easily as a single object.

### How does this relate to the DALi public API?

Actors are effectively nodes that receive input (touch events) and act as a
container for draw-able elements (which are also nodes) and other actors.

@class _Guide_DALi_Fundamentals

*/
