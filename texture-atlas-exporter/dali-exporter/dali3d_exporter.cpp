// Created with TexturePacker (http://www.codeandweb.com/texturepacker)
// DALi Exporter: nick.holland@partner.samsung.com
//
// {{smartUpdateKey}}

// For your application cut and paste either:
//
// 1. Lookup table.
// 2. Constants.
// 3. JavaScript property map for using with DALi JS.

// Note: If you use one option, then delete code for the other two




//
// 1.  ------  lookup table method  ------
//
// Handy if you want to get image with a postfix, e.g. image_1, image_2, image_3
// Or if some of the image names contain special characters which are not allowed
// in constant definitions ( e.g. spaces and full stops).


const char* ATLAS_FILE_NAME( "{{texture.fullName}}" );  ///< Atlas image filename


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
{% for sprite in allSprites %} { "{{sprite.trimmedName}}", {{sprite.frameRect.x}}, {{sprite.frameRect.y}}, {{sprite.frameRect.width}}, {{sprite.frameRect.height}}, {%if sprite.isSolid %}BlendingMode::OFF{% else%}BlendingMode::ON{% endif %} }{% if not forloop.last %},{% endif %}
{% endfor %}
};

const unsigned int ATLAS_IMAGE_COUNT = sizeof(ImageAtlas)/sizeof(ImageAtlas[0]);

// Example function on how to get an image info from the table
//
// std::string fileName = std::string( DALI_IMAGE_DIR ) + ATLAS_FILE_NAME;
// Image imageAtlas = Image::New( fileName );
//
//
//  const ImageInfo* info = GetImageInfo("left_icon");
//
//  if( info)
//  {
//     ImageActor myActor = ImageActor::New( imageAtlas, ImageActor::PixelArea( info->x, info->y, info->w, info->h) );
//     myActor->SetBlendMode( info->blendMode );
//
//

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

//
//
// 2. ------ constants code ------
//
//

const char* ATLAS_FILE_NAME( "{{texture.fullName}}" );

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
  Dali::BlendingMode::Type blendMode;  // only enable blending if image had alpha
};

{% for sprite in allSprites %}const ImageInfo {{ sprite.trimmedName|upper}}( {{sprite.frameRect.x}}, {{sprite.frameRect.y}}, {{sprite.frameRect.width}}, {{sprite.frameRect.height}} ,{%if sprite.isSolid %}BlendingMode::OFF{% else%}BlendingMode::ON{% endif %} );
{% endfor %}


// Example on using the Atlas, please delete this code.
void LoadAtlasImages()
{
  std::string fileName = std::string(DALI_IMAGE_DIR) + ATLAS_FILE_NAME;
  Image atlasImage = Image::New( fileName );
  {% for sprite in allSprites %}ImageActor {{sprite.trimmedName|capfirst}} = ImageActor::New( atlasImage,  {{sprite.trimmedName|upper}}.pixelArea);
  {{sprite.trimmedName|capfirst}}.SetBlendMode( {{sprite.trimmedName|upper}}.blendMode );

  {% endfor %}
}

//
//
// 3.  ------  JavaScript key/value lookup table   ------
//
//
//

ATLAS_IMAGE_LIST : [
{% for sprite in allSprites %} { name: "{{sprite.trimmedName}}", x: {{sprite.frameRect.x}}, y:{{sprite.frameRect.y}}, w:{{sprite.frameRect.width}}, h:{{sprite.frameRect.height}}, blendMode:{%if sprite.isSolid %}dali.BLENDING_OFF{% else%}dali.BLENDING_ON{% endif %}  }{% if not forloop.last %},{% endif %}
{% endfor %}
]
