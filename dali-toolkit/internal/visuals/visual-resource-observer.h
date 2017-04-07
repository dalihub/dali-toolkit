#ifndef DALI_INTERNAL_TOOLKIT_VISUAL_RESOURCE_OBSERVER_H
#define DALI_INTERNAL_TOOLKIT_VISUAL_RESOURCE_OBSERVER_H

/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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



namespace Dali
{
namespace Toolkit
{
namespace Internal
{
namespace Visual
{

class Base;


/**
 * Observer to be informed when a visuals resources are ready.
 */
class ResourceObserver
{
  public:

    /**
     * Inform the observer of the object that it's connections have changed
     * @param[in] object The connection owner
     */
    virtual void ResourceReady( Visual::Base& object) = 0;

  protected:

    /**
     * constructor
     */
    ResourceObserver()
    {
    };

    /**
     * virtual destructor
     */
    virtual ~ResourceObserver()
    {
    };

    // Undefined copy constructor.
    ResourceObserver( const ResourceObserver& );

    // Undefined assignment operator.
    ResourceObserver& operator=( const ResourceObserver& );
};
} // Visual
} // Internal
} // Toolkit
} // Dali

#endif // DALI_INTERNAL_TOOLKIT_VISUAL_RESOURCE_OBSERVER_H
