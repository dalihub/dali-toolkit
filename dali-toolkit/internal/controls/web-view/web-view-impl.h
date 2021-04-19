#ifndef DALI_TOOLKIT_INTERNAL_WEB_VIEW_H
#define DALI_TOOLKIT_INTERNAL_WEB_VIEW_H

/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/adaptor-framework/web-engine.h>
#include <dali/public-api/images/image-operations.h>
#include <dali/public-api/object/property-notification.h>
#include <memory>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/web-view/web-view.h>
#include <dali-toolkit/devel-api/visual-factory/visual-base.h>
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/public-api/controls/image-view/image-view.h>

namespace Dali
{
namespace Toolkit
{
class KeyEvent;
class TouchEvent;
class WebBackForwardList;
class WebContext;
class WebCookieManager;
class WebSettings;
class WebView;

namespace Internal
{
class WebView : public Control
{
protected:
  WebView();

  WebView(const std::string& locale, const std::string& timezoneId);

  WebView(uint32_t argc, char** argv);

  virtual ~WebView();

public:
  /**
   * @copydoc Dali::Toolkit::WebView::New()
   */
  static Toolkit::WebView New();

  /**
   * @copydoc Dali::Toolkit::WebView::New( const std::string&, const std::string& )
   */
  static Toolkit::WebView New(const std::string& locale, const std::string& timezoneId);

  /**
   * @copydoc Dali::Toolkit::WebView::New( uint32_t, char** )
   */
  static Toolkit::WebView New(uint32_t argc, char** argv);

  /**
   * @copydoc Dali::Toolkit::WebView::GetSettings()
   */
  Dali::Toolkit::WebSettings* GetSettings() const;

  /**
   * @copydoc Dali::Toolkit::WebView::GetContext()
   */
  Dali::Toolkit::WebContext* GetContext() const;

  /**
   * @copydoc Dali::Toolkit::WebView::GetCookieManager()
   */
  Dali::Toolkit::WebCookieManager* GetCookieManager() const;

  /**
   * @copydoc Dali::Toolkit::WebView::GetBackForwardList()
   */
  Dali::Toolkit::WebBackForwardList* GetBackForwardList() const;

  /**
   * @copydoc Dali::Toolkit::WebView::GetFavicon()
   */
  Dali::Toolkit::ImageView& GetFavicon();

  /**
   * @copydoc Dali::Toolkit::WebView::LoadUrl()
   */
  void LoadUrl(const std::string& url);

  /**
   * @copydoc Dali::WebEngine::LoadHTMLString()
   */
  void LoadHtmlString(const std::string& htmlString);

  /**
   * @copydoc Dali::WebEngine::LoadHtmlStringOverrideCurrentEntry()
   */
  bool LoadHtmlStringOverrideCurrentEntry(const std::string& html, const std::string& basicUri, const std::string& unreachableUrl);

  /**
   * @copydoc Dali::WebEngine::LoadContents()
   */
  bool LoadContents(const std::string& contents, uint32_t contentSize, const std::string& mimeType, const std::string& encoding, const std::string& baseUri);

  /**
   * @copydoc Dali::Toolkit::WebView::Reload()
   */
  void Reload();

  /**
   * @copydoc Dali::WebEngine::ReloadWithoutCache()
   */
  bool ReloadWithoutCache();

  /**
   * @copydoc Dali::Toolkit::WebView::StopLoading()
   */
  void StopLoading();

  /**
   * @copydoc Dali::Toolkit::WebView::StopLoading()
   */
  void Suspend();

  /**
   * @copydoc Dali::Toolkit::WebView::Resume()
   */
  void Resume();

  /**
   * @copydoc Dali::WebEngine::SuspendNetworkLoading()
   */
  void SuspendNetworkLoading();

  /**
   * @copydoc Dali::WebEngine::ResumeNetworkLoading()
   */
  void ResumeNetworkLoading();

