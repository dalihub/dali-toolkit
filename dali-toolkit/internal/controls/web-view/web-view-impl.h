#ifndef DALI_TOOLKIT_INTERNAL_WEB_VIEW_H
#define DALI_TOOLKIT_INTERNAL_WEB_VIEW_H

/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/adaptor-framework/proxy-accessible.h>
#include <dali/devel-api/adaptor-framework/web-engine/web-engine.h>
#include <dali/public-api/images/image-operations.h>
#include <dali/public-api/object/property-notification.h>
#include <memory>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/control-accessible.h>
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
class WebSettings;
class WebView;

namespace Internal
{
class WebView : public Control
{
protected:
  WebView();

  WebView(const std::string& locale, const std::string& timezoneId);

  WebView(uint32_t argc, char** argv, int32_t type);

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
   * @copydoc Dali::Toolkit::WebView::New( uint32_t, char**, int32_t )
   */
  static Toolkit::WebView New(uint32_t argc, char** argv, int32_t type);

  /**
   * @copydoc Dali::Toolkit::WebView::FindWebView()
   */
  static Toolkit::WebView FindWebView(Dali::WebEnginePlugin* plugin);

  /**
   * @copydoc Dali::Toolkit::WebView::GetContext()
   */
  static Dali::WebEngineContext* GetContext();

  /**
   * @copydoc Dali::Toolkit::WebView::GetCookieManager()
   */
  static Dali::WebEngineCookieManager* GetCookieManager();

  /**
   * @copydoc Dali::Toolkit::WebView::ChangeOrientation()
   */
  void ChangeOrientation(int orientation);

  /**
   * @copydoc Dali::Toolkit::WebView::GetSettings()
   */
  Dali::Toolkit::WebSettings* GetSettings() const;

  /**
   * @copydoc Dali::Toolkit::WebView::GetBackForwardList()
   */
  Dali::Toolkit::WebBackForwardList* GetBackForwardList() const;

  /**
   * @copydoc Dali::Toolkit::WebView::GetPlugin()
   */
  Dali::WebEnginePlugin* GetPlugin() const;

  /**
   * @copydoc Dali::Toolkit::WebView::GetFavicon()
   */
  Dali::Toolkit::ImageView GetFavicon() const;

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
  bool LoadContents(const int8_t* contents, uint32_t contentSize, const std::string& mimeType, const std::string& encoding, const std::string& baseUri);

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
   * @brief Set the style of IME.
   * @param[in] position Position of IME.
   * @param[in] alignment Alignment of IME.
   *
   * @return true if succeeded, false otherwise
   */
  bool SetImePositionAndAlignment(Dali::Vector2 position, int alignment);

  /**
   * @brief Set the theme name of cursor.
   * @param[in] themeName The name of theme of cursor.
   */
  void SetCursorThemeName(const std::string themeName);

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
  void EvaluateJavaScript(const std::string& script, Dali::WebEnginePlugin::JavaScriptMessageHandlerCallback resultHandler);

  /**
   * @copydoc Dali::Toolkit::WebView::AddJavaScriptMessageHandler()
   */
  void AddJavaScriptMessageHandler(const std::string& exposedObjectName, Dali::WebEnginePlugin::JavaScriptMessageHandlerCallback handler);

  /**
   * @copydoc Dali::Toolkit::WebView::AddJavaScriptEntireMessageHandler()
   */
  void AddJavaScriptEntireMessageHandler(const std::string& exposedObjectName, Dali::WebEnginePlugin::JavaScriptEntireMessageHandlerCallback handler);

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
   * @copydoc Dali::Toolkit::WebView::ExitFullscreen()
   */
  void ExitFullscreen();

  /**
   * @copydoc Dali::Toolkit::WebView::RegisterGeolocationPermissionCallback()
   */
  void RegisterGeolocationPermissionCallback(Dali::WebEnginePlugin::GeolocationPermissionCallback callback);

