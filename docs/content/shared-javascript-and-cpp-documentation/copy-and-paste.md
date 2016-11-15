<!--
/**-->

# Copy and Paste  (Selection) {#copy-n-paste}

Text can be selected by a long press or double tapping it.   Depending on certain conditions a popup could be shown giving options including [CUT][COPY][PASTE], [SELECT ALL] or [CLIPBOARD]. Below these conditions will be explained.

[CUT] or [COPY] send the selected text to the clipboard ready to be pasted directly or via the clipboard UI.   Pressing [PASTE] will paste the top item from the clipboard (what has just been copied, possibly from another application).  If the system supports a clipboard UI this can be displayed by pressing the [CLIPBOARD] button.

Empty text means the user has not inputted any text, a text-control containing special characters or purely whitespace is not empty.

Below shows how the popup will look depending on the state of the text-control.

|  |  |
|--|--|
| Condition: Long press/double tap when empty text but clipboard has content  |  Condition: Long press/double tap when text-control contains text |
|[PASTE][CLIPBOARD] buttons shown| [CUT][COPY], [SELECT ALL] unless all text selected and [PASTE][CLIPBOARD] if content to paste. |
|    ![ ](../assets/img/text-controls/EmptyTextClipboardHasContent.png) ![ ](./EmptyTextClipboardHasContent.png) |   ![ ](../assets/img/text-controls/SelectingText.png) ![ ](./SelectingText.png) |
| Condition: Long press/double tap popup when text-control contains just whitespace | Condition: Empty text & clipboard empty |
| Whitespace treated as regular text, [CUT][COPY] shown and [PASTE][CLIPBOARD] if content to paste. As all text is selected there is no need for [SELECT ALL] |  No popup shown after long press/double tap|
|  ![ ](../assets/img/text-controls/SelectAllWhitespace.png) ![ ](./SelectAllWhitespace.png) | ![ ](../assets/img/text-controls/EmptyTextAndNoContentToPaste.png) ![ ](./EmptyTextAndNoContentToPaste.png)|
| Condition: Long press/(double tap) on whitespace which is following text | Condition: Tapping text or panning grab handle |
| [PASTE][CLIPBOARD] shown if something to paste. [SELECT ALL] as more text to select | If content in clipboard [PASTE][CLIPBOARD] popup will be shown. |
| ![ ](../assets/img/text-controls/SelectWhitespaceAfterText.png) ![ ](./SelectWhitespaceAfterText.png) | ![ ](../assets/img/text-controls/TapAfterCopyingText.png) ![ ](./TapAfterCopyingText.png) |


*/
