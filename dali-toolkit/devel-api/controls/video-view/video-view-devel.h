#ifndef DALI_TOOLKIT_VIDEO_VIEW_DEVEL_H
#define DALI_TOOLKIT_VIDEO_VIEW_DEVEL_H

/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/video-view/video-view.h>
#include <dali/public-api/object/any.h>

namespace Dali
{

namespace Toolkit
{

namespace DevelVideoView
{


/**
 * @brief Returns the internal media player.
 * @param[in] videoView The current VideoView
 * @return The internal media player of current VideoView
 */
DALI_TOOLKIT_API Any GetMediaPlayer( VideoView videoView );


} // namespace DevelVideoView

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_VIDEO_VIEW_DEVEL_H
