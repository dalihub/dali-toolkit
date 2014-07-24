/**
 * \page dynamics-bodies Dynamics - Bodies
 * A Dali::DynamicsBody can be "Rigid" or "Soft". Rigid bodies require much less processing and should be used
 * in preference to a soft body.\n
 * All bodies are controlled by the simulation, the application developer can influence them by setting their
 * linear or angular velocities, but direct control of their position is not possible until the Dali::DynamicsBody is flagged
 * as a \ref kinematic-body "kinematic object".\n
 *
 * \section create-body Creating a body
 * <p>
 * Each Dali::DynamicsBody is created by an Dali::Actor through its Dali::Actor::EnableDynamics method using a
 * Dali::DynamicsBodyConfig object to specify options for the Dali::DynamicsBody.
 * \code
 * // Initialize and get a handle to the Dali::DynamicsWorld
 * Dali::DynamicsWorldConfig worldConfig( Dali::DynamicsWorldConfig::New() );
 * Dali::DynamicsWorld dynamicsWorld( Dali::Stage::GetCurrent().InitializeDynamics( worldConfig ) );
 * // Create an actor to represent the world
 * Dali::Actor dynamicsRootActor( Dali::Actor::New() );
 * dynamicsWorld.SetRootActor( dynamicsRootActor );
 * Dali::Stage::GetCurrent().Add( dynamicsRootActor );
 *
 * // create an actor to represent a rigid body
 * Dali::Actor actor( Dali::Actor::New() );
 * actor.SetParentOrigin( Dali::ParentOrigin::CENTER );
 * dynamicsRootActor.Add( actor );
 * // Enable dynamics for the actor, creating a rigid body with default configuration
 * actor.EnableDynamics( Dali::DynamicsBodyConfig::New() );
 * \endcode
 *
 * \section create-body-advanced Specifying options
 * <h4>Mass</h4>
 * Use Dali::DynamicsBodyConfig::SetMass to specify the mass of the body [default: 1].
 * <h4>Elasticity</h4>
 * Use Dali::DynamicsBodyConfig::SetElasticity to specify the elasticity of the body [default: 0.85].\n
 * This may also be known as the co-efficient of restitution or &apos;bounciness&apos;.
 * <h4>Damping</h4>
 * Use Dali::DynamicsBodyConfig::SetLinearDamping to specify the linear damping coefficient [default: 0].\n
 * and Dali::DynamicsBodyConfig::SetAngularDamping to specify the angular damping coefficient [default: 0].\n
 * <h4>Friction</h4>
 * Use Dali::DynamicsBodyConfig::SetFriction to specify the friction of the body [default: 0.5].\n
 * <h4>Collision Filtering</h4>
 * See \link dynamics-collisions Collision Detection and Filtering \endlink\n\n
 * Use Dali::DynamicsBodyConfig::SetCollisionGroup to specify the collision filter group.\n
 * Use Dali::DynamicsBodyConfig::SetCollisionMask to specify the collision filter mask.\n
 * <h3>Soft body specific options</h3>
 * <h4>Stiffness</h4>
 * Use Dali::DynamicsBodyConfig::SetStiffness to specify the stiffness of the links between the mesh vertices used to
 * define the soft body. Values clamped between 0 and 1 [default: 1].\n
 * <h4>Anchor hardness</h4>
 * Use Dali::DynamicsBodyConfig::SetAnchorHardness to specify the hardness or drift correction applied to anchors.
 * Values clamped between 0 and 1 [default: 0.7]. Smaller values mean less drift correction.\n
 * <h4>Conservation</h4>
 * Use Dali::DynamicsBodyConfig::SetShapeConservation to specify the shape conservation coefficient,
 * or the magnitude of the force which will attempt to maintain the soft bodies shape (see \ref Dali::DynamicsBody::ConserveShape).\n
 * Use Dali::DynamicsBodyConfig::SetVolumeConservation to specify the volume conservation coefficient,
 * or the magnitude of the force which will attempt to maintain the soft bodies volume (see \ref Dali::DynamicsBody::ConserveVolume).
 * Smaller values mean less conservation.\n
 * <h4>Create a rigid body with advanced options</h4>
 * \code
 * Dali::DynamicsBodyConfig bodyConfig( Dali::DynamicsBodyConfig::New() );
 * // increase mass from the default
 * bodyConfig.SetMass( 2.5f );
 * // set elasticity so that the velocity of the object will be halved after a collision
 * // (assuming the other body has a mass = 1 and a velocity 0f 0).
 * bodyConfig.SetElasticity( 0.5f );
 * // increase the rate at which a bodies linear velocity will decrease
 * bodyConfig.SetLinearDamping( 0.5f );
 * // reduce the friction to zero
 * bodyConfig.SetFriction( 0.0f );
 * // Ignore all collisions
 * bodyConfig.SetCollisionGroup( 0 );
 * bodyConfig.SetCollisionMask( 0 );
 *
 * // create an actor for the Dali::DynamicsBody
 * Actor actor( Actor::New() );
 * actor.SetParentOrigin( Dali::ParentOrigin::CENTER );
 * // create the Dali::DynamicsBody
 * actor.EnableDynamics( bodyConfig );
 *
 * // add to the simulation
 * dynamicsRootActor.Add( actor );
 * \endcode
 * <h4>Create a soft body with advanced options</h4>
 * \code
 * // Create a unit mesh with 25 vertices
 * Dali::Mesh mesh( Dali::Mesh::NewPlane(1.0f, 1.0f, 5, 5) );
 *
 * Dali::DynamicsBodyConfig bodyConfig( Dali::DynamicsBodyConfig::New() );
 * // select a soft body
 * bodyConfig.SetType( Dali::DynamicsBodyConfig::SOFT );
 * // set the mesh as the soft body shape
 * bodyConfig.SetShape( Dali::DynamicsShape::NewMesh( mesh ) );
 * // decrease the stiffness of the links between the soft body vertices
 * bodyConfig.SetStiffness( 0.25f );
 * // Make anchors very loose/weak
 * bodyConfig.SetAnchorHardness( 0.1f );
 *
 * // create an actor for the Dali::DynamicsBody
 * Actor actor( MeshActor::New(mesh) );
 * actor.SetParentOrigin( Dali::ParentOrigin::CENTER );
 * // create the Dali::DynamicsBody
 * actor.EnableDynamics( bodyConfig );
 *
 * // add to the simulation
 * dynamicsRootActor.Add( actor );
 * \endcode
 * \image html dynamics/dynamics-soft.png "A soft body (with debug rendering enabled)"
 * \section kinematic-body Kinematic bodies
 * A kinematic body is not controlled by the simulation, there is a one-way interaction with other dynamic objects
 * under control of the simulation, where other objects will be pushed away, but the kinematic object will be unaffected.\n
 * Kinematic objects can be animated with DALi's \ref animation-example "animation system", each DALi update the simulation will
 * get the current position of associated DALi actor.\n Use Dali::DynamicsBody::SetKinematic to make a kinematic object.
 * <h3>Animating a kinematic object</h3>
 * Other dynamics enabled actors that collide with the kinematic object during the animation will be pushed
 * away.
 * \code
 * ...
 * // create an actor to represent a rigid body
 * Dali::Actor actor( Dali::Actor::New() );
 * dynamicsRootActor.Add( actor );
 * // Enable dynamics for the actor, creating a rigid body with default configuration
 * actor.EnableDynamics( Dali::DynamicsBodyConfig::New() );
 * // get the DynamicsBody handle
 * DynamicsBody body( actor.GetDynamicsBody() );
 * body.SetKinematic( true );
 * // create a second animation to move the actor 100 units to the right
 * Animation animation( Animation::New( 1 ) );
 * animation.MoveBy( actor, Vector3( 100, 0, 0 ), AlphaFunctions::Linear );
 * animation.Play();
 * \endcode
 * <hr>
 * <p>See also
 * <ul>
 *  <li>Dali::DynamicsBodyConfig</li>
 *  <li>Dali::Actor::EnableDynamics</li>
 *  <li>\link dynamics-initialization DynamicsWorld Initialization and Usage\endlink</li>
 * </ul>
 * </p>
 */