  /**
   * @copydoc Dali::Toolkit::WebView::SetTtsFocus()
   */
  void SetTtsFocus(bool focused);

  /**
   * @copydoc Dali::Toolkit::WebView::RegisterPageLoadStartedCallback()
   */
  void RegisterPageLoadStartedCallback(Dali::WebEnginePlugin::WebEnginePageLoadCallback callback);

  /**
   * @copydoc Dali::Toolkit::WebView::RegisterPageLoadInProgressCallback()
   */
  void RegisterPageLoadInProgressCallback(Dali::WebEnginePlugin::WebEnginePageLoadCallback callback);

  /**
   * @copydoc Dali::Toolkit::WebView::RegisterPageLoadFinishedCallback()
   */
  void RegisterPageLoadFinishedCallback(Dali::WebEnginePlugin::WebEnginePageLoadCallback callback);

  /**
   * @copydoc Dali::Toolkit::WebView::RegisterPageLoadErrorCallback()
   */
  void RegisterPageLoadErrorCallback(Dali::WebEnginePlugin::WebEnginePageLoadErrorCallback callback);

  /**
   * @copydoc Dali::Toolkit::WebView::RegisterScrollEdgeReachedCallback()
   */
  void RegisterScrollEdgeReachedCallback(Dali::WebEnginePlugin::WebEngineScrollEdgeReachedCallback callback);

  /**
   * @copydoc Dali::Toolkit::WebView::RegisterOverScrolledCallback()
   */
  void RegisterOverScrolledCallback(Dali::WebEnginePlugin::WebEngineOverScrolledCallback callback);

  /**
   * @copydoc Dali::Toolkit::WebView::RegisterUrlChangedCallback()
   */
  void RegisterUrlChangedCallback(Dali::WebEnginePlugin::WebEngineUrlChangedCallback callback);

  /**
   * @copydoc Dali::Toolkit::WebView::RegisterFormRepostDecidedCallback()
   */
  void RegisterFormRepostDecidedCallback(Dali::WebEnginePlugin::WebEngineFormRepostDecidedCallback callback);

  /**
   * @copydoc Dali::Toolkit::WebView::RegisterFrameRenderedCallback()
   */
  void RegisterFrameRenderedCallback(Dali::WebEnginePlugin::WebEngineFrameRenderedCallback callback);

  /**
   * @copydoc Dali::Toolkit::WebView::RegisterConsoleMessageReceivedCallback()
   */
  void RegisterConsoleMessageReceivedCallback(Dali::WebEnginePlugin::WebEngineConsoleMessageReceivedCallback callback);

  /**
   * @copydoc Dali::Toolkit::WebView::RegisterResponsePolicyDecidedCallback()
   */
  void RegisterResponsePolicyDecidedCallback(Dali::WebEnginePlugin::WebEngineResponsePolicyDecidedCallback callback);

  /**
   * @copydoc Dali::Toolkit::WebView::RegisterNavigationPolicyDecidedCallback()
   */
  void RegisterNavigationPolicyDecidedCallback(Dali::WebEnginePlugin::WebEngineNavigationPolicyDecidedCallback callback);

  /**
   * @copydoc Dali::Toolkit::WebView::RegisterNewWindowPolicyDecidedCallback()
   */
  void RegisterNewWindowPolicyDecidedCallback(Dali::WebEnginePlugin::WebEngineNewWindowPolicyDecidedCallback callback);

  /**
   * @copydoc Dali::Toolkit::WebView::RegisterNewWindowCreatedCallback()
   */
  void RegisterNewWindowCreatedCallback(Dali::WebEnginePlugin::WebEngineNewWindowCreatedCallback callback);

  /**
   * @copydoc Dali::Toolkit::WebView::RegisterCertificateConfirmedCallback()
   */
  void RegisterCertificateConfirmedCallback(Dali::WebEnginePlugin::WebEngineCertificateCallback callback);

