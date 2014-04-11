#ifndef __TCT_DALI_TOOLKIT_UNMANAGED_CORE_H__
#define __TCT_DALI_TOOLKIT_UNMANAGED_CORE_H__

#include "testcase.h"

extern void album_layout_startup(void);
extern void album_layout_cleanup(void);
extern void bendy_effect_startup(void);
extern void bendy_effect_cleanup(void);
extern void blind_effect_startup(void);
extern void blind_effect_cleanup(void);
extern void bloom_view_startup(void);
extern void bloom_view_cleanup(void);
extern void builder_startup(void);
extern void builder_cleanup(void);
extern void carousel_effect_startup(void);
extern void carousel_effect_cleanup(void);
extern void checkbox_button_startup(void);
extern void checkbox_button_cleanup(void);
extern void cluster_startup(void);
extern void cluster_cleanup(void);
extern void cube_transition_effect_startup(void);
extern void cube_transition_effect_cleanup(void);
extern void depth_layout_startup(void);
extern void depth_layout_cleanup(void);
extern void utc_displacement_startup(void);
extern void utc_displacement_cleanup(void);
extern void utc_distance_field_effect_startup(void);
extern void utc_distance_field_effect_cleanup(void);
extern void utc_gaussian_blur_view_startup(void);
extern void utc_gaussian_blur_view_cleanup(void);
extern void utc_json_parser_startup(void);
extern void utc_json_parser_cleanup(void);
extern void key_input_focus_manager_startup(void);
extern void key_input_focus_manager_cleanup(void);
extern void navigation_control_startup(void);
extern void navigation_control_cleanup(void);
extern void overlay_effect_startup(void);
extern void overlay_effect_cleanup(void);
extern void dali_page_startup(void);
extern void dali_page_cleanup(void);
extern void page_turn_effect_startup(void);
extern void page_turn_effect_cleanup(void);
extern void dali_page_turn_view_startup(void);
extern void dali_page_turn_view_cleanup(void);
extern void dali_roll_layout_startup(void);
extern void dali_roll_layout_cleanup(void);
extern void rotating_selector_startup(void);
extern void rotating_selector_cleanup(void);
extern void scroll_view_startup(void);
extern void scroll_view_cleanup(void);
extern void shadow_view_startup(void);
extern void shadow_view_cleanup(void);
extern void shear_effect_startup(void);
extern void shear_effect_cleanup(void);
extern void dali_slider_startup(void);
extern void dali_slider_cleanup(void);
extern void soft_button_effect_startup(void);
extern void soft_button_effect_cleanup(void);
extern void spiral_layout_startup(void);
extern void spiral_layout_cleanup(void);
extern void spot_effect_startup(void);
extern void spot_effect_cleanup(void);
extern void square_dissolve_effect_startup(void);
extern void square_dissolve_effect_cleanup(void);
extern void dali_tableview_startup(void);
extern void dali_tableview_cleanup(void);
extern void dali_toolbar_startup(void);
extern void dali_toolbar_cleanup(void);
extern void dali_view_startup(void);
extern void dali_view_cleanup(void);
extern void dali_water_effect_startup(void);
extern void dali_water_effect_cleanup(void);
extern void dali_button_startup(void);
extern void dali_button_cleanup(void);
extern void control_impl_startup(void);
extern void control_impl_cleanup(void);
extern void dali_default_controls_startup(void);
extern void dali_default_controls_cleanup(void);
extern void dali_keyboard_focus_manager_startup(void);
extern void dali_keyboard_focus_manager_cleanup(void);
extern void dali_scroll_view_effect_startup(void);
extern void dali_scroll_view_effect_cleanup(void);

