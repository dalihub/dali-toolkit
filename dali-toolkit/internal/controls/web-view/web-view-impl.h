#ifndef DALI_TOOLKIT_INTERNAL_WEB_VIEW_H
#define DALI_TOOLKIT_INTERNAL_WEB_VIEW_H

/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/web-engine.h>
#include <dali/public-api/images/image-operations.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/visual-factory/visual-base.h>
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/devel-api/controls/web-view/web-view.h>

namespace Dali
{

namespace Toolkit
{

class KeyEvent;
class TouchData;
class WebView;

namespace Internal
{

class WebView : public Control
{
protected:

  WebView();

  WebView( const std::string& locale, const std::string& timezoneId );

  virtual ~WebView();

public:

  /**
   * @copydoc Dali::Toolkit::WebView::New()
   */
  static Toolkit::WebView New();

  /**
   * @copydoc Dali::Toolkit::WebView::New( const std::string&, const std::string& )
   */
  static Toolkit::WebView New( const std::string& locale, const std::string& timezoneId );

  /**
   * @copydoc Dali::Toolkit::WebView::LoadUrl()
   */
  void LoadUrl( const std::string& url );

  /**
   * @copydoc Dali::Toolkit::WebView::GetUrl()
   */
  const std::string& GetUrl();

  /**
   * @copydoc Dali::WebEngine::LoadHTMLString()
   */
  void LoadHTMLString( const std::string& htmlString );

  /**
   * @copydoc Dali::WebEngine::Reload()
   */
  void Reload();

  /**
   * @copydoc Dali::WebEngine::StopLoading()
   */
  void StopLoading();

  /**
   * @copydoc Dali::WebEngine::CanGoForward()
   */
  bool CanGoForward();

  /**
   * @copydoc Dali::WebEngine::GoForward()
   */
  void GoForward();

  /**
   * @copydoc Dali::WebEngine::CanGoBack()
   */
  bool CanGoBack();

  /**
   * @copydoc Dali::WebEngine::GoBack()
   */
  void GoBack();

  /**
   * @copydoc Dali::WebEngine::EvaluateJavaScript()
   */
  void EvaluateJavaScript( const std::string& script );

  /**
   * @copydoc Dali::WebEngine::AddJavaScriptInterface()
   */
  void AddJavaScriptInterface( const std::string& exposedObjectName, const std::string& jsFunctionName, std::function< std::string(const std::string&) > callback );

  /**
   * @copydoc Dali::WebEngine::RemoveJavascriptInterface()
   */
  void RemoveJavascriptInterface( const std::string& exposedObjectName, const std::string& jsFunctionName );

  /**
   * @copydoc Dali::WebEngine::ClearHistory()
   */
  void ClearHistory();

  /**
   * @copydoc Dali::WebEngine::ClearCache()
   */
  void ClearCache();

  /**
   * @copydoc Dali::Toolkit::WebView::PageLoadStartedSignal()
   */
  Dali::Toolkit::WebView::WebViewSignalType& PageLoadStartedSignal();

  /**
   * @copydoc Dali::Toolkit::WebView::PageLoadFinishedSignal()
   */
  Dali::Toolkit::WebView::WebViewSignalType& PageLoadFinishedSignal();

public: // Properties

  /**
   * @brief Called when a property of an object of this type is set.
   *
   * @param[in] object The object whose property is set.
   * @param[in] index The property index.
   * @param[in] value The new property value.
   */
  static void SetProperty( Dali::BaseObject* object, Dali::Property::Index index, const Dali::Property::Value& value );

  /**
   * @brief Called to retrieve a property of an object of this type.
   *
   * @param[in] object The object whose property is to be retrieved.
   * @param[in] index The property index.
   * @return The current value of the property.
   */
  static Dali::Property::Value GetProperty( Dali::BaseObject* object, Dali::Property::Index propertyIndex );

  /**
   * Connects a callback function with the object's signals.
   * @param[in] object The object providing the signal.
   * @param[in] tracker Used to disconnect the signal.
   * @param[in] signalName The signal to connect to.
   * @param[in] functor A newly allocated FunctorDelegate.
   * @return True if the signal was connected.
   * @post If a signal was connected, ownership of functor was passed to CallbackBase. Otherwise the c
   */
  static bool DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor );

private: // From Control

  /**
   * @copydoc Toolkit::Control::OnInitialize()
   */
  virtual void OnInitialize();

  /**
   * @copydoc Toolkit::Control::GetNaturalSize
   */
  virtual Vector3 GetNaturalSize();

  /**
   * @copydoc Toolkit::Control::OnRelayout()
   */
  virtual void OnRelayout( const Vector2& size, RelayoutContainer& container );

  /**
   * Signal occurs when the Web View has been touched.
   * @param[in] actor The Actor Touched
   * @param[in] touch The Touch Data.
   * @return Whether to consume event or not.
   */
  bool OnTouchEvent( Actor actor, const Dali::TouchData& touch );

  /**
   * @copydoc Toolkit::Control::OnKeyEvent()
   */
  virtual bool OnKeyEvent( const Dali::KeyEvent& event );

private:

  // Undefined
  WebView( const WebView& webView );

  WebView& operator=( const WebView& webView );

  void OnPageLoadStarted( const std::string& url );

  void OnPageLoadFinished( const std::string& url );

private:

  std::string mUrl;
  Dali::Toolkit::Visual::Base mVisual;
  Dali::Size mWebViewSize;
  Dali::WebEngine mWebEngine;
  Dali::Toolkit::WebView::WebViewSignalType mPageLoadStartedSignal;
  Dali::Toolkit::WebView::WebViewSignalType mPageLoadFinishedSignal;
};

} // namespace Internal

inline Toolkit::Internal::WebView& GetImpl( Toolkit::WebView& handle )
{
  DALI_ASSERT_ALWAYS( handle );
  Dali::RefObject& impl = handle.GetImplementation();
  return static_cast< Toolkit::Internal::WebView& >( impl );
}

inline const Toolkit::Internal::WebView& GetImpl( const Toolkit::WebView& handle )
{
  DALI_ASSERT_ALWAYS( handle );
  const Dali::RefObject& impl = handle.GetImplementation();
  return static_cast< const Toolkit::Internal::WebView& >( impl );
}

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_WEB_VIEW_H