  /**
   * @copydoc Dali::WebEngine::AddCustomHeader()
   */
  bool AddCustomHeader(const std::string& name, const std::string& value);

  /**
   * @copydoc Dali::WebEngine::RemoveCustomHeader()
   */
  bool RemoveCustomHeader(const std::string& name);

  /**
   * @copydoc Dali::WebEngine::StartInspectorServer()
   */
  uint32_t StartInspectorServer(uint32_t port);

  /**
   * @copydoc Dali::WebEngine::StopInspectorServer()
   */
  bool StopInspectorServer();

  /**
   * @copydoc Dali::Toolkit::WebView::ScrollBy()
   */
  void ScrollBy(int32_t deltaX, int32_t deltaY);

  /**
   * @copydoc Dali::WebEngine::ScrollEdgeBy()
   */
  bool ScrollEdgeBy(int32_t deltaX, int32_t deltaY);

  /**
   * @copydoc Dali::Toolkit::WebView::CanGoForward()
   */
  bool CanGoForward();

  /**
   * @copydoc Dali::Toolkit::WebView::GoForward()
   */
  void GoForward();

  /**
   * @copydoc Dali::Toolkit::WebView::CanGoBack()
   */
  bool CanGoBack();

  /**
   * @copydoc Dali::Toolkit::WebView::GoBack()
   */
  void GoBack();

  /**
   * @copydoc Dali::Toolkit::WebView::EvaluateJavaScript()
   */
  void EvaluateJavaScript(const std::string& script, std::function<void(const std::string&)> resultHandler);

  /**
   * @copydoc Dali::Toolkit::WebView::AddJavaScriptMessageHandler()
   */
  void AddJavaScriptMessageHandler(const std::string& exposedObjectName, std::function<void(const std::string&)> handler);

  /**
   * @copydoc Dali::Toolkit::WebView::RegisterJavaScriptAlertCallback()
   */
  void RegisterJavaScriptAlertCallback(Dali::WebEnginePlugin::JavaScriptAlertCallback callback);

  /**
   * @copydoc Dali::Toolkit::WebView::JavaScriptAlertReply()
   */
  void JavaScriptAlertReply();

  /**
   * @copydoc Dali::Toolkit::WebView::RegisterJavaScriptConfirmCallback()
   */
  void RegisterJavaScriptConfirmCallback(Dali::WebEnginePlugin::JavaScriptConfirmCallback callback);

  /**
   * @copydoc Dali::Toolkit::WebView::JavaScriptConfirmReply()
   */
  void JavaScriptConfirmReply(bool confirmed);

  /**
   * @copydoc Dali::Toolkit::WebView::RegisterJavaScriptPromptCallback()
   */
  void RegisterJavaScriptPromptCallback(Dali::WebEnginePlugin::JavaScriptPromptCallback callback);

  /**
   * @copydoc Dali::Toolkit::WebView::JavaScriptPromptReply()
   */
  void JavaScriptPromptReply(const std::string& result);

  /**
   * @copydoc Dali::Toolkit::WebView::CreateHitTest()
   */
  std::unique_ptr<Dali::WebEngineHitTest> CreateHitTest(int32_t x, int32_t y, Dali::WebEngineHitTest::HitTestMode mode);

  /**
   * @copydoc Dali::Toolkit::WebView::CreateHitTestAsynchronously()
   */
  bool CreateHitTestAsynchronously(int32_t x, int32_t y, Dali::WebEngineHitTest::HitTestMode mode, Dali::WebEnginePlugin::WebEngineHitTestCreatedCallback callback);

  /**
   * @copydoc Dali::Toolkit::WebView::ClearHistory()
   */
  void ClearHistory();

  /**
   * @copydoc Dali::Toolkit::WebView::ClearAllTilesResources()
   */
  void ClearAllTilesResources();

  /**
   * @copydoc Dali::Toolkit::WebView::SetScaleFactor()
   */
  void SetScaleFactor(float scaleFactor, Dali::Vector2 point);

