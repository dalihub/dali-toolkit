/*! \page texture-atlases Texture Atlases
 *
 * <h2 class="pg">Using Texture Atlases in DALi </h2>
 *
 *
 * <h3> Example demo application </h3>

  \image html image-wall.jpg


<h3> Application above is running slow as there are many small individual images displayed (50)</h3>

<table>
  <tr>
    <td> Launch Time </td>
    <td> Slow </td>
    <td> Has to perform: <br>- 50 file open requests and multiple reads for each image</td>
  </tr>
  <tr>
    <td> Memory Usage </td>
    <td> High </td>
    <td> Has to create:
      <br>- 50 Dali::Image objects
      <br>- 50 OpenGL Textures
    </td>
  </tr>
  <tr>
    <td>Rendering Performance </td>
    <td> Slow </td>
    <td> Has to perform:
      <br>- 50 glBindTexture calls per frame ( each OpenGL calls takes time)
      <br>- 50 a frame = 3000 calls per second @60 FPS.
      <br>- Texture switching is a major state change in the GPU
    </td>
  </tr>
</table>
<br><br>


* <h3> Solutions to problem: Use a Texture Atlas</h3>

A texture atlas is simply one larger image that contains smaller images. A texture atlas is sometimes called a
sprite sheet, bitmap sheet or texture pack.

 \image html atlas.jpg

<br>
Dali::ImageActor has the ability to display a portion of an image using ImageActor::PixelArea setting.
For example to display the first 3 image in the atlas

  \image html example-code.jpg

<h3> Result of using an Atlas</h3>
<table>
  <tr>
    <td> Launch Time </td>
    <td> Fast </td>
    <td> Has to perform: <br>- 1 file open request </td>
  </tr>
  <tr>
    <td> Memory Usage </td>
    <td> Better </td>
    <td> Has to create:
      <br>- 1 Dali::Image objects
      <br>- 1 OpenGL Textures
    </td>
  </tr>
  <tr>
    <td>Rendering Performance </td>
    <td> Fast </td>
    <td> Has to perform:
      <br>- 1 glBindTexture calls per frame ( each OpenGL calls takes time)
      <br>- 1 a frame = 6- calls per second @60 FPS.
    </td>
  </tr>
</table>
<br>
<h2> Atlas creation guide </h2>

Many tools exist for creating texture atlases.<br>
In the following example we are using a tool called TexturePacker as DALi has an exporter script for it.
The exporter automatically generates a source file that has the ImageActor::PixelArea pre-defined.
<br>
<ul>
  <li> Download http://www.codeandweb.com/texturepacker </li>
  <li> Launch TexturePacker </li>
  <li> Go to the menu File -> Preferences</li>
  <li> Set the "Exporter directory" to be the location of dali-toolkit/texture-atlas-exporter  <br></li>
  \image html texture-packer-preferences.jpg
  <br>
  <li> <b> Restart the application! </b></li>
  <li> Select DALi 3D framework for new project</li>
  <br>
  \image html texture-packer-startup.jpg
  <br>
  <li><h3> Create the atlas </h3> <br></li>
  \image html texture-packer-add-sprites.jpg <br><br>
  <li><h3> Click publish to produce the files </h3></li><br><br>
  \image html texture-packer-publish.jpg  <br><br>
</ul>
<h2> Using the generated cpp file </h2>

The generated cpp file contains 3 different ways of describing the atlas. <br>
Copy and paste the section that best suits your application.
<ul><li> Lookup table. Includes code for storing the table in a std::map for fast lookup.</li>
<li> constants.  </li>
<li> JavaScript property map ( see the Dali JavaScript programming guide on how to use it).
</ul>
<h3> Using the lookup table </h3>

Cut and paste the lookup table code into your application.

\code

\\ The following code is automatically generated.
\\
const char* ATLAS_FILE_NAME( "my_first_atlas.png" );  ///< Atlas image filename

/**
 * Structure to hold image name and position within the atlas.
 *
 */
struct ImageInfo
{
  const char* name;
  unsigned int x,y,w,h;
  Dali::BlendingMode::Type blendMode;  // only enable blending if image has alpha
};

/**
 * lookup table
 */
const ImageInfo ImageAtlas[]=
{
 { "blocks-ball", 2, 198, 51, 51, BlendingMode::ON },
 { "bubble-ball", 288, 74, 32, 32, BlendingMode::ON },
 { "gallery-small-52", 2, 2, 128, 128, BlendingMode::OFF },
 { "icon-change", 219, 2, 37, 34, BlendingMode::ON },
 { "icon-cluster-carousel", 180, 2, 37, 34, BlendingMode::ON }
};

