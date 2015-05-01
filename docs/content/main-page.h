/*! \mainpage
 *
 * \section mainintro_sec Introduction
 *
 * It is a quick and easy way of allowing developers to create Rich UI Applications like Home
 * screen, Gallery, Music player, Games, Maps...
 *
 * DALI is based on OpenGL ES 2.0, however it hides the complexity of
 * the OpenGL API from developers and provides a clean cross-platform C++ framework.
 *
 * \section Introduction Introduction
 * - \link fundamentals Dali Fundamentals \endlink
 * - \link dali-application Dali Application and Adaptor \endlink
 * - \link hello-world Hello World - explained \endlink
 * - \link handle-body-idiom Handle – body idiom \endlink
 *
 * \section Actors Actors
 * - \link image-mesh-actor Image and Mesh actors \endlink
 * - \link event-system Event Handling \endlink
 * - \link custom-actor Custom Actor \endlink
 *
 * \section ShaderEffects Shader Effects
 * - \link shader-intro Shader Effects\endlink
 *
 * \section Animation Animation
 * - \link animation-example Example and Usage\endlink
 * - \link animation-rotation Rotation with quaternions \endlink
 * - \link animation-shader Shader Animation \endlink
 * - \link animation-multi-threading-notes Multi-threading Notes \endlink
 *
 * \section Constraints
 * - \link constraints-intro Introduction to Constraints \endlink
 *
 * \section SizeNegotiation Size Negotiation
 * - \link size-negotiation Size Negotiation \endlink
 *
 * \section UIControls UI Controls
 * - \link text-label Text Label \endlink
 * - \link item-view Item View \endlink
 * - \link scroll-view Scroll View \endlink
 * - \link size-negotiation-controls Size Negotiation for Controls \endlink
 * - \link type-registration Type Registration \endlink
 * - \link properties Properties \endlink
 * - \link background Background \endlink
 *
 * \section Dynamics Dynamics
 * - \link dynamics-intro Introduction to Dynamics\endlink
 * - \link dynamics-initialization Initializing the Simulation\endlink
 * - \link dynamics-bodies Bodies - adding and controlling dynamic objects \endlink
 * - \link dynamics-joints Joints - linking objects\endlink
 * - \link dynamics-collisions Collision Detection and Filtering\endlink
 *
 * \section Scripting
 * - \link script-overview Overview \endlink
 * - \link script-howto How to Add a Custom Control \endlink
 * - \link script-hello Hello World in script \endlink
 * - \link script-json-specification.html JSON Specification\endlink
 *
 * \section Rendering
 * - \link viewing-modes Viewing modes \endlink
 *
 * \section Profiling
 * - \link resource-tracking Resource Tracking \endlink
 * - \link performance-profiling Performance Profiling \endlink
 *
 * \section Performance
 * - \link performance-tips Performance Tips \endlink
 * - \link texture-atlases Texture Atlases  \endlink
 * - \link Texture_Compression Compressing Textures \endlink
 *
 * \section Testing
 * See [Automated Tests](@ref auto_testing) for instructions.
 */

/*! \page scene-graph
 *
 * \section scene_intro What is a scene graph?
 * From wikipedia...
 * A scene graph is a collection of nodes in a graph or tree structure.
 * A node may have many children but often only a single parent,
 * with the effect of a parent applied to all its child nodes;
 * an operation performed on a group automatically propagates
 * its effect to all of its members. In many programs, associating
 * a geometrical transformation matrix (see also transformation and matrix)
 * at each group level and concatenating such matrices together is an
 * efficient and natural way to process such operations. A common feature,
 * for instance, is the ability to group related shapes/objects into a
 * compound object that can then be moved, transformed, selected,
 * etc. as easily as a single object.
 *
 * \section scene_dali How does this relate to the Dali public API?
 *
 * Actors are effectively nodes that receive input (touch events) and act as a
 * container for draw-able elements (which are also nodes) and other actors.
 *
 * For example a Button actor will be an actor with several elements such as button background,
 * text label and button face. When the actor is moved around, it's child elements will move with it.
 * When the button is pressed, the actor will receive an event and adjust the color of its button face
 * element.
 *
 * \section scene_internal Why does Dali internally have a second scene graph?
 * Actors and elements are contained in a scene graph which deals with input, layout and some animation
 * it doesn't perform any drawing.
 *
 * All the drawing is done via the Render Manager which has it's own render scene graph, where it's nodes
 * are just for drawing things like image/text and moving them around. So when you create an Image element
 * it will ask the RenderManager to create an Image node. The separation allows the RenderManager to
 * run in a separate thread to maintain a high frame rate, without being slowed down by any logic
 * performed on the actor/element side.
 *
 */
