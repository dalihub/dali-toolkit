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

#include "toolkit-application.h"

#include <dali-toolkit/public-api/dali-toolkit-common.h>
#include <dali/devel-api/adaptor-framework/orientation.h>
#include <dali/public-api/signals/dali-signal.h>

namespace Dali
{
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Stub for the Application
 */
class Application
{
public:
public:
  Application(ToolkitApplication& toolkitApplication);
  ~Application();

public:
  static std::string GetResourcePath();

  //Orientation& GetOrientation();

public: // static methods
public: // Signals
private:
  // Undefined
  Application(const Application&);
  Application& operator=(Application&);

  ToolkitApplication& mToolkitApplication;

  //Dali::Orientation* mOrientation;
};

namespace
{
Application* gApplication = NULL;
}

Application::Application(ToolkitApplication& toolkitApplication)
: mToolkitApplication(toolkitApplication)
//  ,mOrientation( new Dali::Orientation() )
{
}

Application::~Application()
{
  //delete mOrientation;
}

std::string Application::GetResourcePath()
{
  return "";
}
//Orientation& Application::GetOrientation()
//{
//  return *mOrientation;
//}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool ToolkitApplication::DECODED_IMAGES_SUPPORTED;
bool ToolkitApplication::ADD_IDLE_SUCCESS = true;

ToolkitApplication::ToolkitApplication()
: mApplicationStub(new Application(*this))
{
  gApplication = mApplicationStub;
}

ToolkitApplication::~ToolkitApplication()
{
  delete mApplicationStub;
  gApplication = NULL;
}

Application& ToolkitApplication::GetApplication()
{
  DALI_ASSERT_ALWAYS(gApplication);
  return *gApplication;
}

} // namespace Dali
