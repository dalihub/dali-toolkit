<!--
/**-->
# Layer ( Layer inherits from Actor) {#layer}

 Layers provide a mechanism for overlaying groups of actors on top of each other.
 Layers can also clip their contents to exclude any content outside a user defined area.
  
 ![ ](layers.png)
  
 When a layer is added to the stage it is assigned a unique depth value. By default the stage has a root layer with a depth value of 0.
  
 Layers are actors and inherit position, orientation and scale of their parent actor.
 They are drawn in an order determined by a layer depth value.
 The depth buffer is cleared before each layer is rendered, unless depth
 test is disabled or there's no need for it based on the layers contents.

**Note: Layers work independently of the actor hierarchy.**
They can be positioned anywhere in the actor tree, but their draw order is always defined by their layer.getDepth() value.
  
  
~~~{.cpp}
// C++ example of adding an actor to the root layer

//  using stage.add() will automatically add actor to the root layer
Stage stage = Stage::GetCurrent();
stage.add( myActor );

// Or you can explicitly add actor to the root layer.
Layer rootLayer = stage.GetRootLayer();
rootLayer.add( myActor );  // adds an actor to the root layer

// rootLayer.getDepth() == 0

~~~

Example To create two new layers on top of the root layer.
  

## Layer clipping

Clips the contents of the layer to a rectangle.

~~~{.cpp}
// C++

layer1.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER );
layer1.SetParentOrigin( ParentOrigin::CENTER );
layer1.SetClipping( true );
layer1.SetClippingBox( 20, 20, 100, 100 ); // X, Y, Width, Height

~~~

## Re-ordering layers

The following functions can be used to change the draw order of the layers.


 - Raise() Raise the layer up by 1
 - Lower() Lower the layer down by 1
 - RaiseAbove( layer ) Ensures the layers depth is greater than the target layer
 - LowerBelow( layer ) Ensures the layers depth is less than the target layer
 - RaiseToTop() raise the layer to the top
 - LowerToBottom() lower the layer to the bottom
 - MoveAbove( layer ) Moves the layer directly above the given layer.
 - MoveBelow( layer ) Moves the layer directly below the given layer.

Note:
 - The root layer can be moved above and below other layers. However, stage.add( actor ), will always use the root layer object.

## Rendering order of actors inside of a layer

Layers have two behaviour modes:

 - LAYER_2D ( Default )
 - LAYER_3D

## Layer_2D

~~~{.cpp}
// C++
layer.SetBehavior( Layer::LAYER_2D );
~~~

### Background

 - Graphics are drawn in DALi using renderers
 - Actors can have zero or many renderers
 - Renderers can be shared by actors
 - Renderers have a depth index property
 - In LAYER_2D mode, draw order of a renderer within a layer = Tree depth + renderer depth index
  
 When using  Layer_2D mode depth testing is disabled (depth buffer not used).
  
  With LAYER_2D, the draw order of the renderers is defined by both:

 - Renderer depth index.
 - Position of actor in the actor tree
  

Example:
  
We have two layers below. Everything in the root layer is drawn first.
If we did dali.stage.getRootLayer().raiseToTop(), then the root layer would be drawn last.

  
![ ](layer2d.png)
  

The formula for calculating the draw order of a renderer is:  depthIndex + ( TREE_DEPTH_MULTIPLIER * tree depth ).
Currently Layer::TREE_DEPTH_MULTIPLIER == 1000:
~~~
 Root (root layer) ( depth index offset of  0)
 +->  Actor1    ( depth index offset of 1000)
 ++-> Actor2    ( depth Index offset of 2000)
 +++-> Actor3   ( depth Index offset of 3000)
 +++-> Actor4   ( depth Index offset of 3000)
 +++-> Actor5   ( depth Index offset of 3000)
 +++-> Layer1   ( depth Index has no meaning for layers, layer draw order is independent of the hierarchy).
 ++++-> Actor6   ( depth Index offset of 4000)
 ++++-> Actor7   ( depth Index offset of 4000)
 ++++-> Actor8   ( depth Index offset of 4000)
~~~
  
Renderers with higher depth indices are rendered in front of renderers with smaller values.
  
Everything in the root layer gets rendered first, actors 1..5
Then layer 1, actors 6..8
  
If we want to determine draw order of actors 6..8, we set the depthIndex on their renderers.
For example if we want the render draw order to be 8, 7, 6, with 6 being drawn last.

~~~{.js}
  var rendererForActor6 = new dali.Renderer( geometry, material );
  var rendererForActor7 = new dali.Renderer( geometry, material );
  var rendererForActor8 = new dali.Renderer( geometry, material );

  rendererForActor6.depthIndex = 2;   // drawn on top ( last)
  rendererForActor7.depthIndex = 1;   // draw in the middle
  rendererForActor8.depthIndex = 0;   // drawn on bottom ( first)

  daliactor6.addRenderer( rendererForActor6 );  // renderer 6 drawn with index of 2 + 4000 = 4002
  daliactor7.addRenderer( rendererForActor7 );  // renderer 7 drawn with index of 1 + 4000 = 4001
  daliactor8.addRenderer( rendererForActor8 );  // renderer 8 drawn with depth index of 0 + 4000 = 4000

~~~

## Layer_3D

~~~{.cpp}
// C++
layer.SetBehavior( Layer::LAYER_3D );
~~~
  
When using this mode depth testing will be used ( depth buffer enabled ).
  
Opaque renderers are drawn first and write to the depth buffer.
  
Then transparent renderers are drawn with depth test enabled but depth write switched off.
  
 ![ ](layers3d.png)

  
Transparent renderers are drawn in order of distance
from the camera ( painter's algorithm ).

 ![ ](transSort.png)
  

Note:

 - In LAYER_3D mode, actor tree hierarchy makes no difference to draw order
 - When 2 transparent renderers are the same distance from the camera, you can use depth index to adjust which renderer is drawn first.

  
## Actor drawMode OVERLAY_2D

Inside a layer it is possible to force a tree actors to be drawn on top everything else in the layer.
  
The draw order of the actors inside the tree marked OVERLAY_2D, the draw order is defined by the renderers depth index.
Depth testing is not used.
  

## Layer Actor Specific Properties

| Name                   |    Type    | Writable     | Animatable|
|------------------------|------------|--------------|-----------|
| clippingEnable         |BOOLEAN     | 0     |  X |
| clippingBox            | ARRAY [0,0,400,600]) | 0 | X|
| behaviour              | STRING ( "LAYER_2D" or "LAYER_3D") | 0 | X|

  @extends Actor

*/
