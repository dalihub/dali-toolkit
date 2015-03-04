/**
 *
 ## Actor API

  Actor is the primary object with which Dali applications interact. UI controls can be built by combining multiple actors.

  There are different types of Actors supported by Dali. They all have the same
  base functionality of the actor class.

```
var actor = new dali.Actor();
var imageActor = new dali.ImageActor();
var textActor = new dali.TextActor("hello world");
var meshActor = new dali.MeshActor();
var camera = new dali.CameraActor();
var layer = new dali.Layer();
```

### Hello world example </h3>
```
var myActor = new dali.TextActor("hello-world");

myActor.name = "my first actor";
myActor.color = [ 1, 0, 0, 1];    // Red,Green,Blue, Alpha ( 1 == max, 0 = none )
myActor.scale = [ 2, 2, 1];      // double the width and height

// by default an actor is anchored to the top-left of it's parent actor
// change it to the middle

myActor.parentOrigin = [0.5,0.5,0.5];

// add to the stage
dali.stage.add( myActor );
```


### Positioning Actors

An actor inherits its parent's position.  The relative position between the actor & parent is determined by 3 properties:

1) ParentOrigin.  This Vector3 property defines a point within the parent actor's area.

<img src="../assets/img/shared/parent-origin.png">

The default is "top-left", which can be visualized in 2D as (0, 0), but is actually Vector3(0, 0, 0.5) in the 3D DALi world.  The actor's position is relative to this point.
```
// to change parent origin to the centre
myActor.parentOrigin = [0.5, 0.5, 0.5];
```

2) AnchorPoint.  This Vector3 property defines a point within the child actor's area.

<img src="../assets/img/shared/anchor-point.png">

The default is "center", which can be visualized in 2D as (0.5, 0.5), but is actually Vector3(0.5, 0.5, 0.5) in the 3D DALi world.  The actor's position is also relative to this point.
```
// setting anchor point to the centre
myActor.anchorPoint = [0.5, 0.5, 0.5];
```

3) Position.  This is the position vector between the parent-origin and anchor-point.

<img src="../assets/img/shared/actor-position.png">

Therefore by default, an actors position is the distance between its center and the top-left corner of its parent.

An actor added directly to the stage with position (X = stageWidth*0.5, Y = stageHeight*0.5), would appear in the center of the screen.  Likewise an actor with position (X = actorWidth*0.5, Y = actorWidth*0.5), would appear at the top-left of the screen.

Note that since DALi is a 3D toolkit, this behaviour is the result of a default perspective camera setup.

### Actor callback events

The actor provides the following call back events

| Name              | Description                            | Parameters passed to call back |
|-------------------|----------------------------------------|--------------------------|
|touched            | touch event                            | (actor, touchEvent )     |
|hovered            | mouse or pointer hovering over actor   | (actor, hoverEvent)      |
|mouse-wheel-event  | mouse wheel events                     | (actor, wheelEvent)      |
|on-stage           | actor has been moved on stage          | (actor)                  |
|off-stage          | actor has been moved off stage         | (actor)                  |


#### Touch event

Used to detect multiple touch events on the actor. The state of each touch point can be:
+ "down"        = touch down
+ "up"          = Touch up
+ "motion"      = Finger dragged or hovered
+ "leave"       =  Leave the boundary of an actor
+ "stationary"  = No change from last event.  Useful when a multi-point event occurs where
all points are sent but indicates that this particular point has not changed since the last time
+ "interrupted"  = A system event has occurred which has interrupted the touch or hover event sequence



```
touchEvent = {
  
  pointCount: int,  // number of points touched ( multi-touch )
  time: int,        // The time in milliseconds that the touch event occurred.
  points = [ touchPoints ],    // array of TouchPoints, to support
  
  TouchPoint = {
  
    "deviceId" : int,      // Each touch point has a unique device ID
    "state" : string,      // touch state ="down,up,motion,leave,stationary, interrupted }
    "sourceActor" : actor, // the actor that is emitting the callback (the actor that is hit maybe a child of it)
    "hitActor" : actor,    // actor that was hit
    "local" :  {x,y},      // co-ordinates of top left of hit actor (local.x, local.y)
    "screen" : {x,y}       // co-ordinates of top left of hit actor (screen.x, screen.y)
    }
}

function OnPressed( actor, touchEvent )
{
  var firstPoint = touchEvent.points[0];
  log("first touch point = " + firstPoint.screen.x + "," +firstPoint.screen.x + "actor= "+firstPoint.hitActor );
  
  var anim = new dali.Animation( 4 );
  var rotation = new dali.Rotation( 90, 0, 0 ); // pitch, yaw, roll
  anim.animateBy( actor, "rotation", rotation );
  anim.play();
  return true;
}
  
// connect to touch events
myActor.connect( "touched", onPressed );

```

#### Hover event

```
hoverEvent = {
  
  pointCount  // number of points hovered over
  time        // The time in milliseconds that the hover event occurred.
  points[]    // array of TouchPoints
  
  TouchPoint = {
      // See touchEvent TouchPoint object
  }
}
```
      // connect to touch events
      myActor.connect( "hovered", onHover);

#### Mouse wheel event

```
mouseWheelEvent = {
  
  direction,       // "vertical" or "horizontal" direction the wheel is being rolled
  shiftPressed,    // boolean, shift key is held
  ctrlPressed,     // boolean, ctrl key is held
  altPressed,      // boolean, alt key is held
  keyModifiers,    // bitmask of keys pressed
  point {x,y},     // The co-ordinates of the mouse cursor relative to the top-left of the screen when the wheel is being rolled.
  rolled,          // offset of mouse wheel rolling, positive = rolling down, negative = rolling up
  timestamp        // The time in milliseconds that the mouse event occurred
}
  
// connect to touch events
myActor.connect( "mouse-wheel-event", onMouseWheel );
```
#### Key events

Key events are performed using the dali.stage object and dali.keyboardFocusManager.
 - {{#crossLink "stage"}}Stage{{/crossLink}}


#### Multi-touch events

See
 - {{#crossLink "MultiTouch"}}Multi Touch Events.{{/crossLink}}


### Actor Properties

 Name                   |    Type    | Writable     | Animatable
------------------------|------------|--------------|-----------
 anchorPoint            |VECTOR3     | &#10004;     | &#10008;
 anchorPointX           |FLOAT       | &#10004;     | &#10008;
 anchorPointY           |FLOAT       | &#10004;     | &#10008;
 anchorPointZ           |FLOAT       | &#10004;     | &#10008;
 size                   |VECTOR3     | &#10004;     | &#10004;
 sizeWidth              |FLOAT       | &#10004;     | &#10004;
 sizeHeight             |FLOAT       | &#10004;     | &#10004;
 sizeDepth              |FLOAT       | &#10004;     | &#10004;
 position               |VECTOR3     | &#10004;     | &#10004;
 positionX              |FLOAT       | &#10004;     | &#10004;
 positionY              |FLOAT       | &#10004;     | &#10004;
 positionZ              |FLOAT       | &#10004;     | &#10004;
 worldPosition          |VECTOR3     | &#10008;     | &#10008;
 worldPositionX         |FLOAT       | &#10008;     | &#10008;
 worldPositionY         |FLOAT       | &#10008;     | &#10008;
 worldPositionZ         |FLOAT       | &#10008;     | &#10008;
 rotation               |ROTATION    | &#10004;     | &#10004;
 worldRotation          |ROTATION    | &#10008;     | &#10008;
 scale                  |VECTOR3     | &#10004;     | &#10004;
 scaleX                 |FLOAT       | &#10004;     | &#10004;
 scaleY                 |FLOAT       | &#10004;     | &#10004;
 scaleZ                 |FLOAT       | &#10004;     | &#10004;
 worldScale             |VECTOR3     | &#10008;     | &#10008;
 visible                |BOOLEAN     | &#10004;     | &#10004;
 color                  |VECTOR4     | &#10004;     | &#10004;
 colorRed               |FLOAT       | &#10004;     | &#10004;
 colorGreen             |FLOAT       | &#10004;     | &#10004;
 colorBlue              |FLOAT       | &#10004;     | &#10004;
 colorAlpha             |FLOAT       | &#10004;     | &#10004;
 worldColor             |VECTOR4     | &#10008;     | &#10008;
 worldMatrix            |MATRIX      | &#10008;     | &#10008;
 name                   |STRING      | &#10004;     | &#10008;
 sensitive              |BOOLEAN     | &#10004;     | &#10008;
 leaveRequired          |BOOLEAN     | &#10004;     | &#10008;
 inheritRotation        |BOOLEAN     | &#10004;     | &#10008;
 inheritScale           |BOOLEAN     | &#10004;     | &#10008;
 colorMode              |NUMBER      | &#10004;     | &#10008;
 positionInheritance    |NUMBER      | &#10004;     | &#10008;
 drawMode               |NUMBER      | &#10004;     | &#10008;
 sizeMode               |NUMBER      | &#10004;     | &#10008;
 sizeModeFactor         |VECTOR3     | &#10004;     | &#10008;




 * @class Actor
 */


