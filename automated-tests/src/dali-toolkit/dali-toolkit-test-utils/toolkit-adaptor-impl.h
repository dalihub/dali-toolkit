#ifndef __DALI_TOOLKIT_ADAPTOR_IMPL_H__
#define __DALI_TOOLKIT_ADAPTOR_IMPL_H__

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

#include <dali/integration-api/adaptors/render-surface-interface.h>

namespace Dali
{
class EglInterface;
class DisplayConnection;
class ThreadSynchronizationInterface;

namespace Internal
{

namespace Adaptor
{

class GraphicsInterface;

} // namespace Adaptor

} // namespace Internal

namespace Integration
{

class GlAbstraction;

} // namespace Integration

namespace Internal
{
namespace Adaptor
{

class Adaptor: public BaseObject
{
public:
  static Dali::Adaptor& Get();
  Adaptor();
  ~Adaptor();

public:
  static Dali::RenderSurfaceInterface& GetSurface();
  static Dali::Adaptor::AdaptorSignalType& AdaptorSignal();
  static bool mAvailable;
  static Vector<CallbackBase*> mCallbacks;
};

} // namespace Adaptor
} // namespace Internal
} // namespace Dali

#endif // __DALI_TOOLKIT_ADAPTOR_IMPL_H__
