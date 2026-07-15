#ifndef DALI_TOOLKIT_VIDEO_VIEW_DEVEL_TIZEN_H
#define DALI_TOOLKIT_VIDEO_VIEW_DEVEL_TIZEN_H

/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/adaptor-framework/video-player-plugin.h>
#include <esplusplayer_capi/esplusplayer_capi.h>
#include <player.h>

namespace Dali
{
namespace Toolkit
{
namespace DevelVideoView
{
namespace Tizen
{
static constexpr const char* MMPLAYER_PROVIDER_ID = "tizen.mmplayer";
static constexpr const char* ESPLAYER_PROVIDER_ID = "tizen.esplayer";

/**
 * @brief Builds a VideoSourceDescriptor that wraps an externally created Tizen MMPlayer handle.
 *
 * The caller owns the native player lifecycle (create / prepare / destroy); DevelVideoView::New()
 * only attaches the session for display and playback forwarding.
 *
 * @param[in] player A native MMPlayer handle. Must be attached to the view while PLAYER_STATE_IDLE,
 *                    i.e. before player_prepare(); the platform display APIs only bind in IDLE.
 * @return A descriptor for use with DevelVideoView::New(source, syncMode)
 *
 * @note Header-only so dali-toolkit itself does not build-depend on the Tizen player headers;
 *       only the consuming application/binding pulls them in.
 */
inline Dali::VideoPlayerPlugin::VideoSourceDescriptor CreateVideoSourceFromMMPlayer(player_h player)
{
  Dali::VideoPlayerPlugin::VideoSourceDescriptor source;
  source.providerId    = MMPLAYER_PROVIDER_ID;
  source.nativeSession = Dali::Any(static_cast<void*>(player));
  source.capabilities.flags =
    Dali::VideoPlayerPlugin::VideoSourceCapabilities::SUPPORTS_UNDERLAY |
    Dali::VideoPlayerPlugin::VideoSourceCapabilities::SUPPORTS_SEEK |
    Dali::VideoPlayerPlugin::VideoSourceCapabilities::SUPPORTS_VOLUME;
  return source;
}

/**
 * @brief Builds a VideoSourceDescriptor that wraps an externally created Tizen ESPlayer handle.
 *
 * @param[in] player A native ESPlayer handle.
 * @return A descriptor for use with DevelVideoView::New(source, syncMode)
 */
inline Dali::VideoPlayerPlugin::VideoSourceDescriptor CreateVideoSourceFromESPlayer(esplusplayer_handle player)
{
  Dali::VideoPlayerPlugin::VideoSourceDescriptor source;
  source.providerId         = ESPLAYER_PROVIDER_ID;
  source.nativeSession      = Dali::Any(static_cast<void*>(player));
  source.capabilities.flags = Dali::VideoPlayerPlugin::VideoSourceCapabilities::SUPPORTS_NATIVE_IMAGE;
  return source;
}

} // namespace Tizen
} // namespace DevelVideoView
} // namespace Toolkit
} // namespace Dali

#endif // DALI_TOOLKIT_VIDEO_VIEW_DEVEL_TIZEN_H
