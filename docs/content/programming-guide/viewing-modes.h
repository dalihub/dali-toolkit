/*! \page viewing-modes Viewing modes
 *
 *
 * <h2 class="pg">Introduction</h2>
 *
 * DALi supports multiple viewing modes: <br>
* <ul>
*  <li>MONO <br>
*  <li>STEREO_HORIZONTAL <br>
*  <li>STEREO_VERTICAL <br>
* </ul>
* MONO is the default viewing mode. When using MONO mode, only one image of the scene is produced in every update. In this  setup, DALi will create a default render task, which will render everything added to the stage root layer, and one default camera for that render task.<br><br>
* STEREO_HORIZONTAL and STEREO_VERTICAL allow the application to produce stereo images. Stereo rendering techniques produce two images of the scene with an slight offset between them which will be presented to the left and the right eye independently. In this setup, DALi will create two additional render tasks, one for the right eye and one for the left, each with it's own camera. Those cameras will be parented to the default camera. The method to generate the projection transformation for each camera is known as "parallel axis asymmetric frustum perspective projection" and is illustrated in the image below.<br><br>

* \image html viewing-modes/stereo-projection.png

* <br>The separation between the two cameras is configurable, but, typically, it should range between 50 and 70 millimeters. Too large a separation can be hard to resolve and is known as hyperstereo. The convergence plane for both frustums is the 2D plane, so, the projection of anything lying in the 2D plane will be coincident for both left and right eyes, hence zero parallax. Objects that lie in front of the projection plane will appear to be in front of the screen and object behind the projection plane will appear to be "into" the screen. Object behind the projection plane ( positive parallax ) are easier to look and minimises eye strain.<br>

 * In order to view stereoscopic images correctly, each image must be presented to each eye independently. There exist devices in the market for this, like head-mounted displays, which is a display device, worn in the head, that have two small displays, one for each eye. A cheaper alternative to this often expensive devices is Google's cardboard viewer.<br>
 * \image html viewing-modes/google-cardboard.png
 *<br>With Google cardboard, or some similar device, you can transform your phone into a basic virtual reality headset.
 *
 * <h2 class="pg">Stereoscopic rendering in DALi</h2>
 * STEREO_HORIZONTAL mode will present the left image on the top half of the screen and the right image on the bottom half. It is intended  to be used in landspace orientation, so the images are rotated 90 degrees counterclockwise. STEREO_VERTICAL mode, on the other hand, will render the left image on the left half of the screen and the right image on the right side.<br><br>
*
*To define the viewing mode to be used, applications can use the function Dali::Application::SetViewMode defined in the Application class, passing as the parameter which mode to use. It is possible for an application to query which view mode is being used by calling the funcion Dali::Application::GetViewMode. To define the separation between the left and right cameras, the application can use the function Dali::Application::SetStereoBase, passing as a parameter the distance in millimeters. This distance can be queried using Dali::Application::GetStereoBase.<br>
*It is also possible to set the viewing mode and offset at initialization time using two command-line arguments for this purpose: --view-mode ( or -v ) and 0 for MONO, 1 for STEREO_HORIZONTAL, 2 for STEREO_VERTICAL, and --stereo-base ( or --s ) and the separation in millimeters.<br><br>

* <h2 class="pg">Considerations</h2>
*There are a some restrictions to be aware of when writing stereoscopic applications using DALi.

<ul>
*  <li>When stereo mode is selected, the default's render task source actor will be set to an uninitialized actor so it doesn't render anything. Changing the default's render task source actor later on will produce undesired results, as you, typically, do not want to see anything rendered by the default camera when stereo mode is on.<br>

*  <li>Stereo cameras are parented to the default camera, so if the application need to change camera position or orientation it will need to change the default camera transformation. The handle to the default camera can be obtained form the default render task as shown below.<br>
 * @code
 * Dali::RenderTask defaultRenderTask = Dali::Stage::GetCurrent().GetRenderTaskList().GetTask(0);
 * Dali::CameraActor defaultCamera = defaultRenderTask.GetCameraActor();
 * @endcode

*  <li>Stereo render tasks will render everything added to the stage's root layer. If the application need to render any object hierarchy to an off-screen buffer, it will have to set the exclusive flag on that render task so stereo tasks doesn't render that particular subtree. To set the exclusive flag in a render task, the application can call the method Dali::RenderTask::SetExclusive defined in the render task.<br>
*</ul>
 *
 */