  /**
   * @copydoc Dali::Toolkit::WebView::GetScaleFactor()
   */
  float GetScaleFactor() const;

  /**
   * @copydoc Dali::Toolkit::WebView::ActivateAccessibility()
   */
  void ActivateAccessibility(bool activated);

  /**
   * @copydoc Dali::Toolkit::WebView::HighlightText()
   */
  bool HighlightText(const std::string& text, Dali::WebEnginePlugin::FindOption options, uint32_t maxMatchCount);

  /**
   * @copydoc Dali::Toolkit::WebView::AddDynamicCertificatePath()
   */
  void AddDynamicCertificatePath(const std::string& host, const std::string& certPath);

  /**
   * @copydoc Dali::Toolkit::WebView::GetScreenshot()
   */
  Dali::Toolkit::ImageView GetScreenshot(Dali::Rect<int32_t> viewArea, float scaleFactor);

  /**
   * @copydoc Dali::Toolkit::WebView::GetScreenshotAsynchronously()
   */
  bool GetScreenshotAsynchronously(Dali::Rect<int32_t> viewArea, float scaleFactor, Dali::Toolkit::WebView::WebViewScreenshotCapturedCallback callback);

  /**
   * @copydoc Dali::Toolkit::WebView::CheckVideoPlayingAsynchronously()
   */
  bool CheckVideoPlayingAsynchronously(Dali::WebEnginePlugin::VideoPlayingCallback callback);

  /**
   * @copydoc Dali::Toolkit::WebView::RegisterGeolocationPermissionCallback()
   */
  void RegisterGeolocationPermissionCallback(Dali::WebEnginePlugin::GeolocationPermissionCallback callback);

  /**
   * @copydoc Dali::Toolkit::WebView::PageLoadStartedSignal()
   */
  Dali::Toolkit::WebView::WebViewPageLoadSignalType& PageLoadStartedSignal();

  /**
   * @copydoc Dali::Toolkit::WebView::PageLoadInProgressSignal()
   */
  Dali::Toolkit::WebView::WebViewPageLoadSignalType& PageLoadInProgressSignal();

  /**
   * @copydoc Dali::Toolkit::WebView::PageLoadFinishedSignal()
   */
  Dali::Toolkit::WebView::WebViewPageLoadSignalType& PageLoadFinishedSignal();

  /**
   * @copydoc Dali::Toolkit::WebView::PageLoadErrorSignal()
   */
  Dali::Toolkit::WebView::WebViewPageLoadErrorSignalType& PageLoadErrorSignal();

  /**
   * @copydoc Dali::Toolkit::WebView::ScrollEdgeReachedSignal()
   */
  Dali::Toolkit::WebView::WebViewScrollEdgeReachedSignalType& ScrollEdgeReachedSignal();

  /**
   * @copydoc Dali::Toolkit::WebView::UrlChangedSignal()
   */
  Dali::Toolkit::WebView::WebViewUrlChangedSignalType& UrlChangedSignal();

  /**
   * @copydoc Dali::Toolkit::WebView::FormRepostDecisionSignal()
   */
  Dali::Toolkit::WebView::WebViewFormRepostDecisionSignalType& FormRepostDecisionSignal();

  /**
   * @copydoc Dali::Toolkit::WebView::FrameRenderedSignal()
   */
  Dali::Toolkit::WebView::WebViewFrameRenderedSignalType& FrameRenderedSignal();

  /**
   * @copydoc Dali::Toolkit::WebView::RequestInterceptorSignal()
   */
  Dali::Toolkit::WebView::WebViewRequestInterceptorSignalType& RequestInterceptorSignal();

  /**
   * @copydoc Dali::Toolkit::WebView::ConsoleMessageSignal()
   */
  Dali::Toolkit::WebView::WebViewConsoleMessageSignalType& ConsoleMessageSignal();