/**
 * Actors parent origin
 *
 * @property parentOrigin
 * @type dali Vector3
 * @default TOP_LEFT (0.0, 0.0, 0.5).
 */
parentOrigin

/**
 * Actors parent origin X
 *
 * @property parentOriginX
 * @readOnly
 * @type Number
 */
parent - origin - x

/**
 * Actors parent origin-y
 * @property parentOriginY
 * @readOnly
 * @type Number
 */
parent - origin - y

/**
 * Actors parent origin-z
 * @property parentOriginZ
 * @readOnly
 * @type Number
 */
parent - origin - z

/**
 * Actors anchor point
 * @property anchorPoint
 * @type dali Vector3
 * @default CENTER (0.5, 0.5, 0.5)
 */
ANCHOR_POINT;

/**
 * Actors anchor point x
 * @property anchorPointX
 * @type Number
 */
ANCHOR_POINT_X

/**
 * Actors anchor point y
 * @property anchorPointY
 * @type Number
 */
ANCHOR_POINT_Y

/**
 * Actors anchor point z
 * @property anchorPointZ
 * @type Number
 */
ANCHOR_POINT_Z

/**
 * Actors size
 * @property size
 * @type dali Vector3
 */
SIZE


/**
 * Actors width
 * @property sizeWidth
 * @type Number
 */
