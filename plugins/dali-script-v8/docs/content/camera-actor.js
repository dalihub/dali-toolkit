/**
 ## Camera Actor API ( extends Actor API)


Allows the developer to use actor semantics to control a camera.

There are two types of camera actor, FREE_LOOK and LOOK_AT_TARGET. By default
the camera actor will be FREE_LOOK.

A FREE_LOOK camera uses actor's orientation to control where the camera is looking.
If no additional rotations are specified, the camera looks in the negative Z direction.

For LOOK_AT_TARGET the actor's orientation is ignored, instead the camera looks at TARGET_POSITION
in world coordinates.
```
// get the camera currently used
var defaultCamera = dali.stage.getRenderTaskList().getTask(0).getCameraActor();
  
var fov = defaultCamera.fieldOfView;
  
// increase field of view by 10%
defaultCamera.fieldOfView = fov * 1.1;
  
// shift the camera to the left, the rendered scene will shift to the right
defaultCamera.x -= 10;
```
  
### Camera Actor Specific Properties

| Name                   |    Type    | Writable     | Animatable|
|------------------------|------------|--------------|-----------|
| type                  |  dali.CAMERA_FREE_LOOK or  dali.CAMERA_LOOK_AT_TARGET  | &#10004; | &#10008; |
| projectionMode        | dali.CAMERA_ORTHOGRAPHIC_PROJECTION or dali.CAMERA_PERSPECTIVE_PROJECTION  | &#10004;| &#10008; |
| fieldOfView           | FLOAT      | &#10004; | &#10008;  |
| aspectRatio           | FLOAT      | &#10004; | &#10008;  |
| nearPlaneDistance     | FLOAT      | &#10004; | &#10008;  |
| farPlaneDistance      | FLOAT      | &#10004; | &#10008;  |
| leftPlaneDistance     | FLOAT      | &#10004; | &#10008;  |
| rightPlaneDistance    | FLOAT      | &#10004; | &#10008;  |
| topPlaneDistance      | FLOAT      | &#10004; | &#10008;  |
| bottomPlaneDistance   | FLOAT      | &#10004; | &#10008;  |
| targetPosition        | VECTOR3    | &#10004; | &#10008;  |
| projectionMatrix      | MATRIX     | &#10004; | &#10008;  |
| viewMatrix            | MATRIX     | &#10004; | &#10008;  |
| invertYAxis           | BOOLEAN    | &#10004; | &#10008;  |



  @class CameraActor
  @extends Actor
 */