extern int UtcDaliAlbumLayoutNew(void);
extern int UtcDaliAlbumSetAndGetItemSizeFunction(void);
extern int UtcDaliAlbumSetAndGetScrollSpeedFactor(void);
extern int UtcDaliAlbumSetAndGetMaximumSwipeSpeed(void);
extern int UtcDaliAlbumLayoutSetAndGetItemFlickAnimationDuration(void);
extern int UtcDaliAlbumSetNumOfItems(void);
extern int UtcDaliAlbumSetStackNum(void);
extern int UtcDaliAlbumSetPosition(void);
extern int UtcDaliAlbumSetRotationX(void);
extern int UtcDaliAlbumSetRotationZ(void);
extern int UtcDaliAlbumSetScale(void);
extern int UtcDaliAlbumSetColor(void);
extern int UtcDaliAlbumSetCenterPosition(void);
extern int UtcDaliAlbumSetSetCenterScale(void);
extern int UtcDaliAlbumSetSetCenterColor(void);
extern int UtcDaliAlbumSetStackPosition(void);
extern int UtcDaliAlbumSetSetStackScale(void);
extern int UtcDaliAlbumSetStackColor(void);
extern int UtcDaliBendyUninitializedEffect(void);
extern int UtcDaliBendyPropertyNamesEffect(void);
extern int UtcDaliBendyDefaultValuesEffect(void);
extern int UtcDaliBendyCustomValuesEffect(void);
extern int UtcDaliBlindEffectUninitialized(void);
extern int UtcDaliBlindEffectPropertyNames(void);
extern int UtcDaliBlindEffectDefaultValues(void);
extern int UtcDaliBlindEffectCustomValues(void);
extern int UtcDaliBloomViewUninitialized(void);
extern int UtcDaliBloomViewNew(void);
extern int UtcDaliBloomViewDownCast(void);
extern int UtcDaliBloomViewPropertyNames(void);
extern int UtcDaliBloomViewAddRemove(void);
extern int UtcDaliBloomActivateDeactivate(void);
extern int UtcDaliBuilderTextActorCreateFromStyle(void);
extern int UtcDaliBuilderTextActorCreateAnimation(void);
extern int UtcDaliBuilderTextActorApplyFromStyle(void);
extern int UtcDaliBuilderAddActors(void);
extern int UtcDaliBuilderAddActorsOther(void);
extern int UtcDaliBuilderStyles(void);
extern int UtcDaliBuilderSetProperty(void);
extern int UtcDaliCarouselEffectUninitialized(void);
extern int UtcDaliCarouselEffectPropertyNames(void);
extern int UtcDaliCarouselEffectDefaultValues(void);
extern int UtcDaliCarouselEffectCustomValues(void);
extern int UtcDaliCheckBoxButtonSetGetChecked(void);
extern int UtcDaliCheckBoxButtonSetImages(void);
extern int UtcDaliClusterNew(void);
extern int UtcDaliClusterDownCast(void);
extern int UtcDaliClusterAddAndRemoveChild(void);
extern int UtcDaliClusterExpandAndCollapseChild(void);
extern int UtcDaliClusterSetAndGetStyle(void);
extern int UtcDaliCubeTransitionWaveEffectNew(void);
extern int UtcDaliCubeTransitionCrossEffectNew(void);
extern int UtcDaliCubeTransitionFoldEffectNew(void);
extern int UtcDaliCubeTransitionEffectSetGetTransitionDuration(void);
extern int UtcDaliCubeTransitionEffectSetGetCubeDisplacement(void);
extern int UtcDaliCubeTransitionEffectGetRoot(void);
extern int UtcDaliCubeTransitionEffectIsTransiting(void);
extern int UtcDaliCubeTransitionEffectSetCurrentImage(void);
extern int UtcDaliCubeTransitionEffectSetTargetImage(void);
extern int UtcDaliCubeTransitionWaveEffectStartTransition(void);
extern int UtcDaliCubeTransitionCrossEffectStartTransition(void);
extern int UtcDaliCubeTransitionFoldEffectStartTransition(void);
extern int UtcDaliCubeTransitionEffectSignalTransitionCompleted(void);
extern int UtcDaliCubeTransitionEffectPauseResumeTransition(void);
extern int UtcDaliCubeTransitionWaveEffectStopTransition(void);
extern int UtcDaliCubeTransitionCrossEffectStopTransition(void);
extern int UtcDaliCubeTransitionFoldEffectStopTransition(void);
extern int UtcDaliDepthLayoutNew(void);
extern int UtcDaliDepthLayoutSetAndGetNumberOfColumns(void);
extern int UtcDaliDepthLayoutSetAndGetNumberOfRows(void);
extern int UtcDaliDepthLayoutSetAndGetRowSpacing(void);
extern int UtcDaliDepthLayoutSetAndGetTiltAngle(void);
extern int UtcDaliDepthLayoutSetAndGetItemSizeFunction(void);
extern int UtcDaliDepthLayoutSetAndGetBottomMarginFunction(void);
extern int UtcDaliDepthLayoutSetAndGetItemTiltAngle(void);
extern int UtcDaliDepthLayoutSetAndGetColumnPositionFunction(void);
extern int UtcDaliDepthLayoutSetAndGetScrollSpeedFactor(void);
extern int UtcDaliDepthLayoutSetAndGetMaximumSwipeSpeed(void);
extern int UtcDaliDepthLayoutSetAndGetItemFlickAnimationDuration(void);
extern int UtcDaliDepthLayoutConstraintLeft(void);
extern int UtcDaliDepthLayoutConstraintRight(void);
extern int UtcDaliDepthLayoutConstraintUp(void);
extern int UtcDaliDepthLayoutConstraintDown(void);
extern int UtcDaliDepthLayoutGetScrollToPosition(void);
extern int UtcDaliDepthLayoutScrollDirection(void);
extern int UtcDaliDisplacementEffectUninitialized(void);
extern int UtcDaliDisplacementEffectNew(void);
extern int UtcDaliDisplacementEffectPropertyNames(void);
extern int UtcDaliDisplacementEffectTestSetProperty(void);
extern int UtcDaliDistanceFieldEffectUninitialized(void);
extern int UtcDaliDistanceFieldEffectPropertyNames(void);
extern int UtcDaliDistanceFieldEffectDefaultValues(void);
extern int UtcDaliDistanceFieldEffectCustomValues(void);
extern int UtcDaliGaussianBlurViewUninitialized(void);
extern int UtcDaliGaussianBlurViewNew(void);
extern int UtcDaliGaussianBlurViewDownCast(void);
extern int UtcDaliGaussianBlurViewPropertyNames(void);
extern int UtcDaliGaussianBlurViewAddRemove(void);
extern int UtcDaliGaussianBlurActivateDeactivate(void);
extern int UtcDaliGaussianBlurViewSetGetBackgroundColor(void);
extern int UtcDaliGaussianBlurViewSetGetRenderTarget(void);
extern int UtcDaliJsonParserMethod01(void);
extern int UtcDaliJsonParserMethod02(void);
extern int UtcDaliJsonParserMethod03(void);
extern int UtcDaliJsonParserMethod04(void);
extern int UtcDaliJsonParserMethod05(void);
extern int UtcDaliJsonParserMethod06(void);
extern int UtcDaliJsonParserMethod07(void);
extern int UtcDaliJsonParserMethod08(void);
extern int UtcDaliJsonParserMethod09(void);
extern int UtcDaliJsonParserMethod10(void);
extern int UtcDaliKeyInputFocusManagerGet(void);
extern int UtcDaliKeyInputFocusManagerSetFocus(void);
extern int UtcDaliKeyInputFocusManagerGetCurrentFocusControl(void);
extern int UtcDaliKeyInputFocusManagerRemoveFocus(void);
extern int UtcDaliKeyInputFocusManagerIsKeyboardListener(void);
extern int UtcDaliKeyInputFocusManagerSignalKeyInputFocusChanged(void);
extern int UtcDaliKeyInputFocusManagerSignalUnhandledKeyEvent(void);
extern int UtcDaliNavigationControlNew(void);
extern int UtcDaliNavigationControlDownCast(void);
extern int UtcDaliNavigationControlPushItem(void);
extern int UtcDaliNavigationControlPopItem(void);
extern int UtcDaliNavigationControlGetItemCount(void);
extern int UtcDaliNavigationControlGetItem(void);
extern int UtcDaliNavigationControlGetCurrentItem(void);
extern int UtcDaliNavigationControlSetBackground(void);
extern int UtcDaliNavigationControlCreateNavigationToolBar(void);
extern int UtcDaliNavigationControlCreateNavigationTitleBar(void);
extern int UtcDaliNavigationLayoutNew(void);
extern int UtcDaliNavigationLayoutColumns(void);
extern int UtcDaliNavigationLayoutSetGetOrientation(void);
extern int UtcDaliNavigationLayoutTestConstraintLeft(void);
extern int UtcDaliNavigationLayoutTestConstraintRight(void);
extern int UtcDaliNavigationLayoutTestConstraintUp(void);
extern int UtcDaliNavigationLayoutTestConstraintDown(void);
extern int UtcDaliNavigationLayoutScrollDirection(void);
extern int UtcDaliNavigationLayoutSetGetColumnSpacing(void);
extern int UtcDaliNavigationLayoutSetGetTopMargin(void);
extern int UtcDaliNavigationLayoutSetGetBottomMargin(void);
extern int UtcDaliNavigationLayoutSetGetScrollSpeedFactor(void);
extern int UtcDaliNavigationLayoutSetGetMaximumSwipeSpeed(void);
extern int UtcDaliNavigationLayoutSetAndGetItemFlickAnimationDuration(void);
extern int UtcDaliNavigationLayoutGetScrollToPosition(void);
extern int UtcDaliOverlayConstructor(void);
extern int UtcDaliOverlayUninitializedEffect(void);
extern int UtcDaliPageNew(void);
extern int UtcDaliPageDownCast(void);
extern int UtcDaliPageSetGetTitle(void);
extern int UtcDaliPageSetGetSubTitle(void);
extern int UtcDaliPageSetGetTitleIcon(void);
extern int UtcDaliPageAddGetToolBarControl(void);
extern int UtcDaliPageAddGetTitleBarControl(void);
extern int UtcDaliPageSetGetPopupMenu(void);
extern int UtcDaliPageTurnEffectApply(void);
extern int UtcDaliPageTurnEffectConstruct(void);
extern int UtcDaliPageTurnPortraitViewNew(void);
extern int UtcDaliPageTurnLandscapeViewNew(void);
extern int UtcDaliPageTurnViewSetAndGetSpineShadowParameter(void);
extern int UtcDaliPageTurnViewGoToPageAndGetCurrentPage(void);
extern int UtcDaliPageTurnViewEnterLeaveEditMode(void);
extern int UtcDaliPageTurnViewGetHitActor(void);
extern int UtcDaliPageTurnViewRefresh(void);
extern int UtcDaliPageTurnViewSignals(void);
extern int UtcDaliRollLayoutNew(void);
extern int UtcDaliRollLayoutSetAndGetRowSpacing(void);
extern int UtcDaliRollLayoutSetAndGetItemSizeFunction(void);
extern int UtcDaliRollLayoutSetAndGetScrollSpeedFactor(void);
extern int UtcDaliRollLayoutSetAndGetMaximumSwipeSpeed(void);
extern int UtcDaliRollLayoutSetAndGetItemFlickAnimationDuration(void);
extern int UtcDaliRollLayoutConstraintLeft(void);
extern int UtcDaliRollLayoutConstraintRight(void);
extern int UtcDaliRollLayoutConstraintUp(void);
extern int UtcDaliRollLayoutConstraintDown(void);
extern int UtcDaliRollLayoutScrollDirection(void);
extern int UtcDaliRotatingSelectorNew(void);
extern int UtcDaliRotatingSelectorSetSelected(void);
extern int UtcDaliRotatingSelectorSetSelectedAndUnSelectedActor(void);
extern int UtcDaliRotatingSelectorSetSelectable(void);
extern int UtcDaliRotatingSelectorSignalSelected(void);
extern int UtcDaliScrollViewNew(void);
extern int UtcDaliScrollViewDownCast(void);
extern int UtcDaliScrollViewScrollToPosition(void);
extern int UtcDaliScrollViewScrollToPage(void);
extern int UtcDaliScrollViewScrollToActor(void);
extern int UtcDaliScrollViewScrollToSnapPoint(void);
extern int UtcDaliScrollViewRulerScale(void);
extern int UtcDaliScrollViewTransformTo(void);
extern int UtcDaliScrollViewRefreshInterval(void);
extern int UtcDaliScrollViewWrapMode(void);
extern int UtcDaliScrollViewActorAutoSnap(void);
extern int UtcDaliScrollViewSignalsStartComplete(void);
extern int UtcDaliScrollViewSignalsUpdate(void);
extern int UtcDaliScrollViewSignalsClamped(void);
extern int UtcDaliScrollViewScrollSensitive(void);
extern int UtcDaliScrollViewTouchesRequired(void);
extern int UtcDaliScrollViewAxisAutoLock(void);
extern int UtcDaliScrollViewAxisAutoLockGradient(void);
extern int UtcDaliScrollViewConstraints(void);
extern int UtcDaliScrollViewBind(void);
extern int UtcDaliRulerEnableDisable(void);
extern int UtcDaliRulerDomainEnableDisable(void);
extern int UtcDaliRulerSnapAndClamp(void);
extern int UtcDaliRulerFixedRulerSpacing(void);
extern int UtcDaliScrollViewOvershoot(void);
extern int UtcDaliScrollViewSnapAlphaFunction(void);
extern int UtcDaliScrollViewSnapDuration(void);
extern int UtcDaliScrollViewSignalsSnapStart(void);
extern int UtcDaliScrollViewSetMouseWheelScrollDistanceStep(void);
extern int UtcDaliScrollViewGetSet(void);
extern int UtcDaliShadowViewUninitialized(void);
extern int UtcDaliShadowViewNew(void);
extern int UtcDaliShadowViewDownCast(void);
extern int UtcDaliShadowViewPropertyNames(void);
extern int UtcDaliShadowViewAddRemove(void);
extern int UtcDaliShadowViewActivateDeactivate(void);
extern int UtcDaliShearEffectUninitialized(void);
extern int UtcDaliShearEffectPropertyNames(void);
extern int UtcDaliShearEffectDefaultValues(void);
extern int UtcDaliShearEffectCustomValues(void);
extern int UtcDaliSliderNew(void);
extern int UtcDaliSliderDestructor(void);
extern int UtcDaliSliderDownCast(void);
extern int UtcDaliSliderSignals(void);
extern int UtcDaliSoftButtonEffectUninitialized(void);
extern int UtcDaliSoftButtonEffectNew(void);
extern int UtcDaliSoftButtonEffectPropertyNames(void);
extern int UtcDaliSpiralLayoutNew(void);
extern int UtcDaliSpiralSetAndGetItemSizeFunction(void);
extern int UtcDaliSpiralSetAndGetItemSpacing(void);
extern int UtcDaliSpiralSetAndGetRevolutionDistance(void);
extern int UtcDaliSpiralSetAndGetSpiralRadiusFunction(void);
extern int UtcDaliSpiralSetAndGetTopItemAlignment(void);
extern int UtcDaliSpiralSetAndGetScrollSpeedFactor(void);
extern int UtcDaliSpiralSetAndGetMaximumSwipeSpeed(void);
extern int UtcDaliSpiralLayoutSetAndGetItemFlickAnimationDuration(void);
extern int UtcDaliSpiralLayoutConstraintLeft(void);
extern int UtcDaliSpiralLayoutConstraintRight(void);
extern int UtcDaliSpiralLayoutConstraintUp(void);
extern int UtcDaliSpiralLayoutConstraintDown(void);
extern int UtcDaliSpiralLayoutScrollDirection(void);
extern int UtcDaliSpiralLayoutGetScrollToPosition(void);
extern int UtcDaliSpotUninitializedEffect(void);
extern int UtcDaliSpotPropertyNamesEffect(void);
extern int UtcDaliSpotDefaultValuesEffect(void);
extern int UtcDaliSpotCustomValuesEffect(void);
extern int UtcDaliSquareDissolveEffectUninitialized(void);
extern int UtcDaliSquareDissolveEffectPropertyNames(void);
extern int UtcDaliSquareDissolveEffectDefaultValues(void);
extern int UtcDaliSquareDissolveEffectCustomValues(void);
extern int UtcDaliTableViewNew(void);
extern int UtcDaliTableViewMetricsPadding(void);
extern int UtcDaliTableViewMetricsFixed(void);
extern int UtcDaliTableViewMetricsRelative(void);
extern int UtcDaliTableViewAnimation(void);
extern int UtcDaliTableViewChild(void);
extern int UtcDaliTableViewAdd(void);
extern int UtcDaliTableViewCells(void);
extern int UtcDaliTableViewChildAssert(void);
extern int UtcDaliTableViewMetricsAssert(void);
extern int UtcDaliToolBarNew(void);
extern int UtcDaliToolBarSetBackground(void);
extern int UtcDaliToolBarAddControl01(void);
extern int UtcDaliToolBarAddControl02(void);
extern int UtcDaliToolBarRemoveControl01(void);
extern int UtcDaliToolBarRemoveControl02(void);
extern int UtcDaliViewNew(void);
extern int UtcDaliViewAddGetRemoveContentLayer01(void);
extern int UtcDaliViewAddGetRemoveContentLayer02(void);
extern int UtcDaliViewSetGetBackgroundLayer01(void);
extern int UtcDaliViewSetGetBackgroundLayer02(void);
extern int UtcDaliViewSetOrientationFunction(void);
extern int UtcDaliViewOrientationChanged(void);
extern int UtcSetAutoRotate(void);
extern int UtcDaliWaterEffectUninitialized(void);
extern int UtcDaliWaterEffectPropertyNames(void);
extern int UtcDaliWaterEffectOutOfBounds(void);
extern int UtcDaliWaterEffectDefaultValues(void);
extern int UtcDaliWaterEffectCustomValues(void);
extern int UtcDaliWaterEffectGetAmplitudePositive(void);
extern int UtcDaliWaterEffectGetAmplitudeNegative(void);
extern int UtcDaliWaterEffectGetCenterPositive(void);
extern int UtcDaliWaterEffectGetCenterNegative(void);
extern int UtcDaliWaterEffectGetPropagationPositive(void);
extern int UtcDaliWaterEffectGetPropagationNegative(void);
extern int UtcDaliButtonNew(void);
extern int UtcDaliButtonProperties(void);
extern int UtcDaliButtonSetGetDimmed(void);
extern int UtcDaliControlImplTypeRegistry(void);
extern int UtcDaliControlImplMouseWheelEvent(void);
extern int UtcDaliDefaultControlsCreateCheckBoxButton(void);
extern int UtcDaliItemViewAddAndGetLayout(void);
extern int UtcDaliItemViewAddAndRemoveLayout(void);
extern int UtcDaliItemViewActivateLayoutAndGetActiveLayout(void);
extern int UtcDaliKeyboardFocusManagerGet(void);
extern int UtcDaliKeyboardFocusManagerMoveFocus(void);
extern int UtcDaliKeyboardFocusManagerSignalFocusGroupChanged(void);
extern int UtcDaliScrollViewPageCubeEffectSetup(void);
extern int UtcDaliScrollViewPageCarouselEffectSetup(void);
extern int UtcDaliScrollViewCarouselEffectSetup(void);
extern int UtcDaliScrollViewDepthEffectSetup(void);
extern int UtcDaliScrollViewPageCubeEffectTest(void);
extern int UtcDaliScrollViewPageCarouselEffectTest(void);
extern int UtcDaliScrollViewCarouselEffectTest(void);
extern int UtcDaliScrollViewDepthEffectTest(void);
extern int UtcDaliTextInputSetActiveStyle(void);
extern int UtcDaliTextInputApplyStyleToSelectedText(void);
extern int UtcDaliTextInputApplyStyleToAll(void);
extern int UtcDaliTextInputGetStyleAtCursor(void);
extern int UtcDaliTextInputSetAndGetTextAlignment(void);
extern int UtcDaliTextInputSetAndGetMultilinePolicy(void);
extern int UtcDaliTextInputSetAndGetExceedEnabled(void);

