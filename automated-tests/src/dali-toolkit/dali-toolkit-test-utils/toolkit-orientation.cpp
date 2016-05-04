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

#include <dali/devel-api/adaptor-framework/orientation.h>

#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/signals/dali-signal.h>

using namespace Dali;

namespace Dali
{
class Adaptor;

namespace Internal
{
namespace Adaptor
{
class Window;


struct RotationEvent
{
  int angle;     ///< one of 0, 90, 180, 270
  int winResize; ///< true if the window should be resized
  int width;     ///< new window width
  int height;    ///< new window height
};

/**
 * The RotationObserver can be overridden in order to listen to rotation events.
 */
class RotationObserver
{
public:
  virtual void OnRotationPrepare( const RotationEvent& rotation ) = 0;
  virtual void OnRotationRequest( ) = 0;

protected:
  RotationObserver(){}
  virtual ~RotationObserver(){}
};

class Orientation : public BaseObject, public RotationObserver
{
public:
  typedef Dali::Orientation::OrientationSignalType OrientationSignalType;

  static Orientation* New(Window* window)
  {
    Orientation* orientation = new Orientation(window);
    return orientation;
  }
  Orientation(Window* window)
  {
  }

protected:
  virtual ~Orientation()
  {
  }
public:
  void SetAdaptor(Dali::Adaptor& adaptor)
  {
  }
  int GetDegrees() const
  {
    return 0;
  }
  float GetRadians() const
  {
    return 0.0f;
  }
  OrientationSignalType& ChangedSignal()
  {
    return mChangedSignal;
  }
  virtual void OnRotationPrepare( const RotationEvent& rotation )
  {
  };
  virtual void OnRotationRequest( )
  {
  };

private:
  Orientation(const Orientation&);
  Orientation& operator=(Orientation&);
  OrientationSignalType mChangedSignal;
};

} // Adaptor namespace
} // Internal namespace

inline Internal::Adaptor::Orientation& GetImplementation (Dali::Orientation& orientation)
{
  DALI_ASSERT_ALWAYS(orientation && "Orientation handle is empty");
  BaseObject& handle = orientation.GetBaseObject();
  return static_cast<Internal::Adaptor::Orientation&>(handle);
}
inline const Internal::Adaptor::Orientation& GetImplementation(const Dali::Orientation& orientation)
{
  DALI_ASSERT_ALWAYS(orientation && "Orientation handle is empty");
  const BaseObject& handle = orientation.GetBaseObject();
  return static_cast<const Internal::Adaptor::Orientation&>(handle);
}

Orientation::Orientation()
{
}
Orientation::~Orientation()
{
}
Orientation::Orientation(const Orientation& handle)
: BaseHandle(handle)
{
}

Orientation& Orientation::operator=(const Orientation& rhs)
{
  BaseHandle::operator=(rhs);
  return *this;
}

int Orientation::GetDegrees() const
{
  return GetImplementation(*this).GetDegrees();
}

float Orientation::GetRadians() const
{
  return GetImplementation(*this).GetRadians();
}

Orientation::OrientationSignalType& Orientation::ChangedSignal()
{
  return GetImplementation(*this).ChangedSignal();
}

Orientation::Orientation( Internal::Adaptor::Orientation* orientation )
: BaseHandle(orientation)
{
}

} // Dali
