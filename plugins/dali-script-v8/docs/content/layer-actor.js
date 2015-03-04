/**
## Layer Actor API ( extends Actor API)

 Layers provide a mechanism for overlaying groups of actors on top of each other.

 When added to the stage, a layer can be ordered relative to other layers. The bottom
 layer is at depth zero. The stage provides a default layer for it's children.

 Layered actors inherit position etc. as normal, but are drawn in an order determined
 by the layers. The depth buffer is cleared before each layer is rendered unless depth
 test is disabled or there's no need for it based on the layers contents;
 actors in lower layers cannot obscure actors in higher layers.

 If depth test is disabled, there is no performance overhead from clearing the depth buffer.


### Simple example

```

var textActor1 = new dali.TextActor( "I'm above" );
var textActor2 = new dali.TextActor( "I'm below" );
  
// add first actor to the stage
dali.stage.add( textActor1 );
  
// create a layer and add second actor
var layer = new dali.Layer();
layer.add( textActor2 );
dali.stage.add( layer );  // textActor2 is now above textActor1
  
// shift layer to bottom
layer.lowerToBottom();	  // textActor2 is now below textActor1
```

### Layer Actor Specific Properties

| Name                   |    Type    | Writable     | Animatable|
|------------------------|------------|--------------|-----------|
| clippingEnabled        |BOOLEAN     | &#10004;     |  &#10008; |
| clippingBox            | RECTANGLE ([0,0,400,600]) | &#10004; | &#10008;|

  @class Layer
  @extends Actor
 */