  /**
   * @copydoc Dali::Toolkit::WebView::RegisterSslCertificateChangedCallback()
   */
  void RegisterSslCertificateChangedCallback(Dali::WebEnginePlugin::WebEngineCertificateCallback callback);

  /**
   * @copydoc Dali::Toolkit::WebView::RegisterHttpAuthHandlerCallback()
   */
  void RegisterHttpAuthHandlerCallback(Dali::WebEnginePlugin::WebEngineHttpAuthHandlerCallback callback);

  /**
   * @copydoc Dali::Toolkit::WebView::RegisterContextMenuShownCallback()
   */
  void RegisterContextMenuShownCallback(Dali::WebEnginePlugin::WebEngineContextMenuShownCallback callback);

  /**
   * @copydoc Dali::Toolkit::WebView::RegisterContextMenuHiddenCallback()
   */
  void RegisterContextMenuHiddenCallback(Dali::WebEnginePlugin::WebEngineContextMenuHiddenCallback callback);

  /**
   * @copydoc Dali::Toolkit::WebView::RegisterFullscreenEnteredCallback()
   */
  void RegisterFullscreenEnteredCallback(Dali::WebEnginePlugin::WebEngineFullscreenEnteredCallback callback);

  /**
   * @copydoc Dali::Toolkit::WebView::RegisterFullscreenExitedCallback()
   */
  void RegisterFullscreenExitedCallback(Dali::WebEnginePlugin::WebEngineFullscreenExitedCallback callback);

  /**
   * @copydoc Dali::Toolkit::WebView::RegisterTextFoundCallback()
   */
  void RegisterTextFoundCallback(Dali::WebEnginePlugin::WebEngineTextFoundCallback callback);

  /**
   * @copydoc Dali::Toolkit::WebView::GetPlainTextAsynchronously()
   */
  void GetPlainTextAsynchronously(Dali::WebEnginePlugin::PlainTextReceivedCallback callback);

  /**
   * @copydoc Dali::Toolkit::WebView::WebAuthenticationCancel()
   */
  void WebAuthenticationCancel();

  /**
   * @copydoc Dali::Toolkit::WebView::RegisterWebAuthDisplayQRCallback()
   */
  void RegisterWebAuthDisplayQRCallback(Dali::WebEnginePlugin::WebEngineWebAuthDisplayQRCallback callback);

  /**
   * @copydoc Dali::Toolkit::WebView::RegisterWebAuthResponseCallback()
   */
  void RegisterWebAuthResponseCallback(Dali::WebEnginePlugin::WebEngineWebAuthResponseCallback callback);

  /**
   * @copydoc Dali::Toolkit::WebView::RegisterUserMediaPermissionRequestCallback()
   */
  void RegisterUserMediaPermissionRequestCallback(Dali::WebEnginePlugin::WebEngineUserMediaPermissionRequestCallback callback);

  /**
   * @copydoc Dali::Toolkit::WebView::RegisterDeviceConnectionChangedCallback()
   */
  void RegisterDeviceConnectionChangedCallback(Dali::WebEnginePlugin::WebEngineDeviceConnectionChangedCallback callback);

  /**
   * @copydoc Dali::Toolkit::WebView::RegisterDeviceListGetCallback()
   */
  void RegisterDeviceListGetCallback(Dali::WebEnginePlugin::WebEngineDeviceListGetCallback callback);

  /**
   * @copydoc Dali::Toolkit::WebView::FeedMouseWheel()
   */
  void FeedMouseWheel(bool yDirection, int step, int x, int y);

  /**
   * @copydoc Dali::Toolkit::WebView::SetVideoHole()
   */
  void SetVideoHole(bool enabled, bool isWaylandWindow);

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

private: // From Control
  /**
   * @copydoc Toolkit::Control::OnInitialize()
   */
  void OnInitialize() override;

