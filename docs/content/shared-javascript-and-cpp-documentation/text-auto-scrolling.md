<!--
/**-->

# Text auto scrolling {#text-auto-scrolling}

## Overview

Auto TextLabel scrolling enables the text to scroll within the control, it can be used if text exceeds the boundary of the control hence showing the full content.
It will also scroll text that is smaller than the control and ensure the same bit of text is not visible at the same time, this gap can be configured to be larger.

If the number of loops (repetitions) is not set then once triggered to start it will scroll until requested to stop.
If loop count is set to 3 for example it will scroll the text 3 times.

![ ](../assets/img/text-controls/AutoScroll.gif)
![ ](AutoScroll.gif)

### Usage

At version 1.1.35 auto scrolling is only supported in single line, mutliline text will not scroll and Text should be BEGIN aligned.

The ENABLE_AUTO_SCROLL property should be set to TRUE to enable scrolling.

The scroll speed, gap and loop count can be set in the stylesheet or provided by Dali::Handle::SetProperty. See the description of each below.

Once enabled it will start scrolling until the loop count is completed or the ENABLE_AUTO_SCROLL set to false, setting ENABLE_AUTO_SCROLL to false will let the
text complete it's current scrolling loop then stop.

## The additional properties below can be set to customise the scrolling behaviour

#### AUTO_SCROLL_SPEED

This controls the speed of the scrolling, the speed should be provided as pixels/second.

#### AUTO_SCROLL_LOOP_COUNT

This specifies how many times the text will complete a full scroll cycle.
If not set then it will keep scrolling until ENABLE_AUTO_SCROLL is set to false.

Setting ENABLE_AUTO_SCROLL to false will stop scrolling whilst still maintaining the original loop count value for when it is next started.

#### AUTO_SCROLL_GAP

This specifies the amount of whitespace to display before the scrolling text is shown again.

This will be increased if the given value is not large enough to prevent the same bit of text being visible at two locations in the control.

Provide the distance in pixels.

### Scroll Direction

The scroll direction is choosen automatically with the following rules:

If the text is single-lined it will scroll left when the text is Left to Right (LTR) or scroll right if text is Right to Left (RTL).

If the text is multi-lined it will scroll upwards. ( Not supported at 1.1.35 )

### Text Label Scrolling Properties

The properties used by TextLabel for Auto Scrolling are listed [here](@ref TextLabelProperties)

@class TextLabel_Scrolling

*/
