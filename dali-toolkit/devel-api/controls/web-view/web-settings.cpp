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

// CLASS HEADER
#include <dali-toolkit/devel-api/controls/web-view/web-settings.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/web-engine-settings.h>

namespace Dali
{
namespace Toolkit
{
WebSettings::WebSettings(Dali::WebEngineSettings& settings)
: mWebEngineSettings(settings)
{
}

WebSettings::~WebSettings()
{
}

void WebSettings::AllowMixedContents(bool allowed)
{
  mWebEngineSettings.AllowMixedContents(allowed);
}

void WebSettings::EnableSpatialNavigation(bool enabled)
{
  mWebEngineSettings.AllowMixedContents(enabled);
}

int WebSettings::GetDefaultFontSize() const
{
  return mWebEngineSettings.GetDefaultFontSize();
}

void WebSettings::SetDefaultFontSize(int defaultFontSize)
{
  mWebEngineSettings.SetDefaultFontSize(defaultFontSize);
}

void WebSettings::EnableWebSecurity(bool enabled)
{
  mWebEngineSettings.EnableWebSecurity(enabled);
}

void WebSettings::EnableCacheBuilder(bool enabled)
{
  mWebEngineSettings.EnableCacheBuilder(enabled);
}

void WebSettings::UseScrollbarThumbFocusNotifications(bool used)
{
  mWebEngineSettings.UseScrollbarThumbFocusNotifications(used);
}

void WebSettings::EnableDoNotTrack( bool enabled )
{
  mWebEngineSettings.EnableDoNotTrack(enabled);
}

void WebSettings::AllowFileAccessFromExternalUrl(bool allowed)
{
  mWebEngineSettings.AllowFileAccessFromExternalUrl(allowed);
}

bool WebSettings::IsJavaScriptEnabled() const
{
  return mWebEngineSettings.IsJavaScriptEnabled();
}

void WebSettings::EnableJavaScript(bool enabled)
{
  mWebEngineSettings.EnableJavaScript(enabled);
}

bool WebSettings::IsAutoFittingEnabled() const
{
  return mWebEngineSettings.IsAutoFittingEnabled();
}

void WebSettings::EnableAutoFitting(bool enabled)
{
  mWebEngineSettings.EnableAutoFitting(enabled);
}

bool WebSettings::ArePluginsEnabled() const
{
  return mWebEngineSettings.ArePluginsEnabled();
}

void WebSettings::EnablePlugins(bool enabled)
{
  mWebEngineSettings.EnablePlugins(enabled);
}

bool WebSettings::IsPrivateBrowsingEnabled() const
{
  return mWebEngineSettings.IsPrivateBrowsingEnabled();
}

void WebSettings::EnablePrivateBrowsing(bool enabled)
{
  mWebEngineSettings.EnablePrivateBrowsing(enabled);
}

bool WebSettings::IsLinkMagnifierEnabled() const
{
  return mWebEngineSettings.IsLinkMagnifierEnabled();
}

void WebSettings::EnableLinkMagnifier(bool enabled)
{
  mWebEngineSettings.EnableLinkMagnifier(enabled);
}

bool WebSettings::IsKeypadWithoutUserActionUsed() const
{
  return mWebEngineSettings.IsKeypadWithoutUserActionUsed();
}

void WebSettings::UseKeypadWithoutUserAction(bool used)
{
  mWebEngineSettings.UseKeypadWithoutUserAction(used);
}

bool WebSettings::IsAutofillPasswordFormEnabled() const
{
  return mWebEngineSettings.IsAutofillPasswordFormEnabled();
}

void WebSettings::EnableAutofillPasswordForm(bool enabled)
{
  mWebEngineSettings.EnableAutofillPasswordForm(enabled);
}

bool WebSettings::IsFormCandidateDataEnabled() const
{
  return mWebEngineSettings.IsFormCandidateDataEnabled();
}

void WebSettings::EnableFormCandidateData( bool enabled )
{
  mWebEngineSettings.EnableFormCandidateData(enabled);
}

bool WebSettings::IsTextSelectionEnabled() const
{
  return mWebEngineSettings.IsTextSelectionEnabled();
}

void WebSettings::EnableTextSelection( bool enabled )
{
  return mWebEngineSettings.EnableTextSelection(enabled);
}

bool WebSettings::IsTextAutosizingEnabled() const
{
  return mWebEngineSettings.IsTextAutosizingEnabled();
}

void WebSettings::EnableTextAutosizing( bool enabled )
{
  return mWebEngineSettings.EnableTextAutosizing(enabled);
}

bool WebSettings::IsArrowScrollEnabled() const
{
  return mWebEngineSettings.IsArrowScrollEnabled();
}

void WebSettings::EnableArrowScroll( bool enabled )
{
  return mWebEngineSettings.EnableArrowScroll(enabled);
}

bool WebSettings::IsClipboardEnabled() const
{
  return mWebEngineSettings.IsClipboardEnabled();
}

void WebSettings::EnableClipboard( bool enabled )
{
  return mWebEngineSettings.EnableClipboard(enabled);
}

bool WebSettings::IsImePanelEnabled() const
{
  return mWebEngineSettings.IsImePanelEnabled();
}

void WebSettings::EnableImePanel( bool enabled )
{
  return mWebEngineSettings.EnableImePanel(enabled);
}

void WebSettings::AllowScriptsOpenWindows(bool allowed)
{
  mWebEngineSettings.AllowScriptsOpenWindows(allowed);
}

bool WebSettings::AreImagesLoadedAutomatically() const
{
  return mWebEngineSettings.AreImagesLoadedAutomatically();
}

void WebSettings::AllowImagesLoadAutomatically(bool automatic)
{
  mWebEngineSettings.AllowImagesLoadAutomatically(automatic);
}

std::string WebSettings::GetDefaultTextEncodingName() const
{
  return mWebEngineSettings.GetDefaultTextEncodingName();
}

void WebSettings::SetDefaultTextEncodingName(const std::string& defaultTextEncodingName)
{
  mWebEngineSettings.SetDefaultTextEncodingName(defaultTextEncodingName);
}

bool WebSettings::SetViewportMetaTag(bool enable)
{
  return mWebEngineSettings.SetViewportMetaTag(enable);
}

bool WebSettings::SetForceZoom(bool enable)
{
  return mWebEngineSettings.SetForceZoom(enable);
}

bool WebSettings::IsZoomForced() const
{
  return mWebEngineSettings.IsZoomForced();
}

bool WebSettings::SetTextZoomEnabled(bool enable)
{
  return mWebEngineSettings.SetTextZoomEnabled(enable);
}

bool WebSettings::IsTextZoomEnabled() const
{
  return mWebEngineSettings.IsTextZoomEnabled();
}

void WebSettings::SetExtraFeature(const std::string& feature, bool enable)
{
  mWebEngineSettings.SetExtraFeature(feature, enable);
}

bool WebSettings::IsExtraFeatureEnabled(const std::string& feature) const
{
  return mWebEngineSettings.IsExtraFeatureEnabled(feature);
}


} // namespace Toolkit

} // namespace Dali