  /**
   * @copydoc Dali::Toolkit::WebView::PolicyDecisionSignal()
   */
  Dali::Toolkit::WebView::WebViewPolicyDecisionSignalType& PolicyDecisionSignal();

  /**
   * @copydoc Dali::Toolkit::WebView::CertificateConfirmSignal()
   */
  Dali::Toolkit::WebView::WebViewCertificateSignalType& CertificateConfirmSignal();

  /**
   * @copydoc Dali::Toolkit::WebView::SslCertificateChangedSignal()
   */
  Dali::Toolkit::WebView::WebViewCertificateSignalType& SslCertificateChangedSignal();

  /**
   * @copydoc Dali::Toolkit::WebView::HttpAuthHandlerSignal()
   */
  Dali::Toolkit::WebView::WebViewHttpAuthHandlerSignalType& HttpAuthHandlerSignal();

  /**
   * @copydoc Dali::Toolkit::WebView::ContextMenuCustomizedSignal()
   */
  Dali::Toolkit::WebView::WebViewContextMenuCustomizedSignalType& ContextMenuCustomizedSignal();

  /**
   * @copydoc Dali::Toolkit::WebView::ContextMenuItemSelectedSignal()
   */
  Dali::Toolkit::WebView::WebViewContextMenuItemSelectedSignalType& ContextMenuItemSelectedSignal();

public: // Properties
  /**
   * @brief Called when a property of an object of this type is set.
   *
   * @param[in] object The object whose property is set.
   * @param[in] index The property index.
   * @param[in] value The new property value.
   */
  static void SetProperty(Dali::BaseObject* object, Dali::Property::Index index, const Dali::Property::Value& value);

  /**
   * @brief Called to retrieve a property of an object of this type.
   *
   * @param[in] object The object whose property is to be retrieved.
   * @param[in] index The property index.
   * @return The current value of the property.
   */
  static Dali::Property::Value GetProperty(Dali::BaseObject* object, Dali::Property::Index propertyIndex);

  /**
   * Connects a callback function with the object's signals.
   * @param[in] object The object providing the signal.
   * @param[in] tracker Used to disconnect the signal.
   * @param[in] signalName The signal to connect to.
   * @param[in] functor A newly allocated FunctorDelegate.
   * @return True if the signal was connected.
   * @post If a signal was connected, ownership of functor was passed to CallbackBase. Otherwise the c
   */
  static bool DoConnectSignal(BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor);

private: // From Control
  /**
   * @copydoc Toolkit::Control::OnInitialize()
   */
  void OnInitialize() override;

  /**
   * @copydoc Toolkit::Control::GetNaturalSize
   */
  Vector3 GetNaturalSize() override;

  /**
   * @copydoc Toolkit::Control::OnKeyEvent()
   */
  bool OnKeyEvent(const Dali::KeyEvent& event) override;

  /**
   * @copydoc Toolkit::Control::OnKeyInputFocusGained()
   */
  void OnKeyInputFocusGained() override;

  /**
   * @copydoc Toolkit::Control::OnKeyInputFocusLost()
   */
  void OnKeyInputFocusLost() override;

  /**
   * @copydoc Toolkit::Control::OnSceneConnection()
   */
  void OnSceneConnection(int depth) override;

private:
  // Undefined
  WebView(const WebView& webView);

  WebView& operator=(const WebView& webView);

  /**
   * @brief Set an absolute scroll of the given view.
   * @param[in] x The coordinate x of scroll
   * @param[in] y The coordinate y of scroll
   */
  void SetScrollPosition(int32_t x, int32_t y);

  /**
   * @brief Get the current scroll position of the given view.
   * @param[out] x The coordinate x of scroll
   * @param[out] y The coordinate y of scroll
   */
  Dali::Vector2 GetScrollPosition() const;

  /**
   * @brief Get the possible scroll size of the given view.
   * @param[out] width The width of scroll size
   * @param[out] height The height of scroll size
   */
  Dali::Vector2 GetScrollSize() const;

