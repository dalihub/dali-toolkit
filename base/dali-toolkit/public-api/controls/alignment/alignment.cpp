/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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

#include <dali-toolkit/public-api/controls/alignment/alignment.h>

// EXTERNAL INCLUDES

// INTERNAL INCLUDES

#include <dali/integration-api/debug.h>
#include <dali-toolkit/internal/controls/alignment/alignment-impl.h>

namespace Dali
{

namespace Toolkit
{

Alignment::Alignment()
{
}

Alignment Alignment::New( Type horizontal, Type vertical )
{
  return Internal::Alignment::New( horizontal, vertical );
}

Alignment::Alignment(const Alignment& alignment)
: Control( alignment )
{
}

Alignment::~Alignment()
{
}

Alignment Alignment::DownCast( BaseHandle handle )
{
  return Control::DownCast<Alignment, Internal::Alignment>(handle);
}

void Alignment::SetAlignmentType( Type type )
{
  GetImpl( *this ).SetAlignmentType( type );
}

Alignment::Type Alignment::GetAlignmentType() const
{
  return GetImpl( *this ).GetAlignmentType();
}

void Alignment::SetScaling( Scaling scaling )
{
  GetImpl( *this ).SetScaling( scaling );
}

Alignment::Scaling Alignment::GetScaling() const
{
  return GetImpl( *this ).GetScaling();
}

void Alignment::SetPadding( const Alignment::Padding& padding )
{
  GetImpl( *this ).SetPadding( padding );
}

const Alignment::Padding& Alignment::GetPadding() const
{
  return GetImpl( *this ).GetPadding();
}

Alignment::Alignment( Internal::Alignment& implementation )
: Control( implementation )
{
}

Alignment& Alignment::operator=(const Alignment& alignment)
{
  if( &alignment != this )
  {
    Control::operator=( alignment );
  }
  return *this;
}

Alignment::Alignment( Dali::Internal::CustomActor* internal )
: Control( internal )
{
  VerifyCustomActorPointer<Internal::Alignment>(internal);
}

} // namespace Toolkit

} // namespace Dali
