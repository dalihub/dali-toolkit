/*! \page animation-shader Shader Effect Animation
 *

The uniforms of a shader can be animated using the Animation::AnimateTo functions.

For example, to animate the center point of the Bendy shader effect:
@code
Dali::Animation animation = Dali::Animation::New( 1.0f );
//...
Vector2 newPosition( 0.0f, 0.0f );
animation.AnimateTo( Property(shaderEffect, shaderEffect.GetPositionPropertyName()), newPosition );
@endcode

To animate a uniform of a custom shader effect, the developer must use the name of the uniform:
@code
Dali::Animation animation = Dali::Animation::New( 1.0f );
//...
// Set the initial value for the uniform
shaderEffect.SetUniform( "myUniform", -0.5f );
//...
// Animate the uniform to a value
animation.AnimateTo( Property(shaderEffect, "myUniform"), 0.5f );
@endcode

 *
 */
