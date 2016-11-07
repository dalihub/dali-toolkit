#ifndef DALI_TOOLKIT_VISUAL_FACTORY_IMPL_H
#define DALI_TOOLKIT_VISUAL_FACTORY_IMPL_H

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

// EXTERNAL INCLUDES
#include <dali/public-api/object/base-object.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>
#include <dali-toolkit/devel-api/visual-factory/visual-base.h>
#include <dali-toolkit/internal/visuals/visual-base-impl.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

class VisualFactoryCache;
typedef IntrusivePtr<VisualFactoryCache> VisualFactoryCachePtr;

/**
 * @copydoc Toolkit::VisualFactory
 */
class VisualFactory : public BaseObject
{
public:

  /**
   * @brief Constructor
   *
   * @param[in] debugEnabled If true, use debug renderer to replace all the concrete renderer.
   */
  VisualFactory( bool debugEnabled );

  /**
   * @copydoc Toolkit::RenderFactory::CreateVisual( const Property::Map& )
   */
  Toolkit::Visual::Base CreateVisual( const Property::Map& propertyMap );

  /**
   * @copydoc Toolkit::RenderFactory::CreateVisual( const Image& )
   */
  Toolkit::Visual::Base CreateVisual( const Image& image );

  /**
   * @copydoc Toolkit::RenderFactory::CreateVisual( const std::string&, ImageDimensions )
   */
  Toolkit::Visual::Base CreateVisual( const std::string& image, ImageDimensions size );

protected:

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~VisualFactory();

private:

  /**
   * Undefined copy constructor.
   */
  VisualFactory(const VisualFactory&);

  /**
   * Undefined assignment operator.
   */
  VisualFactory& operator=(const VisualFactory& rhs);

private:

  VisualFactoryCachePtr   mFactoryCache;
  bool                    mDebugEnabled;
};

/**
 * @brief Template to allow discard old visual, get new one and set it on stage if possible
 *
 * @tparam ParameterType0 The type of first argument passed to the CreateVisual()
 * @tparam ParameterType1 The type of second argument passed to the CreateVisual()
 * @SINCE_1_0.39
 * @param[in] actor Actor for which the visual will be replaced
 * @param[in,out] visual The visual to be replaced
 * @param[in] param0 First template based argument passed to the visual factory
 * @param[in] param1 Second template based argument passed to the visual factory
 */
template< class ParameterType0, class ParameterType1 >
void InitializeVisual( Actor& actor, Toolkit::Visual::Base& visual, ParameterType0& param0, ParameterType1& param1 )
{
  visual.RemoveAndReset( actor );
  visual = Toolkit::VisualFactory::Get().CreateVisual( param0, param1 );
  if( visual && actor && actor.OnStage() )
  {
    Toolkit::GetImplementation(visual).SetOnStage(actor);
  }
}

/**
 * @brief Template to allow discard old visual, get new one and set it on stage if possible
 *
 * @tparam ParameterType The type of argument passed to the CreateVisual()
 * @SINCE_1_0.39
 * @param[in] actor Actor for which the visual will be replaced
 * @param[in,out] visual The visual to be replaced
 * @param[in] param Template based argument passed to the visual factory
 */
template< class ParameterType >
void InitializeVisual( Actor& actor, Toolkit::Visual::Base& visual, ParameterType& param )
{
  visual.RemoveAndReset( actor );
  visual =  Toolkit::VisualFactory::Get().CreateVisual( param );
  if( visual && actor && actor.OnStage() )
  {
    Toolkit::GetImplementation(visual).SetOnStage(actor);
  }
}

} // namespace Internal

inline const Internal::VisualFactory& GetImplementation(const Toolkit::VisualFactory& factory)
{
  DALI_ASSERT_ALWAYS( factory && "VisualFactory handle is empty" );

  const BaseObject& handle = factory.GetBaseObject();

  return static_cast<const Internal::VisualFactory&>(handle);
}

inline Internal::VisualFactory& GetImplementation(Toolkit::VisualFactory& factory)
{
  DALI_ASSERT_ALWAYS( factory && "VisualFactory handle is empty" );

  BaseObject& handle = factory.GetBaseObject();

  return static_cast<Internal::VisualFactory&>(handle);
}

} // namespace Toolkit

} // namespace Dali

#endif /* DALI_TOOLKIT_VISUAL_FACTORY_IMPL_H */
