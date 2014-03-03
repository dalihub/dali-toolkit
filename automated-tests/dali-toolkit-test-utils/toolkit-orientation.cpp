//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include "toolkit-orientation.h"

#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/signals/dali-signal-v2.h>

namespace Dali
{

namespace
{
ToolkitOrientation* gToolkitOrientation(NULL);
} // unnamed namespace

namespace Internal
{

namespace Adaptor
{

/**
 * Stub for the Orientation
 */
class Orientation : public BaseObject
{
public: // Creation & Destruction

  Orientation();
  Orientation(ToolkitOrientation *orientation);
  ~Orientation();

public: // Setters & Getters

  void SetDegrees( int degrees )
  {
    mOrientation = degrees;
  }

  int GetDegrees() const;
  float GetRadians() const;

public: // Signals

  Dali::Orientation::OrientationSignalV2& ChangedSignal();

  void EmitChangedSignal()
  {
    mChangedSignal.Emit(Dali::Orientation(this));
  }

private:

  Dali::Orientation::OrientationSignalV2 mChangedSignal;

  ToolkitOrientation* mToolkitOrientation;

  int mOrientation;
};

Orientation::Orientation()
: mToolkitOrientation(NULL),
  mOrientation(0)
{
}

Orientation::Orientation(ToolkitOrientation *orientation)
: mToolkitOrientation(orientation),
  mOrientation(0)
{
}

Orientation::~Orientation()
{
}

int Orientation::GetDegrees() const
{
  mToolkitOrientation->mFunctionsCalled.GetDegrees = true;
  return mOrientation;
}

float Orientation::GetRadians() const
{
  mToolkitOrientation->mFunctionsCalled.GetRadians = true;
  return Math::PI * (float)mOrientation / 180.0f;
}

Dali::Orientation::OrientationSignalV2& Orientation::ChangedSignal()
{
  mToolkitOrientation->mFunctionsCalled.ChangedSignal = true;
  return mChangedSignal;
}

} // namespace Adaptor

} // namespace Internal

////////////////////////////////////////////////////////////////////////////////////////////////////

ToolkitOrientation::ToolkitOrientation()
: mOrientationStub(new Internal::Adaptor::Orientation(this)),
  mOrientation( mOrientationStub )
{
  gToolkitOrientation = this;
}

ToolkitOrientation::~ToolkitOrientation()
{
  gToolkitOrientation = NULL;
}

Orientation ToolkitOrientation::GetHandle()
{
  return mOrientation;
}

void ToolkitOrientation::SetDegrees( int degrees )
{
  mOrientationStub->SetDegrees( degrees );
}

void ToolkitOrientation::EmitChangedSignal()
{
  mOrientationStub->EmitChangedSignal();
}

} // namespace Dali
