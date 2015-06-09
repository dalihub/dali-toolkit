<!--
/**-->

# Texture Atlases {#textureatlases}

## Example demo application

![ ](../assets/img/texture-atlas/image-wall.jpg)
![ ](image-wall.jpg)
  

Application above is running slow as there are many small individual images displayed (50)
  
| Metric | Result | Explanation |
|--------|--------|-------------|
| Launch time | Slow | Has to perform: 50 file open requests and multiple reads for each image |
| Memory consumption|  High | Has to create:50 Dali::Image objects,50 OpenGL Textures|
| Rendering | Slow | Has to perform: 50 glBindTexture calls per frame ( each OpenGL calls takes time) 50 a frame = 3000 calls per second @60 FPS.Texture switching is a major state change in the GPU|
  


## Solutions to problem: Use a Texture Atlas

A texture atlas is simply one larger image that contains smaller images. A texture atlas is sometimes called a
sprite sheet, bitmap sheet or texture pack.

![ ](../assets/img/texture-atlas/atlas.jpg)
![ ](atlas.jpg)
  
Dali::ImageActor has the ability to display a portion of an image using ImageActor::PixelArea setting.
For example to display the first 3 image in the atlas
  
![ ](../assets/img/texture-atlas/example-javascript-code.jpg)
![ ](example-code.jpg)

### Result of using an Atlas

| Metric | Result | Explanation |
|--------|--------|-------------|
| Launch time | Fast | Has to perform: - 1 file open request  |
| Memory consumption|  Low | Has to create: 1 Dali::Image objects 1 OpenGL Textures|
| Rendering | Fast | HHas to perform: 1 glBindTexture calls per frame ( each OpenGL calls takes time) 1 a frame = 6- calls per second @60 FPS.|

  
## Atlas creation guide

Many tools exist for creating texture atlases.
In the following example we are using a tool called TexturePacker as DALi has an exporter script for it.
The exporter automatically generates a source file that has the ImageActor::PixelArea pre-defined.
  
- Download http://www.codeandweb.com/texturepacker
- Launch TexturePacker
- Go to the menu File -> Preferences
- Set the "Exporter directory" to be the location of dali-toolkit/texture-atlas-exporter
  
![ ](../assets/img/texture-atlas/texture-packer-preferences.jpg)
![ ](texture-packer-preferences.jpg)
  
- Restart the application!
- Select DALi 3D framework for new project
  
![ ](../assets/img/texture-atlas/texture-packer-startup.jpg)
![ ](texture-packer-startup.jpg)
  
- Create the atlas
![ ](../assets/img/texture-atlas/texture-packer-add-sprites.jpg)
![ ](texture-packer-add-sprites.jpg)
- Click publish to produce the files
![ ](../assets/img/texture-atlas/texture-packer-publish.jpg)
![ ](texture-packer-publish.jpg)



## Using the generated cpp ( contains JavaScript code as well)

The generated cpp file contains 3 different ways of describing the atlas.
Copy and paste the section that best suits your application.
-  Lookup table. Includes code for storing the table in a std::map for fast lookup.
- constants.
- JavaScript property map

### Using the JavaScript generated property map

The property map should be cut and paste in to your application. It just looks like
  
~~~{.js}
var ATLAS_IMAGE_LIST : [
    { name: "add_user_usericon_bg", x: 2, y:109, w:105, h:105,  blendMode:dali.BLENDING_ON  },
    { name: "app_background", x: 180, y:183, w:1, h:1,  blendMode:dali.BLENDING_OFF  },
    { name: "btn_arrow_left", x: 141, y:183, w:11, h:20,  blendMode:dali.BLENDING_ON  },
    { name: "btn_arrow_right", x: 154, y:183, w:11, h:20,  blendMode:dali.BLENDING_ON  },
    { name: "icn_app_foc", x: 194, y:2, w:72, h:72,  blendMode:dali.BLENDING_ON  },
    { name: "icn_app_nor", x: 109, y:109, w:72, h:72, blendMode:dali.BLENDING_ON  }
    ]
var atlas = new dali.ResourceImage( { url: "atlas_filename.png" });

// display the user icon using the size / position data in the ATLAS_IMAGE_LIST
var userIconData = ATLAS_IMAGE_LIST[0];
var userIconRect = [ userIconData.x, userIconData.y,userIconData.w,userIconData.h];

var btnArrowLeft = new dali.ImageActor( atlas, userIconRect );
btnArrowLeft.setBlendMode(userIconData.blendMode);

~~~

![ ](example-javascript-code.jpg)


### Using the lookup table in C++

Cut and paste the lookup table code into your application.

~~~{.cpp}

// The following code is automatically generated when TexturePacker publishes to a cpp file.
const char* ATLAS_FILE_NAME( "my_first_atlas.png" );  ///< Atlas image filename

// Structure to hold image name and position within the atlas.
struct ImageInfo
{
  const char* name;
  unsigned int x,y,w,h;
  Dali::BlendingMode::Type blendMode;  // only enable blending if image has alpha
};


// lookup table
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

~~~

To use the lookup table you can do something like this:

~~~{.cpp}
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

~~~

### Using the constant definitions (C++)

1. Cut and paste the constant definition code into your application.

You'll notice the code below won't compile because C++ variables can't have a dash character.
E.g. BLOCKS-BALL, BUBBLE-BALL will cause errors. Do a search and replace for - and replace with underscores.
This is one reason why using lookup table which holds the filename as a string maybe easier to use.
  
~~~{.cpp}

// The following code is automatically generated when TexturePacker publishes to a cpp file.
const char* ATLAS_FILE_NAME( "my_first_atlas.png" );


// Structure to hold position / blend mode within the atlas.
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
~~~
  
  2. To use it, you can copy example code from the generated cpp file which looks
like this

~~~{.cpp}
void LoadAtlasImages()
{
  std::string fileName = std::string(DALI_IMAGE_DIR) + ATLAS_FILE_NAME;
  Image atlasImage = Image::New( fileName );
  ImageActor Blocksball = ImageActor::New( atlasImage,  BLOCKS_BALL.pixelArea);
  Blocksball.SetBlendMode( BLOCKS_BALL.blendMode );

  ImageActor Bubbleball = ImageActor::New( atlasImage,  BUBBLE_BALL.pixelArea);
  Bubbleball.SetBlendMode( BUBBLE_BALL.blendMode );
  ...
~~~


## Atlas creation tips

- Compress the atlas  - \link texturecompression Compressing Textures \endlink
- Avoid adding large images to the Atlas.
- E.g. don't add background images to it. Medium to large images should be kept seperate.
  
![ ](../assets/img/texture-atlas/atlas-size.jpg)
![ ](atlas-size.jpg)
  

- Try to ensure the atlas contains only images that are frequently used.  There's no point in having images taking up GPU texture memory if they're not displayed.
- Avoid very large atlases.   Try to create multiple atlases based on how they are used within your application.
Alternatively Texture packer has the option to split atlases ( search help for Multipack)



@class _Guide_TextureAtlases

*/