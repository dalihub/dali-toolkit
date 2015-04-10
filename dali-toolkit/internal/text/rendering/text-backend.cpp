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

// CLASS HEADER
#include <dali-toolkit/internal/text/rendering/text-backend.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/rendering/text-backend-impl.h>

namespace Dali
{

namespace Toolkit
{

namespace Text
{

Backend Backend::Get()
{
  return Internal::Backend::Get();
}

RendererPtr Backend::NewRenderer( unsigned int renderingType )
{
  return GetImplementation(*this).NewRenderer( renderingType );
}

Backend::Backend()
{
}

Backend::~Backend()
{
}

Backend::Backend( const Backend& handle )
: BaseHandle( handle )
{
}

Backend& Backend::operator=( const Backend& handle )
{
  BaseHandle::operator=( handle );
  return *this;
}

Backend::Backend( Internal::Backend* internal )
: BaseHandle( internal )
{
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
