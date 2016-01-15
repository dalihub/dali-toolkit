<!--
/**-->

# Debug Renderer {#debugrenderer}

## Enable debug rendering

Setting DALI_DEBUG_RENDERING environment variable will enable the renderer debuging.

Then, every concrete control renderer( ColorRenderer, BorderRenderer, ImageRenderer, GradientRenderer, etc. ) is replaced with a DebugRenderer object.
Debug renderer renders a simple quad wireframe, so that the control layout and scene structure is clearly displayed.

### Example:
~~~{.bash}
sh-4.1$ DALI_DEBUG_RENDERING=1 /usr/apps/com.samsung.dali-demo/bin/blocks.example
~~~

![ ](../assets/img/debug-renderer/debug-blocks.png)
![ ](debug-blocks.png)


