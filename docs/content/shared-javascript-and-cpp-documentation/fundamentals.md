/**
 *
# Dali Fundamentals  {#fundamentals}

## Actors and the Stage {#actors-and-stage}

A Dali application uses a hierachy of Dali::Actor objects to position visible content.  An actor inherits a position relative to its parent, and can be moved relative to this point.  UI controls can be built by combining multiple actors.
  
To display the contents of an actor, it must be connected to the Dali::Stage.  This provides an invisible root (top-level) actor, to which all other actors are added.  A direct or indirect child of the root actor is considered "on-stage".  Multi-touch events are received through signals emitted by on-stage actors.
  
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

The Stage has a 2D size, which matches the size of the application window.  The default coordinate system in Dali has the origin at the top-left corner, with positive X to right, and position Y going
downwards.  This is intended to be convenient when laying-out 2D views.

![ ](../assets/img/coordinate-system-and-stage.png)
![ ](coordinate-system-and-stage.png)

## Positioning Actors {#positioning-actors}

An actor inherits its parent's position.  The relative position between the actor & parent is determined by 3 properties:
1) ParentOrigin.  This Vector3 property defines a point within the parent actor's area.

![ ](../assets/img/parent-origin.png)
![ ](parent-origin.png)

The default is "top-left", which can be visualized in 2D as (0, 0), but is actually Vector3(0, 0, 0.5) in the 3D Dali world.  The actor's position is relative to this point.

2) AnchorPoint.  This Vector3 property defines a point within the child actor's area.

![ ](../assets/img/anchor-point.png)
![ ](anchor-point.png)

The default is "center", which can be visualized in 2D as (0.5, 0.5), but is actually Vector3(0.5, 0.5, 0.5) in the 3D Dali world.  The actor's position is also relative to this point.

3) Position.  This is the position vector between the parent-origin and anchor-point.

![ ](../assets/img/actor-position.png)
![ ](actor-position.png)

Therefore by default, an actors position is the distance between its center and the top-left corner of its parent.

An actor added directly to the stage with position (X = stageWidth*0.5, Y = stageHeight*0.5), would appear in the center of the screen.  Likewise an actor with position (X = actorWidth*0.5, Y = actorWidth*0.5), would appear at the top-left of the screen.

Note that since Dali is a 3D toolkit, this behaviour is the result of a default perspective camera setup.

@class _Guide_DALi_Fundamentals

*/