SIZE_WIDTH

/**
 * Actors height
 * @property sizeHeight
 * @type Number
 */
SIZE_HEIGHT

/**
 * Actors depth
 * @property sizeDepth
 * @type Number
 */
SIZE_DEPTH


/**
 * Actors position
 * @property position
 * @type dali Vector3
 */
POSITION

/**
 * Actors x position
 * @property positionX
 * @type Number
 */
POSITION_X

/**
 * Actors y position
 * @property positionY
 * @type Number
 */
POSITION_Y

/**
 * Actors z position
 * @property positionZ
 * @type Number
 */
POSITION_Z


/**
 * Actors world position
 * @property position
 * @type dali Vector3  ( read-only, not animatable )
 */
WORLD_POSITION

/**
 * Actors world x position
 * @property worldPositionX
 * @type Number ( read-only )
 */
WORLD_POSITION_X

/**
 * Actors world y position
 * @property worldPositionY
 * @type Number ( read-only )
 */
WORLD_POSITION_Y

/**
 * Actors world z position
 * @property worldPositionZ
 * @type Number ( read-only )
 */
WORLD_POSITION_Z


/**
 * Actors rotation
 * @property rotation
 * @type dali Rotation object
 */
ROTATION


/**
 * Actors world-rotation
 * @property worldRotation
 * @type dali Rotation object ( read only)
 */
WORLD_ROTATION

/**
 * Actors scale
 * @property scale
 * @type dali Vector3
 */
SCALE

/**
 * Actors x scale
 * @property scaleX
 * @type Number
 */
SCALE_X

