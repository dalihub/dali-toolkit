<!--
/**-->

[TOC]

# DALi JSON Specification  {#script-json-specification}

## Overview {#overview}

This document describes the DALi JSON specification.
The format is not yet formally versioned within the JSON.

# General format {#format}

The JSON format supports

- Named templates for on demand creation of
 - Actors/ Controls
 - Animations
- Style sets
 - Dynamically change the style of Actor hierarchies
 - Animate to a style set
- Includes
 - Build style sets up by merging JSON
- Creating Scene content on load
 - Controls created without any code


Concrete Actors and Controls can be created from types registered in the
DALi Type Registry.

Template, style and scene sections all configure Actors and Controls via
the DALi property system.

The JSON format deviates from the formal JSON specification and allows C style comments.

~~~
    {                                        //
      "version": 0,                          // Version Number
      "includes":                            // Include section
      [                                      //
       "base-theme.json"                     // Include file to merge into the
                                             // JSON
      ]                                      //
      "constants":                           // Constant replacement section
      {                                      //
        ...                                  //
      },                                     //
      "templates":                           // Template library section
      {                                      //
        "basic-text":                        // A named template
        {                                    // } key value properties
         ...                                 //
         "actors":                           // A tree of sub actors
         [                                   //
         ...                                 //
         ]                                   //
        }                                    //
        },                                   //
        "styles":                            // Named Style set section
        {                                    //
        "light-theme":                       // Style set name
        {                                    //
         ...                                 //
        },                                   //
        dark-theme":                         //
        {                                    //
         ...                                 //
        },                                   //
      }                                      //
      "stage":                               // Stage section
      [                                      //
       {                                     // Actors|Controls to create on JSON file load
       "type": "basic-text",                 // A DALi Control or a template name
       "styles":["base-theme","light-theme"] // Style list to apply to this instance
       }                                     //
      ]                                      //
    }                                        //
~~~

# Top level sections {#sections}

## Includes {#includes}

The "includes" section is an array of filenames to be merged in order to
create a final in memory JSON tree.

The merge process will walk key, value attributes from the root JSON
node and overwrite previous values with the newer values.

- If the key does not exist then it will be created.
- If the newer value is a different type then the newer type will persist.
- Where both values are objects the merge will descend into the object.
- Where both values are arrays the merge will replace the old array entirely.

The merge is relatively simple and is expected to be used to build up
from base themes to custom themes where there are few conflicts and
simple direct replacements are desired.

### Constants {#constants}

The merge behaviour when a file has constants and includes is

1. Constants are loaded first
2. Includes file list is merged in order
3. All other (non constant) data is merged

The include merge is recursive, so step (2) above will cause the
constants in the first included file to be merged first.

## Constants {#constantdetail}

The constants section supports sub-string and full property replacement.

~~~
    {
    "constants":                        // Constant replacement section
    {                                   //
      "IMAGES": "/usr/share/images/",   // Constants can be set here or in code.
      "SIZE": [100,100,1]               //
    },                                  //
    ...                                 //
    {                                   //
      "type":"ImageActor"               // An DALi type or a template name
      "image":                          //
      {                                 //
        "filename":"{IMAGES}b.jpg"      // Image filename substring replacement
      },                                //
      "size": "{SIZE}"                  //
    }                                   //  Property replacement
    }                                   //
~~~

The type of the constant should match the expected type of the property.

A property constant cannot be used for sub string replacement; a string
constant should be used.

With a property replacement, the replace site must contain a string
where the first and last positions are braces.

## Templates {#templates}

The template section supports the creation of actor instances. The
template name can be used in code to create an actor tree.

~~~{.js}
// JavaScript

var builder = new dali.Builder();
builder.loadFromFile( "my-app.json");
var userActorTree = builder.create( { template:"basic-text"} );
~~~

~~~{.cpp}
// C++

Builder builder = Builder::New();
std::string jsonData = loadFile("my-app.json");
builder.LoadFromString( jsonData );

actorTree = builder.Create("basic-text");
~~~

Templates consist of a name, multiple property-value configurations and
an optional actor sub hierarchy.

~~~{.json}
   {                                    //
   "templates":                         //  Template library section
   {                                    //
   "basic-text":                        //  The template name
   {                                    //
     "type":"ImageActor",               //  Concrete DALi Type/Class to create
     "styles":["base-style"],           //  Style list to apply
     "name":"image",                    //  }
     "image":                           //  } property name : value
     {                                  //  }
     "filename":"{IMAGES}/b.jpg"        //
     },                                 //
     "parentOrigin": "CENTER"           //
     ...                                //
     "actors":                          //  A tree of sub actors
     [                                  //
     {                                  //
     "type":"TextView"                  //
     "name":"text",                     //
     "text":"Hello World",              //
     "parentOrigin": "CENTER",          //
     }                                  //
     ]                                  //
   }                                    //
   }                                    //
~~~

A template has a special 'type' property which must contain a concrete
DALi Actor or Control type name.

A template has a special 'styles' property which contains a list of
styles to apply when creating using the template.

## Styles {#styles}

The styles section supports a named set of properties that can be
applied to an actor or actor tree.

~~~{.cpp}
// C++

Builder.ApplyStyle("light-theme", myActor);
~~~

~~~{.js}
// JavaScript

builder.applyStyle("light-theme", myActor);
~~~


The styles can also be applied as an animation.

~~~{.cpp}
Builder.AnimateTo("light-theme", myActor, TimePeriod(0, 10));
~~~



~~~
   {                                   //
   "styles":                           // Style set section
   {                                   //
   "light-theme":                      // Style-set name
   {                                   //
     "color":[1,1,1,1]                 // }
     "position":[0,-120,0],            // } properties to set on the given actor
     "rotation":[0,0,30],              // }
     "actors":                         //
     {                                 // Sub Actors are referenced by name
       "title-text":                   // Actor name to search for under given actor
       {                               //
         "color":[1,1,1,1]             // }
         "position":[0,-120,0],        // } properties to set if 'title-text' is found
         "rotation":[0,0,30],          // }
       }
     },                                //
     "icon":                           //
     {                                 //
       "color":[1,1,1,1]               //
     }                                 //
    },                                 //
    "dark-theme":                      //
    {                                  //
    }                                  //
   }                                   //
~~~

When applied to an actor tree the actors are referenced by name. Names
are not unique in Dali.

When a style is applied in code DALi will perform a depth first search
stopping with the first matching name.

Typically an application developer will apply the style to the template
root actor and not the stage root actor. Therefore in most uses cases
name conflicts are not expected.

## Animations {#animations}

The animation section defines a library of animation definitions.

The animations can be created by name from code.

They can also be created automatically from JSON in an actor signal.

~~~
    {                                    //
    "animations":                        // Animation library
    {                                    //
     "rotate":                           // An Animation named rotate
     {                                   //
     "duration": 10,                     // Duration in seconds
     "loop": false,                      // Whether to loop.
     "endAction": "Bake",                // Whether to set final value(bake) or
                                         // reset
     "disconnectAction": "Discard",      // Whether 'Bake' or 'Discard' when disconnected
     "properties":
     [
                                         // Properties changed in this animation
     {
     "actor":"image",                    // Actor found by name from the stage
     "property":"rotation",              // Property to change
     "value":[0, 0.1, 0, 0],             // Value to set
     "alphaFunction": "EASE\_IN\_OUT",   // Interpolation function
                                         //
     "timePeriod":                       // Time period for change
     {"delay": 0,
      "duration": 3
      }
     },
     ...                                 // 1 or more property changes per animation
     },                                  //
     ...                                 //
    },                                   //
~~~

### Splines {#splines}

An animation property can be defined with a path and forward direction
instead of a single final value.

Paths are defined in a top level path section and referenced by the
animation property.

~~~
    {                                    //
    "paths":                             // Path library
     {                                   //
     "path0":                            // Path definition by name
     {                                   //
     "points":[                          // points
       [-150, -50, 0],
       [0.0,70.0,0.0],
       [190.0,-150.0,0.0]
      ],
                                         // curvature automatically creates
     "curvature":0.35,                   // controlPoints
                                         //
     "controlPoints": [...]              // Otherwise controlPoints can be
                                         // directly specified.
     }                                   //
     },                                  //
    "animations":                        //
    {                                    //
     "pathAnimation":
     {
     "duration": 3.0,
     "properties":
     [
     {
     "actor": "greeting2",
                                         // Path is mandatory for spline
     "path":"path0",                     // animation.
     "forward":[1,0,0],                  // Forward vector specifies orientation
                                         // whilst travelling along the path
     "alphaFunction": "EASE\_IN\_OUT",   // (optional)
     "timePeriod":
     {
     "delay": 0,
     "duration": 3
     }
     },
     ...
                                         // Other properties changes can use
     ]                                   // paths or values in the same
                                         // animation.
    },                                   //
    }                                    //
~~~

## Shaders {#shaders}

The shader section of the JSON file defines a library of shader effect
instances that are created on demand.

The shaders are referred to by name from the template, style, stage or
animation sections.

Multiple actors can set the same shader as the name refers to a single
instance.

Similarly one named shader instance can be set to several actors and can
be animated by one animation.

~~~
    {                                             //
    "shaderEffects":                              // Shader Effect section
    {                                             //
      "myshader1":                                // Shader  instance  name
      {                                           //
       "program":                                 //
       {                                          // Prefixs are placed before DALi uniforms.
         "vertexPrefix": "",                      // (Useful for \#defines.)
         "vertex":"",                             // Glsl vertex program
         "fragmentPrefix": "",
         "fragment": "",                          // Glsl fragment program.
         "geometryType": "GEOMETRY_TYPE_IMAGE",   // Geometry type(see DALi documentation)
       },
       "geometryHints": "HINT_NONE":              // Geometry hints (see DALi documentation)
       "gridDensity": 0,                          // Grid density(see DALi documentation)
       "image":
       {
         "filename": ""                           // Effect image available as a second texture unit.
       }
     },
     ...
    },
    "stage":
    [{
     "type": "ImageActor",
     "effect": "myshader1",
     ...
    }]
    }
~~~

At least one of the vertex or fragment fields is mandatory. All
other fields are optional will use internal defaults.

Actors have an "effect" field that refers to the shader effect
instance to use with that actor.

### Animating shaders {#animatingshaders}

Shader uniforms can be animated as if they are properties of the actor.

When the animation is created from code (or from a signal) the property
name search begins on the actor, if it isn't found the search continues
on the attached shader object.

The actor property names and shader uniform names must not clash for the
uniform to animate correctly. The convention in DALi is to prepend
uniforms with 'u'.

~~~
    {                                    \\
    "animations":                        \\ Animation library
    {                                    \\
     "rotate":                           \\ An Animation named rotate
     {                                   \\
     "duration": 10,                     \\
     "properties":                       \\ Properties changed in this animation
     [                                   \\
     {
     "actor":"image",                    \\ Actor found by name from the stage
     "property":"uTranslate",            \\ Uniform name specified as if it is a
                                         \\ property of the object.
     "value":[10, 20],
     ...
     },
     ...
     ]
     },
     "shaderEffects":
     {
     "myshader1":
     {
                                        \\ Shader program with uniform
     "program": {...}                   \\ "uTranslate"
     }
     },
     "actors":
     [
     {
     "name": "image",
     "effect": "myshader1"              \\ Actor using shader effect instance
                                        \\ "myshader1"
     }
     ]
    },
~~~

## Stage {#stage}

The stage section supports the immediate creation of actors at the time
the JSON is loaded.

The stage is a tree of actors that can be added to Dali's stage object.

~~~
// C++
builder = Dali.Builder();
json_text = load("layout.json");
builder.Load(json\_text);
stage = Dali.Stage.GetCurrent();
builder.AddActors( stage.GetRootLayer()); // Add actors to the stage root layer
~~~

~~~{.js}
// JavaScript

builder.addActors( dali.stage.getRootLayer() );
~~~

~~~
    {                                    \\
    "stage":                             \\  Stage Section Number
    [                                    \\  An array of actors
     {
     "type": "ImageActor",
     ...
     "actors":                           \\  Each actor can have children
                                         \\ creating a tree
     [
     {
     "type": "TextView",
                                         \\  The Type to create; this can be a
     ...                                 \\ concrete DALi type (actor/control)
                                         \\ or a template name.
     "styles": ["base-style"]
                                         \\  A list of styles to apply to the
     }                                   \\ created type.
     ]
     }
    ]
    }
~~~

# Actor and Control Properties {#actorprop}

Each control has a set of supported properties documented in the "Dali
UI Control Specification".

Please refer to the above document for further information about specific
controls.

@class _Guide_JSON_Specification
*/
