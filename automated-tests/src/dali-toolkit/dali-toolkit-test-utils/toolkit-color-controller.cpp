/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

#include <dali/devel-api/adaptor-framework/color-controller.h>

#include <dali/public-api/object/base-object.h>

using namespace Dali;

namespace Dali
{
namespace Internal
{
namespace Adaptor
{

class ColorController : public BaseObject
{
public:
  ColorController(){}
  static Dali::ColorController Get()
  {
    return Dali::ColorController( new ColorController() );
  }

  bool RetrieveColor( const std::string& colorCode, Vector4& colorValue )
  {
    return false;
  }

  bool RetrieveColor( const std::string& colorCode ,
                      Vector4&           textColor,
                      Vector4&           textOutlineColor,
                      Vector4&           textShadowColor)
  {
    return false;
  }
protected:
  virtual ~ColorController(){}
};

} // Adaptor
} // Internal

inline Internal::Adaptor::ColorController& GetImplementation(Dali::ColorController& controller)
{
  DALI_ASSERT_ALWAYS(controller && "ColorController handle is empty");
  BaseObject& handle = controller.GetBaseObject();
  return static_cast<Internal::Adaptor::ColorController&>(handle);
}

inline const Internal::Adaptor::ColorController& GetImplementation(const Dali::ColorController& controller)
{
  DALI_ASSERT_ALWAYS(controller && "ColorController handle is empty");
  const BaseObject& handle = controller.GetBaseObject();
  return static_cast<const Internal::Adaptor::ColorController&>(handle);
}

ColorController ColorController::Get()
{
  return Internal::Adaptor::ColorController::Get();
}

ColorController::ColorController(Internal::Adaptor::ColorController* internal)
: BaseHandle(internal)
{
}

ColorController::~ColorController()
{
}

bool ColorController::RetrieveColor( const std::string& colorCode, Vector4& colorValue )
{
  return GetImplementation(*this).RetrieveColor( colorCode, colorValue );
}

} // Dali
