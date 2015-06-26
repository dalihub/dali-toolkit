#ifndef __DALI_TOOLKIT_CONTROL_DEPTH_INDEX_RANGES_H__
#define __DALI_TOOLKIT_CONTROL_DEPTH_INDEX_RANGES_H__

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

namespace Dali
{
namespace Toolkit
{
enum ControlDepthIndexRanges
{
    BACKGROUND_DEPTH_INDEX    = -10000000,
    CONTENT_DEPTH_INDEX       =  0,
    DECORATION_DEPTH_INDEX    =  10000000
};

}

}


#endif
