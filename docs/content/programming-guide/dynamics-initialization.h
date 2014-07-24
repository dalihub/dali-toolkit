/**
 * \page dynamics-initialization Initializing the Simulation
 * \section dynamics-prerequisites Dynamics prerequisites
 * <p>
 * In order to reduce binary size on devices, by default Dali core is built without Dynamics support. \n
 * This can be enabled by adding the --enable-debug option to configure. The configure command should output this: \n
 * \code
 * Configuration
 * -------------
 *  Dynamics support:                 yes
 * \endcode
 * <p>In addition to rebuilding Dali, a physics plugin (e.g. bullet, havok) should be installed on the target device.
 * Dali adaptor provides the bullet plugin e.g. install dali-adaptor-dali-bullet-plugin-X.armv7l.rpm.
 *
 * \section initializing-world Initializing the World
 * <p>
 * The simulation is encapsulated and controlled by a instance of a Dali::DynamicsWorld object.\n
 * \code
 * // DynamicsWorld initialisation code
 * Dali::DynamicsWorldConfig worldConfig( Dali::DynamicsWorldConfig::New() );
 * Dali::Stage::GetCurrent().InitializeDynamics( worldConfig );
 * \endcode
 * <p>If the DynamicsWorld handle empty, then a prerequisite is missing (see above).
 * Use a Dali::DynamicsWorldConfig object to specify options for the type of simulation required.\n
 * Dali::DynamicsWorldConfig::RIGID supports rigid body dynamics only, while Dali::DynamicsWorldConfig::SOFT supports
 * both rigid and soft body dynamics. Rigid body dynamics uses less CPU than soft body dynamics and is simpler to set up.
 * \code
 * // DynamicsWorld initialisation code
 * Dali::DynamicsWorldConfig worldConfig( Dali::DynamicsWorldConfig::New() );
 * // Choose a rigid body capable simulation
 * worldConfig.SetType( Dali::DynamicsWorldConfig::RIGID );
 * // or a soft and rigid body simulation
 * worldConfig.SetType( Dali::DynamicsWorldConfig::SOFT );
 * // Request Dali::Stage to create an instance of the DynamicsWorld
 * Dali::Stage::GetCurrent().InitializeDynamics( worldConfig );
 * \endcode
 *
 * \section initializing-world-advanced Advanced Initialization
 * <h3>Units</h3>
 * <p>All distance units in the simulation are based on meters, so positioning an actor at (0, -500, -1000)
 * will position it 0.5km in the air and 1km away from the camera.\n So if the actor was to fall under
 * the control of gravity it will seem to fall in slow motion. To counteract this the simulation units can
 * be modified using Dali::DynamicsWorldConfig::SetUnit. The default value is set to 0.01 to change the
 * simulation units to centimeters.
 * </p>
 * \code
 * // change simulation back to meters
 * worldConfig.SetUnit(1.0f);
 * // or change simulation unit to millimeters
 * worldConfig.SetUnit(1.0f/1000.0f);
 * \endcode
 *
 * <h3>Simulation update ticks</h3>
 * <p>The application developer can set the number of simulation time steps / DALi update tick using
 * Dali::DynamicsWorldConfig::SetSimulationSubSteps.\n
 * Use this to advance the simulation in smaller time steps, thus gaining a more accurate
 * simulation for collision detection.\n
 * Using this API may adversely affect performance, as the dynamics simulation is performing many more
 * calculations each DAli tick than normal.</p>
 * \code
 * // Assume DAli is updating at 60Hz (16.667ms / update)
 * //Setting subSteps to 1 will update the simulation once per DALi update.
 * worldConfig.SetSimulationSubSteps(1);
 * //Setting subSteps to 4 will perform 4 simulation updates per DALi update each with a time step of Approx 4.2ms.
 * worldConfig.SetSimulationSubSteps(1);
 * \endcode
 * \section manipulating-world Using the World
 * <h3>The Dynamics Simulation Root Actor</h3>
 * <p>To manipulate the world within the scene-graph it must be connected to a Dali::Actor.\n
 * All Rigid or Soft bodies that will be simulated must each be connected to a Dali::Actor which is a
 * direct child of the dynamics root actor.</p>
 * \code
 * // Create an actor to represent our view of the simulation
 * Dali::Actor dynamicsRootActor( Dali::Actor::New() );
 * // retrieve a handle to the DynamicsWorld object initialized previously
 * Dali::DynamicsWorld dynamicsWorld( Dali::Stage::GetCurrent().GetDynamicsWorld() );
 * // Connect the Dali::DynamicsWorld and the Dali::Actor
 * dynamicsWorld.SetRootActor( dynamicsRootActor );
 * // Add root actor to Dali::Stage
 * Dali::Stage::GetCurrent().Add( dynamicsRootActor );
 * \endcode
 * <h3>Gravity</h3>
 * <p>The gravity applicable to the entire simulation can be set through Dali::DynamicsWorld::SetGravity.\n
 * The gravity will apply a constant force on all Dali::DynamicsBody objects added to the world which have a
 * non zero mass and are not flagged as kinematic.</p>
 * \code
 * // Set gravity to apply a force on the negative Y axis
 * dynamicsWorld.SetGravity( Vector3( 0.0f, -10.0f, 0.0f ) );
 * \endcode
 * <hr>
 * <p>See also
* <p>See
 * <ul>
 *  <li>Dali::DynamicsWorldConfig</li>
 *  <li>Dali::DynamicsWorld</li>
 *  <li>Dali::Stage::InitializeDynamics</li>
 * </ul>
 * </p>
 *
 */

