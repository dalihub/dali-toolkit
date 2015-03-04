/**
 *

## Using Texture Atlases in DALi


### Example demo application

<img src="../assets/img/shared/texture-atlas/image-wall.jpg">


### Application above is running slow as there are many small individual images displayed (50)


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


### Solutions to problem: Use a Texture Atlas

A texture atlas is simply one larger image that contains smaller images.<br>
A texture atlas is sometimes called a sprite sheet, bitmap sheet or texture pack.
<br><br>
<img src="../assets/img/shared/texture-atlas/atlas.jpg">
<br><br>
Dali::ImageActor has the ability to display a portion of an image using ImageActor::PixelArea setting.
For example to display the first 3 image in the atlas
<br><br>
```
var imageAtlas = new dali.ResourceImage( {url:"atlas.png"} )
```
<img src="../assets/img/example-javascript-code.png">
<br>
### Result of using an Atlas
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
## Atlas creation guide

Many tools exist for creating texture atlases.<br>
In the following example we are using a tool called TexturePacker as DALi has an exporter script for it.
The exporter automatically generates a source file that has the ImageActor::PixelArea pre-defined.
<br>

+ Download http://www.codeandweb.com/texturepacker
+ Launch TexturePacker
+ Go to the menu File -> Preferences
+ Set the "Exporter directory" to be the location of <b>dali-toolkit/texture-atlas-exporter</b>  <br>
  
 <img src="../assets/img/shared/texture-atlas/texture-packer-preferences.jpg"> <br>
+ <b> Restart the application! </b>
+ Select DALi 3D framework for new project
  
 <img src="../assets/img/shared/texture-atlas/texture-packer-startup.jpg"><br>
+ <h3> Create the atlas </h3>
   <img src="../assets/img/shared/texture-atlas/texture-packer-add-sprites.jpg">
+ <h3> Click publish to produce the files </h3><br><br>
   <img src="../assets/img/shared/texture-atlas/texture-packer-publish.jpg">
  
## Using the generated cpp file

The generated cpp file contains 3 different ways of describing the atlas.
2 ways are designed for the DALi C++ API, the 3rd way is for JavaScript.
Example exported property map from TexturePacker:
```
var ATLAS_IMAGE_LIST : [
    { name: "add_user_usericon_bg", x: 2, y:109, w:105, h:105, dali.BLENDING_ON  },
    { name: "app_background", x: 180, y:183, w:1, h:1, dali.BLENDING_OFF  },
    { name: "btn_arrow_left", x: 141, y:183, w:11, h:20, dali.BLENDING_ON  },
    { name: "btn_arrow_right", x: 154, y:183, w:11, h:20, dali.BLENDING_ON  },
    { name: "icn_app_foc", x: 194, y:2, w:72, h:72, dali.BLENDING_ON  },
    { name: "icn_app_nor", x: 109, y:109, w:72, h:72, dali.BLENDING_ON  }
    ]
var atlas = new dali.ResourceImage( { url: "atlas_filename.png" });

// display the user icon using the size / position data in the ATLAS_IMAGE_LIST
var userIconData = ATLAS_IMAGE_LIST[0];
var userIconRect = [ userIconData.x, userIconData.y,userIconData.w,userIconData.h];

var btnArrowLeft = new dali.ImageActor( atlas, userIconRect );
```

## Atlas creation tips


 - Compress the atlas
 - Avoid adding large images to the Atlas.
    E.g. don't add background images to it. Medium to large images should
    be kept seperate. <br><br>
   <img src="../assets/img/shared/texture-atlas/atlas-size.jpg">

 - Try to ensure the atlas contains only images that are frequently used. <br>
    There's no point in having images taking up GPU texture memory if they're not displayed.<br>

 - Avoid very large atlases <br>
    Try to create multiple atlases based on how they are used within your application.<br>
    <br>
    Alternatively Texture packer has the option to split atlases ( search help for Multipack)
 -



  @class TextureAtlases
 */

