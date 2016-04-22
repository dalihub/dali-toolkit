#ifndef __DALI_TOOLKIT_INTERNAL_PAGE_TURN_VIEW_IMPL_H__
#define __DALI_TOOLKIT_INTERNAL_PAGE_TURN_VIEW_IMPL_H__

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

// EXTERNAL INCLUDES
#include <dali/devel-api/common/map-wrapper.h>
#include <dali/public-api/actors/layer.h>
#include <dali/devel-api/rendering/renderer.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/public-api/controls/page-turn-view/page-turn-view.h>
#include <dali-toolkit/public-api/controls/page-turn-view/page-factory.h>
#include <dali-toolkit/devel-api/controls/shadow-view/shadow-view.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

class PageTurnView : public Control
{
protected:

  /**
   * The book page class
   */
  struct Page
  {
    /**
     * Constructor
     */
    Page();
    /**
     * Destructor
     */
    ~Page(){};

    /**
     * Set the page image content
     * @param[in] image The content of the page.
     */
    void SetImage( Image image );

    /**
     * Apply an effect onto the page actor.
     * @param[in] newShader The shader for rendering effect.
     */
    void UseEffect(Shader newShader);

    /**
     * Apply an effect onto the page actor.
     * @param[in] newShader The shader for rendering effect.
     * @param[in] geometry The geometry for rendering effect.
     */
    void UseEffect(Shader newShader, Geometry geometry);

    /**
     * Change the page turning direction.
     */
    void ChangeTurnDirection();

    /**
     * Set the pan displacement property
     * @param[in] value The property value
     */
    void SetPanDisplacement(float value);

    /**
     * Set the pan center property
     * @param[in] value The property value
     */
    void SetPanCenter( const Vector2& value );

    /**
     * Set the original center property to be used by shader
     * @param[in] value The property value
     */
    void SetOriginalCenter( const Vector2& value );

    /**
     * Set the current center property to be used by shader
     * @param[in] value The property value
     */
    void SetCurrentCenter( const Vector2& value );

    Actor actor;                              ///< The page actor
    Shader shader;                            ///< The shader used by the actor
    TextureSet textureSet;                    ///< The set of textures used by the actor
    Renderer renderer;                        ///< The renderer of the actor
    bool isTurnBack;                          ///< The turning direction
    Property::Index propertyPanDisplacement;  ///< The horizontal displacement of the pan
    Property::Index propertyPanCenter;        ///< The current pan position
    Property::Index propertyOriginalCenter;   ///< The original center to be used by the shader
    Property::Index propertyCurrentCenter;    ///< The current center to be used by the shader
    Property::Index propertyTurnDirection;    ///< The turning direction property
  };


protected:

  /**
   * Constructor.
   * It initializes the PageTurnView members
   */
  PageTurnView( PageFactory& pageFactory, const Vector2& pageSize );

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~PageTurnView();

public:

  /**
   * Set the page size
   * @param[in] pageSize The size of pages
   */
  void SetPageSize( const Vector2& pageSize );

  /**
   * Retrieve the page size.
   * @return The page size.
   */
  Vector2 GetPageSize();

  /**
   * Set the spine shadow parameter to the shader effects.
   * The two parameters are the major&minor radius (in pixels) to form an ellipse shape.
   * The top-left quarter of this ellipse is used to calculate spine normal for simulating shadow.
   * @param [in] spineShadowParameter The major&minor ellipse radius for the simulated spine shadow.
   */
  void SetSpineShadowParameter( const Vector2& spineShadowParameter );

  /**
   * Retrieve the spine shadow parameter of the shader effects.
   * @return The spine shadow parameter.
   */
  Vector2 GetSpineShadowParameter();

  /*
   * Jump to a given page.
   * @param[in] pageId The new current page id.
   */
  void GoToPage( unsigned int pageId );

  /**
   * Retrieve the id of the current Page.
   * @return The current page id.
   */
  unsigned int GetCurrentPage();

protected:

  /**
   * This method gets a page from the factory and add to the control
   * to keep NUMBER_OF_CACHED_PAGES_EACH_SIDE pages available in each side
   * @param[in] pageIndex The index of the page to be added
   */
  void AddPage( int pageIndex );

  /**
   * This method removes a page from the control
   * to keep only NUMBER_OF_CACHED_PAGES_EACH_SIDE pages available in each side
   * @param[in] pageIndex The index of the page to be removed
   */
  void RemovePage( int pageIndex );

  /**
   * This method updates the actor and animation states after one page is turned over
   * This method is a callback, connected when receiving the finished signal of a page turning over animation.
   * @param [in] the page turning over animation handle
   */
  void TurnedOver( Animation& animation );

  /**
   * This method organize the depth of the pages on stage
   * It is called when there is page added or removed from the control
   */
  void OrganizePageDepth();

private:

  /**
   * Create shader from a property map.
   * @param[in] shaderMap The shader property map;
   * @return The created shader.
   */
  Shader CreateShader( const Property::Map& shaderMap );

 /**
  * Set up the shadow view control to cast shadow
  */
  void SetupShadowView();

  /**
   * This method defines the processes when the pan started, gets called by OnPan( .. )
   * @param[in] gesturePosition The current touch position in local page actor coordinates.
   */
  void PanStarted( const Vector2& gesturePosition );

  /**
   * This method defines the processes when the pan continuing, gets called by OnPan( .. )
   * @param[in] gesturePosition The current touch position in local page actor coordinates.
   */
  void PanContinuing( const Vector2& gesturePosition );

  /**
   * This method defines the processes when the pan finished, gets called by OnPanGesture( .. )
   * @param[in] gesturePosition The current touch position in local page actor coordinates.
   * @param[in] gestureSpeed The speed of the pan ( in pixels per millisecond )
   */
  void PanFinished( const Vector2& gesturePosition, float gestureSpeed );

  /**
   * This method updates the actor and the animation states after one page is slidden back instead of turned over
   * This method is a callback, connected when receiving the finished signal of a page sliding back animation.
   * @param [in] the page sliding back animation handle
   */
  void SliddenBack( Animation& animation );

  /**
   * Stop the page turning animation and contraint.
   * This method should be called when taking off stage or jump to a specified page.
   */
  void StopTurning();

private: // from Control

  /**
   * @copydoc Toolkit::Control::OnPan
   */
  virtual void OnPan( const PanGesture& gesture );

  /**
   * @copydoc Toolkit::Control::OnInitialize
   */
  virtual void OnInitialize();

  /**
   * @copydoc CustomActorImpl::OnStageConnection()
   */
  virtual void OnStageConnection( int depth );

  /**
   * @copydoc CustomActorImpl::OnStageDisconnection()
   */
  virtual void OnStageDisconnection();

private: // implemented differently by PageTurnLandscapeView and PageTurnPortraitView

  /**
   * This method is called after the pageTurnView initialization.
   * To set the size of the control size and the parent origin of turning page layer
   * Implemented in subclasses to provide specific behaviour.
   */
  virtual void OnPageTurnViewInitialize() = 0;

  /**
   * This method is called after the a new page is added to the stage.
   * Could be re-implemented in subclasses to provide specific behaviour
   * @param[in] newPage The added page actor
   * @param[in] isLeftSide Which side the new page is added to
   */
  virtual void OnAddPage( Actor newPage, bool isLeftSide ) { }

  /**
   * This method is called when pan started or continuing
   * to calculate the pan position in local page actor coordinate given the pan position in control coordinate
   * Implemented in subclasses to provide specific behaviour.
   * @param[in] gesturePosition The pan position in the control coordinate
   * @return The pan position in the page actor local coordinate
   */
  virtual Vector2 SetPanPosition( const Vector2& gesturePosition ) = 0;