/**
 * Actors y scale
 * @property scaleY
 * @type Number
 */
SCALE_Y

/**
 * Actors z scale
 * @property scaleZ
 * @type Number
 */
SCALE_Z

/**
 * Actors world scale
 * @property worldScale
 * @type dali Vector3 ( read only )
 */
WORLD_SCALE

/**
 * Actors visible flag
 * If an actor is not visible, then the actor and its children will not be rendered.
 * This is regardless of the individual visibility values of the children i.e. an actor will only be
 * rendered if all of its parents have visibility set to true.
 *
 * @property visible
 * @type Boolean
 */
VISIBLE

/**
 * Actors color.
 * The final color of the actor depends on its color mode.
 * 4 components, red, green, blue and alpha. Each range from 0..1
 * @property color
 * @type dali Vector 4
 */
COLOR

/**
 * Actors red color
 * @property colorRed
 * @type Number  ( 0..1)
 */
COLOR_RED

/**
 * Actors green color
 * @property colorGreen
 * @type Number  ( 0..1)
 */
COLOR_GREEN

/**
 * Actors blue color
 * @property colorBlue
 * @type Number  ( 0..1)
 */
COLOR_BLUE

/**
 * Actors world color.
 * 4 components, red, green, blue and alpha. Each range from 0..1
 * @property worldColor
 * @type dali Vector 4 ( read only)
 */
WORLD_COLOR

/**
 * Actors name
 * @property name
 * @type String
 */

/**
 * Actors sensitive flag
 * brief Sets whether an actor should emit touch event signals; @see SignalTouched().
 *
 * An actor is sensitive by default, which means that as soon as an application connects to the SignalTouched(),
 * the touch event signal will be emitted.
 *
 * If the application wishes to temporarily disable the touch event signal emission, then they can do so by calling
 *
 *    actor.sensitve = false;
 *
 * Then, to re-enable the touch event signal emission, the application should call:
 *
 *    actor.sensitive = true;
 *
 * @property sensitive
 * @type Boolean
 * @default true ( is sensistive )
 */
SENSITIVE

/**
 * Controls whether the actor should receive a notification when touch motion events leave
 * the boundary of the actor.
 *
 * Note: Need to connect to the SignalTouch to actually receive this event.
 *  Should be set to true if a Leave event is required
 * @type Boolean
 * @property leaveRequired
 * @default false, this is set to false as most actors do not require this.
 */
LEAVE_REQUIRED

/**
 * Set whether a child actor inherits it's parent's orientation.
 * @type Boolean
 * @property inheritRotation
 * @default true
 */
INHERIT_ROTATION,


/**
 * Set whether a child actor inherits it's parent's scale.
 * @type Boolean
 * @property inheritScale
 * @default true
 */
INHERIT_SCALE,


/**
 * Set how the actor and its children should be drawn.
 *
 * Not all actors are renderable, but DrawMode can be inherited from any actor.
 * By default a renderable actor will be drawn as a 3D object. It will be depth-tested against
 * other objects in the world i.e. it may be obscured if other objects are in front.
 *
 * If OVERLAY is used, the actor and its children will be drawn as a 2D overlay.
 * Overlay actors are drawn in a separate pass, after all non-overlay actors within the Layer.
 * For overlay actors, the drawing order is determined by the hierachy (depth-first search order),
 * and depth-testing will not be used.
 *
 * If STENCIL is used, the actor and its children will be used to stencil-test other actors
 * within the Layer. Stencil actors are therefore drawn into the stencil buffer before any other
 * actors within the Layer.
 *
 * @example
 *
 *      var actor.drawMode = dali.DRAW_MODE_NORMAL;  // binary 00. The default draw-mode
 *      var actor.drawMode = dali.DRAW_MODE_OVERLAY; // binary 01. Draw the actor and its children as an overlay
 *      var actor.drawMode = dali.DRAW_MODE_STENCIL ;// binary 11. Draw the actor and its children into the stencil buffer
 *
 *
 * @type Number
 * @property drawMode
 * @default 0 (Normal )
 */