  /**
   * @brief Get the last known content's size.
   * @param[out] width The width of content's size
   * @param[out] height The height of content's size
   */
  Dali::Vector2 GetContentSize() const;

  /**
   * @brief Return the title of the Web.
   *
   * @return The title of web page
   */
  std::string GetTitle() const;

  /**
   * @brief Set the background color of web page.
   * @param[in] color The value of background color.
   */
  void SetDocumentBackgroundColor(Dali::Vector4 color);

  /**
   * @brief Clear tiles when hidden.
   *
   * @param[in] cleared Whether tiles are cleared or not
   */
  void ClearTilesWhenHidden(bool cleared);

  /**
   * @brief Set multiplier of cover area of tile.
   *
   * @param[in] multiplier The multiplier of cover area
   */
  void SetTileCoverAreaMultiplier(float multiplier);

  /**
   * @brief Enable cursor by client.
   * @param[in] enabled Whether cursor is enabled or not.
   */
  void EnableCursorByClient(bool enabled);

  /**
   * @brief Get the selected text.
   * @return The selected text
   */
  std::string GetSelectedText() const;

  /**
   * @brief Get user agent string.
   * @return The string value of user agent
   */
  const std::string& GetUserAgent() const;

  /**
   * @brief Set user agent string.
   * @param[in] userAgent The string value of user agent
   */
  void SetUserAgent(const std::string& userAgent);

  /**
   * @brief Set zoom factor of the current page.
   * @param[in] zoomFactor a new factor to be set.
   */
  void SetPageZoomFactor(float zoomFactor);

  /**
   * @brief Query the current zoom factor of the page。
   * @return The current page zoom factor.
   */
  float GetPageZoomFactor() const;

  /**
   * @brief Set the current text zoom level。.
   * @param[in] zoomFactor a new factor to be set.
   */
  void SetTextZoomFactor(float zoomFactor);

  /**
   * @brief Get the current text zoom level.
   * @return The current text zoom factor.
   */
  float GetTextZoomFactor() const;

  /**
   * @brief Get the current load progress of the page.
   * @return The load progress of the page.
   */
  float GetLoadProgressPercentage() const;

  /**
   * @brief Request to set the current page's visibility.
   * @param[in] visible Visible or not.
   *
   * @return true if succeeded, false otherwise
   */
  bool SetVisibility(bool visible);

  /**
   * @brief Update display area of web view.
   * @param[in] source The soource triggers Notification.
   */
  void UpdateDisplayArea(Dali::PropertyNotification& source);

  /**
   * @brief Enable/Disable video hole for video playing.
   * @param[in] enabled True if video hole is enabled, false otherwise.
   */
  void EnableVideoHole(bool enabled);

  /**
   * @brief Enable blend mode.
   * @param[in] blendEnabled True if turn on blend mode, false otherwise.
   */
  void EnableBlendMode(bool blendEnabled);

  /**
   * @brief Enable/disable mouse events. The default is enabled.
   *
   * @param[in] enabled True if mouse events are enabled, false otherwise
   */
  void EnableMouseEvents(bool enabled);

  /**
   * @brief Enable/disable key events. The default is enabled.
   *
   * @param[in] enabled True if key events enabled, false otherwise
   */
  void EnableKeyEvents(bool enabled);

  /**
   * @brief Create image view by pixel data.
   * @param[in] pixel Pixel data
   * @return The new image view
   */
  Dali::Toolkit::ImageView CreateImageView(Dali::PixelData pixel);

  /**
   * @brief Callback function to be called when page load started.
   * @param[in] url The url currently being loaded
   */
  void OnPageLoadStarted(const std::string& url);

  /**
   * @brief Callback function to be called when page is loading in progress.
   * @param[in] url The url currently being loaded
   */
  void OnPageLoadInProgress(const std::string& url);

  /**
   * @brief Callback function to be called when page load finished.
   * @param[in] url The url currently being loaded
   */
  void OnPageLoadFinished(const std::string& url);

