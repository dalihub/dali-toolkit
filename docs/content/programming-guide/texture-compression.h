
/*! \page Texture_Compression Texture Compression


Using compressing the textures will:
<ul>
<li> Speed up rendering in time the GPU == less power used due to less texture data being transferred.
<li> Reduce texture memory usage.
<li> Speed up load times. Smaller files mean quicker load times.
</ul>
<br><br>
DALi supports the KTX file format.
You just load the compressed texture like you would any other image.
\code
Image::New("my_compressed_file.ktx");
\endcode
<br>
ARMS texture compression tool<br>
http://malideveloper.arm.com/develop-for-mali/tools/asset-creation/mali-gpu-texture-compression-tool/  <br>

Here is an example of using the ARM compression tool.
\image html compression-options.jpg

<br><br>

\image html compression-example.jpg
<br>
<br>
As shown above the ETC-1 compression format does not support alpha.<br> As a work around the tool will export
the alpha as a seperate compressed image.<br>
In order to combine both the images you need to use a custom shader.<br>
Here is an example shader:<br>
\code
  const char* const COMPRESSED_RGB_PLUS_SEPARATE_ALPHA_FRAGMENT_SOURCE =
    "\n"
    "void main()\n"
    "{\n"
    "    vec4 v4Color  = (texture2D(sTexture, vTexCoord) * uColor);\n"
    "    v4Color.a =  texture2D(sEffect, vTexCoord ).r;\n"
    "   gl_FragColor = v4Color;"
    "}\n";


  mShaderEffect = ShaderEffect::New( "", COMPRESSED_RGB_PLUS_SEPARATE_ALPHA_FRAGMENT_SOURCE);

  mAtlasImageRGB = Image::New( ATLAS_RGB_FILENAME.KTX);

  mAtlasImageAlpha = Image::New( ATLAS_ALPHA_FILENAME.KTX );

  mShaderEffect.SetEffectImage( mAtlasImageAlpha );



  // to create Image Actor
  ImageActor  imageActor = ImageActor::New( mAtlasImageRGB, GetImagePosition( info) );

  imageActor.SetShaderEffect( mShaderEffect );

  imageActor.SetBlendMode(BlendingMode::ON);

\endcode


 */
