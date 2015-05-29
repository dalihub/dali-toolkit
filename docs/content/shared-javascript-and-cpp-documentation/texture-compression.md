/**
 *

# Texture Compression {#texturecompression}


Using compressing the textures will:

- Reduce memory bandwidth in rendering due to less texture data being transferred per frame.
  - Reduces power consumption.
  - Speeds up rendering.
- Reduce texture memory usage.
- Speed up load times. There is no CPU decoding step in loading: the file data can be copied directly to GPU memory.

DALi supports the KTX file format.

You load the compressed texture just like you would any other image.

~~~{.cpp}
// C++
ResourceImage image = ResourceImage::New("my_compressed_file.ktx");
~~~
~~~{.js}
// JavaScript
var image = new dali.ResourceImage( { url:"my_compressed_file.ktx"});

~~~

### ARMS texture compression tool

http://malideveloper.arm.com/develop-for-mali/tools/asset-creation/mali-gpu-texture-compression-tool/

Here is an example of using the ARM compression tool.

![ ](../assets/img/texture-atlas/compression-options.jpg)
![ ](compression-options.jpg)

![ ](../assets/img/texture-atlas/compression-example.jpg)
![ ](compression-example.jpg)


As shown above the ETC-1 compression format does not support alpha.

As a work around the tool will export the alpha as a separate compressed image.

In order to combine both the images you need to use a custom shader.
Here is an example shader:

~~~{.cpp}
// C++ Code
  const char* const COMPRESSED_RGB_PLUS_SEPARATE_ALPHA_FRAGMENT_SOURCE =
    "\n"
    "void main()\n"
    "{\n"
    "    vec4 v4Color  = (texture2D(sTexture, vTexCoord) * uColor);\n"
    "    v4Color.a =  texture2D(sEffect, vTexCoord ).r;\n"
    "   gl_FragColor = v4Color;"
    "}\n";


  mShaderEffect = ShaderEffect::New( "", COMPRESSED_RGB_PLUS_SEPARATE_ALPHA_FRAGMENT_SOURCE);

  mAtlasImageRGB = ResourceImage::New( ATLAS_RGB_FILENAME.KTX);

  mAtlasImageAlpha = ResourceImage::New( ATLAS_ALPHA_FILENAME.KTX );

  mShaderEffect.SetEffectImage( mAtlasImageAlpha );



  // to create Image Actor
  ImageActor  imageActor = ImageActor::New( mAtlasImageRGB, GetImagePosition( info) );

  imageActor.SetShaderEffect( mShaderEffect );

  imageActor.SetBlendMode(BlendingMode::ON);
~~~

~~~{.js}
// JavaScript code
var fragSource = "  \
void main()                                                   \
{                                                             \
    vec4 v4Color  = (texture2D(sTexture, vTexCoord) * uColor); \
    v4Color.a =  texture2D(sEffect, vTexCoord ).r;             \
   gl_FragColor = v4Color;                                     \
}";

var shaderEffect = new dali.ShaderEffect( "", fragSource);

var atlasImageRGB = new dali.ResourceImage( { url:"ATLAS_RGB_FILENAME.KTX"} );

var atlasImageAlpha = new dali.ResourceImage( { url:"ATLAS_ALPHA_FILENAME.KTX"} );

shaderEffect.setEffectImage( atlasImageAlpha );

// to create Image Actor
ImageActor  imageActor = ImageActor::New( mAtlasImageRGB, GetImagePosition( info) );

imageActor.setShaderEffect( shaderEffect );

imageActor.setBlendMode( dali.BLENDING_ON );
~~~

@class _Guide_Texture_compression


*/
