
/**
 *

## Texture Compression

Using compressing the textures will:

- Speed up rendering in time the GPU == less power used due to less texture data being transferred.
- Reduce texture memory usage.
- Speed up load times. Smaller files mean quicker load times.
  
DALi supports the KTX file format.
You just load the compressed texture like you would any other image.

    var image = new dali.ResourceImage({url:"my_compressed_file.ktx"});

ARMS texture compression tool<br>
http://malideveloper.arm.com/develop-for-mali/tools/asset-creation/mali-gpu-texture-compression-tool/  <br>

Here is an example of using the ARM compression tool.

<img src="../assets/img/shared/texture-atlas/compression-options.jpg">

<img src="../assets/img/shared/texture-atlas/compression-example.jpg">

As shown above the ETC-1 compression format does not support alpha.<br> As a work around the tool will export
the alpha as a seperate compressed image.
In order to combine both the images you need to use a custom shader.
Here is an example shader:
```
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
```
 @class TextureCompression


 */
