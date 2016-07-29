/*! \page shader-intro Shader Effects
 *
 *
 * <h2 class="pg">Introduction</h2>
 *
 * The shader effects allow the developer to apply visual deformations on Image Views.
 * They can affect the geometry, the colors and textures of the Image View.
 *
 * <br>
 * <br>
 * <h2 class="pg">Custom Shader Effects</h2>
 * The custom shader lets the developers create their own shader effects by specifying the vertex and pixel shaders.
 *
 * To set a custom shader to ImageVisual you have to pass it through as a Property::Map
 * @code
 * //an example vertex shader
 * const char* VERTEX_SHADER = DALI_COMPOSE_SHADER(
 *   attribute mediump vec2 aPosition;\n
 *   varying mediump vec2 vTexCoord;\n
 *   uniform mediump mat4 uMvpMatrix;\n
 *   uniform mediump vec3 uSize;\n
 *   \n
 *   void main()\n
 *   {\n
 *     mediump vec4 vertexPosition = vec4(aPosition, 0.0, 1.0);\n
 *     vertexPosition.xyz *= uSize;\n
 *     vertexPosition = uMvpMatrix * vertexPosition;\n
 *     \n
 *     vTexCoord = aPosition + vec2(0.5);\n
 *     gl_Position = vertexPosition;\n
 *   }\n
 * );
 *
 * //an example fragment shader
 * const char* FRAGMENT_SHADER = DALI_COMPOSE_SHADER(
 *   varying mediump vec2 vTexCoord;\n
 *   uniform sampler2D sTexture;\n
 *   uniform lowp vec4 uColor;\n
 *   \n
 *   void main()\n
 *   {\n
 *     gl_FragColor = texture2D( sTexture, vTexCoord ) * uColor;\n
 *   }\n
 * );
 *
 * Property::Map customShader;
 *
 * customShader.Insert(“vertexShader”, VERTEX_SHADER); //if this is not set then the default ImageView vertex shader will be used
 * customShader.Insert(“fragmentShader”, FRAGMENT_SHADER); //if this is not set then the default ImageView fragment shader will be used
 *
 * Property::Map map;
 * map.Insert(“shader”, customShader);
 *
 * ImageView imageView = ImageView::New("image-url.png")
 * imageView.SetProperty(ImageView::Property::IMAGE, map);
 * @endcode
 *
 * Optionally, you can subdivide the grid horizontally or vertically before you add it to the map but you should not do this if a quad is used.
 * @code
 * int X_SUB_DIVISIONS = 20;
 * int Y_SUB_DIVISIONS = 20;
 * customShader.Insert(“subdivideGridX”, X_SUB_DIVISIONS); //optional number of times to subdivide the grid horizontally, don’t add if you just want to use a quad
 * customShader.Insert(“subdivideGridY”, Y_SUB_DIVISIONS); //optional number of times to subdivide the grid vertically, don’t add if you just want to use a quad
 *
 * //shader hints can be an array or a string
 * optional array of shader hints
 *
 * Property::Array shaderHints;
 * shaderHints.PushBack(“requiresSelfDepthTest”);
 * shaderHints.PushBack(“outputIsTransparent”);
 * shaderHints.PushBack(“outputIsOpaque”);
 * shaderHints.PushBack(“modifiesGeometry”);
 * customShader.Insert(“hints”, shaderHints);
 *
 * //or optional single shader hint as a string
 * //customShader.Insert(“hints”, “outputIsTransparent”);
 * @endcode
 *
 * The value of a uniform can be set on the imageView
 * @code
 * // if the uniform was declared like this in the shader: uniform float myUniform;
 * imageView.RegisterProperty( "myUniform", 0.5f );
 * @endcode
 *
 *
 */
