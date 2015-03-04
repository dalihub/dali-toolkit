/**
 *

## RenderTask API


RenderTasks describe how the Dali scene should be rendered.

The {{#crossLink "Stage/getRenderTaskList:method"}}{{/crossLink}} method provides access to an ordered list of render-tasks.

Each RenderTask must specify the source actors to be rendered, and a camera actor from
which the scene is viewed.


RenderTasks may optionally target a frame-buffer, otherwise the default GL surface is used;
typically this is a window provided by the native system.


By default Dali provides a single RenderTask, which renders the entire actor hierachy using
a default camera actor and GL surface. If stereoscopic rendering is enabled, Dali will create
two additional render tasks, on for each eye. Each render task will have its own camera parented
to the default camera actor.

The first RenderTask used for input handling will be the last one rendered, which also has input enabled,
and has a valid source & camera actor; see {{#crossLink "RenderTask/setInputEnabled:method"}}{{/crossLink}}.

If none of the actors are hit in the last RenderTask rendered, then input handling will continue
with the second last RenderTask rendered, and so on.

All RenderTasks which target a frame-buffer (i.e. off screen) will be rendered before all RenderTasks
which target the default GL surface. This allows the user to render intermediate targets which are used
later when targetting the screen.

A RenderTask targetting a frame-buffer can still be hit-tested, provided that the
screen->frame-buffer coordinate conversion is successful; see SetScreenToFrameBufferMappingActor().
```
var tasklist = dali.stage.getRenderTaskList();
  
var renderTask0 = tasklist.createTask();

renderTask0.setSourceActor( myActor );
renderTask0.setClearEnabled( true );
renderTask0.clearColor = [1.0,0.0,0.0,0.0];

renderTask0.clearColor = [1.0,0.0,0.0,0.0];

var camera = new dali.CameraActor();

renderTask0.setCameraActor( camera );
```

### Render Task Specific Properties


| Name                   |    Type    | Writable     | Animatable|
|------------------------|------------|--------------|-----------|
|viewportPosition       | VECTOR2    | &#10004;     | &#10008; |
|viewportSize           | VECTOR2    | &#10004;     | &#10008; |
|clearColor             | VECTOR4    | &#10004;     | &#10008; |


@class RenderTask
*/

/**
 * View port position
 *
 * @property viewportPosition
 * @type dali Vector2
 */
viewportPosition

/**
 * View port size
 *
 * @property parentOrigin
 * @type dali Vector3
 */
viewportSize

 /**
 * clear color
 *
 * @property clearColor
 * @type dali Vector4
 */
 clearColor