/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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

#include <dali/devel-api/text-abstraction/icu.h>
#include <dali/devel-api/text-abstraction/text-abstraction-definitions.h>
#include <dali/public-api/dali-adaptor-common.h>
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/object/base-object.h>
#include <toolkit-application.h>
#include <toolkit-icu.h>
#include <dali-toolkit-test-suite-utils.h>

namespace Dali
{
namespace TextAbstraction
{
namespace Internal
{
namespace
{
Dali::Vector<Dali::TextAbstraction::Internal::ICU*> gICUs;
uint32_t                                            gInitializedCount = 0u;
}

class ICU: public Dali::BaseObject
{
public:

ICU()
: mInitialized(false)
{
}

~ICU()
{
}

void Initialize()
{
  // Try once.
  mInitialized = true;
  tet_printf("ICU::Initialize:%d\n", mInitialized);
}

void UpdateLineBreakInfoByLocale(const std::string& text,
                                 Length             numberOfCharacters,
                                 const char*        locale,
                                 LineBreakInfo*     breakInfo)
{
  tet_printf("ICU::UpdateLineBreakInfoByLocale\n");
  if(!mInitialized)
  {
    Initialize();
  }
}

// Test method
void ResetInitialize()
{
  mInitialized = false;
}

// Test method
bool IsInitialized()
{
  return mInitialized;
}

private:
  bool mInitialized : 1; ///< Whether Initialize() has been called, prevents dlopen from being attempted again.
};

inline ICU& GetImplementation(TextAbstraction::ICU& icu)
{
  DALI_ASSERT_ALWAYS(icu && "icu handle is empty");
  BaseObject& handle = icu.GetBaseObject();
  return static_cast<Internal::ICU&>(handle);
}

inline const ICU& GetImplementation(const TextAbstraction::ICU& icu)
{
  DALI_ASSERT_ALWAYS(icu && "icu handle is empty");
  const BaseObject& handle = icu.GetBaseObject();
  return static_cast<const Internal::ICU&>(handle);
}

} // namespace Internal

/********************************************************************************/
/*********************************  PUBLIC CLASS  *******************************/
/********************************************************************************/

ICU::ICU()
{
}

ICU::~ICU()
{
}

ICU::ICU(Internal::ICU* impl)
: BaseHandle(impl)
{
}

ICU ICU::New()
{
  auto icuImpl = new Internal::ICU();
  Dali::TextAbstraction::Internal::gICUs.PushBack(icuImpl);
  return ICU(icuImpl);
}

void ICU::UpdateLineBreakInfoByLocale(const std::string& text,
                                      Length             numberOfCharacters,
                                      const char*        locale,
                                      LineBreakInfo*     breakInfo)
{
  Internal::GetImplementation(*this).UpdateLineBreakInfoByLocale(text, numberOfCharacters, locale, breakInfo);
}

} // namespace TextAbstraction
} // namespace Dali;

namespace Test
{
namespace ICU
{
void ResetInitializeFlag()
{
  for(auto& icu : Dali::TextAbstraction::Internal::gICUs)
  {
    icu->ResetInitialize();
  }
  Dali::TextAbstraction::Internal::gInitializedCount = 0u;
}

uint32_t GetInitializedCount()
{
  Dali::TextAbstraction::Internal::gInitializedCount = 0u;
  for(auto& icu : Dali::TextAbstraction::Internal::gICUs)
  {
    if(icu->IsInitialized())
    {
      Dali::TextAbstraction::Internal::gInitializedCount++;
    }
  }
  return Dali::TextAbstraction::Internal::gInitializedCount;
}

} // namespace ICU
} // namespace Test
