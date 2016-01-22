/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

// CLASS HEADER
#include "scroll-view-api.h"

// EXTERNAL INCLUDES

// INTERNAL INCLUDES
#include <v8-utils.h>
#include <actors/actor-wrapper.h>
#include <controls/control-wrapper.h>

namespace Dali
{

namespace V8Plugin
{

namespace  // unanmed namespace
{

Toolkit::ScrollView GetScrollView( v8::Isolate* isolate, const v8::FunctionCallbackInfo<v8::Value>& args )
{
  HandleWrapper* handleWrapper = HandleWrapper::Unwrap( isolate, args.This() );
  return Toolkit::ScrollView::DownCast( handleWrapper->mHandle );
}

} //unanmed namespace

/***************************************
 * SCROLLVIEW API FUNCTIONS
 ***************************************/

/**
 * Constructor
 *
 * @for ScrollView
 * @constructor
 * @method ScrollView
 * @return {Object} scrollView
 */
Toolkit::Control ScrollViewApi::New( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Toolkit::ScrollView scrollView = Toolkit::ScrollView::New();
  return scrollView;
}

/**
 * Set the scroll mode of ScrollView.
 *
 * This defines whether scrolling is enabled horizontally or vertically, how
 * scrolling is snapped, and the boundary in which the scroll view can pan.
 *
 * When no specific scroll mode is set, scroll view can scroll to any position
 * both horizontally and vertically and no snapping is enabled.
 *
 * Example of setting the scroll boundary of scroll view in the X axis to
 * three pages (page size equals to the width of scroll view) and allowing
 * snapping between pages, and disabling scrolling in the Y axis.
 *
 * ```
 *  var scrollMode = {
 *                     xAxisScrollEnabled  : true,
 *                     xAxisSnapToInterval : scrollView.sizeWidth,
 *                     xAxisScrollBoundary : scrollView.sizeWidth * 3,
 *                     yAxisScrollEnabled  : false
 *                   }
 *
 *  scrollView.setScrollMode(scrollMode);
 * ```
 *
 * @for ScrollView
 * @method setScrollMode
 * @param {Object} scrollMode
 * @param {Boolean} scrollMode.xAxisScrollEnabled True if the content can be scrolled in X axis or false if not.
 * @param {Float} [scrollMode.xAxisSnapToInterval] When set, causes scroll view to snap to multiples of the value of the interval in the X axis while flicking. (by default no snapping)
 * @param {Float} [scrollMode.xAxisScrollBoundary] When set, causes scroll view unable to scroll beyond the value of the boundary in the X axis (by default no boundary)
 * @param {Boolean} scrollMode.yAxisScrollEnabled True if the content can be scrolled in Y axis or false if not.
 * @param {Float} [scrollMode.yAxisSnapToInterval] When set, causes scroll view to snap to multiples of the value of the interval in the Y axis while flicking. (by default no snapping)
 * @param {Float} [scrollMode.yAxisScrollBoundary] When set, causes scroll view unable to scroll beyond the value of the boundary in the Y axis (by default no boundary)
 */
void ScrollViewApi::SetScrollMode( const v8::FunctionCallbackInfo< v8::Value >& args)
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Toolkit::ScrollView scrollView = GetScrollView( isolate, args );

