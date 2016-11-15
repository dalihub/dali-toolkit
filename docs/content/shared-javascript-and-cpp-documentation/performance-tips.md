<!--
/**-->

# Performance Tips {#performancetips}

## High CPU occupancy

  - Try to reduce actor count ( less actors == less processing)
  - Delete any actors that are not visible, or move them off stage
  - Use TextureAtlases ( reduces OpenGL driver calls to glBindTexture
  - Optimise / reduce any constraints used

## High GPU occupancy

  - Reduce visible actor count ( == less draw calls)
  - For 2D UI graphics which require no z sorting you can use

~~~{.cpp}
// C++
// In this mode depth testing is turned off and order is determined by the hierachy (depth-first search order).
// Not always recommended if there is going to be a lot of overdraw ( if lots of actors are on top of each other)

Actor::SetDrawMode( DrawMode::OVERLAY_2D ); // C++
~~~

~~~{.js}
// JavaScript
// In this mode depth testing is turned off and order is determined by the hierachy (depth-first search order).
// Not always recommended if there is going to be a lot of overdraw ( if lots of actors are on top of each other)

actor.drawMode = dali.DRAW_MODE_OVERLAY_2D;
~~~
  - Use TextureAtlases ( reduces state changes in the GPU)
  - Use compressed textures
  - Use lower quality textures, e.g. smaller, lower number of bits per pixel
  - Use Dali::NinePatchImage  where possible.
  - Avoid using too many textures which contain alpha and require blending
  - Avoid using too many Dali::Layer with depth testing enabled. Otherwise the layer has to clear the depth buffer.
  - Optimise any shaders used. Pixel shaders should be kept as lean as possible.


@class _Guide_Performance_Tips

*/


