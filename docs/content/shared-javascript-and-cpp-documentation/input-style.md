<!--
/**-->

# Input Style {#input-style}

The input style can be changed through the control properties. All subsequent characters added will be rendered with the new input style.

Note the input style may change if the cursor is updated by tapping in a new position.

Current supported input style properties are:

- *INPUT_COLOR* Sets the input color. The property expects a Vector4 with the red, green, blue and alpha values clamped between 0 and 1.
- *INPUT_FONT_FAMILY* Sets the input font's family name. The property expects the name of the font. If the new text is not supported by the given font a suitable one will be set.
- *INPUT_FONT_STYLE* Sets the input font's style. The property expects a json formatted string with the font's style. See the [Font Selection](@ref font-selection) section for more details.
- *INPUT_POINT_SIZE* Sets the input font's size. The property expects a float with the font's size in points. See the [Font Selection](@ref font-selection) section for more details.

*/
