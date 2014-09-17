/*! \page performance-tips Performance Tips


<table>
  <tr>
    <th>High CPU occupancy <br></th>
  </tr>
  <tr>
  <td>
       - Try to reduce actor count ( less actors == less processing) <br>
       - Delete any actors that are not visible, or move them off stage <br>
       - Use TextureAtlases ( greatly reduces OpenGL driver calls to glBindTexture <br>
       - Optimise / reduce any constraints used
       </td>
       </tr>
</table>
<br><br>
<table>
  <tr>
    <th>High GPU occupancy <br></th>
  </tr>
  <tr>
  <td>
       - Reduce visible actor count ( == less draw calls) <br>
       - For 2D UI graphics which require no z sorting use @code Actor::SetDrawMode( DrawMode::OVERLAY );
// In this mode depth testing is turned off and order is determined by the hierachy (depth-first search order).
        @endcode
       - Use TextureAtlases ( reduces state changes in the GPU) <br>
       - Use compressed textures
       - Use lower quality textures, e.g. smaller, lower number of bits per pixel
       - Use Dali::NinePatchImage  where possible.
       - Avoid using too many textures which contain alpha and require blending
       - Avoid using too many Dali::Layer with depth testing enabled. Otherwise the layer has to clear the depth buffer.
       - Optimise any shaders used. Pixel shaders should be kept as lean as possible.
       </td>
       </tr>
</table>

 */
