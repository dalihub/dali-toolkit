/**
 * \page dynamics-joints Joints
 * A Dali::DynamicsJoint represents a connection (or link) between a Dali::DynamicsBody pair.
 * A Joint can optionally allow \ref linear "linear motion" and/or \ref angular "angular rotation" around its origin on one or more axis, and
 * can have a \ref motors "motor" or \ref springs "spring" enabled on those axis.
 * \image html dynamics/dynamics-joint2.png
 * \section create-joint Creating a joint
 * <p>Each Dali::DynamicsJoint is created by a Dali::Actor through its Dali::Actor::AddDynamicsJoint method.\n
 * This method takes two parameters\n
 * 1. The other Dali::Actor in the joint relationship.\n
 * 2. A Dali::Vector3 relative offset from the owning Dali::Actor's current position.\n
 * A joint is active in the simulation when both of the actors are connected to the Stage via the Dali::Actor
 * set with Dali::DynamicsWorld::SetRootActorDynamics.
 * </p>
 * <h3>A code example creating two actors connected by a joint</h3>
 * \code
 * // create an actor to represent a rigid body
 * Dali::Actor actor1( Dali::Actor::New() );
 * // Enable dynamics for the actor, creating a rigid body with default configuration
 * actor1.EnableDynamics( Dali::DynamicsBodyConfig::New() );
 * actor1.SetPosition( Vector3( 0, 0, 0 ) );
 * // create an actor to represent a second rigid body
 * Dali::Actor actor2( Dali::Actor::New() );
 * actor1.SetPosition( Vector3( 100, 0, 0 ) );
 * // Enable dynamics for the actor, creating a rigid body with default configuration
 * actor2.EnableDynamics( Dali::DynamicsBodyConfig::New() );
 * // create the joint
 * Vector3 relativeOffset( 25, 0, 0 );
 * actor1.AddDynamicsJoint( actor2, relativeOffset );
 * \endcode
 * The joint is 25 units to the right of actor1 and 75 units to the left of actor2. If
 * either actor is moved the joint will follow pulling the other actor with it.
 * \section linear Linear Limits
 * \image html dynamics/dynamics-joint.png "A joint allowing linear motion on the Y Axis"
 * Limits control how much translation is allowed relative to the joints origin point, use Dali::DynamicsJoint::SetLinearLimit
 * to set linear limits.
 * \code
 * ...
 * actor1.AddDynamicsJoint( actor2, Vector3(0, 0, 0) );
 * DynamicsJoint joint( actor1.GetDynamicsJoint(actor2) );
 *
 * // Allow translation from the joint's origin along the X axis of +/- 50 units
 * joint.SetLinearLimit( Dali::DynamicsJoint::LINEAR_X, -50, 50);
 * \endcode
 * \section angular Angular Limits
 * Limits control how much rotation is allowed around the joint's origin point, use Dali::DynamicsJoint::SetAngularLimit
 * to set angular limits.
 * \code
 * ...
 * actor1.AddDynamicsJoint( actor2, Vector3(0, 0, 0) );
 * DynamicsJoint joint( actor1.GetDynamicsJoint(actor2) );
 *
 * // Allow rotation around the joint's origin on the Z axis of - 45 degrees and +90 degrees
 * joint.SetAngularLimit( Dali::DynamicsJoint::ANGULAR_Z, -Dali::Degree(45), Dali::Degree(90) );
 * \endcode
 * \section motors Motors
 * Motors apply a force along a given axis towards the lower or upper limit set on that axis.\n
 * Use Dali::DynamicsJoint::EnableMotor to enable/disable a motor.\n
 * The torque of the motor can be set with Dali::DynamicsJoint::SetMotorForce and the velocity
 * with Dali::DynamicsJoint::SetMotorVelocity.
 * \code
 * ...
 * actor1.AddDynamicsJoint( actor2, Vector3(0, 0, 0) );
 * DynamicsJoint joint( actor1.GetDynamicsJoint(actor2) );
 *
 * // allow angular rotation on the Z axis
 * joint.SetAngularLimit(Dali::DynamicsJoint::ANGULAR_Z, -Dali::Degree(90), Dali::Degree(90));
 * // enable the Z axis angular motor
 * joint.EnableMotor(Dali::DynamicsJoint::ANGULAR_Z, true);
 * // set the motor torque
 * joint.SetMotorForce(Dali::DynamicsJoint::ANGULAR_Z, 0.5f);
 * // set the motor velocity (acts towards lower limit)
 * joint.SetMotorVelocity(Dali::DynamicsJoint::ANGULAR_Z, -10.0f);
 * \endcode
 * \section springs Springs
 * Springs apply a force to keep the Dali::DynamicsJoint origin at the spring's center point.
 * A spring can be enabled for a given axis using Dali::DynamicsJoint::EnableSpring.\n
 * The center point is set as a ratio between the lower and upper limits on the given axis using
 * Dali::DynamicsJoint::SetSpringCenterPoint.\n
 * The magnitude of the spring's centering force can be set with Dali::DynamicsJoint::SetSpringStiffness.\n
 * Dali::DynamicsJoint::SetSpringDamping can be used to limit the amount of overshoot and oscillations
 * of the spring as it settles at its center point.
 * \code
 * ...
 * actor1.AddDynamicsJoint( actor2, Vector3(0, 0, 0) );
 * DynamicsJoint joint( actor1.GetDynamicsJoint(actor2) );
 *
 * // allow linear motion on Y axis
 * joint.SetLinearLimit(Dali::DynamicsJoint::LINEAR_Y, -50, 50);
 * // enable the Y axis linear spring
 * joint.EnableSpring(Dali::DynamicsJoint::LINEAR_Y, true);
 * // set the center point of the spring at -40 ( 10% of the limits set )
 * joint.SetSpringCenterPoint(Dali::DynamicsJoint::LINEAR_Y, 0.1f);
 * // set the springs stiffness or centering force
 * joint.SetSpringStiffness(Dali::DynamicsJoint::LINEAR_Y, 40.0f);
 * // allow more oscillations before the spring comes to reset
 * joint.SetSpringDamping(Dali::DynamicsJoint::LINEAR_Y, 0.1f);
 * \endcode
 * <hr>
 * <p>See also
 * <ul>
 *  <li>Dali::DynamicsJoint</li>
 *  <li>Dali::Actor::AddDynamicsJoint</li>
 *  <li>Dali::Actor::GetDynamicsJoint</li>
 * </ul>
 * </p>
 */