  /**
   * @brief Callback function to be called when there is an error in page loading.
   * @param[in] url The url currently being loaded
   * @param[in] errorCode The error code
   */
  void OnPageLoadError(std::shared_ptr<Dali::WebEngineLoadError> error);

  /**
   * @brief Callback function to be called when scroll edge is reached.
   * @param[in] edge The scroll edge reached.
   */
  void OnScrollEdgeReached(Dali::WebEnginePlugin::ScrollEdge edge);

  /**
   * @brief Callback function to be called when url is changed.
   * @param[in] url The url currently being loaded
   */
  void OnUrlChanged(const std::string& url);

  /**
   * @brief Signal occurs when the Web View has been touched.
   * @param[in] actor The Actor Touched
   * @param[in] touch The Touch Data.
   * @return Whether to consume event or not.
   */
  bool OnTouchEvent(Actor actor, const Dali::TouchEvent& touch);

  /**
   * @brief Signal occurs when the Web View has been hovered.
   * @param[in] actor The Actor Hovered
   * @param[in] hover The Hover Data.
   * @return Whether to consume event or not.
   */
  bool OnHoverEvent(Actor actor, const Dali::HoverEvent& hover);

  /**
   * @brief Signal occurs when the Web View receives wheel event.
   * @param[in] actor The Actor that receives Wheel event.
   * @param[in] wheel The Wheel Data.
   * @return Whether to consume event or not.
   */
  bool OnWheelEvent(Actor actor, const Dali::WheelEvent& wheel);

  /**
   * @brief Callback function to be called when form repost decision need be checked.
   * @param[in] decision The new decision for form repost
   */
  void OnFormRepostDecision(std::shared_ptr<Dali::WebEngineFormRepostDecision> decision);

  /**
   * @brief Callback function to be called when frame is rendered.
   */
  void OnFrameRendered();

  /**
   * @brief Callback function to be called when visibility is changed.
   * @param[in] actor The actor, or child of actor, whose visibility has changed
   * @param[in] isVisible Whether the actor is now visible or not
   * @param[in] type, Whether the actor's visible property has changed or a parent's
   */
  void OnVisibilityChanged(Actor actor, bool isVisible, Dali::DevelActor::VisibilityChange::Type type);

  /**
   * @brief callback for screen shot captured.
   * @param[in] pixel Pixel data of screen shot.
   */
  void OnScreenshotCaptured(Dali::PixelData pixel);

  /**
   * @brief Callback function to be called when http request need be intercepted.
   * @param [in] request The http request interceptor.
   */
  void OnInterceptRequest(std::shared_ptr<Dali::WebEngineRequestInterceptor> interceptor);

  /**
   * @brief Callback function to be called when console message will be logged.
   * @param[in] message The message logged.
   */
  void OnConsoleMessage(std::shared_ptr<Dali::WebEngineConsoleMessage> message);

  /**
   * @brief Callback function to be called when policy need be decided.
   * @param[in] decision The policy decided.
   */
  void OnPolicyDecisionRequest(std::shared_ptr<Dali::WebEnginePolicyDecision> decision);

  /**
   * @brief Callback function to be called when certificate need be confirmed.
   * @param[in] certificate The certificate policy decision.
   */
  void OnCertificateConfirm(std::shared_ptr<Dali::WebEngineCertificate> certificate);

  /**
   * @brief Callback function to be called when ssl certificate is changed.
   * @param[in] certificate The certificate information received.
   */
  void OnSslCertificateChanged(std::shared_ptr<Dali::WebEngineCertificate> certificate);

  /**
   * @brief Callback function to be called when http authentication need be confirmed.
   * @param[in] handler The handler for http authentication
   */
  void OnHttpAuthenticationRequest(std::shared_ptr<Dali::WebEngineHttpAuthHandler> handler);

  /**
   * @brief Callback function to be called when context menu would be customized.
   * @param[in] e The scroll edge reached.
   */
  void OnContextMenuCustomized(std::shared_ptr<Dali::WebEngineContextMenu> menu);

