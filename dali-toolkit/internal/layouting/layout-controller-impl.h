#ifndef DALI_TOOLKIT_INTERNAL_LAYOUT_CONTROLLER_H
#define DALI_TOOLKIT_INTERNAL_LAYOUT_CONTROLLER_H

/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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
#include <list>
#include <dali/public-api/object/base-object.h>
#include <dali/integration-api/processor-interface.h>
#include <dali-toolkit/public-api/controls/control.h>
#include <dali-toolkit/devel-api/layouting/layout-controller.h>
#include <dali-toolkit/devel-api/layouting/layout-item-impl.h>
#include <dali-toolkit/devel-api/layouting/layout-group-impl.h>
#include <dali-toolkit/internal/layouting/layout-transition-data-impl.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{

/**
 * @brief Layout controller handles measurement and layout of all
 * controls that utilize Layouts.
 */
class LayoutController : public Dali::BaseObject, public Integration::Processor
{
public:
  /**
   * Constructor
   */
  LayoutController();

  /**
   * Destructor
   */
  ~LayoutController();

  /**
   * Second stage initialization
   */
  void Initialize();

  /**
   * This marks the given layout and all its parents as dirty and triggers a transition if set.
   */
  void RequestLayout( LayoutItem& layout, int layoutTransitionType, Actor gainedChild, Actor lostChild );

  /**
   * Measures next level of layouts in the actor hierarchy.
   */
  void MeasureHierarchy( Actor root, MeasureSpec widthSpec, MeasureSpec heightSpec );

  /**
   * Perform layout of the hierarchy
   */
  void PerformLayout( Actor root, int left, int top, int right, int bottom );

  /**
   * Perform positioning of actors after layout update
   */
  void PerformLayoutPositioning( LayoutPositionDataArray& layoutPositionDataArray, bool all ) const;

  /**
   * Perform animation of actors properties after layout update
   */
  void PerformLayoutAnimation( LayoutTransition& layoutTransition, LayoutPositionDataArray& layoutPositionDataArray, LayoutDataArray& layoutDataArray, LayoutAnimatorArray& layoutAnimatorArray );

  /**
   * Focus change callback.
   */
  void KeyInputFocusChangedCallback( Control gainingActor, Control lostActor );

protected: // Implementation of Processor

  /**
   * @copydoc Dali::Integration::Processor::Process()
   */
  virtual void Process();

private:
  struct ActorSizeSpec
  {
    ActorSizeSpec(Actor actor)
    : actor( actor )
    , widthSpec( actor.GetProperty<int>( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION ) )
    , heightSpec( actor.GetProperty<int>( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION ) )
    {
    }

    WeakHandle<Actor> actor;
    int widthSpec;
    int heightSpec;
  };
  std::vector<ActorSizeSpec> mActorSizeSpecs;

  void UpdateMeasureHierarchyForAnimation( LayoutData& layoutData );

  void UpdateMeasureHierarchyForAnimation( Actor root, LayoutData& layoutData );

  void RestoreActorsSpecs();

  std::list< LayoutTransition > mLayoutTransitions;
  struct AnimationFinishedFunctor
  {
    AnimationFinishedFunctor( LayoutController& layoutController, LayoutTransition& layoutTransition, LayoutPositionDataArray& array )
    : layoutController( layoutController ),
      layoutDataPositionArray(),
      layoutItem( layoutTransition.layoutItem ),
      layoutTransitionType( layoutTransition.layoutTransitionType )
    {
      layoutDataPositionArray.swap( array );
    }

    void operator()( Animation& animation )
    {
      layoutController.PerformLayoutPositioning( layoutDataPositionArray, true );
      layoutController.mAnimationFinishedFunctors.pop_front();
      if( layoutTransitionType != -1 )
      {
        LayoutTransitionDataPtr layoutTransitionDataPtr = layoutItem->GetTransitionData( layoutTransitionType );
        layoutTransitionDataPtr->EmitSignalFinish( layoutTransitionType );
      }
    }

    LayoutController& layoutController;
    LayoutPositionDataArray layoutDataPositionArray;
    LayoutItemPtr layoutItem;
    int layoutTransitionType;
  };

  bool mLayoutRequested;
  Animation mAnimation;
  std::list< AnimationFinishedFunctor > mAnimationFinishedFunctors;

  struct FocusChangedFunctor
  {
    FocusChangedFunctor( LayoutController& layoutController )
    : layoutController( layoutController )
    {
    }

    void operator() ( Dali::Toolkit::Control gainingControl, Dali::Toolkit::Control lostActor );
    LayoutController& layoutController;
  };
  FocusChangedFunctor mFocusChangedFunctor;

  SlotDelegate<LayoutController> mSlotDelegate;
};

} // namespace Internal

inline Internal::LayoutController& GetImpl( Dali::Toolkit::LayoutController& handle )
{
  DALI_ASSERT_ALWAYS(handle);
  Dali::BaseObject& object = handle.GetBaseObject();
  return static_cast<Internal::LayoutController&>(object);
}

inline const Internal::LayoutController& GetImpl( const Dali::Toolkit::LayoutController& handle )
{
  DALI_ASSERT_ALWAYS(handle);
  const Dali::BaseObject& object = handle.GetBaseObject();
  return static_cast<const Internal::LayoutController&>(object);
}

} // namespace Toolkit
} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_LAYOUT_CONTROLLER_H
