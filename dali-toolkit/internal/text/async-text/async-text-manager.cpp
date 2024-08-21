/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/text/async-text/async-text-manager.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/async-text/async-text-manager-impl.h>


namespace Dali
{
namespace Toolkit
{
namespace Text
{
AsyncTextManager::AsyncTextManager()
{
}

AsyncTextManager::~AsyncTextManager()
{
}

AsyncTextManager::AsyncTextManager(Internal::AsyncTextManager* implementation)
: BaseHandle(implementation)
{
}

AsyncTextManager AsyncTextManager::Get()
{
  return Internal::AsyncTextManager::Get();
}

uint32_t AsyncTextManager::RequestLoad(AsyncTextParameters& parameters, TextLoadObserver* observer)
{
  return GetImplementation(*this).RequestLoad(parameters, observer);
}

void AsyncTextManager::RequestCancel(uint32_t taskId)
{
  GetImplementation(*this).RequestCancel(taskId);
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
