/*! \page shader-intro Shader Effects
 *

<h2 class="pg">Introduction</h2>

The shader effects allow the developer to apply visual deformations on the actors.
They can affect the geometry, the colors and textures of the actor.

There are some utility functions in Dali::Toolkit to create some useful shader effects:
@code
//Bendy effect bends the geometry around a point, useful to do a page turn effect
Dali::ShaderEffect bendyEffect = Dali::Toolkit::CreateBendyEffect();

//Dissolve effect does a dissolve effect on the actor texture
Dali::ShaderEffect dissolveEffect = Dali::Toolkit::CreateDissolveEffect();

//Ripple effect does a concentric wave effect on the actor texture
Dali::ShaderEffect rippleEffect = Dali::Toolkit::CreateRippleEffect();

@endcode

@image html shader-effect-ripple.png "Ripple Effect"

<br>
<br>
<h2 class="pg">Example and Usage</h2>
Here is an example on how to use a shader effect, using the RippleEffect.

First create the shader effect.
@code
Dali::ShaderEffect effect = Dali::Toolkit::CreateRippleEffect();
@endcode

Then set the values of the uniforms.
@code
// Set the amplitude
effect.SetUniform("uAmplitude", 45.0f );
// Set the center
effect.SetUniform("uCenter", Vector2() );
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