DRAW_MODE,


/**
 * Sets the actor's color mode.
 *
 * This specifies whether the Actor uses its own color, or inherits
 * its parent color. The default is USE_OWN_MULTIPLY_PARENT_ALPHA.
 *
 * @example
 *    actor.colorMode = dali.COLOR_MODE_USE_OWN_COLOR; // Actor will use its own color
 *    actor.colorMode = dali.COLOR_MODE_USE_PARENT_COLOR;  // Actor will use its parent color
 *    actor.colorMode = dali. COLOR_MODE_USE_OWN_MULTIPLY_PARENT_COLOR; // Actor will blend its color with its parents color.
 *    actor.colorMode = dali.COLOR_MODE_USE_OWN_MULTIPLY_PARENT_ALPHA ;  // Actor will blend its alpha with its parents alpha. This means when parent fades in or out child does as well. This is the default.
 *
 *
 * @type Number
 * @property colorMode
 * @default 2 (USE_OWN_MULTIPLY_PARENT_ALPHA )
 */
COLOR_MODE

/**
 * Set the actors position inheritance mode.
 *
 * @example
 *    actor.positionInheritance = dali.POSITION_INHERITANCE_INHERIT_PARENT_POSITION;  // Actor will inherit its parent position. This is the default
 *    actor.positionInheritance = dali.POSITION_INHERITANCE_USE_PARENT_POSITION;      // Actor will copy its parent position. This is useful if many actors are stacked together in the same place. This option ignores parent origin and anchor point.
 *    actor.positionInheritance = dali.POSITION_INHERITANCE_USE_PARENT_POSITION_PLUS_LOCAL_POSITION; // Actor will copy its parent position and add local position. This is useful if many actors are stacked together in the same place with an offset.  This option ignores parent origin and anchor point.
 *    actor.positionInheritance = dali.POSITION_INHERITANCE_DONT_INHERIT_POSITION;           // Actor will not inherit position. Local position is treated as world position. This is useful if a constraint is used to override local position or if an actor is positioned globally. This option ignores parent origin, anchor point and local position.
 *
 * Switching this off means that using SetPosition() sets the actor's world position.
 * @type Number
 * @property positionInheritance
 * @default 0 (INHERIT_PARENT_POSITION )
 */
POSTITION_INHERITANCE


/**
 *  Defines how a child actor's size is affected by its parent's size.
 *
 * The default is to ignore the parent's size and use the size property of this actor.
 *
 * If USE_OWN_SIZE is used, this option is bypassed and the actor's size
 *     property is used.
 *
 * If SIZE_EQUAL_TO_PARENT is used, this actor's size will be equal to that
 *     of its parent. The actor's size property is ignored.
 *
 * If SIZE_RELATIVE_TO_PARENT is used, this actor's size will be based on
 *     its parent's size by multiplying the parent size by
 *     SizeModeFactor.
 *
 * If SIZE_FIXED_OFFSET_FROM_PARENT is used, this actor's size will be based on
 *     its parent's size plus SizeModeFactor.
 *
 *
 * @example
 *    actor.sizeMode = dali.USE_OWN_SIZE;
 *    actor.sizeMode = dali.SIZE_EQUAL_TO_PARENT;
 *    actor.sizeMode = dali.SIZE_RELATIVE_TO_PARENT;
 *    actor.sizeMode = dali.SIZE_FIXED_OFFSET_FROM_PARENT
 *
 * @type Number
 * @property sizeMode
 * @default 0 (dali.SIZE_MODE_USE_OWN_SIZE; )
 */
 SIZE_MODE

/**
 *
 * @brief Sets the relative to parent size factor of the actor.
 *
 * This factor is only used when SizeMode is set to either:
 * SIZE_RELATIVE_TO_PARENT or SIZE_FIXED_OFFSET_FROM_PARENT.
 * This actor's size is set to the actor's parent size multipled by or added to this factor,
 * depending on SideMode (See SetSizeMode).
 * @type Vector3
 * @property sizeModeFactor
 */
SIZE_MODE_FACTOR