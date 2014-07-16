/*! \page animation-rotation Rotation with quaternions
 *
 * \ref Dali::Quaternion "Quaternions" are used to specify unique orientations on actors. They are also
 * very useful for calculating smooth transitions between orientations.
 *
 * The Dali::Actor class provides the ability to set the orientation
 * by both angle+axis and quaternion. It also allows you to rotate the
 * actor around another quaternion.
 *
 * @code
 * Dali::Actor actor;
 * actor.SetRotation(Quaternion(Radian(Degree(45.0f)).value, Vector3::XAXIS)),
 *
 * Quaternion q(Radian(Degree(30.0f)).value, Vector3(1.0f, 1.0f, 0.0f));
 * actor.RotateBy(q);
 * @endcode
 *
 * The Dali::Animation class provides several RotateTo methods that
 * use \ref Dali::Quaternion "Quaternions" directly.  The other
 * RotateTo methods are specified using Axis+Angle, and these convert
 * to Quaternion internally. You will only need to use the quaternion
 * based methods when you are doing something more complex than
 * initialising with Axis+Angle, such as applying several rotations
 * together.
 * @code
 * mAnimation = Animation::New(5.0f); // 5 seconds
 * Quaternion q(Radian(Degree(45.0f)).value, Vector3::YAXIS);
 * Quaternion r(Radian(Degree(30.0f)).value, Vector3::ZAXIS);
 * q *= r;
 * mAnimation.RotateTo(mActor, q, AlphaFunctions::EaseInOut);
 * mAnimation.Play();
 * @endcode
 */