  /**
   * This method is called when pan started to determined which page is panned given the pan position in control coordinate
   * Implemented in subclasses to provide specific behaviour.
   * @param[in] gesturePosition The pan position in the control coordinate
   */
  virtual void SetPanActor( const Vector2& panPosition ) = 0;

  /**
   * This method is called when pan finished to detect outwards flick
   * In portrait view, start an animation of turning previous page back when outwards flick is detected
   * In landscape view, nothing to do
   * @param[in] panPosition The pan position in the page actor local coordinate
   * @param[in] gestureSpeed The speed of the pan gesture( in pixels per millisecond )
   */
  virtual void OnPossibleOutwardsFlick( const Vector2& panPosition, float gestureSpeed ) { }

  /**
   * This method is called when page is turned over
   * In portrait view, the page on the left side is not rendered
   * @param[in] actor The page actor
   * @param[in] isLeftSide Which side the page is turned to
   */
  virtual void OnTurnedOver( Actor actor, bool isLeftSide ) { }

public: //signal and property

  /**
   * @copydoc Toolkit::PageTurnView::PageTurnStartedSignal()
   */
  Toolkit::PageTurnView::PageTurnSignal& PageTurnStartedSignal();

  /**
   * @copydoc Toolkit::PageTurnView::PageTurnFinishedSignal()
   */
  Toolkit::PageTurnView::PageTurnSignal& PageTurnFinishedSignal();

  /**
   * @copydoc Toolkit::PageTurnView::PagePanStartSignal()
   */
  Toolkit::PageTurnView::PagePanSignal& PagePanStartedSignal();

  /**
   * @copydoc Toolkit::PageTurnView::PagePanFinishedSignal()
   */
  Toolkit::PageTurnView::PagePanSignal& PagePanFinishedSignal();

  /**
    * Connects a callback function with the object's signals.
    * @param[in] object The object providing the signal.
    * @param[in] tracker Used to disconnect the signal.
    * @param[in] signalName The signal to connect to.
    * @param[in] functor A newly allocated FunctorDelegate.
    * @return True if the signal was connected.
    * @post If a signal was connected, ownership of functor was passed to CallbackBase. Otherwise the caller is responsible for deleting the unused functor.
    */
  static bool DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor );

   // Properties

  /**
   * Called when a property of an object of this type is set.
   * @param[in] object The object whose property is set.
   * @param[in] index The property index.
   * @param[in] value The new property value.
   */
  static void SetProperty( BaseObject* object, Property::Index index, const Property::Value& value );

  /**
   * Called to retrieve a property of an object of this type.
   * @param[in] object The object whose property is to be retrieved.
   * @param[in] index The property index.
   * @return The current value of the property.
   */
  static Property::Value GetProperty( BaseObject* object, Property::Index index );

private:

  //Undefined
  PageTurnView( const PageTurnView& );

  //undefined
  PageTurnView& operator=(const PageTurnView& rhs);