testcase tc_array[] = {
    {"UtcDaliAlbumLayoutNew", UtcDaliAlbumLayoutNew, album_layout_startup, album_layout_cleanup},
    {"UtcDaliAlbumSetAndGetItemSizeFunction", UtcDaliAlbumSetAndGetItemSizeFunction, album_layout_startup, album_layout_cleanup},
    {"UtcDaliAlbumSetAndGetScrollSpeedFactor", UtcDaliAlbumSetAndGetScrollSpeedFactor, album_layout_startup, album_layout_cleanup},
    {"UtcDaliAlbumSetAndGetMaximumSwipeSpeed", UtcDaliAlbumSetAndGetMaximumSwipeSpeed, album_layout_startup, album_layout_cleanup},
    {"UtcDaliAlbumLayoutSetAndGetItemFlickAnimationDuration", UtcDaliAlbumLayoutSetAndGetItemFlickAnimationDuration, album_layout_startup, album_layout_cleanup},
    {"UtcDaliAlbumSetNumOfItems", UtcDaliAlbumSetNumOfItems, album_layout_startup, album_layout_cleanup},
    {"UtcDaliAlbumSetStackNum", UtcDaliAlbumSetStackNum, album_layout_startup, album_layout_cleanup},
    {"UtcDaliAlbumSetPosition", UtcDaliAlbumSetPosition, album_layout_startup, album_layout_cleanup},
    {"UtcDaliAlbumSetRotationX", UtcDaliAlbumSetRotationX, album_layout_startup, album_layout_cleanup},
    {"UtcDaliAlbumSetRotationZ", UtcDaliAlbumSetRotationZ, album_layout_startup, album_layout_cleanup},
    {"UtcDaliAlbumSetScale", UtcDaliAlbumSetScale, album_layout_startup, album_layout_cleanup},
    {"UtcDaliAlbumSetColor", UtcDaliAlbumSetColor, album_layout_startup, album_layout_cleanup},
    {"UtcDaliAlbumSetCenterPosition", UtcDaliAlbumSetCenterPosition, album_layout_startup, album_layout_cleanup},
    {"UtcDaliAlbumSetSetCenterScale", UtcDaliAlbumSetSetCenterScale, album_layout_startup, album_layout_cleanup},
    {"UtcDaliAlbumSetSetCenterColor", UtcDaliAlbumSetSetCenterColor, album_layout_startup, album_layout_cleanup},
    {"UtcDaliAlbumSetStackPosition", UtcDaliAlbumSetStackPosition, album_layout_startup, album_layout_cleanup},
    {"UtcDaliAlbumSetSetStackScale", UtcDaliAlbumSetSetStackScale, album_layout_startup, album_layout_cleanup},
    {"UtcDaliAlbumSetStackColor", UtcDaliAlbumSetStackColor, album_layout_startup, album_layout_cleanup},
    {"UtcDaliBendyUninitializedEffect", UtcDaliBendyUninitializedEffect, bendy_effect_startup, bendy_effect_cleanup},
    {"UtcDaliBendyPropertyNamesEffect", UtcDaliBendyPropertyNamesEffect, bendy_effect_startup, bendy_effect_cleanup},
    {"UtcDaliBendyDefaultValuesEffect", UtcDaliBendyDefaultValuesEffect, bendy_effect_startup, bendy_effect_cleanup},
    {"UtcDaliBendyCustomValuesEffect", UtcDaliBendyCustomValuesEffect, bendy_effect_startup, bendy_effect_cleanup},
    {"UtcDaliBlindEffectUninitialized", UtcDaliBlindEffectUninitialized, blind_effect_startup, blind_effect_cleanup},
    {"UtcDaliBlindEffectPropertyNames", UtcDaliBlindEffectPropertyNames, blind_effect_startup, blind_effect_cleanup},
    {"UtcDaliBlindEffectDefaultValues", UtcDaliBlindEffectDefaultValues, blind_effect_startup, blind_effect_cleanup},
    {"UtcDaliBlindEffectCustomValues", UtcDaliBlindEffectCustomValues, blind_effect_startup, blind_effect_cleanup},
    {"UtcDaliBloomViewUninitialized", UtcDaliBloomViewUninitialized, bloom_view_startup, bloom_view_cleanup},
    {"UtcDaliBloomViewNew", UtcDaliBloomViewNew, bloom_view_startup, bloom_view_cleanup},
    {"UtcDaliBloomViewDownCast", UtcDaliBloomViewDownCast, bloom_view_startup, bloom_view_cleanup},
    {"UtcDaliBloomViewPropertyNames", UtcDaliBloomViewPropertyNames, bloom_view_startup, bloom_view_cleanup},
    {"UtcDaliBloomViewAddRemove", UtcDaliBloomViewAddRemove, bloom_view_startup, bloom_view_cleanup},
    {"UtcDaliBloomActivateDeactivate", UtcDaliBloomActivateDeactivate, bloom_view_startup, bloom_view_cleanup},
    {"UtcDaliBuilderTextActorCreateFromStyle", UtcDaliBuilderTextActorCreateFromStyle, builder_startup, builder_cleanup},
    {"UtcDaliBuilderTextActorCreateAnimation", UtcDaliBuilderTextActorCreateAnimation, builder_startup, builder_cleanup},
    {"UtcDaliBuilderTextActorApplyFromStyle", UtcDaliBuilderTextActorApplyFromStyle, builder_startup, builder_cleanup},
    {"UtcDaliBuilderAddActors", UtcDaliBuilderAddActors, builder_startup, builder_cleanup},
    {"UtcDaliBuilderAddActorsOther", UtcDaliBuilderAddActorsOther, builder_startup, builder_cleanup},
    {"UtcDaliBuilderStyles", UtcDaliBuilderStyles, builder_startup, builder_cleanup},
    {"UtcDaliBuilderSetProperty", UtcDaliBuilderSetProperty, builder_startup, builder_cleanup},
    {"UtcDaliCarouselEffectUninitialized", UtcDaliCarouselEffectUninitialized, carousel_effect_startup, carousel_effect_cleanup},
    {"UtcDaliCarouselEffectPropertyNames", UtcDaliCarouselEffectPropertyNames, carousel_effect_startup, carousel_effect_cleanup},
    {"UtcDaliCarouselEffectDefaultValues", UtcDaliCarouselEffectDefaultValues, carousel_effect_startup, carousel_effect_cleanup},
    {"UtcDaliCarouselEffectCustomValues", UtcDaliCarouselEffectCustomValues, carousel_effect_startup, carousel_effect_cleanup},
    {"UtcDaliCheckBoxButtonSetGetChecked", UtcDaliCheckBoxButtonSetGetChecked, checkbox_button_startup, checkbox_button_cleanup},
    {"UtcDaliCheckBoxButtonSetImages", UtcDaliCheckBoxButtonSetImages, checkbox_button_startup, checkbox_button_cleanup},
    {"UtcDaliClusterNew", UtcDaliClusterNew, cluster_startup, cluster_cleanup},
    {"UtcDaliClusterDownCast", UtcDaliClusterDownCast, cluster_startup, cluster_cleanup},
    {"UtcDaliClusterAddAndRemoveChild", UtcDaliClusterAddAndRemoveChild, cluster_startup, cluster_cleanup},
    {"UtcDaliClusterExpandAndCollapseChild", UtcDaliClusterExpandAndCollapseChild, cluster_startup, cluster_cleanup},
    {"UtcDaliClusterSetAndGetStyle", UtcDaliClusterSetAndGetStyle, cluster_startup, cluster_cleanup},
    {"UtcDaliCubeTransitionWaveEffectNew", UtcDaliCubeTransitionWaveEffectNew, cube_transition_effect_startup, cube_transition_effect_cleanup},
    {"UtcDaliCubeTransitionCrossEffectNew", UtcDaliCubeTransitionCrossEffectNew, cube_transition_effect_startup, cube_transition_effect_cleanup},
    {"UtcDaliCubeTransitionFoldEffectNew", UtcDaliCubeTransitionFoldEffectNew, cube_transition_effect_startup, cube_transition_effect_cleanup},
    {"UtcDaliCubeTransitionEffectSetGetTransitionDuration", UtcDaliCubeTransitionEffectSetGetTransitionDuration, cube_transition_effect_startup, cube_transition_effect_cleanup},
    {"UtcDaliCubeTransitionEffectSetGetCubeDisplacement", UtcDaliCubeTransitionEffectSetGetCubeDisplacement, cube_transition_effect_startup, cube_transition_effect_cleanup},
    {"UtcDaliCubeTransitionEffectGetRoot", UtcDaliCubeTransitionEffectGetRoot, cube_transition_effect_startup, cube_transition_effect_cleanup},
    {"UtcDaliCubeTransitionEffectIsTransiting", UtcDaliCubeTransitionEffectIsTransiting, cube_transition_effect_startup, cube_transition_effect_cleanup},
    {"UtcDaliCubeTransitionEffectSetCurrentImage", UtcDaliCubeTransitionEffectSetCurrentImage, cube_transition_effect_startup, cube_transition_effect_cleanup},
    {"UtcDaliCubeTransitionEffectSetTargetImage", UtcDaliCubeTransitionEffectSetTargetImage, cube_transition_effect_startup, cube_transition_effect_cleanup},
    {"UtcDaliCubeTransitionWaveEffectStartTransition", UtcDaliCubeTransitionWaveEffectStartTransition, cube_transition_effect_startup, cube_transition_effect_cleanup},
    {"UtcDaliCubeTransitionCrossEffectStartTransition", UtcDaliCubeTransitionCrossEffectStartTransition, cube_transition_effect_startup, cube_transition_effect_cleanup},
    {"UtcDaliCubeTransitionFoldEffectStartTransition", UtcDaliCubeTransitionFoldEffectStartTransition, cube_transition_effect_startup, cube_transition_effect_cleanup},
    {"UtcDaliCubeTransitionEffectSignalTransitionCompleted", UtcDaliCubeTransitionEffectSignalTransitionCompleted, cube_transition_effect_startup, cube_transition_effect_cleanup},
    {"UtcDaliCubeTransitionEffectPauseResumeTransition", UtcDaliCubeTransitionEffectPauseResumeTransition, cube_transition_effect_startup, cube_transition_effect_cleanup},
    {"UtcDaliCubeTransitionWaveEffectStopTransition", UtcDaliCubeTransitionWaveEffectStopTransition, cube_transition_effect_startup, cube_transition_effect_cleanup},
    {"UtcDaliCubeTransitionCrossEffectStopTransition", UtcDaliCubeTransitionCrossEffectStopTransition, cube_transition_effect_startup, cube_transition_effect_cleanup},
    {"UtcDaliCubeTransitionFoldEffectStopTransition", UtcDaliCubeTransitionFoldEffectStopTransition, cube_transition_effect_startup, cube_transition_effect_cleanup},
    {"UtcDaliDepthLayoutNew", UtcDaliDepthLayoutNew, depth_layout_startup, depth_layout_cleanup},
    {"UtcDaliDepthLayoutSetAndGetNumberOfColumns", UtcDaliDepthLayoutSetAndGetNumberOfColumns, depth_layout_startup, depth_layout_cleanup},
    {"UtcDaliDepthLayoutSetAndGetNumberOfRows", UtcDaliDepthLayoutSetAndGetNumberOfRows, depth_layout_startup, depth_layout_cleanup},
    {"UtcDaliDepthLayoutSetAndGetRowSpacing", UtcDaliDepthLayoutSetAndGetRowSpacing, depth_layout_startup, depth_layout_cleanup},
    {"UtcDaliDepthLayoutSetAndGetTiltAngle", UtcDaliDepthLayoutSetAndGetTiltAngle, depth_layout_startup, depth_layout_cleanup},
    {"UtcDaliDepthLayoutSetAndGetItemSizeFunction", UtcDaliDepthLayoutSetAndGetItemSizeFunction, depth_layout_startup, depth_layout_cleanup},
    {"UtcDaliDepthLayoutSetAndGetBottomMarginFunction", UtcDaliDepthLayoutSetAndGetBottomMarginFunction, depth_layout_startup, depth_layout_cleanup},
    {"UtcDaliDepthLayoutSetAndGetItemTiltAngle", UtcDaliDepthLayoutSetAndGetItemTiltAngle, depth_layout_startup, depth_layout_cleanup},
    {"UtcDaliDepthLayoutSetAndGetColumnPositionFunction", UtcDaliDepthLayoutSetAndGetColumnPositionFunction, depth_layout_startup, depth_layout_cleanup},
    {"UtcDaliDepthLayoutSetAndGetScrollSpeedFactor", UtcDaliDepthLayoutSetAndGetScrollSpeedFactor, depth_layout_startup, depth_layout_cleanup},
    {"UtcDaliDepthLayoutSetAndGetMaximumSwipeSpeed", UtcDaliDepthLayoutSetAndGetMaximumSwipeSpeed, depth_layout_startup, depth_layout_cleanup},
    {"UtcDaliDepthLayoutSetAndGetItemFlickAnimationDuration", UtcDaliDepthLayoutSetAndGetItemFlickAnimationDuration, depth_layout_startup, depth_layout_cleanup},
    {"UtcDaliDepthLayoutConstraintLeft", UtcDaliDepthLayoutConstraintLeft, depth_layout_startup, depth_layout_cleanup},
    {"UtcDaliDepthLayoutConstraintRight", UtcDaliDepthLayoutConstraintRight, depth_layout_startup, depth_layout_cleanup},
    {"UtcDaliDepthLayoutConstraintUp", UtcDaliDepthLayoutConstraintUp, depth_layout_startup, depth_layout_cleanup},
    {"UtcDaliDepthLayoutConstraintDown", UtcDaliDepthLayoutConstraintDown, depth_layout_startup, depth_layout_cleanup},
    {"UtcDaliDepthLayoutGetScrollToPosition", UtcDaliDepthLayoutGetScrollToPosition, depth_layout_startup, depth_layout_cleanup},
    {"UtcDaliDepthLayoutScrollDirection", UtcDaliDepthLayoutScrollDirection, depth_layout_startup, depth_layout_cleanup},
    {"UtcDaliDisplacementEffectUninitialized", UtcDaliDisplacementEffectUninitialized, utc_displacement_startup, utc_displacement_cleanup},
    {"UtcDaliDisplacementEffectNew", UtcDaliDisplacementEffectNew, utc_displacement_startup, utc_displacement_cleanup},
    {"UtcDaliDisplacementEffectPropertyNames", UtcDaliDisplacementEffectPropertyNames, utc_displacement_startup, utc_displacement_cleanup},
    {"UtcDaliDisplacementEffectTestSetProperty", UtcDaliDisplacementEffectTestSetProperty, utc_displacement_startup, utc_displacement_cleanup},
    {"UtcDaliDistanceFieldEffectUninitialized", UtcDaliDistanceFieldEffectUninitialized, utc_distance_field_effect_startup, utc_distance_field_effect_cleanup},
    {"UtcDaliDistanceFieldEffectPropertyNames", UtcDaliDistanceFieldEffectPropertyNames, utc_distance_field_effect_startup, utc_distance_field_effect_cleanup},
    {"UtcDaliDistanceFieldEffectDefaultValues", UtcDaliDistanceFieldEffectDefaultValues, utc_distance_field_effect_startup, utc_distance_field_effect_cleanup},
    {"UtcDaliDistanceFieldEffectCustomValues", UtcDaliDistanceFieldEffectCustomValues, utc_distance_field_effect_startup, utc_distance_field_effect_cleanup},
    {"UtcDaliGaussianBlurViewUninitialized", UtcDaliGaussianBlurViewUninitialized, utc_gaussian_blur_view_startup, utc_gaussian_blur_view_cleanup},
    {"UtcDaliGaussianBlurViewNew", UtcDaliGaussianBlurViewNew, utc_gaussian_blur_view_startup, utc_gaussian_blur_view_cleanup},
    {"UtcDaliGaussianBlurViewDownCast", UtcDaliGaussianBlurViewDownCast, utc_gaussian_blur_view_startup, utc_gaussian_blur_view_cleanup},
    {"UtcDaliGaussianBlurViewPropertyNames", UtcDaliGaussianBlurViewPropertyNames, utc_gaussian_blur_view_startup, utc_gaussian_blur_view_cleanup},
    {"UtcDaliGaussianBlurViewAddRemove", UtcDaliGaussianBlurViewAddRemove, utc_gaussian_blur_view_startup, utc_gaussian_blur_view_cleanup},
    {"UtcDaliGaussianBlurActivateDeactivate", UtcDaliGaussianBlurActivateDeactivate, utc_gaussian_blur_view_startup, utc_gaussian_blur_view_cleanup},
    {"UtcDaliGaussianBlurViewSetGetBackgroundColor", UtcDaliGaussianBlurViewSetGetBackgroundColor, utc_gaussian_blur_view_startup, utc_gaussian_blur_view_cleanup},
    {"UtcDaliGaussianBlurViewSetGetRenderTarget", UtcDaliGaussianBlurViewSetGetRenderTarget, utc_gaussian_blur_view_startup, utc_gaussian_blur_view_cleanup},
    {"UtcDaliJsonParserMethod01", UtcDaliJsonParserMethod01, utc_json_parser_startup, utc_json_parser_cleanup},
    {"UtcDaliJsonParserMethod02", UtcDaliJsonParserMethod02, utc_json_parser_startup, utc_json_parser_cleanup},
    {"UtcDaliJsonParserMethod03", UtcDaliJsonParserMethod03, utc_json_parser_startup, utc_json_parser_cleanup},
    {"UtcDaliJsonParserMethod04", UtcDaliJsonParserMethod04, utc_json_parser_startup, utc_json_parser_cleanup},
    {"UtcDaliJsonParserMethod05", UtcDaliJsonParserMethod05, utc_json_parser_startup, utc_json_parser_cleanup},
    {"UtcDaliJsonParserMethod06", UtcDaliJsonParserMethod06, utc_json_parser_startup, utc_json_parser_cleanup},
    {"UtcDaliJsonParserMethod07", UtcDaliJsonParserMethod07, utc_json_parser_startup, utc_json_parser_cleanup},
    {"UtcDaliJsonParserMethod08", UtcDaliJsonParserMethod08, utc_json_parser_startup, utc_json_parser_cleanup},
    {"UtcDaliJsonParserMethod09", UtcDaliJsonParserMethod09, utc_json_parser_startup, utc_json_parser_cleanup},
    {"UtcDaliJsonParserMethod10", UtcDaliJsonParserMethod10, utc_json_parser_startup, utc_json_parser_cleanup},
    {"UtcDaliKeyInputFocusManagerGet", UtcDaliKeyInputFocusManagerGet, key_input_focus_manager_startup, key_input_focus_manager_cleanup},
    {"UtcDaliKeyInputFocusManagerSetFocus", UtcDaliKeyInputFocusManagerSetFocus, key_input_focus_manager_startup, key_input_focus_manager_cleanup},
    {"UtcDaliKeyInputFocusManagerGetCurrentFocusControl", UtcDaliKeyInputFocusManagerGetCurrentFocusControl, key_input_focus_manager_startup, key_input_focus_manager_cleanup},
    {"UtcDaliKeyInputFocusManagerRemoveFocus", UtcDaliKeyInputFocusManagerRemoveFocus, key_input_focus_manager_startup, key_input_focus_manager_cleanup},
    {"UtcDaliKeyInputFocusManagerIsKeyboardListener", UtcDaliKeyInputFocusManagerIsKeyboardListener, key_input_focus_manager_startup, key_input_focus_manager_cleanup},
    {"UtcDaliKeyInputFocusManagerSignalKeyInputFocusChanged", UtcDaliKeyInputFocusManagerSignalKeyInputFocusChanged, key_input_focus_manager_startup, key_input_focus_manager_cleanup},
    {"UtcDaliKeyInputFocusManagerSignalUnhandledKeyEvent", UtcDaliKeyInputFocusManagerSignalUnhandledKeyEvent, key_input_focus_manager_startup, key_input_focus_manager_cleanup},
    {"UtcDaliNavigationControlNew", UtcDaliNavigationControlNew, navigation_control_startup, navigation_control_cleanup},
    {"UtcDaliNavigationControlDownCast", UtcDaliNavigationControlDownCast, navigation_control_startup, navigation_control_cleanup},
    {"UtcDaliNavigationControlPushItem", UtcDaliNavigationControlPushItem, navigation_control_startup, navigation_control_cleanup},
    {"UtcDaliNavigationControlPopItem", UtcDaliNavigationControlPopItem, navigation_control_startup, navigation_control_cleanup},
    {"UtcDaliNavigationControlGetItemCount", UtcDaliNavigationControlGetItemCount, navigation_control_startup, navigation_control_cleanup},
    {"UtcDaliNavigationControlGetItem", UtcDaliNavigationControlGetItem, navigation_control_startup, navigation_control_cleanup},
    {"UtcDaliNavigationControlGetCurrentItem", UtcDaliNavigationControlGetCurrentItem, navigation_control_startup, navigation_control_cleanup},
    {"UtcDaliNavigationControlSetBackground", UtcDaliNavigationControlSetBackground, navigation_control_startup, navigation_control_cleanup},
    {"UtcDaliNavigationControlCreateNavigationToolBar", UtcDaliNavigationControlCreateNavigationToolBar, navigation_control_startup, navigation_control_cleanup},
    {"UtcDaliNavigationControlCreateNavigationTitleBar", UtcDaliNavigationControlCreateNavigationTitleBar, navigation_control_startup, navigation_control_cleanup},
    {"UtcDaliNavigationLayoutNew", UtcDaliNavigationLayoutNew, navigation_control_startup, navigation_control_cleanup},
    {"UtcDaliNavigationLayoutColumns", UtcDaliNavigationLayoutColumns, navigation_control_startup, navigation_control_cleanup},
    {"UtcDaliNavigationLayoutSetGetOrientation", UtcDaliNavigationLayoutSetGetOrientation, navigation_control_startup, navigation_control_cleanup},
    {"UtcDaliNavigationLayoutTestConstraintLeft", UtcDaliNavigationLayoutTestConstraintLeft, navigation_control_startup, navigation_control_cleanup},
    {"UtcDaliNavigationLayoutTestConstraintRight", UtcDaliNavigationLayoutTestConstraintRight, navigation_control_startup, navigation_control_cleanup},
    {"UtcDaliNavigationLayoutTestConstraintUp", UtcDaliNavigationLayoutTestConstraintUp, navigation_control_startup, navigation_control_cleanup},
    {"UtcDaliNavigationLayoutTestConstraintDown", UtcDaliNavigationLayoutTestConstraintDown, navigation_control_startup, navigation_control_cleanup},
    {"UtcDaliNavigationLayoutScrollDirection", UtcDaliNavigationLayoutScrollDirection, navigation_control_startup, navigation_control_cleanup},
    {"UtcDaliNavigationLayoutSetGetColumnSpacing", UtcDaliNavigationLayoutSetGetColumnSpacing, navigation_control_startup, navigation_control_cleanup},
    {"UtcDaliNavigationLayoutSetGetTopMargin", UtcDaliNavigationLayoutSetGetTopMargin, navigation_control_startup, navigation_control_cleanup},
    {"UtcDaliNavigationLayoutSetGetBottomMargin", UtcDaliNavigationLayoutSetGetBottomMargin, navigation_control_startup, navigation_control_cleanup},
    {"UtcDaliNavigationLayoutSetGetScrollSpeedFactor", UtcDaliNavigationLayoutSetGetScrollSpeedFactor, navigation_control_startup, navigation_control_cleanup},
    {"UtcDaliNavigationLayoutSetGetMaximumSwipeSpeed", UtcDaliNavigationLayoutSetGetMaximumSwipeSpeed, navigation_control_startup, navigation_control_cleanup},
    {"UtcDaliNavigationLayoutSetAndGetItemFlickAnimationDuration", UtcDaliNavigationLayoutSetAndGetItemFlickAnimationDuration, navigation_control_startup, navigation_control_cleanup},
    {"UtcDaliNavigationLayoutGetScrollToPosition", UtcDaliNavigationLayoutGetScrollToPosition, navigation_control_startup, navigation_control_cleanup},
    {"UtcDaliOverlayConstructor", UtcDaliOverlayConstructor, overlay_effect_startup, overlay_effect_cleanup},
    {"UtcDaliOverlayUninitializedEffect", UtcDaliOverlayUninitializedEffect, overlay_effect_startup, overlay_effect_cleanup},
    {"UtcDaliPageNew", UtcDaliPageNew, dali_page_startup, dali_page_cleanup},
    {"UtcDaliPageDownCast", UtcDaliPageDownCast, dali_page_startup, dali_page_cleanup},
    {"UtcDaliPageSetGetTitle", UtcDaliPageSetGetTitle, dali_page_startup, dali_page_cleanup},
    {"UtcDaliPageSetGetSubTitle", UtcDaliPageSetGetSubTitle, dali_page_startup, dali_page_cleanup},
    {"UtcDaliPageSetGetTitleIcon", UtcDaliPageSetGetTitleIcon, dali_page_startup, dali_page_cleanup},
    {"UtcDaliPageAddGetToolBarControl", UtcDaliPageAddGetToolBarControl, dali_page_startup, dali_page_cleanup},
    {"UtcDaliPageAddGetTitleBarControl", UtcDaliPageAddGetTitleBarControl, dali_page_startup, dali_page_cleanup},
    {"UtcDaliPageSetGetPopupMenu", UtcDaliPageSetGetPopupMenu, dali_page_startup, dali_page_cleanup},
    {"UtcDaliPageTurnEffectApply", UtcDaliPageTurnEffectApply, page_turn_effect_startup, page_turn_effect_cleanup},
    {"UtcDaliPageTurnEffectConstruct", UtcDaliPageTurnEffectConstruct, page_turn_effect_startup, page_turn_effect_cleanup},
    {"UtcDaliPageTurnPortraitViewNew", UtcDaliPageTurnPortraitViewNew, dali_page_turn_view_startup, dali_page_turn_view_cleanup},
    {"UtcDaliPageTurnLandscapeViewNew", UtcDaliPageTurnLandscapeViewNew, dali_page_turn_view_startup, dali_page_turn_view_cleanup},
    {"UtcDaliPageTurnViewSetAndGetSpineShadowParameter", UtcDaliPageTurnViewSetAndGetSpineShadowParameter, dali_page_turn_view_startup, dali_page_turn_view_cleanup},
    {"UtcDaliPageTurnViewGoToPageAndGetCurrentPage", UtcDaliPageTurnViewGoToPageAndGetCurrentPage, dali_page_turn_view_startup, dali_page_turn_view_cleanup},
    {"UtcDaliPageTurnViewEnterLeaveEditMode", UtcDaliPageTurnViewEnterLeaveEditMode, dali_page_turn_view_startup, dali_page_turn_view_cleanup},
    {"UtcDaliPageTurnViewGetHitActor", UtcDaliPageTurnViewGetHitActor, dali_page_turn_view_startup, dali_page_turn_view_cleanup},
    {"UtcDaliPageTurnViewRefresh", UtcDaliPageTurnViewRefresh, dali_page_turn_view_startup, dali_page_turn_view_cleanup},
    {"UtcDaliPageTurnViewSignals", UtcDaliPageTurnViewSignals, dali_page_turn_view_startup, dali_page_turn_view_cleanup},
    {"UtcDaliRollLayoutNew", UtcDaliRollLayoutNew, dali_roll_layout_startup, dali_roll_layout_cleanup},
    {"UtcDaliRollLayoutSetAndGetRowSpacing", UtcDaliRollLayoutSetAndGetRowSpacing, dali_roll_layout_startup, dali_roll_layout_cleanup},
    {"UtcDaliRollLayoutSetAndGetItemSizeFunction", UtcDaliRollLayoutSetAndGetItemSizeFunction, dali_roll_layout_startup, dali_roll_layout_cleanup},
    {"UtcDaliRollLayoutSetAndGetScrollSpeedFactor", UtcDaliRollLayoutSetAndGetScrollSpeedFactor, dali_roll_layout_startup, dali_roll_layout_cleanup},
    {"UtcDaliRollLayoutSetAndGetMaximumSwipeSpeed", UtcDaliRollLayoutSetAndGetMaximumSwipeSpeed, dali_roll_layout_startup, dali_roll_layout_cleanup},
    {"UtcDaliRollLayoutSetAndGetItemFlickAnimationDuration", UtcDaliRollLayoutSetAndGetItemFlickAnimationDuration, dali_roll_layout_startup, dali_roll_layout_cleanup},
    {"UtcDaliRollLayoutConstraintLeft", UtcDaliRollLayoutConstraintLeft, dali_roll_layout_startup, dali_roll_layout_cleanup},
    {"UtcDaliRollLayoutConstraintRight", UtcDaliRollLayoutConstraintRight, dali_roll_layout_startup, dali_roll_layout_cleanup},
    {"UtcDaliRollLayoutConstraintUp", UtcDaliRollLayoutConstraintUp, dali_roll_layout_startup, dali_roll_layout_cleanup},
    {"UtcDaliRollLayoutConstraintDown", UtcDaliRollLayoutConstraintDown, dali_roll_layout_startup, dali_roll_layout_cleanup},
    {"UtcDaliRollLayoutScrollDirection", UtcDaliRollLayoutScrollDirection, dali_roll_layout_startup, dali_roll_layout_cleanup},
    {"UtcDaliRotatingSelectorNew", UtcDaliRotatingSelectorNew, rotating_selector_startup, rotating_selector_cleanup},
    {"UtcDaliRotatingSelectorSetSelected", UtcDaliRotatingSelectorSetSelected, rotating_selector_startup, rotating_selector_cleanup},
    {"UtcDaliRotatingSelectorSetSelectedAndUnSelectedActor", UtcDaliRotatingSelectorSetSelectedAndUnSelectedActor, rotating_selector_startup, rotating_selector_cleanup},
    {"UtcDaliRotatingSelectorSetSelectable", UtcDaliRotatingSelectorSetSelectable, rotating_selector_startup, rotating_selector_cleanup},
    {"UtcDaliRotatingSelectorSignalSelected", UtcDaliRotatingSelectorSignalSelected, rotating_selector_startup, rotating_selector_cleanup},
    {"UtcDaliScrollViewNew", UtcDaliScrollViewNew, scroll_view_startup, scroll_view_cleanup},
    {"UtcDaliScrollViewDownCast", UtcDaliScrollViewDownCast, scroll_view_startup, scroll_view_cleanup},
    {"UtcDaliScrollViewScrollToPosition", UtcDaliScrollViewScrollToPosition, scroll_view_startup, scroll_view_cleanup},
    {"UtcDaliScrollViewScrollToPage", UtcDaliScrollViewScrollToPage, scroll_view_startup, scroll_view_cleanup},
    {"UtcDaliScrollViewScrollToActor", UtcDaliScrollViewScrollToActor, scroll_view_startup, scroll_view_cleanup},
    {"UtcDaliScrollViewScrollToSnapPoint", UtcDaliScrollViewScrollToSnapPoint, scroll_view_startup, scroll_view_cleanup},
    {"UtcDaliScrollViewRulerScale", UtcDaliScrollViewRulerScale, scroll_view_startup, scroll_view_cleanup},
    {"UtcDaliScrollViewTransformTo", UtcDaliScrollViewTransformTo, scroll_view_startup, scroll_view_cleanup},
    {"UtcDaliScrollViewRefreshInterval", UtcDaliScrollViewRefreshInterval, scroll_view_startup, scroll_view_cleanup},
    {"UtcDaliScrollViewWrapMode", UtcDaliScrollViewWrapMode, scroll_view_startup, scroll_view_cleanup},
    {"UtcDaliScrollViewActorAutoSnap", UtcDaliScrollViewActorAutoSnap, scroll_view_startup, scroll_view_cleanup},
    {"UtcDaliScrollViewSignalsStartComplete", UtcDaliScrollViewSignalsStartComplete, scroll_view_startup, scroll_view_cleanup},
    {"UtcDaliScrollViewSignalsUpdate", UtcDaliScrollViewSignalsUpdate, scroll_view_startup, scroll_view_cleanup},
    {"UtcDaliScrollViewSignalsClamped", UtcDaliScrollViewSignalsClamped, scroll_view_startup, scroll_view_cleanup},
    {"UtcDaliScrollViewScrollSensitive", UtcDaliScrollViewScrollSensitive, scroll_view_startup, scroll_view_cleanup},
    {"UtcDaliScrollViewTouchesRequired", UtcDaliScrollViewTouchesRequired, scroll_view_startup, scroll_view_cleanup},
    {"UtcDaliScrollViewAxisAutoLock", UtcDaliScrollViewAxisAutoLock, scroll_view_startup, scroll_view_cleanup},
    {"UtcDaliScrollViewAxisAutoLockGradient", UtcDaliScrollViewAxisAutoLockGradient, scroll_view_startup, scroll_view_cleanup},
    {"UtcDaliScrollViewConstraints", UtcDaliScrollViewConstraints, scroll_view_startup, scroll_view_cleanup},
    {"UtcDaliScrollViewBind", UtcDaliScrollViewBind, scroll_view_startup, scroll_view_cleanup},
    {"UtcDaliRulerEnableDisable", UtcDaliRulerEnableDisable, scroll_view_startup, scroll_view_cleanup},
    {"UtcDaliRulerDomainEnableDisable", UtcDaliRulerDomainEnableDisable, scroll_view_startup, scroll_view_cleanup},
    {"UtcDaliRulerSnapAndClamp", UtcDaliRulerSnapAndClamp, scroll_view_startup, scroll_view_cleanup},
    {"UtcDaliRulerFixedRulerSpacing", UtcDaliRulerFixedRulerSpacing, scroll_view_startup, scroll_view_cleanup},
    {"UtcDaliScrollViewOvershoot", UtcDaliScrollViewOvershoot, scroll_view_startup, scroll_view_cleanup},
    {"UtcDaliScrollViewSnapAlphaFunction", UtcDaliScrollViewSnapAlphaFunction, scroll_view_startup, scroll_view_cleanup},
    {"UtcDaliScrollViewSnapDuration", UtcDaliScrollViewSnapDuration, scroll_view_startup, scroll_view_cleanup},
    {"UtcDaliScrollViewSignalsSnapStart", UtcDaliScrollViewSignalsSnapStart, scroll_view_startup, scroll_view_cleanup},
    {"UtcDaliScrollViewSetMouseWheelScrollDistanceStep", UtcDaliScrollViewSetMouseWheelScrollDistanceStep, scroll_view_startup, scroll_view_cleanup},
    {"UtcDaliScrollViewGetSet", UtcDaliScrollViewGetSet, scroll_view_startup, scroll_view_cleanup},
    {"UtcDaliShadowViewUninitialized", UtcDaliShadowViewUninitialized, shadow_view_startup, shadow_view_cleanup},
    {"UtcDaliShadowViewNew", UtcDaliShadowViewNew, shadow_view_startup, shadow_view_cleanup},
    {"UtcDaliShadowViewDownCast", UtcDaliShadowViewDownCast, shadow_view_startup, shadow_view_cleanup},
    {"UtcDaliShadowViewPropertyNames", UtcDaliShadowViewPropertyNames, shadow_view_startup, shadow_view_cleanup},
    {"UtcDaliShadowViewAddRemove", UtcDaliShadowViewAddRemove, shadow_view_startup, shadow_view_cleanup},
    {"UtcDaliShadowViewActivateDeactivate", UtcDaliShadowViewActivateDeactivate, shadow_view_startup, shadow_view_cleanup},
    {"UtcDaliShearEffectUninitialized", UtcDaliShearEffectUninitialized, shear_effect_startup, shear_effect_cleanup},
    {"UtcDaliShearEffectPropertyNames", UtcDaliShearEffectPropertyNames, shear_effect_startup, shear_effect_cleanup},
    {"UtcDaliShearEffectDefaultValues", UtcDaliShearEffectDefaultValues, shear_effect_startup, shear_effect_cleanup},
    {"UtcDaliShearEffectCustomValues", UtcDaliShearEffectCustomValues, shear_effect_startup, shear_effect_cleanup},
    {"UtcDaliSliderNew", UtcDaliSliderNew, dali_slider_startup, dali_slider_cleanup},
    {"UtcDaliSliderDestructor", UtcDaliSliderDestructor, dali_slider_startup, dali_slider_cleanup},
    {"UtcDaliSliderDownCast", UtcDaliSliderDownCast, dali_slider_startup, dali_slider_cleanup},
    {"UtcDaliSliderSignals", UtcDaliSliderSignals, dali_slider_startup, dali_slider_cleanup},
    {"UtcDaliSoftButtonEffectUninitialized", UtcDaliSoftButtonEffectUninitialized, soft_button_effect_startup, soft_button_effect_cleanup},
    {"UtcDaliSoftButtonEffectNew", UtcDaliSoftButtonEffectNew, soft_button_effect_startup, soft_button_effect_cleanup},
    {"UtcDaliSoftButtonEffectPropertyNames", UtcDaliSoftButtonEffectPropertyNames, soft_button_effect_startup, soft_button_effect_cleanup},
    {"UtcDaliSpiralLayoutNew", UtcDaliSpiralLayoutNew, spiral_layout_startup, spiral_layout_cleanup},
    {"UtcDaliSpiralSetAndGetItemSizeFunction", UtcDaliSpiralSetAndGetItemSizeFunction, spiral_layout_startup, spiral_layout_cleanup},
    {"UtcDaliSpiralSetAndGetItemSpacing", UtcDaliSpiralSetAndGetItemSpacing, spiral_layout_startup, spiral_layout_cleanup},
    {"UtcDaliSpiralSetAndGetRevolutionDistance", UtcDaliSpiralSetAndGetRevolutionDistance, spiral_layout_startup, spiral_layout_cleanup},
    {"UtcDaliSpiralSetAndGetSpiralRadiusFunction", UtcDaliSpiralSetAndGetSpiralRadiusFunction, spiral_layout_startup, spiral_layout_cleanup},
    {"UtcDaliSpiralSetAndGetTopItemAlignment", UtcDaliSpiralSetAndGetTopItemAlignment, spiral_layout_startup, spiral_layout_cleanup},
    {"UtcDaliSpiralSetAndGetScrollSpeedFactor", UtcDaliSpiralSetAndGetScrollSpeedFactor, spiral_layout_startup, spiral_layout_cleanup},
    {"UtcDaliSpiralSetAndGetMaximumSwipeSpeed", UtcDaliSpiralSetAndGetMaximumSwipeSpeed, spiral_layout_startup, spiral_layout_cleanup},
    {"UtcDaliSpiralLayoutSetAndGetItemFlickAnimationDuration", UtcDaliSpiralLayoutSetAndGetItemFlickAnimationDuration, spiral_layout_startup, spiral_layout_cleanup},
    {"UtcDaliSpiralLayoutConstraintLeft", UtcDaliSpiralLayoutConstraintLeft, spiral_layout_startup, spiral_layout_cleanup},
    {"UtcDaliSpiralLayoutConstraintRight", UtcDaliSpiralLayoutConstraintRight, spiral_layout_startup, spiral_layout_cleanup},
    {"UtcDaliSpiralLayoutConstraintUp", UtcDaliSpiralLayoutConstraintUp, spiral_layout_startup, spiral_layout_cleanup},
    {"UtcDaliSpiralLayoutConstraintDown", UtcDaliSpiralLayoutConstraintDown, spiral_layout_startup, spiral_layout_cleanup},
    {"UtcDaliSpiralLayoutScrollDirection", UtcDaliSpiralLayoutScrollDirection, spiral_layout_startup, spiral_layout_cleanup},
    {"UtcDaliSpiralLayoutGetScrollToPosition", UtcDaliSpiralLayoutGetScrollToPosition, spiral_layout_startup, spiral_layout_cleanup},
    {"UtcDaliSpotUninitializedEffect", UtcDaliSpotUninitializedEffect, spot_effect_startup, spot_effect_cleanup},
    {"UtcDaliSpotPropertyNamesEffect", UtcDaliSpotPropertyNamesEffect, spot_effect_startup, spot_effect_cleanup},
    {"UtcDaliSpotDefaultValuesEffect", UtcDaliSpotDefaultValuesEffect, spot_effect_startup, spot_effect_cleanup},
    {"UtcDaliSpotCustomValuesEffect", UtcDaliSpotCustomValuesEffect, spot_effect_startup, spot_effect_cleanup},
    {"UtcDaliSquareDissolveEffectUninitialized", UtcDaliSquareDissolveEffectUninitialized, square_dissolve_effect_startup, square_dissolve_effect_cleanup},
    {"UtcDaliSquareDissolveEffectPropertyNames", UtcDaliSquareDissolveEffectPropertyNames, square_dissolve_effect_startup, square_dissolve_effect_cleanup},
    {"UtcDaliSquareDissolveEffectDefaultValues", UtcDaliSquareDissolveEffectDefaultValues, square_dissolve_effect_startup, square_dissolve_effect_cleanup},
    {"UtcDaliSquareDissolveEffectCustomValues", UtcDaliSquareDissolveEffectCustomValues, square_dissolve_effect_startup, square_dissolve_effect_cleanup},
    {"UtcDaliTableViewNew", UtcDaliTableViewNew, dali_tableview_startup, dali_tableview_cleanup},
    {"UtcDaliTableViewMetricsPadding", UtcDaliTableViewMetricsPadding, dali_tableview_startup, dali_tableview_cleanup},
    {"UtcDaliTableViewMetricsFixed", UtcDaliTableViewMetricsFixed, dali_tableview_startup, dali_tableview_cleanup},
    {"UtcDaliTableViewMetricsRelative", UtcDaliTableViewMetricsRelative, dali_tableview_startup, dali_tableview_cleanup},
    {"UtcDaliTableViewAnimation", UtcDaliTableViewAnimation, dali_tableview_startup, dali_tableview_cleanup},
    {"UtcDaliTableViewChild", UtcDaliTableViewChild, dali_tableview_startup, dali_tableview_cleanup},
    {"UtcDaliTableViewAdd", UtcDaliTableViewAdd, dali_tableview_startup, dali_tableview_cleanup},
    {"UtcDaliTableViewCells", UtcDaliTableViewCells, dali_tableview_startup, dali_tableview_cleanup},
    {"UtcDaliTableViewChildAssert", UtcDaliTableViewChildAssert, dali_tableview_startup, dali_tableview_cleanup},
    {"UtcDaliTableViewMetricsAssert", UtcDaliTableViewMetricsAssert, dali_tableview_startup, dali_tableview_cleanup},
    {"UtcDaliToolBarNew", UtcDaliToolBarNew, dali_toolbar_startup, dali_toolbar_cleanup},
    {"UtcDaliToolBarSetBackground", UtcDaliToolBarSetBackground, dali_toolbar_startup, dali_toolbar_cleanup},
    {"UtcDaliToolBarAddControl01", UtcDaliToolBarAddControl01, dali_toolbar_startup, dali_toolbar_cleanup},
    {"UtcDaliToolBarAddControl02", UtcDaliToolBarAddControl02, dali_toolbar_startup, dali_toolbar_cleanup},
    {"UtcDaliToolBarRemoveControl01", UtcDaliToolBarRemoveControl01, dali_toolbar_startup, dali_toolbar_cleanup},
    {"UtcDaliToolBarRemoveControl02", UtcDaliToolBarRemoveControl02, dali_toolbar_startup, dali_toolbar_cleanup},
    {"UtcDaliViewNew", UtcDaliViewNew, dali_view_startup, dali_view_cleanup},
    {"UtcDaliViewAddGetRemoveContentLayer01", UtcDaliViewAddGetRemoveContentLayer01, dali_view_startup, dali_view_cleanup},
    {"UtcDaliViewAddGetRemoveContentLayer02", UtcDaliViewAddGetRemoveContentLayer02, dali_view_startup, dali_view_cleanup},
    {"UtcDaliViewSetGetBackgroundLayer01", UtcDaliViewSetGetBackgroundLayer01, dali_view_startup, dali_view_cleanup},
    {"UtcDaliViewSetGetBackgroundLayer02", UtcDaliViewSetGetBackgroundLayer02, dali_view_startup, dali_view_cleanup},
    {"UtcDaliViewSetOrientationFunction", UtcDaliViewSetOrientationFunction, dali_view_startup, dali_view_cleanup},
    {"UtcDaliViewOrientationChanged", UtcDaliViewOrientationChanged, dali_view_startup, dali_view_cleanup},
    {"UtcSetAutoRotate", UtcSetAutoRotate, dali_view_startup, dali_view_cleanup},
    {"UtcDaliWaterEffectUninitialized", UtcDaliWaterEffectUninitialized, dali_water_effect_startup, dali_water_effect_cleanup},
    {"UtcDaliWaterEffectPropertyNames", UtcDaliWaterEffectPropertyNames, dali_water_effect_startup, dali_water_effect_cleanup},
    {"UtcDaliWaterEffectOutOfBounds", UtcDaliWaterEffectOutOfBounds, dali_water_effect_startup, dali_water_effect_cleanup},
    {"UtcDaliWaterEffectDefaultValues", UtcDaliWaterEffectDefaultValues, dali_water_effect_startup, dali_water_effect_cleanup},
    {"UtcDaliWaterEffectCustomValues", UtcDaliWaterEffectCustomValues, dali_water_effect_startup, dali_water_effect_cleanup},
    {"UtcDaliWaterEffectGetAmplitudePositive", UtcDaliWaterEffectGetAmplitudePositive, dali_water_effect_startup, dali_water_effect_cleanup},
    {"UtcDaliWaterEffectGetAmplitudeNegative", UtcDaliWaterEffectGetAmplitudeNegative, dali_water_effect_startup, dali_water_effect_cleanup},
    {"UtcDaliWaterEffectGetCenterPositive", UtcDaliWaterEffectGetCenterPositive, dali_water_effect_startup, dali_water_effect_cleanup},
    {"UtcDaliWaterEffectGetCenterNegative", UtcDaliWaterEffectGetCenterNegative, dali_water_effect_startup, dali_water_effect_cleanup},
    {"UtcDaliWaterEffectGetPropagationPositive", UtcDaliWaterEffectGetPropagationPositive, dali_water_effect_startup, dali_water_effect_cleanup},
    {"UtcDaliWaterEffectGetPropagationNegative", UtcDaliWaterEffectGetPropagationNegative, dali_water_effect_startup, dali_water_effect_cleanup},
    {"UtcDaliButtonNew", UtcDaliButtonNew, dali_button_startup, dali_button_cleanup},
    {"UtcDaliButtonProperties", UtcDaliButtonProperties, dali_button_startup, dali_button_cleanup},
    {"UtcDaliButtonSetGetDimmed", UtcDaliButtonSetGetDimmed, dali_button_startup, dali_button_cleanup},
    {"UtcDaliControlImplTypeRegistry", UtcDaliControlImplTypeRegistry, control_impl_startup, control_impl_cleanup},
    {"UtcDaliControlImplMouseWheelEvent", UtcDaliControlImplMouseWheelEvent, control_impl_startup, control_impl_cleanup},
    {"UtcDaliDefaultControlsCreateCheckBoxButton", UtcDaliDefaultControlsCreateCheckBoxButton, dali_default_controls_startup, dali_default_controls_cleanup},
    {"UtcDaliItemViewAddAndGetLayout", UtcDaliItemViewAddAndGetLayout, dali_default_controls_startup, dali_default_controls_cleanup},
    {"UtcDaliItemViewAddAndRemoveLayout", UtcDaliItemViewAddAndRemoveLayout, dali_default_controls_startup, dali_default_controls_cleanup},
    {"UtcDaliItemViewActivateLayoutAndGetActiveLayout", UtcDaliItemViewActivateLayoutAndGetActiveLayout, dali_default_controls_startup, dali_default_controls_cleanup},
    {"UtcDaliKeyboardFocusManagerGet", UtcDaliKeyboardFocusManagerGet, dali_keyboard_focus_manager_startup, dali_keyboard_focus_manager_cleanup},
    {"UtcDaliKeyboardFocusManagerMoveFocus", UtcDaliKeyboardFocusManagerMoveFocus, dali_keyboard_focus_manager_startup, dali_keyboard_focus_manager_cleanup},
    {"UtcDaliKeyboardFocusManagerSignalFocusGroupChanged", UtcDaliKeyboardFocusManagerSignalFocusGroupChanged, dali_keyboard_focus_manager_startup, dali_keyboard_focus_manager_cleanup},
    {"UtcDaliScrollViewPageCubeEffectSetup", UtcDaliScrollViewPageCubeEffectSetup, dali_scroll_view_effect_startup, dali_scroll_view_effect_cleanup},
    {"UtcDaliScrollViewPageCarouselEffectSetup", UtcDaliScrollViewPageCarouselEffectSetup, dali_scroll_view_effect_startup, dali_scroll_view_effect_cleanup},
    {"UtcDaliScrollViewCarouselEffectSetup", UtcDaliScrollViewCarouselEffectSetup, dali_scroll_view_effect_startup, dali_scroll_view_effect_cleanup},
    {"UtcDaliScrollViewDepthEffectSetup", UtcDaliScrollViewDepthEffectSetup, dali_scroll_view_effect_startup, dali_scroll_view_effect_cleanup},
    {"UtcDaliScrollViewPageCubeEffectTest", UtcDaliScrollViewPageCubeEffectTest, dali_scroll_view_effect_startup, dali_scroll_view_effect_cleanup},
    {"UtcDaliScrollViewPageCarouselEffectTest", UtcDaliScrollViewPageCarouselEffectTest, dali_scroll_view_effect_startup, dali_scroll_view_effect_cleanup},
    {"UtcDaliScrollViewCarouselEffectTest", UtcDaliScrollViewCarouselEffectTest, dali_scroll_view_effect_startup, dali_scroll_view_effect_cleanup},
    {"UtcDaliScrollViewDepthEffectTest", UtcDaliScrollViewDepthEffectTest, dali_scroll_view_effect_startup, dali_scroll_view_effect_cleanup},
    {"UtcDaliTextInputSetActiveStyle", UtcDaliTextInputSetActiveStyle, dali_scroll_view_effect_startup, dali_scroll_view_effect_cleanup},
    {"UtcDaliTextInputApplyStyleToSelectedText", UtcDaliTextInputApplyStyleToSelectedText, dali_scroll_view_effect_startup, dali_scroll_view_effect_cleanup},
    {"UtcDaliTextInputApplyStyleToAll", UtcDaliTextInputApplyStyleToAll, dali_scroll_view_effect_startup, dali_scroll_view_effect_cleanup},
    {"UtcDaliTextInputGetStyleAtCursor", UtcDaliTextInputGetStyleAtCursor, dali_scroll_view_effect_startup, dali_scroll_view_effect_cleanup},
    {"UtcDaliTextInputSetAndGetTextAlignment", UtcDaliTextInputSetAndGetTextAlignment, dali_scroll_view_effect_startup, dali_scroll_view_effect_cleanup},
    {"UtcDaliTextInputSetAndGetMultilinePolicy", UtcDaliTextInputSetAndGetMultilinePolicy, dali_scroll_view_effect_startup, dali_scroll_view_effect_cleanup},
    {"UtcDaliTextInputSetAndGetExceedEnabled", UtcDaliTextInputSetAndGetExceedEnabled, dali_scroll_view_effect_startup, dali_scroll_view_effect_cleanup},
    {NULL, NULL}
};

#endif // __TCT_DALI_TOOLKIT_UNMANAGED_CORE_H__
