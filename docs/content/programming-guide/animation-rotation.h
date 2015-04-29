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
 * actor.SetOrientation(Quaternion(Radian(Degree(45.0f)).value, Vector3::XAXIS)),
 *
 * Quaternion q(Radian(Degree(30.0f)).value, Vector3(1.0f, 1.0f, 0.0f));
 * actor.RotateBy(q);
 * @endcode
 *
 * The Dali::Animation class provides several AnimateTo methods that
 * use \ref Dali::Quaternion "Quaternions" directly to change the orientation.
 *
 * @code
 * mAnimation = Animation::New(5.0f); // 5 seconds
 * Quaternion q(Radian(Degree(45.0f)).value, Vector3::YAXIS);
 * Quaternion r(Radian(Degree(30.0f)).value, Vector3::ZAXIS);
 * q *= r;
 * mAnimation.AnimateTo(Property(mActor, Actor::Property::ORIENTATION), q, AlphaFunction::EASE_IN_OUT);
 * mAnimation.Play();
 * @endcode
 */
