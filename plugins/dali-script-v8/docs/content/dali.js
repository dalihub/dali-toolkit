/**
 *

## DALi 3D ( Dynamic Animation Library )

DALi is a quick and easy way of allowing developers to create Rich UI Applications like:

 + Image & Video galleries
 + Music players
 + Games
 + Maps
 + Homescreens / launch pads
 + Advanced watch faces for wearable devices
  

DALi is based on OpenGL ES 2.0 & 3.0, however it hides the complexity of
the OpenGL API from developers and provides a clean cross-platform JavaScript framework.

+ Create Images, Text and Meshes
+ Create shaders using GLSL
+ Provide multiple cameras and render targets
+ Provides Layers to aid in 2D UI layout
+ Easy to use Animation framework
+ Automatic background loading of resources ( images / text / meshes )
+ Runs all animations in a seperate thread. This helps maintain 60 FPS even if JavaScript is performing a long operation ( e.g. Garbage Collection ).
+ Provides keyboard / touch / mouse handling
  
![Screen shots](../assets/img/screen-shot.png)

## Running JavaScript from DALi C++ API
```
mScript = Dali::Toolkit::Script::New();
  
mScript.ExecuteFile( mScriptFileName);
```

## Actors and the Stage

A DALi application uses a hierachy of Dali::Actor objects to position visible content.  An actor inherits a position relative to its parent, and can be moved relative to this point.  UI controls can be built by combining multiple actors.

To display the contents of an actor, it must be connected to the Dali::Stage.  This provides an invisible root (top-level) actor, to which all other actors are added.  A direct or indirect child of the root actor is considered "onStage".  Multi-touch events are received through signals emitted by on-stage actors.

The following example shows how to connect a new actor to the stage:
```
	var actor = new dali.actor();
  
	dali.stage.add(actor);
```
## The Coordinate System

The Stage has a 2D size, which matches the size of the application window.  The default coordinate system in DALi has the origin at the top-left corner, with positive X to right, and position Y going
downwards.  This is intended to be convenient when laying-out 2D views.

![Screen shots](../assets/img/coordinate-system-and-stage.png)


 * @module DALi
 * @main  DALi
 */