  /**
   * @brief Callback function to be called when context menu item is selected.
   * @param[in] url The url currently being loaded
   */
  void OnContextMenuItemSelected(std::shared_ptr<Dali::WebEngineContextMenuItem> item);

private:
  std::string                 mUrl;
  Dali::Toolkit::Visual::Base mVisual;
  Dali::Size                  mWebViewSize;
  Dali::WebEngine             mWebEngine;

  Dali::Toolkit::WebView::WebViewPageLoadSignalType                mPageLoadStartedSignal;
  Dali::Toolkit::WebView::WebViewPageLoadSignalType                mPageLoadInProgressSignal;
  Dali::Toolkit::WebView::WebViewPageLoadSignalType                mPageLoadFinishedSignal;
  Dali::Toolkit::WebView::WebViewPageLoadErrorSignalType           mPageLoadErrorSignal;
  Dali::Toolkit::WebView::WebViewUrlChangedSignalType              mUrlChangedSignal;
  Dali::Toolkit::WebView::WebViewScrollEdgeReachedSignalType       mScrollEdgeReachedSignal;
  Dali::Toolkit::WebView::WebViewFormRepostDecisionSignalType      mFormRepostDecisionSignal;
  Dali::Toolkit::WebView::WebViewFrameRenderedSignalType           mFrameRenderedSignal;
  Dali::Toolkit::WebView::WebViewRequestInterceptorSignalType      mRequestInterceptorSignal;
  Dali::Toolkit::WebView::WebViewConsoleMessageSignalType          mConsoleMessageSignal;
  Dali::Toolkit::WebView::WebViewPolicyDecisionSignalType          mPolicyDecisionSignal;
  Dali::Toolkit::WebView::WebViewCertificateSignalType             mCertificateConfirmSignal;
  Dali::Toolkit::WebView::WebViewCertificateSignalType             mSslCertificateChangedSignal;
  Dali::Toolkit::WebView::WebViewHttpAuthHandlerSignalType         mHttpAuthHandlerSignal;
  Dali::Toolkit::WebView::WebViewContextMenuCustomizedSignalType   mContextMenuCustomizedSignal;
  Dali::Toolkit::WebView::WebViewContextMenuItemSelectedSignalType mContextMenuItemSelectedSignal;

  std::unique_ptr<Dali::Toolkit::WebContext>         mWebContext;
  std::unique_ptr<Dali::Toolkit::WebCookieManager>   mWebCookieManager;
  std::unique_ptr<Dali::Toolkit::WebSettings>        mWebSettings;
  std::unique_ptr<Dali::Toolkit::WebBackForwardList> mWebBackForwardList;

  Dali::Toolkit::ImageView mFaviconView;

  Dali::PropertyNotification mPositionUpdateNotification;
  Dali::PropertyNotification mSizeUpdateNotification;
  Dali::PropertyNotification mScaleUpdateNotification;
  Dali::Rect<int32_t>        mWebViewArea;
  bool                       mVideoHoleEnabled;
  bool                       mMouseEventsEnabled;
  bool                       mKeyEventsEnabled;

  Dali::Toolkit::WebView::WebViewScreenshotCapturedCallback mScreenshotCapturedCallback;
};

} // namespace Internal

inline Toolkit::Internal::WebView& GetImpl(Toolkit::WebView& handle)
{
  DALI_ASSERT_ALWAYS(handle);
  Dali::RefObject& impl = handle.GetImplementation();
  return static_cast<Toolkit::Internal::WebView&>(impl);
}

inline const Toolkit::Internal::WebView& GetImpl(const Toolkit::WebView& handle)
{
  DALI_ASSERT_ALWAYS(handle);
  const Dali::RefObject& impl = handle.GetImplementation();
  return static_cast<const Toolkit::Internal::WebView&>(impl);
}

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_WEB_VIEW_H