  v8::Local<v8::Value> scrollMode( args[0] );
  if( !scrollMode->IsObject() )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "invalid scroll mode parameter" );
    return;
  }

  v8::Local<v8::Object> scrollModeObj = scrollMode->ToObject();

  Toolkit::RulerPtr rulerX, rulerY;

  // Check the scroll mode in the X axis
  bool xAxisScrollEnabled = true;
  v8::Local<v8::Value> xAxisScrollEnabledValue= scrollModeObj->Get( v8::String::NewFromUtf8( isolate, "xAxisScrollEnabled" ) );
  if( xAxisScrollEnabledValue->IsBoolean() )
  {
    xAxisScrollEnabled = xAxisScrollEnabledValue->ToBoolean()->Value();
  }
  else
  {
    DALI_SCRIPT_EXCEPTION( isolate, "Missing xAxisScrollEnabled");
    return;
  }

  if(!xAxisScrollEnabled)
  {
    // Default ruler and disabled
    rulerX = new Toolkit::DefaultRuler();
    rulerX->Disable();
  }
  else
  {
    v8::Local<v8::Value> xAxisSnapToIntervalValue= scrollModeObj->Get( v8::String::NewFromUtf8( isolate, "xAxisSnapToInterval" ) );
    if( xAxisSnapToIntervalValue->IsNumber() )
    {
      // Fixed ruler and enabled
      float xAxisSnapToInterval = xAxisSnapToIntervalValue->ToNumber()->Value();
      rulerX = new Toolkit::FixedRuler(xAxisSnapToInterval);
    }
    else
    {
      // Default ruler and enabled
      rulerX = new Toolkit::DefaultRuler();
    }

    v8::Local<v8::Value> xAxisScrollBoundaryValue= scrollModeObj->Get( v8::String::NewFromUtf8( isolate, "xAxisScrollBoundary" ) );
    if( xAxisScrollBoundaryValue->IsNumber() )
    {
      // By default ruler domain is disabled unless set
      float xAxisScrollBoundary = xAxisScrollBoundaryValue->ToNumber()->Value();
      rulerX->SetDomain( Toolkit::RulerDomain( 0, xAxisScrollBoundary, true ) );
    }
  }

  // Check the scroll mode in the Y axis
  bool yAxisScrollEnabled = true;
  v8::Local<v8::Value> yAxisScrollEnabledValue= scrollModeObj->Get( v8::String::NewFromUtf8( isolate, "yAxisScrollEnabled" ) );
  if( yAxisScrollEnabledValue->IsBoolean() )
  {
    yAxisScrollEnabled = yAxisScrollEnabledValue->ToBoolean()->Value();
  }
  else
  {
    DALI_SCRIPT_EXCEPTION( isolate, "Missing yAxisScrollEnabled");
    return;
  }

  if(!yAxisScrollEnabled)
  {
    // Default ruler and disabled
    rulerY = new Toolkit::DefaultRuler();
    rulerY->Disable();
  }
  else
  {
    v8::Local<v8::Value> yAxisSnapToIntervalValue= scrollModeObj->Get( v8::String::NewFromUtf8( isolate, "yAxisSnapToInterval" ) );
    if( yAxisSnapToIntervalValue->IsNumber() )
    {
      // Fixed ruler and enabled
      float yAxisSnapToInterval = yAxisSnapToIntervalValue->ToNumber()->Value();
      rulerY = new Toolkit::FixedRuler(yAxisSnapToInterval);
    }
    else
    {
      // Default ruler and enabled
      rulerY = new Toolkit::DefaultRuler();
    }

    v8::Local<v8::Value> yAxisScrollBoundaryValue= scrollModeObj->Get( v8::String::NewFromUtf8( isolate, "yAxisScrollBoundary" ) );
    if( yAxisScrollBoundaryValue->IsNumber() )
    {
      // By default ruler domain is disabled unless set
      float yAxisScrollBoundary = yAxisScrollBoundaryValue->ToNumber()->Value();
      rulerY->SetDomain( Toolkit::RulerDomain( 0, yAxisScrollBoundary, true ) );
    }
  }

  scrollView.SetRulerX(rulerX);
  scrollView.SetRulerY(rulerY);
}

/**
 * Retrieves current scroll page based on the defined snap interval being the
 * size of one page, and all pages laid out in a grid fashion, increasing from
 * left to right until the end of the scroll boundary. Pages start from 0 as the
 * first page.
 *
 * If no snap interval is defined, this API will return undefined value.
 *
 * @for ScrollView
 * @method getCurrentPage
 * @return {Integer} The index of current page in scroll view
 */
void ScrollViewApi::GetCurrentPage( const v8::FunctionCallbackInfo< v8::Value >& args)
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Toolkit::ScrollView scrollView = GetScrollView( isolate, args );
  args.GetReturnValue().Set( v8::Integer::New( isolate, scrollView.GetCurrentPage() ) );
}

