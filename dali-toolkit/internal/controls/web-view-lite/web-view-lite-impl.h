#ifndef DALI_TOOLKIT_INTERNAL_WEB_VIEW_LITE_H
#define DALI_TOOLKIT_INTERNAL_WEB_VIEW_LITE_H

/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/adaptor-framework/web-engine-lite.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/visuals/image/image-visual.h>
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/devel-api/controls/web-view-lite/web-view-lite.h>

namespace Dali
{

namespace Toolkit
{

class WebViewLite;

namespace Internal
{

class WebViewLite: public Control
{
protected:

  WebViewLite();

  virtual ~WebViewLite();

public:

  /**
   * @copydoc Toolkit::WebViewLite::New()
   */
  static Toolkit::WebViewLite New();

  /**
   * @copydoc Dali::Toolkit::WebViewLite::CreateInstance()
   */
  void CreateInstance(int width, int height, int windowX, int windowY, const std::string& locale, const std::string& timezoneID);

  /**
   * @copydoc Dali::Toolkit::WebViewLite::DestroyInstance()
   */
  void DestroyInstance();

  /**
   * @copydoc Dali::Toolkit::WebViewLite::LoadHtml()
   */
  void LoadHtml(const std::string& path);

 /**
   * @copydoc Dali::Toolkit::WebViewLite::FinishedSignal()
   */
  Dali::Toolkit::WebViewLite::WebViewLiteSignalType& FinishedSignal();

  /**
   * @brief Emit the finished signal
   */
  void EmitFinishedSignal();

  static bool DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor );

private: // From Control

  /**
   * @copydoc Toolkit::Control::OnInitialize()
   */
  virtual void OnInitialize();

private:

  // Undefined
  WebViewLite( const WebViewLite& webViewLite );

  WebViewLite& operator=( const WebViewLite& webViewLite );

private:

  Dali::WebEngineLite mWebViewLite;
  Dali::Toolkit::WebViewLite::WebViewLiteSignalType mFinishedSignal;
};

} // namespace Internal

inline Toolkit::Internal::WebViewLite& GetImpl( Toolkit::WebViewLite& handle )
{
  DALI_ASSERT_ALWAYS( handle );
  Dali::RefObject& impl = handle.GetImplementation();
  return static_cast< Toolkit::Internal::WebViewLite& >( impl );
}

inline const Toolkit::Internal::WebViewLite& GetImpl( const Toolkit::WebViewLite& handle )
{
  DALI_ASSERT_ALWAYS( handle );
  const Dali::RefObject& impl = handle.GetImplementation();
  return static_cast< const Toolkit::Internal::WebViewLite& >( impl );
}

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_WEB_VIEW_LITE_H