  /**
   * @copydoc Toolkit::Internal::Control::CreateAccessibleObject()
   */
  DevelControl::ControlAccessible* CreateAccessibleObject() override;

  /**
   * @copydoc Toolkit::Internal::Control::OnRelayout()
   */
  void OnRelayout(const Vector2& size, RelayoutContainer& container) override;

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

  /**
   * @copydoc Toolkit::Control::OnSceneDisconnection()
   */
  void OnSceneDisconnection() override;

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
   * @brief Get url of web page.
   * @return The string of url
   */
  std::string GetUrl() const;

  /**
   * @brief Get user agent string.
   * @return The string value of user agent
   */
  std::string GetUserAgent() const;

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
  Dali::Toolkit::ImageView CreateImageView(Dali::PixelData pixel) const;

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
   * @brief Callback function to be called when frame is rendered.
   */
  void OnFrameRendered();

  /**
   * @brief Callback for updating display area of web view.
   * @param[in] source The soource triggers Notification.
   */
  void OnDisplayAreaUpdated(Dali::PropertyNotification& source);

  /**
   * @brief Callback function to be called when visibility is changed.
   * @param[in] actor The actor, whose inherit visibility has changed
   * @param[in] isVisible Whether the actor is now visible or not
   */
  void OnInheritedVisibilityChanged(Actor actor, bool isVisible);

  /**
   * @brief callback for screen shot captured.
   * @param[in] pixel Pixel data of screen shot.
   */
  void OnScreenshotCaptured(Dali::PixelData pixel);

  /**
   * @brief Set DisplayArea by input value.
   * It will send changes area infomations to webengine internally.
   *
   * @param[in] displayArea The display area for current webview want to show.
   */
  void SetDisplayArea(const Dali::Rect<int32_t>& displayArea);

protected:
  class WebViewAccessible : public DevelControl::ControlAccessible
  {
  public:
    WebViewAccessible() = delete;

    WebViewAccessible(Dali::Actor self, Dali::WebEngine& webEngine);

  protected:
    /**
     * @copydoc Dali::Accessibility::Accessible::GetAttributes()
     */
    Dali::Accessibility::Attributes GetAttributes() const override;

    /**
     * @copydoc Dali::Accessibility::ActorAccessible::DoGetChildren()
     */
    void DoGetChildren(std::vector<Dali::Accessibility::Accessible*>& children) override;

  private:
    void OnAccessibilityEnabled();
    void OnAccessibilityDisabled();
    void SetRemoteChildAddress(Dali::Accessibility::Address address);

    Dali::Accessibility::ProxyAccessible mRemoteChild;
    Dali::WebEngine&                     mWebEngine;
  };

private:
  Dali::Toolkit::Visual::Base mVisual;
  Dali::Size                  mWebViewSize;
  Dali::WebEngine             mWebEngine;

  uint32_t mLastRenderedNativeImageWidth;
  uint32_t mLastRenderedNativeImageHeight;

  std::unique_ptr<Dali::Toolkit::WebSettings>        mWebSettings;
  std::unique_ptr<Dali::Toolkit::WebBackForwardList> mWebBackForwardList;

  Dali::PropertyNotification mPositionUpdateNotification;
  Dali::PropertyNotification mSizeUpdateNotification;
  Dali::PropertyNotification mScaleUpdateNotification;
  Dali::Rect<int32_t>        mWebViewArea;
  bool                       mVideoHoleEnabled;
  bool                       mMouseEventsEnabled;
  bool                       mKeyEventsEnabled;

  bool mVisualChangeRequired;

  Dali::Toolkit::WebView::WebViewScreenshotCapturedCallback mScreenshotCapturedCallback;
  Dali::WebEnginePlugin::WebEngineFrameRenderedCallback     mFrameRenderedCallback;

  static std::unordered_map<Dali::WebEnginePlugin*, Dali::WeakHandle<Toolkit::WebView>> mPluginWebViewMap;
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
