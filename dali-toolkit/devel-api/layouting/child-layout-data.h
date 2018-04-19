#ifndef DALI_TOOLKIT_LAYOUTING_CHILD_LAYOUT_DATA_H
#define DALI_TOOLKIT_LAYOUTING_CHILD_LAYOUT_DATA_H

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
 */


namespace Dali
{
namespace Toolkit
{

// used in property system - must be integer values.
namespace ChildLayoutData
{
static const int MATCH_PARENT = -1; ///< Constant which indicates child size should match parent size
static const int WRAP_CONTENT = -2; ///< Constant which indicates parent should take the smallest size possible to wrap it's children with their desired size

} // namespace ChildLayoutData

} // namespace Toolkit
} // namespace Dali

#endif // DALI_TOOLKIT_LAYOUTING_CHILD_LAYOUT_DATA_H