const ImageInfo* GetImageInfo(const char* name)
{
  typedef std::map< const char*, const ImageInfo* > LookupMap;
  static LookupMap lookup;
  if( lookup.empty() )
  {
    for( unsigned int i = 0; i < ATLAS_IMAGE_COUNT; ++i)
    {
      lookup[ ImageAtlas[i].name ] =  &ImageAtlas[i];
    }
  }
  LookupMap::const_iterator iter = lookup.find(name);
  if( iter != lookup.end() )
  {
   return (*iter).second;
  }
  DALI_ASSERT_ALWAYS(0 && "image name not found in atlas");
  return NULL;
}

\endcode

To use the lookup table you can do something like this:
\code

// Example function on how to get an image info from the table

std::string fileName = std::string( DALI_IMAGE_DIR ) + ATLAS_FILE_NAME;
Image imageImage = Image::New( fileName );

const ImageInfo* info(NULL);

info = GetImageInfo("blocks-ball");
if( info)
{
  ImageActor ballActor = ImageActor::New( imageAtlas, ImageActor::PixelArea( info->x, info->y, info->w, info->h) );
  ballActor->SetBlendMode( info->blendMode );
}
info = GetImageInfo("bubble-ball");
if( info)
{
  ImageActor bubbleActor = ImageActor::New( imageAtlas, ImageActor::PixelArea( info->x, info->y, info->w, info->h) );
  bubbleActor->SetBlendMode( info->blendMode );
}

\endcode
<h3> Using the constant definitions </h3>

1. Cut and paste the constant definition code into your application.

You'll notice the code below won't compile because C++ variables can't have a dash character.<br>
E.g. BLOCKS-BALL, BUBBLE-BALL will cause errors. Do a search and replace for - and replace with underscores.
This is one reason why using lookup table which holds the filename as a string maybe easier to use.

\code
\\ The following code is automatically generated.
\\
const char* ATLAS_FILE_NAME( "my_first_atlas.png" );

/**
 * Structure to hold position / blend mode within the atlas.
 *
 */
struct ImageInfo
{
  ImageInfo(unsigned int x,unsigned int y,unsigned int w,unsigned int h,  Dali::BlendingMode::Type mode)
  :pixelArea(x,y,w,h),blendMode(mode)
  {}
  ImageActor::PixelArea pixelArea;
  Dali::BlendingMode::Type blendMode;  // only enable blending if image has alpha
};


const ImageInfo BLOCKS-BALL( 2, 198, 51, 51 ,BlendingMode::ON );
const ImageInfo BUBBLE-BALL( 288, 74, 32, 32 ,BlendingMode::ON );
const ImageInfo GALLERY-SMALL-52( 2, 2, 128, 128 ,BlendingMode::OFF );
\endcode

2. To use it, you can copy example code from the generated cpp file which looks
like this
\code
void LoadAtlasImages()
{
  std::string fileName = std::string(DALI_IMAGE_DIR) + ATLAS_FILE_NAME;
  Image atlasImage = Image::New( fileName );
  ImageActor Blocksball = ImageActor::New( atlasImage,  BLOCKS_BALL.pixelArea);
  Blocksball.SetBlendMode( BLOCKS_BALL.blendMode );

  ImageActor Bubbleball = ImageActor::New( atlasImage,  BUBBLE_BALL.pixelArea);
  Bubbleball.SetBlendMode( BUBBLE_BALL.blendMode );
  ...
 \endcode



<br><br>
<h2> Atlas creation tips </h2>

<ul>
  <li> Compress the atlas  - \link Texture_Compression Compressing Textures \endlink <br></li>
  <li> Avoid adding large images to the Atlas.<br>
    E.g. don't add background images to it. Medium to large images should
    be kept seperate. <br><br>
    \image html atlas-size.jpg
    <br><br>
  </li>
  <li> Try to ensure the atlas contains only images that are frequently used. <br>
    There's no point in having images taking up GPU texture memory if they're not displayed.<br>
  </li>
  <li> Avoid very large atlases <br>
    Try to create multiple atlases based on how they are used within your application.<br>
    <br>
    Alternatively Texture packer has the option to split atlases ( search help for Multipack)
  </li>
</ul>



 */

