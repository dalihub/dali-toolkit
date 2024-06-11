#ifndef DALI_TOOLKIT_TEST_DUMMY_VISUAL_H
#define DALI_TOOLKIT_TEST_DUMMY_VISUAL_H

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

// INTERNAL INCLUDES
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/visual-factory/visual-base.h>
#include <dali-toolkit/devel-api/visuals/image-visual-properties-devel.h>

#include <dali-toolkit/internal/visuals/visual-base-impl.h>

// EXTERNAL INCLUDES
#include <dali/public-api/common/intrusive-ptr.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
class DummyVisual;

typedef IntrusivePtr<DummyVisual> DummyVisualPtr;

/**
 * Dummy Visual that can be used for testing
 * Cannot create an instance of an existing Visual, so use this dummy class for the implementation.
 */
class DummyVisual : public Visual::Base
{
public:
  // Actions that the dummy visual can perform.  These actions are called through the Visual::Base::DoAction API.
  enum Type
  {
    TEST_ACTION           = 0, ///< Updates the action counter
    TEST_ACTION_EXTENSION = 1, ///< Updates the action counter when we call DoActionExtension
  };

public:
  // Constructor for DummyVisual
  static DummyVisualPtr New(const Property::Map& properties);

  // Prevent default methods being used.
  DummyVisual(const DummyVisual& dummyVisual)  = delete;
  DummyVisual(const DummyVisual&& dummyVisual) = delete;
  DummyVisual& operator=(const DummyVisual& dummyVisual) = delete;
  DummyVisual& operator=(const DummyVisual&& dummyVisual) = delete;

  // Get the Action counter, action counter incremented with every successful Action
  unsigned int GetActionCounter() const;
  // Reset the Action counter to 0;
  void ResetActionCounter();

protected:
  DummyVisual(VisualFactoryCache& factoryCache);

  void OnInitialize() override;
  void DoCreatePropertyMap(Property::Map& map) const override;
  void DoCreateInstancePropertyMap(Property::Map& map) const override;
  void DoSetProperties(const Property::Map& propertyMap) override;
  void OnSetTransform() override;
  void DoSetOnScene(Actor& actor) override;
  void OnDoAction(const Property::Index actionName, const Property::Value& attributes) override;
  void OnDoActionExtension(const Property::Index actionName, const Dali::Any& attributes) override;

private:
  unsigned int mActionCounter;
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEST_DUMMY_VISUAL_H
