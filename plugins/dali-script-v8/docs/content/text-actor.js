/**
## Text Actor API
TextActor is a basic actor for displaying a text label

By default the text actor always uses the natural size of the text, when the text property is set,
unless SetSize is called to override the size or size is animated to some other size.

Natural size for TextActor is the same as the size returned by dali.font.MeasureText( string )
using the font that the TextActor is using.

By default {{#crossLink "RenderableActor/setCullFace:method"}}CullFaceMode{{/crossLink}} is set to CullNone to enable the TextActor to be viewed from all angles.

### Simple example
```
var textActor = new dali.TextActor( "Hello world" );
  
// by default an actor is anchored to the top-left of it's parent actor
// change it to the middle
textActor.parentOrigin = [0.5,0.5,0.5];
  
// scale it up by 4 times  in x,y
textActor.scale = [ 4, 4, 1  ];
  
// add to the stage
dali.stage.add( textActor );
```

### Example using a font and text options ###
```
var fontInfo =
{
  family : "Arial",
  style :"Bold",
  pointSize:20
}
  
var arialFont = new dali.Font( fontInfo );
var textOptions =
{
  font: arialFont,
  isRightToLeft: true,
  fontDetection: true       // default is true
}
  
var textActor1 = new dali.TextActor( "Hello-world" , textOptions );
  
// this will automatically chose a different font that can display the text
var textActor2 = new dali.TextActor( "繁體中文" , textOptions );
```

| Name                   |    Type    | Writable     | Animatable|
|------------------------|------------|--------------|-----------|
| text                   |  STRING    | &#10004;     |&#10008;   |
| font                   |  STRING    | &#10004;     |&#10008;   |
| font-style             |  STRING    | &#10004;     |&#10008;   |
| outline-enable         |  BOOLEAN   | &#10004;     |&#10008;   |
| outline-color          |  VECTOR4   | &#10004;     |&#10008;   |
| outline-thickness-width| VECTOR2    | &#10004;     |&#10008;   |
| smooth-edge            | FLOAT      | &#10004;     |&#10008;   |
| glow-enable            | BOOLEAN    | &#10004;     |&#10008;   |
| glow-color             | VECTOR4    | &#10004;     |&#10008;   |
| glow-intensity         | FLOAT      | &#10004;     |&#10008;   |
| shadow-enable          | BOOLEAN    | &#10004;     |&#10008;   |
| shadow-color           | VECTOR4    | &#10004;     |&#10008;   |
| shadow-offset          | VECTOR2    | &#10004;     |&#10008;   |
| italics-angle          | FLOAT      | &#10004;     |&#10008;   |
| underline              | BOOLEAN    | &#10004;     |&#10008;   |
| weight                 | INTEGER    | &#10004;     |&#10008;   |
| font-detection-automati| BOOLEAN    | &#10004;     |&#10008;   |
| gradient-color         | VECTOR4    | &#10004;     |&#10008;   |
| gradient-start-point   | VECTOR2    | &#10004;     |&#10008;   |
| gradient-end-point     | VECTOR2    | &#10004;     |&#10008;   |
| shadow-size            |  FLOAT     | &#10004;     |&#10008;   |
| text-color             |  VECTOR4   | &#10004;     |&#10008;   |

@class TextActor
@extends RenderableActor
 */

