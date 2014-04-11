#ifndef __TCT_DALI_TOOLKIT_CORE_H__
#define __TCT_DALI_TOOLKIT_CORE_H__

#include "testcase.h"

extern void utc_dali_toolkit_alignment_startup(void);
extern void utc_dali_toolkit_alignment_cleanup(void);
extern void utc_dali_toolkit_bubble_emitter_startup(void);
extern void utc_dali_toolkit_bubble_emitter_cleanup(void);
extern void utc_dali_toolkit_button_startup(void);
extern void utc_dali_toolkit_button_cleanup(void);
extern void utc_dali_toolkit_control_startup(void);
extern void utc_dali_toolkit_control_cleanup(void);
extern void utc_dali_toolkit_control_impl_startup(void);
extern void utc_dali_toolkit_control_impl_cleanup(void);
extern void utc_dali_toolkit_default_controls_startup(void);
extern void utc_dali_toolkit_default_controls_cleanup(void);
extern void utc_dali_toolkit_dissolve_effect_startup(void);
extern void utc_dali_toolkit_dissolve_effect_cleanup(void);
extern void utc_dali_toolkit_focus_manager_startup(void);
extern void utc_dali_toolkit_focus_manager_cleanup(void);
extern void utc_dali_toolkit_grid_layout_startup(void);
extern void utc_dali_toolkit_grid_layout_cleanup(void);
extern void utc_dali_toolkit_iris_effect_startup(void);
extern void utc_dali_toolkit_iris_effect_cleanup(void);
extern void utc_dali_toolkit_item_view_startup(void);
extern void utc_dali_toolkit_item_view_cleanup(void);
extern void utc_dali_toolkit_keyboard_focus_manager_startup(void);
extern void utc_dali_toolkit_keyboard_focus_manager_cleanup(void);
extern void utc_dali_toolkit_markup_processor_startup(void);
extern void utc_dali_toolkit_markup_processor_cleanup(void);
extern void utc_dali_toolkit_mask_effect_startup(void);
extern void utc_dali_toolkit_mask_effect_cleanup(void);
extern void utc_dali_toolkit_nine_patch_startup(void);
extern void utc_dali_toolkit_nine_patch_cleanup(void);
extern void utc_dali_toolkit_popup_startup(void);
extern void utc_dali_toolkit_popup_cleanup(void);
extern void utc_dali_toolkit_pushbutton_startup(void);
extern void utc_dali_toolkit_pushbutton_cleanup(void);
extern void utc_dali_toolkit_ripple_2d_effect_startup(void);
extern void utc_dali_toolkit_ripple_2d_effect_cleanup(void);
extern void utc_dali_toolkit_ripple_effect_startup(void);
extern void utc_dali_toolkit_ripple_effect_cleanup(void);
extern void utc_dali_toolkit_scroll_view_effect_startup(void);
extern void utc_dali_toolkit_scroll_view_effect_cleanup(void);
extern void utc_dali_toolkit_super_blur_view_startup(void);
extern void utc_dali_toolkit_super_blur_view_cleanup(void);
extern void utc_dali_toolkit_swirl_effect_startup(void);
extern void utc_dali_toolkit_swirl_effect_cleanup(void);
extern void utc_dali_toolkit_text_input_startup(void);
extern void utc_dali_toolkit_text_input_cleanup(void);
extern void utc_dali_toolkit_text_view_startup(void);
extern void utc_dali_toolkit_text_view_cleanup(void);

