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

#include "style-change-processor.h"

#include <boost/thread/tss.hpp>

#include "dali-toolkit/public-api/controls/control.h"
#include "dali-toolkit/public-api/controls/control-impl.h"

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{
boost::thread_specific_ptr<StyleChangeProcessor> gThreadLocalStyleChangeProcessor;
} // unnamed namespace

StyleChangeProcessor::~StyleChangeProcessor()
{
}

void StyleChangeProcessor::Register( Control* control )
{
  // Only create a style change processor if we have not created one in the local thread storage.
  if (!gThreadLocalStyleChangeProcessor.get())
  {
    gThreadLocalStyleChangeProcessor.reset(new StyleChangeProcessor);
  }

  gThreadLocalStyleChangeProcessor->Reference();

  std::vector<Control*>& controls( gThreadLocalStyleChangeProcessor->mControls );

  // Store the Control raw pointer to allow traverse all off stage controls.
  DALI_ASSERT_ALWAYS( ( std::find( controls.begin(), controls.end(), control ) == controls.end() ) && "StyleChangeProcessor::Register. The control has been registered twice." );

  controls.push_back( control );
}

void StyleChangeProcessor::Unregister( Control* control )
{
  if (gThreadLocalStyleChangeProcessor.get())
  {
    std::vector<Control*>& controls( gThreadLocalStyleChangeProcessor->mControls );

    // Removes the control from the vector as is not needed to notify it about style changes.
    std::vector<Control*>::iterator it = std::find( controls.begin(), controls.end(), control );
    std::vector<Control*>::iterator endIt = controls.end();
    DALI_ASSERT_ALWAYS( ( it != endIt ) && "StyleChangeProcessor::UnRegister. The control has not been registered in the StyleChangeProcessor." );

    *it = *(endIt - 1);
    controls.erase( endIt - 1 );

    gThreadLocalStyleChangeProcessor->Unreference();
  }
}

void StyleChangeProcessor::Reference()
{
  ++mCount;
}

void StyleChangeProcessor::Unreference()
{
  if (--mCount == 0)
  {
    // If our count is 0, then we should reset the local storage which will call our destructor as well.
    gThreadLocalStyleChangeProcessor.reset();
  }
}

StyleChangeProcessor::StyleChangeProcessor()
: mCount(0)
{
  if ( Adaptor::IsAvailable() )
  {
    StyleMonitor::Get().StyleChangeSignal().Connect(this, &StyleChangeProcessor::StyleChanged);
  }
}

void StyleChangeProcessor::StyleChanged(Dali::StyleMonitor styleMonitor, StyleChange styleChange)
{
  // Traverse all registered controls.
  std::vector<Control*>& controls( gThreadLocalStyleChangeProcessor->mControls );

  for( std::vector<Control*>::iterator it = controls.begin(), endIt = controls.end(); it != endIt; ++it )
  {
    // Create a valid handle.
    IntrusivePtr<Control> implementation( *it );

    if (implementation)
    {
      implementation->OnStyleChange( styleChange );
    }
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
