/**
 * \page dynamics-collisions Collisions
 * \section collision-detection Collision Detection
 * <p>
 * Collision detection is automatic and occurs between all Dali::DynamicsBody objects in the simulation.@n
 * To respond to a detected collisions, the application developer can connect to a signal provided by
 * a Dali::DynamicsWorld object.
 * \code
 * ...
 *
 * // DynamicsWorld initialization code
 *
 * ...
 *
 * // Connect a signal handler to the signal
 * Dali::DynamicsWorld theWorld( Stage::GetCurrent().GetDynamicsWorld() );
 * theWorld.SignalCollision().Connect( this, &myClass::OnDynamicsCollision );
 *
 * ...
 *
 * // Implement a signal handler
 * void myClass::OnDynamicsCollision( Dali::DynamicsWorld world, Dali::DynamicsCollision collisionData )
 * {
 *   std::cout << "Collision between "
 *             << collisionData.GetActorA().GetName() << " and "
 *             << collisionData.GetActorB().GetName() << " ";
 *
 *   if( collisionData.GetImpactForce() != 0.0f )
 *   {
 *     std::cout << "detected (impact force: " << collisionData.GetImpactForce() << " )";
 *   }
 *   else
 *   {
 *     std::cout << "ended";
 *   }
 *   std::cout << std::endl;
 * }
 * \endcode
 *
 * <hr>
 * \section collision-filtering Collision Filtering
 *
 * <p>
 * When a large number of Dali::DynamicsBody objects are added to the simulation, collision detection can become a
 * significant performance drain, where every possible pairing of objects needs to be checked for collisions.</p>
 * <p>You can significantly reduce the number of pairs considered for collision detection by using a collision filter.</p>
 * <p>Each Dali::DynamicsBody can belong to a user defined collision filter group and have a user defined collision filter mask.<p>
 * <p>A Dali::DynamicsBody pair are considered for collision detection if one or more bits in the filter group from each Dali::DynamicsBody
 * matches one or more bits in the filter mask of the other Dali::DynamicsBody.
 * </p>
 * <center><table border="1">
 * <caption>Truth table</caption>
 * <tr align="center">
 * <th>&nbsp;P&nbsp;</th>
 * <th>&nbsp;Q&nbsp;</th>
 * <th>tested for collision?</th>
 * </tr>
 * <tr align="center">
 * <td>0</th>
 * <td>0</th>
 * <td>no</th>
 * </tr>
 * <tr align="center">
 * <td>0</th>
 * <td>1</th>
 * <td>no</th>
 * </tr>
 * <tr align="center">
 * <td>1</th>
 * <td>0</th>
 * <td>no</th>
 * </tr>
 * <tr align="center">
 * <td>1</th>
 * <td>1</th>
 * <td>yes</th>
 * </tr>
 * </table></center>
 * <p>
 * where <b>P</b> = bitwise AND of the collision group from the first Dali::DynamicsBody and the collision mask from the second Dali::DynamicsBody\n
 * and   <b>Q</b> = bitwise AND of the collision group from the second Dali::DynamicsBody and the collision mask from the first Dali::DynamicsBody.
 * </p><br>
 * <p>
 * <b>Pseudo code for the filter check.</b>
 * \code
 * const bool canCollide( (  firstBody->GetCollisionGroup() & secondBody->GetCollisionMask() &&
 *                        ( secondBody->GetCollisionGroup() &  firstBody->GetCollisionMask() );
 * \endcode
 * </p>
 * <h3 class="pg">Code example - Illustrating how to create multiple filter groups and masks.</h3>
 * \code
 * // Define some collision groups
 * const short int group0( 1 << 1 );
 * const short int group1( 1 << 2 );
 * const short int group2( 1 << 3 );
 *
 * // Create some Dali::DynamicsBodyConfig objects
 * Dali::DynamicsBodyConfig bodyConfig0( Dali::DynamicsBodyConfig::New() );
 * Dali::DynamicsBodyConfig bodyConfig1( Dali::DynamicsBodyConfig::New() );
 * Dali::DynamicsBodyConfig bodyConfig2( Dali::DynamicsBodyConfig::New() );
 *
 * // Assign the collision filters to the configurations
 * bodyConfig0->SetCollisionGroup(group0);
 * bodyConfig0->SetCollisionMask( group1 | group2 );
 * bodyConfig1->SetCollisionGroup(group1);
 * bodyConfig1->SetCollisionMask(group0);
 * bodyConfig2->SetCollisionGroup(group2);
 * bodyConfig2->SetCollisionMask(group0 | group2);
 * \endcode
 * <p>
 * Collision detection is \b enabled between Dali::DynamicsBody pairs of...
 * <ul>
 *  <li>group0 and group1 objects.</li>
 *  <li>group0 and group2 objects.</li>
 *  <li>group2 objects.</li>
 * </ul>
 * Collision detection is \b disabled between Dali::DynamicsBody pairs of...
 * <ul>
 *  <li>group0 objects.</li>
 *  <li>group1 objects.</li>
 *  <li>group1 and group2 objects.</li>
 * </ul>
 * </p>
 *
 * See
 * <ul>
 *  <li>Dali::DynamicsWorld::SignalCollision</li>
 *  <li>Dali::DynamicsCollision</li>
 *  <li>\ref Dali::DynamicsBodyConfig::SetCollisionGroup - to set the collision group</li>
 *  <li>\ref Dali::DynamicsBodyConfig::SetCollisionMask - to set the collision mask</li>
 * </ul>
 */

