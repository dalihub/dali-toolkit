/*! \page shader-intro Shader Effects
 *

<h2 class="pg">Introduction</h2>

The shader effects allow the developer to apply visual deformations on the actors.
They can affect the geometry, the colors and textures of the actor.

There are some built-in shader effects in Dali Toolkit:
- \ref Dali::Toolkit::BendyEffect "Bendy", bends the geometry around a point, useful to do a page turn effect,
- \ref Dali::Toolkit::DissolveEffect "Dissolve", does a dissolve effect on the actor texture,
- \ref Dali::Toolkit::RippleEffect "Ripple", does a concentric wave effect on the actor texture.

@image html shader-effect-ripple.png "Ripple Effect"

<br>
<br>
<h2 class="pg">Example and Usage</h2>
Here is an example on how to use a shader effect, using the RippleEffect.

First create the shader effect.
@code
Dali::RippleEffect effect = Dali::RippleEffect::New();
@endcode

Then set the values of the uniforms.
@code
// Set the radius of the bending
effect.SetAmplitude( 45.0f );
// Set the point around which the geometry will bend
effect.SetCenter( Vector2() );
// Set the direction of the bending
effect.SetDirection( Vector2( 1.0f, 0.0f ) );
@endcode

Finally apply the shader effect to an actor:
@code
actor.SetShaderEffect( effect );
@endcode


<br>
<br>
<h2 class="pg">Custom Shader Effects</h2>
The \ref Dali::ShaderEffect "ShaderEffect" lets the developers create their own shader effects by specifying the vertex and pixel shaders.

A custom shader effect can be created like this:
@code
String myVertexShader; // This variable would contain the code for a vertex shader.
Dali::ShaderEffect myEffect = Dali::ShaderEffect::New( myVertexShader,
                                                       "" // use default pixel shader
                                                     );
@endcode

The value of a uniform can be set like this:
@code
// if the uniform was declared like this in the shader: uniform float myUniform;
myEffect.SetUniform( "myUniform", 0.5f );
@endcode

The custom shader effect can be applied to an actor like any other shader:
@code
actor.SetShaderEffect( myEffect );
@endcode

 *
 */
