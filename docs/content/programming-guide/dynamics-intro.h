/*! \page dynamics-intro Dynamics - Introduction
 *
 * Dynamics gives the application developer a means to simulate physical kinetic properties on solid shapes.
 * Simple physical shapes can be associated with a given actor, e.g., an actor representing a ball would have
 * a spherical shape. Other simple shapes include cube, cone, cylinder and capsule (a pill or lozenge shape).
 * \image html dynamics/dynamics-shapes.png "Simple Shapes"
 * The application developer can provide more complex shapes using arbitrary meshes, however, this will use more
 * CPU than the simple shapes.\n\n
 * Dali supports both rigid body and soft body simulations.
 * <ul>
 * <li>Rigid body simulation means that the shapes used in the simulation cannot deform. This is simpler, and requires
 * less processing power.</li>
 * <li>Soft body simulation allows the shapes used in the simulation to deform, e.g. a rubber ball will squash and change
 * shape when it hits a wall; or a cloth flag will flutter, etc.</li>
 * </ul>
 * Both forms of simulation provide automatic collision detection, and can be detected on all bodies in the simulation.
 * The application developer can use signals to listen for detected collisions.
 *
 * \image html dynamics/dynamics-rigid.png "Example application"
 */

