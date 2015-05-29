/*! \page shader-intro Shader Effects
 *

<h2 class="pg">Introduction</h2>

The shader effects allow the developer to apply visual deformations on the actors.
They can affect the geometry, the colors and textures of the actor.


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