/**
 * Scrolls the contents to the given position.
 *
 * Position 0,0 is the origin. Increasing X scrolls contents left, while
 * increasing Y scrolls contents up. If Rulers have been applied to the axes,
 * then the contents will scroll until reaching the scroll boundary.
 * Contents will not snap.
 *
 * The biasing parameters are provided such that in scenarios with 2 or 2x2 pages
 * in wrap mode, the application developer can decide whether to scroll left or
 * right to get to the target page.
 *
 * @for ScrollView
 * @method scrollToPosition
 * @param {Array} position The position to scroll to.
 * @param {Float} [durationSeconds] The duration of the scroll animation in seconds (default value is scrollView.scrollSnapDuration)
 * @param {Integer} [alphaFunction] The alpha function to use.
 * @param {Integer} [horizontalBias] Whether to bias scrolling to left or right (by default no bias).
 * @param {Integer} [verticalBias] Whether to bias scrolling to top or bottom (by default no bias).
 * @example
 *      // scroll direction bias is one of the following
 *      dali.DIRECTION_BIAS_NONE           // Don't bias scroll snap
 *      dali.DIRECTION_BIAS_LEFT           // Bias scroll snap to Left
 *      dali.DIRECTION_BIAS_RIGHT          // Bias scroll snap to Right
 *
 *      scrollView.scrollToPosition( [150.0, 100.0], 0.5, dali.ALPHA_FUNCTION_EASE_IN_OUT, dali.DIRECTION_BIAS_LEFT, dali.DIRECTION_BIAS_NONE );
 */
void ScrollViewApi::ScrollToPosition( const v8::FunctionCallbackInfo< v8::Value >& args)
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Toolkit::ScrollView scrollView = GetScrollView( isolate, args );

  bool found( false );
  Vector2 position = V8Utils::GetVector2Parameter( PARAMETER_0, found, isolate, args );
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "bad position parameter" );
    return;
  }

  float durationSeconds = V8Utils::GetFloatParameter( PARAMETER_1, found, isolate, args, scrollView.GetScrollSnapDuration() );

  AlphaFunction alphaFunction = scrollView.GetScrollSnapAlphaFunction();

  found = false;
  int alpha = V8Utils::GetIntegerParameter( PARAMETER_2, found, isolate,  args, 0 );
  if(found)
  {
    alphaFunction = static_cast<AlphaFunction::BuiltinFunction>(alpha);
  }

  Toolkit::DirectionBias horizontalBias = static_cast<Toolkit::DirectionBias>( V8Utils::GetIntegerParameter( PARAMETER_3, found, isolate, args, Toolkit::DirectionBiasNone ) );
  Toolkit::DirectionBias verticalBias = static_cast<Toolkit::DirectionBias>( V8Utils::GetIntegerParameter( PARAMETER_4, found, isolate, args, Toolkit::DirectionBiasNone ) );

  scrollView.ScrollTo( position, durationSeconds, alphaFunction, horizontalBias, verticalBias );
}

/**
 * Scrolls the contents to the page with the given index.
 *
 * This is based on assumption that the page index starts from 0 and the
 * position of each page is: [pageIndex * snapToInterval, 0].
 *
 * If no snap interval is defined, calling this API will cause unexpected
 * behaviour.
 *
 * The biasing parameter is provided such that in scenarios with 2 pages
 * in wrap mode, the application developer can decide whether to scroll
 * left or right to get to the target page.
 *
 * @for ScrollView
 * @method scrollToPage
 * @param {Integer} pageIndex The index of the page to scroll to.
 * @param {Float} [durationSeconds] The duration of the scroll animation in seconds (default value is scrollView.scrollSnapDuration)
 * @param {Integer} [bias] Whether to bias scrolling to left or right (by default no bias).
 * @example
 *      // scroll direction bias is one of the following
 *      dali.DIRECTION_BIAS_NONE           // Don't bias scroll snap
 *      dali.DIRECTION_BIAS_LEFT           // Bias scroll snap to Left
 *      dali.DIRECTION_BIAS_RIGHT          // Bias scroll snap to Right
 *
 *      scrollView.scrollToPage( 1, 0.5, dali.DIRECTION_BIAS_RIGHT );
 */
void ScrollViewApi::ScrollToPage( const v8::FunctionCallbackInfo< v8::Value >& args)
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Toolkit::ScrollView scrollView = GetScrollView( isolate, args );

  bool found( false );
  int pageIndex = V8Utils::GetIntegerParameter( PARAMETER_0, found, isolate, args, 0 );
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "bad page index parameter" );
    return;
  }

  float durationSeconds = V8Utils::GetFloatParameter( PARAMETER_1, found, isolate, args, scrollView.GetScrollSnapDuration() );
  Toolkit::DirectionBias bias = static_cast<Toolkit::DirectionBias>( V8Utils::GetIntegerParameter( PARAMETER_2, found, isolate, args, Toolkit::DirectionBiasNone ) );

  scrollView.ScrollTo( pageIndex, durationSeconds, bias );
}