protected:

  Layer                          mTurningPageLayer;        ///< The layer for the turning page, to avoid possible depth conflict
  Toolkit::ShadowView            mShadowView;              ///< The shadow view control for shadow casting
  Actor                          mShadowPlaneBackground;   ///< The plane for the shadow to cast on
  Actor                          mPointLight;              ///< The point light used for shadow casting

  PageFactory* const             mPageFactory;             ///< The factory which provides the page actors
  Shader                         mTurnEffectShader;        ///< The group of PageTurnEffects
  Shader                         mSpineEffectShader;       ///< The book spine shader effect
  Geometry                       mGeometry;                ///< The grid geometry for pages

  std::vector<Page>              mPages;                   ///< The vector of pages on stage
  std::map<Animation,int>        mAnimationPageIdPair;     ///< The map to keep track which page actor is the animation act on

  Vector2                        mPageSize;                ///< The page size
  Vector2                        mControlSize;             ///< The size of the control, it is decided by the page size, the SetSize from application can not change it
  Vector2                        mSpineShadowParameter;    ///< The spine shadow parameter for all the above shader effects
  Vector2                        mOriginalCenter;          ///< The original center set to the PageTurnEffect
  Vector2                        mCurrentCenter;           ///< The current center set to the PageTurnEffect
  Vector2                        mPressDownPosition;       ///< The first press down position of the pan gesture

  float                          mDistanceUpCorner;        ///< The distance between the original center of PageTurnEffect and the top-left corner of the page
  float                          mDistanceBottomCorner;    ///< The distance between the original center of PageTurnEffect and the bottom-left corner of the page
  float                          mPanDisplacement;         ///< The displacement of the pan after the constrains are applied

  int                            mTotalPageCount;          ///< The total number of pages provided by the page factory
  int                            mCurrentPageIndex;        ///< The index of the current page, between 0 ~ mTotalPageCount-1
  int                            mTurningPageIndex;        ///< The index of the turning page
  int                            mIndex;                   ///< The index to keep track which PanDisplacementProperty, CurrentCenterProperty is used for the current panning page
  int                            mSlidingCount;            ///< The boolean vector to keep track whether there are animating pages sliding back
  int                            mAnimatingCount;          ///< The boolean vector to keep track which PageTurnEffect, PanDisplacementProperty, CurrentCenterProperty is available for using

  bool                           mConstraints;             ///< The boolean to keep track the constrains are applied or not
  bool                           mPress;                   ///< The boolean to keep track the state of the pageTurnEffect is activated or not
  bool                           mPageUpdated;             ///< The boolean to keep track whether is page is updated after any turning activity

  Toolkit::PageTurnView::PageTurnSignal   mPageTurnStartedSignal;   ///< The signal to notify that a page has started turning
  Toolkit::PageTurnView::PageTurnSignal   mPageTurnFinishedSignal;  ///< The signal to notify that a page has finished turning
  Toolkit::PageTurnView::PagePanSignal    mPagePanStartedSignal;    ///< The signal to notify that a page has started panning
  Toolkit::PageTurnView::PagePanSignal    mPagePanFinishedSignal;   ///< The signal to notify that a page has finished panning

  static const char * const PROPERTY_TEXTURE_WIDTH;     ///< The uniform name of texture width
  static const char * const PROPERTY_ORIGINAL_CENTER;   ///< The property name of original center, which is used to constrain the uniforms
  static const char * const PROPERTY_CURRENT_CENTER;    ///< The property name of current center, which is used to constrain the uniforms

  static const int               MAXIMUM_TURNING_NUM;                  ///< How many pages are allowed to animating in the same time
  static const int               NUMBER_OF_CACHED_PAGES_EACH_SIDE;     ///< The maximum number of pages kept, (MAXIMUM_ANIMATION_NUM+1) pages for each side
  static const int               NUMBER_OF_CACHED_PAGES;               ///< The maximum number of pages kept, (MAXIMUM_ANIMATION_NUM+1)*2 pages in total
  static const float             STATIC_PAGE_INTERVAL_DISTANCE;        ///< The depth interval between stacked pages (static pages)
};

} // namespace Internal


// Helpers for public-api forwarding methods

inline Toolkit::Internal::PageTurnView& GetImplementation(Toolkit::PageTurnView& pub)
{
  DALI_ASSERT_ALWAYS(pub);

  Dali::RefObject& handle = pub.GetImplementation();

  return static_cast<Toolkit::Internal::PageTurnView&>(handle);
}

inline const Toolkit::Internal::PageTurnView& GetImplementation(const Toolkit::PageTurnView& pub)
{
  DALI_ASSERT_ALWAYS(pub);

  const Dali::RefObject& handle = pub.GetImplementation();

  return static_cast<const Toolkit::Internal::PageTurnView&>(handle);
}

} // namespace Toolkit

} // namespace Dali
#endif /* __DALI_TOOLKIT_INTERNAL_PAGE_TURN_VIEW_IMPL_H__ */