extern int UtcDaliAlignmentConstructorNegative(void);
extern int UtcDaliAlignmentConstructorPositive(void);
extern int UtcDaliAlignmentConstructorRegister(void);
extern int UtcDaliAlignmentSetAlignmentTypePositiveOffStage(void);
extern int UtcDaliAlignmentSetAlignmentTypePositiveOnStage(void);
extern int UtcDaliAlignmentSetAlignmentTypeNegative(void);
extern int UtcDaliAlignmentGetAlignmentType(void);
extern int UtcDaliAlignmentSetScaling(void);
extern int UtcDaliAlignmentGetScaling(void);
extern int UtcDaliAlignmentSetPaddingPositive(void);
extern int UtcDaliAlignmentSetPaddingNegative(void);
extern int UtcDaliAlignmentGetPadding(void);
extern int UtcDaliAlignmentChildAddAndRemove(void);
extern int UtcDaliAlignmentOnSizeSet(void);
extern int UtcDaliAlignmentOnTouchEvent(void);
extern int UtcDaliAlignmentOnKeyEvent(void);
extern int UtcDaliAlignmentOnSizeAnimation(void);
extern int UtcDaliAlignmentCopyAndAssignment(void);
extern int UtcDaliBubbleEmitterNew(void);
extern int UtcDaliBubbleEmitterGetRootActor(void);
extern int UtcDaliBubbleEmitterSetBackground(void);
extern int UtcDaliBubbleEmitterSetShapeImage(void);
extern int UtcDaliBubbleEmitterSetBubbleScale(void);
extern int UtcDaliBubbleEmitterSetBubbleDensity01(void);
extern int UtcDaliBubbleEmitterSetBubbleDensity02(void);
extern int UtcDaliBubbleEmitterSetBlendMode(void);
extern int UtcDaliBubbleEmitterEmitBubble(void);
extern int UtcDaliBubbleEmitterStartExplosion(void);
extern int UtcDaliBubbleEmitterRestore(void);
extern int UtcDaliButtonNew(void);
extern int UtcDaliButtonSetProperty(void);
extern int UtcDaliButtonSetGetDimmed(void);
extern int UtcDaliButtonSize(void);
extern int UtcDaliButtonClicked(void);
extern int UtcDaliButtonConnectSignal(void);
extern int UtcDaliButtonSetGetAnimationTime(void);
extern int UtcDaliControlConstructor(void);
extern int UtcDaliControlNew(void);
extern int UtcDaliControlRegister(void);
extern int UtcDaliControlCopyAndAssignment(void);
extern int UtcDaliControlDownCast(void);
extern int UtcDaliControlDownCastTemplate(void);
extern int UtcDaliControlKeyInputFocus(void);
extern int UtcDaliControlGetImplementation(void);
extern int UtcDaliControlSignalConnectDisconnect(void);
extern int UtcDaliControlSignalAutomaticDisconnect(void);
extern int UtcDaliControlTestParameters(void);
extern int UtcDaliControlImplNew(void);
extern int UtcDaliControlImplEnableGestureDetector(void);
extern int UtcDaliControlImplDisableGestureDetector(void);
extern int UtcDaliControlImplOnGestureMethods(void);
extern int UtcDaliControlImplChildAddAndRemove(void);
extern int UtcDaliControlImplStageConnection(void);
extern int UtcDaliControlImplSizeSet(void);
extern int UtcDaliControlImplSizeAnimation(void);
extern int UtcDaliControlImplTouchEvent(void);
extern int UtcDaliControlImplKeyEvent(void);
extern int UtcDaliControlImplStyleChange(void);
extern int UtcDaliControlImplKeyInputFocusGained(void);
extern int UtcDaliControlImplKeyInputFocusLost(void);
extern int UtcDaliDefaultControlsCreateSolidColorActor(void);
extern int UtcDaliDefaultControlsCreatePushButton(void);
extern int UtcDaliDissolveUninitializedEffect(void);
extern int UtcDaliDissolvePropertyNamesEffect(void);
extern int UtcDaliDissolveDefaultValuesEffect(void);
extern int UtcDaliDissolveCustomValuesEffect(void);
extern int UtcDaliSetEffectImageEffect(void);
extern int UtcDaliFocusManagerGet(void);
extern int UtcDaliFocusManagerSetAndGetAccessibilityAttribute(void);
extern int UtcDaliFocusManagerSetAndGetFocusOrder(void);
extern int UtcDaliFocusManagerGenerateNewFocusOrder(void);
extern int UtcDaliFocusManagerGetActorByFocusOrder(void);
extern int UtcDaliFocusManagerSetAndGetCurrentFocusActor(void);
extern int UtcDaliFocusManagerGetCurrentFocusGroup(void);
extern int UtcDaliFocusManagerGetCurrentFocusOrder(void);
extern int UtcDaliFocusManagerMoveFocusForward(void);
extern int UtcDaliFocusManagerMoveFocusBackward(void);
extern int UtcDaliFocusManagerClearFocus(void);
extern int UtcDaliFocusManagerReset(void);
extern int UtcDaliFocusManagerFocusGroup(void);
extern int UtcDaliFocusManagerSetAndGetFocusIndicator(void);
extern int UtcDaliFocusManagerSignalFocusChanged(void);
extern int UtcDaliFocusManagerSignalFocusOvershot(void);
extern int UtcDaliGridLayoutNew(void);
extern int UtcDaliGridLayoutSetAndGetNumberOfColumns(void);
extern int UtcDaliGridLayoutSetAndGetRowSpacing(void);
extern int UtcDaliGridLayoutSetAndGetColumnSpacing(void);
extern int UtcDaliGridLayoutSetAndGetTopMargin(void);
extern int UtcDaliGridLayoutSetAndGetBottomMargin(void);
extern int UtcDaliGridLayoutSetAndGetSideMargin(void);
extern int UtcDaliGridLayoutSetAndGetZGap(void);
extern int UtcDaliGridLayoutSetAndGetItemSizeFunction(void);
extern int UtcDaliGridLayoutSetAndGetScrollSpeedFactor(void);
extern int UtcDaliGridLayoutSetAndGetMaximumSwipeSpeed(void);
extern int UtcDaliGridLayoutSetAndGetItemFlickAnimationDuration(void);
extern int UtcDaliGridLayoutConstraintLeft(void);
extern int UtcDaliGridLayoutConstraintRight(void);
extern int UtcDaliGridLayoutConstraintUp(void);
extern int UtcDaliGridLayoutConstraintDown(void);
extern int UtcDaliGridLayoutScrollDirection(void);
extern int UtcDaliIrisEffectUninitialized(void);
extern int UtcDaliIrisEffectPropertyNames(void);
extern int UtcDaliIrisEffectDefaultValues(void);
extern int UtcDaliIrisEffectCustomValues(void);
extern int UtcDaliItemLayoutSetAndGetOrientation(void);
extern int UtcDaliItemLayoutGetScrollHints(void);
extern int UtcDaliItemViewNew(void);
extern int UtcDaliItemViewDownCast(void);
extern int UtcDaliItemViewAddAndGetLayout(void);
extern int UtcDaliItemViewAddAndRemoveLayout(void);
extern int UtcDaliItemViewActivateLayoutAndGetActiveLayout(void);
extern int UtcDaliItemViewDeactivateCurrentLayout(void);
extern int UtcDaliItemViewGetItemAndGetItemId(void);
extern int UtcDaliItemViewRemoveItem(void);
extern int UtcDaliItemViewGetCurrentLayoutPosition(void);
extern int UtcDaliItemViewSetAndGetMinimumSwipeSpeed(void);
extern int UtcDaliItemViewSetAndGetMinimumSwipeDistance(void);
extern int UtcDaliItemViewSetAndGetAnchoring(void);
extern int UtcDaliItemViewSetAndGetAnchoringDuration(void);
extern int UtcDaliItemViewSetAndGetRefreshInterval(void);
extern int UtcDaliItemViewScrollToItem(void);
extern int UtcDaliItemViewSetAndGetMouseWheelScrollDistanceStep(void);
extern int UtcDaliKeyboardFocusManagerGet(void);
extern int UtcDaliKeyboardFocusManagerSetAndGetCurrentFocusActor(void);
extern int UtcDaliKeyboardFocusManagerMoveFocus(void);
extern int UtcDaliKeyboardFocusManagerClearFocus(void);
extern int UtcDaliKeyboardFocusManagerSetAndGetFocusGroupLoop(void);
extern int UtcDaliKeyboardFocusManagerSetAsFocusGroup(void);
extern int UtcDaliKeyboardFocusManagerGetFocusGroup(void);
extern int UtcDaliKeyboardFocusManagerSetAndGetFocusIndicator(void);
extern int UtcDaliKeyboardFocusManagerSignalFocusedActorActivated(void);
extern int UtcDaliMarkupProcessor(void);
extern int UtcDaliMarkupProcessorSetTextStyle01(void);
extern int UtcDaliMarkupProcessorSetTextStyle02(void);
extern int UtcDaliMarkupProcessorTestColors(void);
extern int UtcDaliMaskEffectCreateEffect(void);
extern int UtcDaliMaskEffectDestructor(void);
extern int UtcDaliNinePatchMaskEffectApply(void);
extern int UtcDaliPopupNew(void);
extern int UtcDaliPopupDestructor(void);
extern int UtcDaliPopupDownCast(void);
extern int UtcDaliPopoupSetProperty(void);
extern int UtcDaliPopupSetBackgroundImage(void);
extern int UtcDaliPopupSetTitle(void);
extern int UtcDaliPopupSetTitleText(void);
extern int UtcDaliPopupAddButton(void);
extern int UtcDaliPopupSetState(void);
extern int UtcDaliPopupSetStateSlow(void);
extern int UtcDaliPopupShowHide(void);
extern int UtcDaliPopupShowHideTail(void);
extern int UtcDaliPopupOnTouchedOutside(void);
extern int UtcDaliPushButtonSetGetAutoRepeating(void);
extern int UtcDaliPushButtonSetGetToggleButton(void);
extern int UtcDaliPushButtonSetGetAutoRepeatingAndToggleButton(void);
extern int UtcDaliPushButtonSetGetToggled01(void);
extern int UtcDaliPushButtonSetGetToggled02(void);
extern int UtcDaliPushButtonSetGetAutorepeatingDelayValues01(void);
extern int UtcDaliPushButtonSetGetAutorepeatingDelayValues02(void);
extern int UtcDaliPushButtonSetImages(void);
extern int UtcDaliPushButtonSetLabelText(void);
extern int UtcDaliPushButtonPressed(void);
extern int UtcDaliPushButtonReleased(void);
extern int UtcDaliPushButtonToggled(void);
extern int UtcDaliRipple2DEffectUninitialized(void);
extern int UtcDaliRipple2DEffectPropertyNames(void);
extern int UtcDaliRipple2DEffectDefaultValues(void);
extern int UtcDaliRipple2DEffectCustomValues(void);
extern int UtcDaliRippleUninitializedEffect(void);
extern int UtcDaliRipplePropertyNamesEffect(void);
extern int UtcDaliRippleDefaultValuesEffect(void);
extern int UtcDaliRippleCustomValuesEffect(void);
extern int UtcDaliScrollViewCustomEffectSetup(void);
extern int UtcDaliScrollViewCubeEffectSetup(void);
extern int UtcDaliScrollViewSpiralEffectSetup(void);
extern int UtcDaliScrollViewSlideEffectSetup(void);
extern int UtcDaliScrollViewTwistEffectSetup(void);
extern int UtcDaliScrollViewCubeEffectTest(void);
extern int UtcDaliScrollViewSpiralEffectTest(void);
extern int UtcDaliScrollViewSlideEffectTest(void);
extern int UtcDaliScrollViewTwistEffectTest(void);
extern int UtcDaliScrollViewCustomEffectTest(void);
extern int UtcDaliSuperBlurViewNew(void);
extern int UtcDaliSuperBlurViewSetImage(void);
extern int UtcDaliSuperBlurViewSetGetBlurStrength(void);
extern int UtcDaliSuperBlurViewGetBlurStrengthPropertyIndex(void);
extern int UtcDaliSuperBlurViewGetBlurredImage(void);
extern int UtcDaliSwirlUninitializedEffect(void);
extern int UtcDaliSwirlPropertyNamesEffect(void);
extern int UtcDaliSwirlDefaultValuesEffect(void);
extern int UtcDaliSwirlCustomValuesEffect(void);
extern int UtcDaliTextInputConstruction(void);
extern int UtcDaliTextInputDownCast(void);
extern int UtcDaliTextInputGetText(void);
extern int UtcDaliTextInputGetMarkupText(void);
extern int UtcDaliTextInputSetMaxCharacterLength(void);
extern int UtcDaliTextInputSetAndGetNumberOfLines(void);
extern int UtcDaliTextInputGetNumberOfCharacters(void);
extern int UtcDaliTextInputSetAndGetPlaceholderText(void);
extern int UtcDaliTextInputSetInitialText(void);
extern int UtcDaliTextInputSetEditableAndIsEditable(void);
extern int UtcDaliTextInputSetEditOnTouch(void);
extern int UtcDaliTextInputSetTextSelectable(void);
extern int UtcDaliTextInputTextSelection(void);
extern int UtcDaliTextInputEnableGrabHandleAndIsGrabHandleEnabled(void);
extern int UtcDaliTextInputSetAndGetBoundingRectangle(void);
extern int UtcDaliTextInputSetAndGetTextAlignment(void);
extern int UtcDaliTextInputSetSortModifier(void);
extern int UtcDaliTextInputSetAndGetSnapshotModeEnabled(void);
extern int UtcDaliTextInputEndSignalEmit(void);
extern int UtcDaliTextInputStartSignalEmit(void);
extern int UtcDaliTextInputExceedMaxCharacters(void);
extern int UtcDaliTextInputSetAndGetFadeBoundary(void);
extern int UtcDaliTextInputSetAndGetWidthExceedPolicy(void);
extern int UtcDaliTextInputSetAndGetHeightExceedPolicy(void);
extern int UtcDaliTextInputScroll(void);
extern int UtcDaliTextViewNew(void);
extern int UtcDaliTextViewSetAndGetText(void);
extern int UtcDaliTextViewSetStyleToCurrentText(void);
extern int UtcDaliTextViewSetAndGetLineHeight(void);
extern int UtcDaliTextViewSetAndGetFadeBoundary(void);
extern int UtcDaliTextViewSetAndGetEllipsizeText(void);
extern int UtcDaliTextViewSetAndGetWidthExceedPolicy(void);
extern int UtcDaliTextViewSetAndGetHeightExceedPolicy(void);
extern int UtcDaliTextViewTestLayoutOptions01(void);
extern int UtcDaliTextViewTestLayoutOptions02(void);
extern int UtcDaliTextViewInsertRemoveText(void);
extern int UtcDaliTextViewSnapshotEnable(void);
extern int UtcDaliTextViewScroll(void);
extern int UtcDaliTextViewSetProperty(void);
extern int UtcDaliTextViewSetSortModifier(void);
extern int UtcDaliTextViewUnderlineText(void);