/**
 * Scrolls the contents such that the given actor appears in the center of
 * the scroll view.
 *
 * The actor must be a direct child of scroll view.
 *
 * @for ScrollView
 * @method scrollToActor
 * @param {Object} actor The actor to scroll to.
 * @param {Float} [durationSeconds] The duration of the scroll animation in seconds (default value is scrollView.scrollSnapDuration)
 * @example
 *      scrollView.scrollToActor( childActor, 0.5 );
 */
void ScrollViewApi::ScrollToActor( const v8::FunctionCallbackInfo< v8::Value >& args)
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Toolkit::ScrollView scrollView = GetScrollView( isolate, args );

  bool found( false );
  Actor actor = V8Utils::GetActorParameter( 0, found, isolate, args );
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "invalid actor parameter" );
    return;
  }

  float durationSeconds = V8Utils::GetFloatParameter( PARAMETER_1, found, isolate, args, scrollView.GetScrollSnapDuration() );

  scrollView.ScrollTo( actor, durationSeconds );
}

/**
 * Scrolls the content to the nearest snap point as specified by the snap interval.
 * If already at snap points, it will not scroll.
 *
 * @for ScrollView
 * @method scrollToSnapInterval
 * @return {Boolean} True if snapping is needed or false if already at snap points
 * @example
 *      var success = scrollView.scrollToSnapInterval();
 */
void ScrollViewApi::ScrollToSnapInterval( const v8::FunctionCallbackInfo< v8::Value >& args)
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Toolkit::ScrollView scrollView = GetScrollView( isolate, args );

  args.GetReturnValue().Set( v8::Boolean::New( isolate, scrollView.ScrollToSnapPoint() ) );
}

/**
 * Set the alpha function of flick animation.
 *
 * @for ScrollView
 * @method setScrollFlickAlphaFunction
 * @param {Integer} alphaFunction The alpha function to use.
 * @example
 *      scrollView.setScrollFlickAlphaFunction( dali.ALPHA_FUNCTION_EASE_IN_OUT );
 */
void ScrollViewApi::SetScrollFlickAlphaFunction( const v8::FunctionCallbackInfo< v8::Value >& args)
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Toolkit::ScrollView scrollView = GetScrollView( isolate, args );

  bool found( false );
  int alphaFunction = V8Utils::GetIntegerParameter( PARAMETER_0, found, isolate,  args, 0 );
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "invalid alpha function parameter" );
    return;
  }
  else
  {
    scrollView.SetScrollFlickAlphaFunction( static_cast<AlphaFunction::BuiltinFunction>(alphaFunction) );
  }
}

/**
 * Set the alpha function of snap animation.
 *
 * @for ScrollView
 * @method setScrollSnapAlphaFunction
 * @param {String} alphaFunction The alpha function to use.
 * @example
 *      scrollView.setScrollSnapAlphaFunction( dali.ALPHA_FUNCTION_EASE_IN_OUT );
 */
void ScrollViewApi::SetScrollSnapAlphaFunction( const v8::FunctionCallbackInfo< v8::Value >& args)
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Toolkit::ScrollView scrollView = GetScrollView( isolate, args );

  bool found( false );
  int alphaFunction = V8Utils::GetIntegerParameter( PARAMETER_0, found, isolate,  args, 0 );
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "invalid alpha function parameter" );
    return;
  }
  else
  {
    scrollView.SetScrollSnapAlphaFunction( static_cast<AlphaFunction::BuiltinFunction>(alphaFunction) );
  }
}

/**
 * Set the alpha function of overshoot snap animation.
 *
 * @for ScrollView
 * @method setSnapOvershootAlphaFunction
 * @param {String} alphaFunction The alpha function to use.
 * @example
 *      scrollView.setSnapOvershootAlphaFunction( dali.ALPHA_FUNCTION_EASE_IN_OUT );
 */
void ScrollViewApi::SetSnapOvershootAlphaFunction( const v8::FunctionCallbackInfo< v8::Value >& args)
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Toolkit::ScrollView scrollView = GetScrollView( isolate, args );

  bool found( false );
  int alphaFunction = V8Utils::GetIntegerParameter( PARAMETER_0, found, isolate,  args, 0 );
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "invalid alpha function parameter" );
    return;
  }
  else
  {
    scrollView.SetSnapOvershootAlphaFunction( static_cast<AlphaFunction::BuiltinFunction>(alphaFunction) );
  }
}

} // namespace V8Plugin

} // namespace Dali
