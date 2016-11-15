<!--
/**-->

# Debug Visual {#debugvisual}

## Enable debug rendering

Setting DALI_DEBUG_RENDERING environment variable will enable the visual debuging.

Then, every concrete visual ( ColorVisual, BorderVisual, ImageVisual, GradientVisual, etc. ) is replaced with a DebugVisual object.
Debug visual renders a simple quad wireframe, so that the control layout and scene structure is clearly displayed.

### Example:
~~~{.bash}
sh-4.1$ DALI_DEBUG_RENDERING=1 /usr/apps/com.samsung.dali-demo/bin/blocks.example
~~~

![ ](../assets/img/debug-visual/debug-blocks.png)
![ ](debug-blocks.png)