testcase tc_array[] = {
    {"UtcDaliAlignmentConstructorNegative", UtcDaliAlignmentConstructorNegative, utc_dali_toolkit_alignment_startup, utc_dali_toolkit_alignment_cleanup},
    {"UtcDaliAlignmentConstructorPositive", UtcDaliAlignmentConstructorPositive, utc_dali_toolkit_alignment_startup, utc_dali_toolkit_alignment_cleanup},
    {"UtcDaliAlignmentConstructorRegister", UtcDaliAlignmentConstructorRegister, utc_dali_toolkit_alignment_startup, utc_dali_toolkit_alignment_cleanup},
    {"UtcDaliAlignmentSetAlignmentTypePositiveOffStage", UtcDaliAlignmentSetAlignmentTypePositiveOffStage, utc_dali_toolkit_alignment_startup, utc_dali_toolkit_alignment_cleanup},
    {"UtcDaliAlignmentSetAlignmentTypePositiveOnStage", UtcDaliAlignmentSetAlignmentTypePositiveOnStage, utc_dali_toolkit_alignment_startup, utc_dali_toolkit_alignment_cleanup},
    {"UtcDaliAlignmentSetAlignmentTypeNegative", UtcDaliAlignmentSetAlignmentTypeNegative, utc_dali_toolkit_alignment_startup, utc_dali_toolkit_alignment_cleanup},
    {"UtcDaliAlignmentGetAlignmentType", UtcDaliAlignmentGetAlignmentType, utc_dali_toolkit_alignment_startup, utc_dali_toolkit_alignment_cleanup},
    {"UtcDaliAlignmentSetScaling", UtcDaliAlignmentSetScaling, utc_dali_toolkit_alignment_startup, utc_dali_toolkit_alignment_cleanup},
    {"UtcDaliAlignmentGetScaling", UtcDaliAlignmentGetScaling, utc_dali_toolkit_alignment_startup, utc_dali_toolkit_alignment_cleanup},
    {"UtcDaliAlignmentSetPaddingPositive", UtcDaliAlignmentSetPaddingPositive, utc_dali_toolkit_alignment_startup, utc_dali_toolkit_alignment_cleanup},
    {"UtcDaliAlignmentSetPaddingNegative", UtcDaliAlignmentSetPaddingNegative, utc_dali_toolkit_alignment_startup, utc_dali_toolkit_alignment_cleanup},
    {"UtcDaliAlignmentGetPadding", UtcDaliAlignmentGetPadding, utc_dali_toolkit_alignment_startup, utc_dali_toolkit_alignment_cleanup},
    {"UtcDaliAlignmentChildAddAndRemove", UtcDaliAlignmentChildAddAndRemove, utc_dali_toolkit_alignment_startup, utc_dali_toolkit_alignment_cleanup},
    {"UtcDaliAlignmentOnSizeSet", UtcDaliAlignmentOnSizeSet, utc_dali_toolkit_alignment_startup, utc_dali_toolkit_alignment_cleanup},
    {"UtcDaliAlignmentOnTouchEvent", UtcDaliAlignmentOnTouchEvent, utc_dali_toolkit_alignment_startup, utc_dali_toolkit_alignment_cleanup},
    {"UtcDaliAlignmentOnKeyEvent", UtcDaliAlignmentOnKeyEvent, utc_dali_toolkit_alignment_startup, utc_dali_toolkit_alignment_cleanup},
    {"UtcDaliAlignmentOnSizeAnimation", UtcDaliAlignmentOnSizeAnimation, utc_dali_toolkit_alignment_startup, utc_dali_toolkit_alignment_cleanup},
    {"UtcDaliAlignmentCopyAndAssignment", UtcDaliAlignmentCopyAndAssignment, utc_dali_toolkit_alignment_startup, utc_dali_toolkit_alignment_cleanup},
    {"UtcDaliBubbleEmitterNew", UtcDaliBubbleEmitterNew, utc_dali_toolkit_bubble_emitter_startup, utc_dali_toolkit_bubble_emitter_cleanup},
    {"UtcDaliBubbleEmitterGetRootActor", UtcDaliBubbleEmitterGetRootActor, utc_dali_toolkit_bubble_emitter_startup, utc_dali_toolkit_bubble_emitter_cleanup},
    {"UtcDaliBubbleEmitterSetBackground", UtcDaliBubbleEmitterSetBackground, utc_dali_toolkit_bubble_emitter_startup, utc_dali_toolkit_bubble_emitter_cleanup},
    {"UtcDaliBubbleEmitterSetShapeImage", UtcDaliBubbleEmitterSetShapeImage, utc_dali_toolkit_bubble_emitter_startup, utc_dali_toolkit_bubble_emitter_cleanup},
    {"UtcDaliBubbleEmitterSetBubbleScale", UtcDaliBubbleEmitterSetBubbleScale, utc_dali_toolkit_bubble_emitter_startup, utc_dali_toolkit_bubble_emitter_cleanup},
    {"UtcDaliBubbleEmitterSetBubbleDensity01", UtcDaliBubbleEmitterSetBubbleDensity01, utc_dali_toolkit_bubble_emitter_startup, utc_dali_toolkit_bubble_emitter_cleanup},
    {"UtcDaliBubbleEmitterSetBubbleDensity02", UtcDaliBubbleEmitterSetBubbleDensity02, utc_dali_toolkit_bubble_emitter_startup, utc_dali_toolkit_bubble_emitter_cleanup},
    {"UtcDaliBubbleEmitterSetBlendMode", UtcDaliBubbleEmitterSetBlendMode, utc_dali_toolkit_bubble_emitter_startup, utc_dali_toolkit_bubble_emitter_cleanup},
    {"UtcDaliBubbleEmitterEmitBubble", UtcDaliBubbleEmitterEmitBubble, utc_dali_toolkit_bubble_emitter_startup, utc_dali_toolkit_bubble_emitter_cleanup},
    {"UtcDaliBubbleEmitterStartExplosion", UtcDaliBubbleEmitterStartExplosion, utc_dali_toolkit_bubble_emitter_startup, utc_dali_toolkit_bubble_emitter_cleanup},
    {"UtcDaliBubbleEmitterRestore", UtcDaliBubbleEmitterRestore, utc_dali_toolkit_bubble_emitter_startup, utc_dali_toolkit_bubble_emitter_cleanup},
    {"UtcDaliButtonNew", UtcDaliButtonNew, utc_dali_toolkit_button_startup, utc_dali_toolkit_button_cleanup},
    {"UtcDaliButtonSetProperty", UtcDaliButtonSetProperty, utc_dali_toolkit_button_startup, utc_dali_toolkit_button_cleanup},
    {"UtcDaliButtonSetGetDimmed", UtcDaliButtonSetGetDimmed, utc_dali_toolkit_button_startup, utc_dali_toolkit_button_cleanup},
    {"UtcDaliButtonSize", UtcDaliButtonSize, utc_dali_toolkit_button_startup, utc_dali_toolkit_button_cleanup},
    {"UtcDaliButtonClicked", UtcDaliButtonClicked, utc_dali_toolkit_button_startup, utc_dali_toolkit_button_cleanup},
    {"UtcDaliButtonConnectSignal", UtcDaliButtonConnectSignal, utc_dali_toolkit_button_startup, utc_dali_toolkit_button_cleanup},
    {"UtcDaliButtonSetGetAnimationTime", UtcDaliButtonSetGetAnimationTime, utc_dali_toolkit_button_startup, utc_dali_toolkit_button_cleanup},
    {"UtcDaliControlConstructor", UtcDaliControlConstructor, utc_dali_toolkit_control_startup, utc_dali_toolkit_control_cleanup},
    {"UtcDaliControlNew", UtcDaliControlNew, utc_dali_toolkit_control_startup, utc_dali_toolkit_control_cleanup},
    {"UtcDaliControlRegister", UtcDaliControlRegister, utc_dali_toolkit_control_startup, utc_dali_toolkit_control_cleanup},
    {"UtcDaliControlCopyAndAssignment", UtcDaliControlCopyAndAssignment, utc_dali_toolkit_control_startup, utc_dali_toolkit_control_cleanup},
    {"UtcDaliControlDownCast", UtcDaliControlDownCast, utc_dali_toolkit_control_startup, utc_dali_toolkit_control_cleanup},
    {"UtcDaliControlDownCastTemplate", UtcDaliControlDownCastTemplate, utc_dali_toolkit_control_startup, utc_dali_toolkit_control_cleanup},
    {"UtcDaliControlKeyInputFocus", UtcDaliControlKeyInputFocus, utc_dali_toolkit_control_startup, utc_dali_toolkit_control_cleanup},
    {"UtcDaliControlGetImplementation", UtcDaliControlGetImplementation, utc_dali_toolkit_control_startup, utc_dali_toolkit_control_cleanup},
    {"UtcDaliControlSignalConnectDisconnect", UtcDaliControlSignalConnectDisconnect, utc_dali_toolkit_control_startup, utc_dali_toolkit_control_cleanup},
    {"UtcDaliControlSignalAutomaticDisconnect", UtcDaliControlSignalAutomaticDisconnect, utc_dali_toolkit_control_startup, utc_dali_toolkit_control_cleanup},
    {"UtcDaliControlTestParameters", UtcDaliControlTestParameters, utc_dali_toolkit_control_startup, utc_dali_toolkit_control_cleanup},
    {"UtcDaliControlImplNew", UtcDaliControlImplNew, utc_dali_toolkit_control_impl_startup, utc_dali_toolkit_control_impl_cleanup},
    {"UtcDaliControlImplEnableGestureDetector", UtcDaliControlImplEnableGestureDetector, utc_dali_toolkit_control_impl_startup, utc_dali_toolkit_control_impl_cleanup},
    {"UtcDaliControlImplDisableGestureDetector", UtcDaliControlImplDisableGestureDetector, utc_dali_toolkit_control_impl_startup, utc_dali_toolkit_control_impl_cleanup},
    {"UtcDaliControlImplOnGestureMethods", UtcDaliControlImplOnGestureMethods, utc_dali_toolkit_control_impl_startup, utc_dali_toolkit_control_impl_cleanup},
    {"UtcDaliControlImplChildAddAndRemove", UtcDaliControlImplChildAddAndRemove, utc_dali_toolkit_control_impl_startup, utc_dali_toolkit_control_impl_cleanup},
    {"UtcDaliControlImplStageConnection", UtcDaliControlImplStageConnection, utc_dali_toolkit_control_impl_startup, utc_dali_toolkit_control_impl_cleanup},
    {"UtcDaliControlImplSizeSet", UtcDaliControlImplSizeSet, utc_dali_toolkit_control_impl_startup, utc_dali_toolkit_control_impl_cleanup},
    {"UtcDaliControlImplSizeAnimation", UtcDaliControlImplSizeAnimation, utc_dali_toolkit_control_impl_startup, utc_dali_toolkit_control_impl_cleanup},
    {"UtcDaliControlImplTouchEvent", UtcDaliControlImplTouchEvent, utc_dali_toolkit_control_impl_startup, utc_dali_toolkit_control_impl_cleanup},
    {"UtcDaliControlImplKeyEvent", UtcDaliControlImplKeyEvent, utc_dali_toolkit_control_impl_startup, utc_dali_toolkit_control_impl_cleanup},
    {"UtcDaliControlImplStyleChange", UtcDaliControlImplStyleChange, utc_dali_toolkit_control_impl_startup, utc_dali_toolkit_control_impl_cleanup},
    {"UtcDaliControlImplKeyInputFocusGained", UtcDaliControlImplKeyInputFocusGained, utc_dali_toolkit_control_impl_startup, utc_dali_toolkit_control_impl_cleanup},
    {"UtcDaliControlImplKeyInputFocusLost", UtcDaliControlImplKeyInputFocusLost, utc_dali_toolkit_control_impl_startup, utc_dali_toolkit_control_impl_cleanup},
    {"UtcDaliDefaultControlsCreateSolidColorActor", UtcDaliDefaultControlsCreateSolidColorActor, utc_dali_toolkit_default_controls_startup, utc_dali_toolkit_default_controls_cleanup},
    {"UtcDaliDefaultControlsCreatePushButton", UtcDaliDefaultControlsCreatePushButton, utc_dali_toolkit_default_controls_startup, utc_dali_toolkit_default_controls_cleanup},
    {"UtcDaliDissolveUninitializedEffect", UtcDaliDissolveUninitializedEffect, utc_dali_toolkit_dissolve_effect_startup, utc_dali_toolkit_dissolve_effect_cleanup},
    {"UtcDaliDissolvePropertyNamesEffect", UtcDaliDissolvePropertyNamesEffect, utc_dali_toolkit_dissolve_effect_startup, utc_dali_toolkit_dissolve_effect_cleanup},
    {"UtcDaliDissolveDefaultValuesEffect", UtcDaliDissolveDefaultValuesEffect, utc_dali_toolkit_dissolve_effect_startup, utc_dali_toolkit_dissolve_effect_cleanup},
    {"UtcDaliDissolveCustomValuesEffect", UtcDaliDissolveCustomValuesEffect, utc_dali_toolkit_dissolve_effect_startup, utc_dali_toolkit_dissolve_effect_cleanup},
    {"UtcDaliSetEffectImageEffect", UtcDaliSetEffectImageEffect, utc_dali_toolkit_dissolve_effect_startup, utc_dali_toolkit_dissolve_effect_cleanup},
    {"UtcDaliFocusManagerGet", UtcDaliFocusManagerGet, utc_dali_toolkit_focus_manager_startup, utc_dali_toolkit_focus_manager_cleanup},
    {"UtcDaliFocusManagerSetAndGetAccessibilityAttribute", UtcDaliFocusManagerSetAndGetAccessibilityAttribute, utc_dali_toolkit_focus_manager_startup, utc_dali_toolkit_focus_manager_cleanup},
    {"UtcDaliFocusManagerSetAndGetFocusOrder", UtcDaliFocusManagerSetAndGetFocusOrder, utc_dali_toolkit_focus_manager_startup, utc_dali_toolkit_focus_manager_cleanup},
    {"UtcDaliFocusManagerGenerateNewFocusOrder", UtcDaliFocusManagerGenerateNewFocusOrder, utc_dali_toolkit_focus_manager_startup, utc_dali_toolkit_focus_manager_cleanup},
    {"UtcDaliFocusManagerGetActorByFocusOrder", UtcDaliFocusManagerGetActorByFocusOrder, utc_dali_toolkit_focus_manager_startup, utc_dali_toolkit_focus_manager_cleanup},
    {"UtcDaliFocusManagerSetAndGetCurrentFocusActor", UtcDaliFocusManagerSetAndGetCurrentFocusActor, utc_dali_toolkit_focus_manager_startup, utc_dali_toolkit_focus_manager_cleanup},
    {"UtcDaliFocusManagerGetCurrentFocusGroup", UtcDaliFocusManagerGetCurrentFocusGroup, utc_dali_toolkit_focus_manager_startup, utc_dali_toolkit_focus_manager_cleanup},
    {"UtcDaliFocusManagerGetCurrentFocusOrder", UtcDaliFocusManagerGetCurrentFocusOrder, utc_dali_toolkit_focus_manager_startup, utc_dali_toolkit_focus_manager_cleanup},
    {"UtcDaliFocusManagerMoveFocusForward", UtcDaliFocusManagerMoveFocusForward, utc_dali_toolkit_focus_manager_startup, utc_dali_toolkit_focus_manager_cleanup},
    {"UtcDaliFocusManagerMoveFocusBackward", UtcDaliFocusManagerMoveFocusBackward, utc_dali_toolkit_focus_manager_startup, utc_dali_toolkit_focus_manager_cleanup},
    {"UtcDaliFocusManagerClearFocus", UtcDaliFocusManagerClearFocus, utc_dali_toolkit_focus_manager_startup, utc_dali_toolkit_focus_manager_cleanup},
    {"UtcDaliFocusManagerReset", UtcDaliFocusManagerReset, utc_dali_toolkit_focus_manager_startup, utc_dali_toolkit_focus_manager_cleanup},
    {"UtcDaliFocusManagerFocusGroup", UtcDaliFocusManagerFocusGroup, utc_dali_toolkit_focus_manager_startup, utc_dali_toolkit_focus_manager_cleanup},
    {"UtcDaliFocusManagerSetAndGetFocusIndicator", UtcDaliFocusManagerSetAndGetFocusIndicator, utc_dali_toolkit_focus_manager_startup, utc_dali_toolkit_focus_manager_cleanup},
    {"UtcDaliFocusManagerSignalFocusChanged", UtcDaliFocusManagerSignalFocusChanged, utc_dali_toolkit_focus_manager_startup, utc_dali_toolkit_focus_manager_cleanup},
    {"UtcDaliFocusManagerSignalFocusOvershot", UtcDaliFocusManagerSignalFocusOvershot, utc_dali_toolkit_focus_manager_startup, utc_dali_toolkit_focus_manager_cleanup},
    {"UtcDaliGridLayoutNew", UtcDaliGridLayoutNew, utc_dali_toolkit_grid_layout_startup, utc_dali_toolkit_grid_layout_cleanup},
    {"UtcDaliGridLayoutSetAndGetNumberOfColumns", UtcDaliGridLayoutSetAndGetNumberOfColumns, utc_dali_toolkit_grid_layout_startup, utc_dali_toolkit_grid_layout_cleanup},
    {"UtcDaliGridLayoutSetAndGetRowSpacing", UtcDaliGridLayoutSetAndGetRowSpacing, utc_dali_toolkit_grid_layout_startup, utc_dali_toolkit_grid_layout_cleanup},
    {"UtcDaliGridLayoutSetAndGetColumnSpacing", UtcDaliGridLayoutSetAndGetColumnSpacing, utc_dali_toolkit_grid_layout_startup, utc_dali_toolkit_grid_layout_cleanup},
    {"UtcDaliGridLayoutSetAndGetTopMargin", UtcDaliGridLayoutSetAndGetTopMargin, utc_dali_toolkit_grid_layout_startup, utc_dali_toolkit_grid_layout_cleanup},
    {"UtcDaliGridLayoutSetAndGetBottomMargin", UtcDaliGridLayoutSetAndGetBottomMargin, utc_dali_toolkit_grid_layout_startup, utc_dali_toolkit_grid_layout_cleanup},
    {"UtcDaliGridLayoutSetAndGetSideMargin", UtcDaliGridLayoutSetAndGetSideMargin, utc_dali_toolkit_grid_layout_startup, utc_dali_toolkit_grid_layout_cleanup},
    {"UtcDaliGridLayoutSetAndGetZGap", UtcDaliGridLayoutSetAndGetZGap, utc_dali_toolkit_grid_layout_startup, utc_dali_toolkit_grid_layout_cleanup},
    {"UtcDaliGridLayoutSetAndGetItemSizeFunction", UtcDaliGridLayoutSetAndGetItemSizeFunction, utc_dali_toolkit_grid_layout_startup, utc_dali_toolkit_grid_layout_cleanup},
    {"UtcDaliGridLayoutSetAndGetScrollSpeedFactor", UtcDaliGridLayoutSetAndGetScrollSpeedFactor, utc_dali_toolkit_grid_layout_startup, utc_dali_toolkit_grid_layout_cleanup},
    {"UtcDaliGridLayoutSetAndGetMaximumSwipeSpeed", UtcDaliGridLayoutSetAndGetMaximumSwipeSpeed, utc_dali_toolkit_grid_layout_startup, utc_dali_toolkit_grid_layout_cleanup},
    {"UtcDaliGridLayoutSetAndGetItemFlickAnimationDuration", UtcDaliGridLayoutSetAndGetItemFlickAnimationDuration, utc_dali_toolkit_grid_layout_startup, utc_dali_toolkit_grid_layout_cleanup},
    {"UtcDaliGridLayoutConstraintLeft", UtcDaliGridLayoutConstraintLeft, utc_dali_toolkit_grid_layout_startup, utc_dali_toolkit_grid_layout_cleanup},
    {"UtcDaliGridLayoutConstraintRight", UtcDaliGridLayoutConstraintRight, utc_dali_toolkit_grid_layout_startup, utc_dali_toolkit_grid_layout_cleanup},
    {"UtcDaliGridLayoutConstraintUp", UtcDaliGridLayoutConstraintUp, utc_dali_toolkit_grid_layout_startup, utc_dali_toolkit_grid_layout_cleanup},
    {"UtcDaliGridLayoutConstraintDown", UtcDaliGridLayoutConstraintDown, utc_dali_toolkit_grid_layout_startup, utc_dali_toolkit_grid_layout_cleanup},
    {"UtcDaliGridLayoutScrollDirection", UtcDaliGridLayoutScrollDirection, utc_dali_toolkit_grid_layout_startup, utc_dali_toolkit_grid_layout_cleanup},
    {"UtcDaliIrisEffectUninitialized", UtcDaliIrisEffectUninitialized, utc_dali_toolkit_iris_effect_startup, utc_dali_toolkit_iris_effect_cleanup},
    {"UtcDaliIrisEffectPropertyNames", UtcDaliIrisEffectPropertyNames, utc_dali_toolkit_iris_effect_startup, utc_dali_toolkit_iris_effect_cleanup},
    {"UtcDaliIrisEffectDefaultValues", UtcDaliIrisEffectDefaultValues, utc_dali_toolkit_iris_effect_startup, utc_dali_toolkit_iris_effect_cleanup},
    {"UtcDaliIrisEffectCustomValues", UtcDaliIrisEffectCustomValues, utc_dali_toolkit_iris_effect_startup, utc_dali_toolkit_iris_effect_cleanup},
    {"UtcDaliItemLayoutSetAndGetOrientation", UtcDaliItemLayoutSetAndGetOrientation, utc_dali_toolkit_iris_effect_startup, utc_dali_toolkit_iris_effect_cleanup},
    {"UtcDaliItemLayoutGetScrollHints", UtcDaliItemLayoutGetScrollHints, utc_dali_toolkit_iris_effect_startup, utc_dali_toolkit_iris_effect_cleanup},
    {"UtcDaliItemViewNew", UtcDaliItemViewNew, utc_dali_toolkit_item_view_startup, utc_dali_toolkit_item_view_cleanup},
    {"UtcDaliItemViewDownCast", UtcDaliItemViewDownCast, utc_dali_toolkit_item_view_startup, utc_dali_toolkit_item_view_cleanup},
    {"UtcDaliItemViewAddAndGetLayout", UtcDaliItemViewAddAndGetLayout, utc_dali_toolkit_item_view_startup, utc_dali_toolkit_item_view_cleanup},
    {"UtcDaliItemViewAddAndRemoveLayout", UtcDaliItemViewAddAndRemoveLayout, utc_dali_toolkit_item_view_startup, utc_dali_toolkit_item_view_cleanup},
    {"UtcDaliItemViewActivateLayoutAndGetActiveLayout", UtcDaliItemViewActivateLayoutAndGetActiveLayout, utc_dali_toolkit_item_view_startup, utc_dali_toolkit_item_view_cleanup},
    {"UtcDaliItemViewDeactivateCurrentLayout", UtcDaliItemViewDeactivateCurrentLayout, utc_dali_toolkit_item_view_startup, utc_dali_toolkit_item_view_cleanup},
    {"UtcDaliItemViewGetItemAndGetItemId", UtcDaliItemViewGetItemAndGetItemId, utc_dali_toolkit_item_view_startup, utc_dali_toolkit_item_view_cleanup},
    {"UtcDaliItemViewRemoveItem", UtcDaliItemViewRemoveItem, utc_dali_toolkit_item_view_startup, utc_dali_toolkit_item_view_cleanup},
    {"UtcDaliItemViewGetCurrentLayoutPosition", UtcDaliItemViewGetCurrentLayoutPosition, utc_dali_toolkit_item_view_startup, utc_dali_toolkit_item_view_cleanup},
    {"UtcDaliItemViewSetAndGetMinimumSwipeSpeed", UtcDaliItemViewSetAndGetMinimumSwipeSpeed, utc_dali_toolkit_item_view_startup, utc_dali_toolkit_item_view_cleanup},
    {"UtcDaliItemViewSetAndGetMinimumSwipeDistance", UtcDaliItemViewSetAndGetMinimumSwipeDistance, utc_dali_toolkit_item_view_startup, utc_dali_toolkit_item_view_cleanup},
    {"UtcDaliItemViewSetAndGetAnchoring", UtcDaliItemViewSetAndGetAnchoring, utc_dali_toolkit_item_view_startup, utc_dali_toolkit_item_view_cleanup},
    {"UtcDaliItemViewSetAndGetAnchoringDuration", UtcDaliItemViewSetAndGetAnchoringDuration, utc_dali_toolkit_item_view_startup, utc_dali_toolkit_item_view_cleanup},
    {"UtcDaliItemViewSetAndGetRefreshInterval", UtcDaliItemViewSetAndGetRefreshInterval, utc_dali_toolkit_item_view_startup, utc_dali_toolkit_item_view_cleanup},
    {"UtcDaliItemViewScrollToItem", UtcDaliItemViewScrollToItem, utc_dali_toolkit_item_view_startup, utc_dali_toolkit_item_view_cleanup},
    {"UtcDaliItemViewSetAndGetMouseWheelScrollDistanceStep", UtcDaliItemViewSetAndGetMouseWheelScrollDistanceStep, utc_dali_toolkit_item_view_startup, utc_dali_toolkit_item_view_cleanup},
    {"UtcDaliKeyboardFocusManagerGet", UtcDaliKeyboardFocusManagerGet, utc_dali_toolkit_keyboard_focus_manager_startup, utc_dali_toolkit_keyboard_focus_manager_cleanup},
    {"UtcDaliKeyboardFocusManagerSetAndGetCurrentFocusActor", UtcDaliKeyboardFocusManagerSetAndGetCurrentFocusActor, utc_dali_toolkit_keyboard_focus_manager_startup, utc_dali_toolkit_keyboard_focus_manager_cleanup},
    {"UtcDaliKeyboardFocusManagerMoveFocus", UtcDaliKeyboardFocusManagerMoveFocus, utc_dali_toolkit_keyboard_focus_manager_startup, utc_dali_toolkit_keyboard_focus_manager_cleanup},
    {"UtcDaliKeyboardFocusManagerClearFocus", UtcDaliKeyboardFocusManagerClearFocus, utc_dali_toolkit_keyboard_focus_manager_startup, utc_dali_toolkit_keyboard_focus_manager_cleanup},
    {"UtcDaliKeyboardFocusManagerSetAndGetFocusGroupLoop", UtcDaliKeyboardFocusManagerSetAndGetFocusGroupLoop, utc_dali_toolkit_keyboard_focus_manager_startup, utc_dali_toolkit_keyboard_focus_manager_cleanup},
    {"UtcDaliKeyboardFocusManagerSetAsFocusGroup", UtcDaliKeyboardFocusManagerSetAsFocusGroup, utc_dali_toolkit_keyboard_focus_manager_startup, utc_dali_toolkit_keyboard_focus_manager_cleanup},
    {"UtcDaliKeyboardFocusManagerGetFocusGroup", UtcDaliKeyboardFocusManagerGetFocusGroup, utc_dali_toolkit_keyboard_focus_manager_startup, utc_dali_toolkit_keyboard_focus_manager_cleanup},
    {"UtcDaliKeyboardFocusManagerSetAndGetFocusIndicator", UtcDaliKeyboardFocusManagerSetAndGetFocusIndicator, utc_dali_toolkit_keyboard_focus_manager_startup, utc_dali_toolkit_keyboard_focus_manager_cleanup},
    {"UtcDaliKeyboardFocusManagerSignalFocusedActorActivated", UtcDaliKeyboardFocusManagerSignalFocusedActorActivated, utc_dali_toolkit_keyboard_focus_manager_startup, utc_dali_toolkit_keyboard_focus_manager_cleanup},
    {"UtcDaliMarkupProcessor", UtcDaliMarkupProcessor, utc_dali_toolkit_markup_processor_startup, utc_dali_toolkit_markup_processor_cleanup},
    {"UtcDaliMarkupProcessorSetTextStyle01", UtcDaliMarkupProcessorSetTextStyle01, utc_dali_toolkit_markup_processor_startup, utc_dali_toolkit_markup_processor_cleanup},
    {"UtcDaliMarkupProcessorSetTextStyle02", UtcDaliMarkupProcessorSetTextStyle02, utc_dali_toolkit_markup_processor_startup, utc_dali_toolkit_markup_processor_cleanup},
    {"UtcDaliMarkupProcessorTestColors", UtcDaliMarkupProcessorTestColors, utc_dali_toolkit_markup_processor_startup, utc_dali_toolkit_markup_processor_cleanup},
    {"UtcDaliMaskEffectCreateEffect", UtcDaliMaskEffectCreateEffect, utc_dali_toolkit_mask_effect_startup, utc_dali_toolkit_mask_effect_cleanup},
    {"UtcDaliMaskEffectDestructor", UtcDaliMaskEffectDestructor, utc_dali_toolkit_mask_effect_startup, utc_dali_toolkit_mask_effect_cleanup},
    {"UtcDaliNinePatchMaskEffectApply", UtcDaliNinePatchMaskEffectApply, utc_dali_toolkit_nine_patch_startup, utc_dali_toolkit_nine_patch_cleanup},
    {"UtcDaliPopupNew", UtcDaliPopupNew, utc_dali_toolkit_popup_startup, utc_dali_toolkit_popup_cleanup},
    {"UtcDaliPopupDestructor", UtcDaliPopupDestructor, utc_dali_toolkit_popup_startup, utc_dali_toolkit_popup_cleanup},
    {"UtcDaliPopupDownCast", UtcDaliPopupDownCast, utc_dali_toolkit_popup_startup, utc_dali_toolkit_popup_cleanup},
    {"UtcDaliPopoupSetProperty", UtcDaliPopoupSetProperty, utc_dali_toolkit_popup_startup, utc_dali_toolkit_popup_cleanup},
    {"UtcDaliPopupSetBackgroundImage", UtcDaliPopupSetBackgroundImage, utc_dali_toolkit_popup_startup, utc_dali_toolkit_popup_cleanup},
    {"UtcDaliPopupSetTitle", UtcDaliPopupSetTitle, utc_dali_toolkit_popup_startup, utc_dali_toolkit_popup_cleanup},
    {"UtcDaliPopupSetTitleText", UtcDaliPopupSetTitleText, utc_dali_toolkit_popup_startup, utc_dali_toolkit_popup_cleanup},
    {"UtcDaliPopupAddButton", UtcDaliPopupAddButton, utc_dali_toolkit_popup_startup, utc_dali_toolkit_popup_cleanup},
    {"UtcDaliPopupSetState", UtcDaliPopupSetState, utc_dali_toolkit_popup_startup, utc_dali_toolkit_popup_cleanup},
    {"UtcDaliPopupSetStateSlow", UtcDaliPopupSetStateSlow, utc_dali_toolkit_popup_startup, utc_dali_toolkit_popup_cleanup},
    {"UtcDaliPopupShowHide", UtcDaliPopupShowHide, utc_dali_toolkit_popup_startup, utc_dali_toolkit_popup_cleanup},
    {"UtcDaliPopupShowHideTail", UtcDaliPopupShowHideTail, utc_dali_toolkit_popup_startup, utc_dali_toolkit_popup_cleanup},
    {"UtcDaliPopupOnTouchedOutside", UtcDaliPopupOnTouchedOutside, utc_dali_toolkit_popup_startup, utc_dali_toolkit_popup_cleanup},
    {"UtcDaliPushButtonSetGetAutoRepeating", UtcDaliPushButtonSetGetAutoRepeating, utc_dali_toolkit_pushbutton_startup, utc_dali_toolkit_pushbutton_cleanup},
    {"UtcDaliPushButtonSetGetToggleButton", UtcDaliPushButtonSetGetToggleButton, utc_dali_toolkit_pushbutton_startup, utc_dali_toolkit_pushbutton_cleanup},
    {"UtcDaliPushButtonSetGetAutoRepeatingAndToggleButton", UtcDaliPushButtonSetGetAutoRepeatingAndToggleButton, utc_dali_toolkit_pushbutton_startup, utc_dali_toolkit_pushbutton_cleanup},
    {"UtcDaliPushButtonSetGetToggled01", UtcDaliPushButtonSetGetToggled01, utc_dali_toolkit_pushbutton_startup, utc_dali_toolkit_pushbutton_cleanup},
    {"UtcDaliPushButtonSetGetToggled02", UtcDaliPushButtonSetGetToggled02, utc_dali_toolkit_pushbutton_startup, utc_dali_toolkit_pushbutton_cleanup},
    {"UtcDaliPushButtonSetGetAutorepeatingDelayValues01", UtcDaliPushButtonSetGetAutorepeatingDelayValues01, utc_dali_toolkit_pushbutton_startup, utc_dali_toolkit_pushbutton_cleanup},
    {"UtcDaliPushButtonSetGetAutorepeatingDelayValues02", UtcDaliPushButtonSetGetAutorepeatingDelayValues02, utc_dali_toolkit_pushbutton_startup, utc_dali_toolkit_pushbutton_cleanup},
    {"UtcDaliPushButtonSetImages", UtcDaliPushButtonSetImages, utc_dali_toolkit_pushbutton_startup, utc_dali_toolkit_pushbutton_cleanup},
    {"UtcDaliPushButtonSetLabelText", UtcDaliPushButtonSetLabelText, utc_dali_toolkit_pushbutton_startup, utc_dali_toolkit_pushbutton_cleanup},
    {"UtcDaliPushButtonPressed", UtcDaliPushButtonPressed, utc_dali_toolkit_pushbutton_startup, utc_dali_toolkit_pushbutton_cleanup},
    {"UtcDaliPushButtonReleased", UtcDaliPushButtonReleased, utc_dali_toolkit_pushbutton_startup, utc_dali_toolkit_pushbutton_cleanup},
    {"UtcDaliPushButtonToggled", UtcDaliPushButtonToggled, utc_dali_toolkit_pushbutton_startup, utc_dali_toolkit_pushbutton_cleanup},
    {"UtcDaliRipple2DEffectUninitialized", UtcDaliRipple2DEffectUninitialized, utc_dali_toolkit_ripple_2d_effect_startup, utc_dali_toolkit_ripple_2d_effect_cleanup},
    {"UtcDaliRipple2DEffectPropertyNames", UtcDaliRipple2DEffectPropertyNames, utc_dali_toolkit_ripple_2d_effect_startup, utc_dali_toolkit_ripple_2d_effect_cleanup},
    {"UtcDaliRipple2DEffectDefaultValues", UtcDaliRipple2DEffectDefaultValues, utc_dali_toolkit_ripple_2d_effect_startup, utc_dali_toolkit_ripple_2d_effect_cleanup},
    {"UtcDaliRipple2DEffectCustomValues", UtcDaliRipple2DEffectCustomValues, utc_dali_toolkit_ripple_2d_effect_startup, utc_dali_toolkit_ripple_2d_effect_cleanup},
    {"UtcDaliRippleUninitializedEffect", UtcDaliRippleUninitializedEffect, utc_dali_toolkit_ripple_effect_startup, utc_dali_toolkit_ripple_effect_cleanup},
    {"UtcDaliRipplePropertyNamesEffect", UtcDaliRipplePropertyNamesEffect, utc_dali_toolkit_ripple_effect_startup, utc_dali_toolkit_ripple_effect_cleanup},
    {"UtcDaliRippleDefaultValuesEffect", UtcDaliRippleDefaultValuesEffect, utc_dali_toolkit_ripple_effect_startup, utc_dali_toolkit_ripple_effect_cleanup},
    {"UtcDaliRippleCustomValuesEffect", UtcDaliRippleCustomValuesEffect, utc_dali_toolkit_ripple_effect_startup, utc_dali_toolkit_ripple_effect_cleanup},
    {"UtcDaliScrollViewCustomEffectSetup", UtcDaliScrollViewCustomEffectSetup, utc_dali_toolkit_scroll_view_effect_startup, utc_dali_toolkit_scroll_view_effect_cleanup},
    {"UtcDaliScrollViewCubeEffectSetup", UtcDaliScrollViewCubeEffectSetup, utc_dali_toolkit_scroll_view_effect_startup, utc_dali_toolkit_scroll_view_effect_cleanup},
    {"UtcDaliScrollViewSpiralEffectSetup", UtcDaliScrollViewSpiralEffectSetup, utc_dali_toolkit_scroll_view_effect_startup, utc_dali_toolkit_scroll_view_effect_cleanup},
    {"UtcDaliScrollViewSlideEffectSetup", UtcDaliScrollViewSlideEffectSetup, utc_dali_toolkit_scroll_view_effect_startup, utc_dali_toolkit_scroll_view_effect_cleanup},
    {"UtcDaliScrollViewTwistEffectSetup", UtcDaliScrollViewTwistEffectSetup, utc_dali_toolkit_scroll_view_effect_startup, utc_dali_toolkit_scroll_view_effect_cleanup},
    {"UtcDaliScrollViewCubeEffectTest", UtcDaliScrollViewCubeEffectTest, utc_dali_toolkit_scroll_view_effect_startup, utc_dali_toolkit_scroll_view_effect_cleanup},
    {"UtcDaliScrollViewSpiralEffectTest", UtcDaliScrollViewSpiralEffectTest, utc_dali_toolkit_scroll_view_effect_startup, utc_dali_toolkit_scroll_view_effect_cleanup},
    {"UtcDaliScrollViewSlideEffectTest", UtcDaliScrollViewSlideEffectTest, utc_dali_toolkit_scroll_view_effect_startup, utc_dali_toolkit_scroll_view_effect_cleanup},
    {"UtcDaliScrollViewTwistEffectTest", UtcDaliScrollViewTwistEffectTest, utc_dali_toolkit_scroll_view_effect_startup, utc_dali_toolkit_scroll_view_effect_cleanup},
    {"UtcDaliScrollViewCustomEffectTest", UtcDaliScrollViewCustomEffectTest, utc_dali_toolkit_scroll_view_effect_startup, utc_dali_toolkit_scroll_view_effect_cleanup},
    {"UtcDaliSuperBlurViewNew", UtcDaliSuperBlurViewNew, utc_dali_toolkit_super_blur_view_startup, utc_dali_toolkit_super_blur_view_cleanup},
    {"UtcDaliSuperBlurViewSetImage", UtcDaliSuperBlurViewSetImage, utc_dali_toolkit_super_blur_view_startup, utc_dali_toolkit_super_blur_view_cleanup},
    {"UtcDaliSuperBlurViewSetGetBlurStrength", UtcDaliSuperBlurViewSetGetBlurStrength, utc_dali_toolkit_super_blur_view_startup, utc_dali_toolkit_super_blur_view_cleanup},
    {"UtcDaliSuperBlurViewGetBlurStrengthPropertyIndex", UtcDaliSuperBlurViewGetBlurStrengthPropertyIndex, utc_dali_toolkit_super_blur_view_startup, utc_dali_toolkit_super_blur_view_cleanup},
    {"UtcDaliSuperBlurViewGetBlurredImage", UtcDaliSuperBlurViewGetBlurredImage, utc_dali_toolkit_super_blur_view_startup, utc_dali_toolkit_super_blur_view_cleanup},
    {"UtcDaliSwirlUninitializedEffect", UtcDaliSwirlUninitializedEffect, utc_dali_toolkit_swirl_effect_startup, utc_dali_toolkit_swirl_effect_cleanup},
    {"UtcDaliSwirlPropertyNamesEffect", UtcDaliSwirlPropertyNamesEffect, utc_dali_toolkit_swirl_effect_startup, utc_dali_toolkit_swirl_effect_cleanup},
    {"UtcDaliSwirlDefaultValuesEffect", UtcDaliSwirlDefaultValuesEffect, utc_dali_toolkit_swirl_effect_startup, utc_dali_toolkit_swirl_effect_cleanup},
    {"UtcDaliSwirlCustomValuesEffect", UtcDaliSwirlCustomValuesEffect, utc_dali_toolkit_swirl_effect_startup, utc_dali_toolkit_swirl_effect_cleanup},
    {"UtcDaliTextInputConstruction", UtcDaliTextInputConstruction, utc_dali_toolkit_text_input_startup, utc_dali_toolkit_text_input_cleanup},
    {"UtcDaliTextInputDownCast", UtcDaliTextInputDownCast, utc_dali_toolkit_text_input_startup, utc_dali_toolkit_text_input_cleanup},
    {"UtcDaliTextInputGetText", UtcDaliTextInputGetText, utc_dali_toolkit_text_input_startup, utc_dali_toolkit_text_input_cleanup},
    {"UtcDaliTextInputGetMarkupText", UtcDaliTextInputGetMarkupText, utc_dali_toolkit_text_input_startup, utc_dali_toolkit_text_input_cleanup},
    {"UtcDaliTextInputSetMaxCharacterLength", UtcDaliTextInputSetMaxCharacterLength, utc_dali_toolkit_text_input_startup, utc_dali_toolkit_text_input_cleanup},
    {"UtcDaliTextInputSetAndGetNumberOfLines", UtcDaliTextInputSetAndGetNumberOfLines, utc_dali_toolkit_text_input_startup, utc_dali_toolkit_text_input_cleanup},
    {"UtcDaliTextInputGetNumberOfCharacters", UtcDaliTextInputGetNumberOfCharacters, utc_dali_toolkit_text_input_startup, utc_dali_toolkit_text_input_cleanup},
    {"UtcDaliTextInputSetAndGetPlaceholderText", UtcDaliTextInputSetAndGetPlaceholderText, utc_dali_toolkit_text_input_startup, utc_dali_toolkit_text_input_cleanup},
    {"UtcDaliTextInputSetInitialText", UtcDaliTextInputSetInitialText, utc_dali_toolkit_text_input_startup, utc_dali_toolkit_text_input_cleanup},
    {"UtcDaliTextInputSetEditableAndIsEditable", UtcDaliTextInputSetEditableAndIsEditable, utc_dali_toolkit_text_input_startup, utc_dali_toolkit_text_input_cleanup},
    {"UtcDaliTextInputSetEditOnTouch", UtcDaliTextInputSetEditOnTouch, utc_dali_toolkit_text_input_startup, utc_dali_toolkit_text_input_cleanup},
    {"UtcDaliTextInputSetTextSelectable", UtcDaliTextInputSetTextSelectable, utc_dali_toolkit_text_input_startup, utc_dali_toolkit_text_input_cleanup},
    {"UtcDaliTextInputTextSelection", UtcDaliTextInputTextSelection, utc_dali_toolkit_text_input_startup, utc_dali_toolkit_text_input_cleanup},
    {"UtcDaliTextInputEnableGrabHandleAndIsGrabHandleEnabled", UtcDaliTextInputEnableGrabHandleAndIsGrabHandleEnabled, utc_dali_toolkit_text_input_startup, utc_dali_toolkit_text_input_cleanup},
    {"UtcDaliTextInputSetAndGetBoundingRectangle", UtcDaliTextInputSetAndGetBoundingRectangle, utc_dali_toolkit_text_input_startup, utc_dali_toolkit_text_input_cleanup},
    {"UtcDaliTextInputSetAndGetTextAlignment", UtcDaliTextInputSetAndGetTextAlignment, utc_dali_toolkit_text_input_startup, utc_dali_toolkit_text_input_cleanup},
    {"UtcDaliTextInputSetSortModifier", UtcDaliTextInputSetSortModifier, utc_dali_toolkit_text_input_startup, utc_dali_toolkit_text_input_cleanup},
    {"UtcDaliTextInputSetAndGetSnapshotModeEnabled", UtcDaliTextInputSetAndGetSnapshotModeEnabled, utc_dali_toolkit_text_input_startup, utc_dali_toolkit_text_input_cleanup},
    {"UtcDaliTextInputEndSignalEmit", UtcDaliTextInputEndSignalEmit, utc_dali_toolkit_text_input_startup, utc_dali_toolkit_text_input_cleanup},
    {"UtcDaliTextInputStartSignalEmit", UtcDaliTextInputStartSignalEmit, utc_dali_toolkit_text_input_startup, utc_dali_toolkit_text_input_cleanup},
    {"UtcDaliTextInputExceedMaxCharacters", UtcDaliTextInputExceedMaxCharacters, utc_dali_toolkit_text_input_startup, utc_dali_toolkit_text_input_cleanup},
    {"UtcDaliTextInputSetAndGetFadeBoundary", UtcDaliTextInputSetAndGetFadeBoundary, utc_dali_toolkit_text_input_startup, utc_dali_toolkit_text_input_cleanup},
    {"UtcDaliTextInputSetAndGetWidthExceedPolicy", UtcDaliTextInputSetAndGetWidthExceedPolicy, utc_dali_toolkit_text_input_startup, utc_dali_toolkit_text_input_cleanup},
    {"UtcDaliTextInputSetAndGetHeightExceedPolicy", UtcDaliTextInputSetAndGetHeightExceedPolicy, utc_dali_toolkit_text_input_startup, utc_dali_toolkit_text_input_cleanup},
    {"UtcDaliTextInputScroll", UtcDaliTextInputScroll, utc_dali_toolkit_text_input_startup, utc_dali_toolkit_text_input_cleanup},
    {"UtcDaliTextViewNew", UtcDaliTextViewNew, utc_dali_toolkit_text_view_startup, utc_dali_toolkit_text_view_cleanup},
    {"UtcDaliTextViewSetAndGetText", UtcDaliTextViewSetAndGetText, utc_dali_toolkit_text_view_startup, utc_dali_toolkit_text_view_cleanup},
    {"UtcDaliTextViewSetStyleToCurrentText", UtcDaliTextViewSetStyleToCurrentText, utc_dali_toolkit_text_view_startup, utc_dali_toolkit_text_view_cleanup},
    {"UtcDaliTextViewSetAndGetLineHeight", UtcDaliTextViewSetAndGetLineHeight, utc_dali_toolkit_text_view_startup, utc_dali_toolkit_text_view_cleanup},
    {"UtcDaliTextViewSetAndGetFadeBoundary", UtcDaliTextViewSetAndGetFadeBoundary, utc_dali_toolkit_text_view_startup, utc_dali_toolkit_text_view_cleanup},
    {"UtcDaliTextViewSetAndGetEllipsizeText", UtcDaliTextViewSetAndGetEllipsizeText, utc_dali_toolkit_text_view_startup, utc_dali_toolkit_text_view_cleanup},
    {"UtcDaliTextViewSetAndGetWidthExceedPolicy", UtcDaliTextViewSetAndGetWidthExceedPolicy, utc_dali_toolkit_text_view_startup, utc_dali_toolkit_text_view_cleanup},
    {"UtcDaliTextViewSetAndGetHeightExceedPolicy", UtcDaliTextViewSetAndGetHeightExceedPolicy, utc_dali_toolkit_text_view_startup, utc_dali_toolkit_text_view_cleanup},
    {"UtcDaliTextViewTestLayoutOptions01", UtcDaliTextViewTestLayoutOptions01, utc_dali_toolkit_text_view_startup, utc_dali_toolkit_text_view_cleanup},
    {"UtcDaliTextViewTestLayoutOptions02", UtcDaliTextViewTestLayoutOptions02, utc_dali_toolkit_text_view_startup, utc_dali_toolkit_text_view_cleanup},
    {"UtcDaliTextViewInsertRemoveText", UtcDaliTextViewInsertRemoveText, utc_dali_toolkit_text_view_startup, utc_dali_toolkit_text_view_cleanup},
    {"UtcDaliTextViewSnapshotEnable", UtcDaliTextViewSnapshotEnable, utc_dali_toolkit_text_view_startup, utc_dali_toolkit_text_view_cleanup},
    {"UtcDaliTextViewScroll", UtcDaliTextViewScroll, utc_dali_toolkit_text_view_startup, utc_dali_toolkit_text_view_cleanup},
    {"UtcDaliTextViewSetProperty", UtcDaliTextViewSetProperty, utc_dali_toolkit_text_view_startup, utc_dali_toolkit_text_view_cleanup},
    {"UtcDaliTextViewSetSortModifier", UtcDaliTextViewSetSortModifier, utc_dali_toolkit_text_view_startup, utc_dali_toolkit_text_view_cleanup},
    {"UtcDaliTextViewUnderlineText", UtcDaliTextViewUnderlineText, utc_dali_toolkit_text_view_startup, utc_dali_toolkit_text_view_cleanup},
    {NULL, NULL}
};

#endif // __TCT_DALI_TOOLKIT_CORE_H__
