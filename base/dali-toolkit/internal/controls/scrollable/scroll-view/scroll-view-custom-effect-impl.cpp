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

// CLASS HEADER
#include <dali-toolkit/internal/controls/scrollable/scroll-view/scroll-view-custom-effect-impl.h>

// EXTERNAL INCLUDES
#include <boost/bind.hpp>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/scrollable/scroll-view/scroll-view-helper-functions.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace // unnamed namespace
{

using namespace ScrollViewHelperFunctions;

/**
 * ScrollViewCustomEffectInfo
 *
 * ScrollAmountConstraint calculates the attached actor's current scroll position, -1.0f to 1.0f is from one side of the screen to the other.
 * It also calculates if the other constraints can be skipped
 *
 * Color constraint: adjusts the alpha of the page based on their parent page's position relative
 * to the middle of the screen.
 * When at middle of screen Alpha is 100% opacity.
 * When outside the viewable area, the opacity is 0%.
 *
 * Position constraint: adjusts the position of the page based on their parent page's position
 * relative to the middle of the screen.
 * When at middle of the screen the position is not altered.
 * When one screen away from middle the position is rotated as per expected in a 3D inner cube.
 */
class ScrollViewCustomEffectInfo : public Dali::RefObject
{
public:

  ScrollViewCustomEffectInfo( uint flags,
                              Property::Index scrollAmountProperty,
                              Property::Index anchorProperty,
                              const Vector2& pageSpacing,
                              const Vector3& translateIn, const Vector3& translateOut,
                              const Quaternion& globalRotateIn, const Quaternion& globalRotateOut,
                              const Vector3& globalOriginIn, const Vector3& globalOriginOut,
                              const float swingAngleIn, const Vector3& swingAxisIn, const float swingAngleOut, const Vector3& swingAxisOut,
                              const Vector3& swingAnchorIn, const Vector3& swingAnchorOut,
                              const float opacityThresholdIn, const float opacityThresholdOut,
                              AlphaFunction globalRotateAlphaFunctionIn, AlphaFunction globalRotateAlphaFunctionOut,
                              AlphaFunction swingAlphaFunctionIn, AlphaFunction swingAlphaFunctionOut,
                              AlphaFunction swingAnchorAlphaFunctionIn, AlphaFunction swingAnchorAlphaFunctionOut,
                              AlphaFunction translateAlphaFunctionIn, AlphaFunction translateAlphaFunctionOut,
                              AlphaFunction opacityAlphaFunctionIn, AlphaFunction opacityAlphaFunctionOut ) :
    mScrollAmountProperty(scrollAmountProperty),
    mAnchorProperty(anchorProperty),
    mFlags(flags),
    mPageSpacing(pageSpacing),
    mTranslateIn(translateIn), mTranslateOut(translateOut),
    mGlobalRotateIn(globalRotateIn), mGlobalRotateOut(globalRotateOut),
    mGlobalOriginIn(globalOriginIn), mGlobalOriginOut(globalOriginOut),
    mSwingAngleIn(swingAngleIn), mSwingAxisIn(swingAxisIn), mSwingAngleOut(swingAngleOut), mSwingAxisOut(swingAxisOut),
    mSwingAnchorIn(swingAnchorIn), mSwingAnchorOut(swingAnchorOut),
    mOpacityThresholdIn(opacityThresholdIn), mOpacityThresholdOut(opacityThresholdOut),
    mGlobalRotateAlphaFunctionIn(globalRotateAlphaFunctionIn), mGlobalRotateAlphaFunctionOut(globalRotateAlphaFunctionOut),
    mSwingAlphaFunctionIn(swingAlphaFunctionIn), mSwingAlphaFunctionOut(swingAlphaFunctionOut),
    mSwingAnchorAlphaFunctionIn(swingAnchorAlphaFunctionIn), mSwingAnchorAlphaFunctionOut(swingAnchorAlphaFunctionOut),
    mTranslateAlphaFunctionIn(translateAlphaFunctionIn), mTranslateAlphaFunctionOut(translateAlphaFunctionOut),
    mOpacityAlphaFunctionIn(opacityAlphaFunctionIn), mOpacityAlphaFunctionOut(opacityAlphaFunctionOut),
    mPanning(false),
    mScrolling(false),
    mWasOutsideView(true),
    mWrapped(false),
    mWasWrapped(false),
    mSkipConstraints(false),
    mPassedCentreThisFrame(false),
    mForceDirectionUpdate(true),
    mDirectionFlags(0),
    mLastDirectionFlags(0),
    mCurrentOpacityAlphaFunction(NULL)
  {
  }

  Vector3 ScrollAmountConstraint(const Vector3& current,
                                 const PropertyInput& pagePositionProperty,
                                 const PropertyInput& scrollPositionProperty,
                                 const PropertyInput& scrollPositionMin,
                                 const PropertyInput& scrollPositionMax,
                                 const PropertyInput& pageSizeProperty,
                                 const PropertyInput& scrollWrap)
  {
    // store last scroll pos
    mLastScrollPosition = mScrollPos;
    mPagePos = pagePositionProperty.GetVector3();
    mScrollPos = scrollPositionProperty.GetVector3();
    mScrollMin = scrollPositionMin.GetVector3();
    mScrollMax = scrollPositionMax.GetVector3();
    mPageSize = pageSizeProperty.GetVector3();
    mWrap = scrollWrap.GetBoolean();
    mWasWrapped = mWrapped;

    mLastDirectionFlags = mDirectionFlags;

    // Get position of page.
    mPosition = mPagePos + mScrollPos;

    // short circuit: if we're looking straight on at the page (jonny 5 is alive)
    mIsStraightOnView = IsStraightOnView( mPosition );

    mLastScrollAmount = mScrollAmount;
    Vector3 newScrollAmount(mPosition / mPageSize);
    mScrollAmount = newScrollAmount;
    mWrapped = false;
    if( !mIsStraightOnView && mWrap )
    {
      // only need to wrap if not already straight on view
      WrapPositionWithinDomain( mPosition, mPageSize, mScrollMin, mScrollMax );
      mIsStraightOnView = IsStraightOnView( mPosition );
      newScrollAmount = mPosition / mPageSize;
      if((mScrollAmount.x > 0.0f && newScrollAmount.x < 0.0f)
         || (mScrollAmount.x < 0.0f && newScrollAmount.x > 0.0f)
         || (mScrollAmount.y > 0.0f && newScrollAmount.y < 0.0f)
         || (mScrollAmount.y < 0.0f && newScrollAmount.y > 0.0f))
      {
        mWrapped = true;
      }
    }
    mScrollAmount = newScrollAmount;

    return mScrollAmount;
  }

  Quaternion PageDirectionAndRotationConstraint(const Quaternion& current,
                                  const PropertyInput& scrollPositionProperty,
                                  const PropertyInput& panningProperty,
                                  const PropertyInput& scrollingProperty)
  {
    bool panning = panningProperty.GetBoolean();
    bool scrolling = scrollingProperty.GetBoolean();

    bool isOutsideView = IsOutsideView( mPosition, mPageSize );

    mSkipConstraints = isOutsideView | mIsStraightOnView;

    bool bIsCurrentPage = mScrollAmount.x > -0.5f && mScrollAmount.x < 0.5f;

    if(mSkipConstraints)
    {
      mPanning = panning;
      mScrolling = scrolling;
      mWasOutsideView = isOutsideView;
      mWasStraightOnView = mIsStraightOnView;
      return current;
    }
    Vector3 scrollDirection = mScrollAmount - mLastScrollAmount;
    mPassedCentreThisFrame = bIsCurrentPage && (((mLastScrollAmount.x < 0.0f) && (mScrollAmount.x > 0.0f)) || ((mLastScrollAmount.x > 0.0f) && (mScrollAmount.x < 0.0f)) || ((mLastScrollAmount.y < 0.0f) && (mScrollAmount.y > 0.0f)) || ((mLastScrollAmount.y > 0.0f) && (mScrollAmount.y < 0.0f)) || (mWasStraightOnView && !mIsStraightOnView));

    // may have wrapped this frame and never gone out of view
    bool bWrappedOffScreen = (mWrapped != mWasWrapped && (fabs(scrollDirection.x) > 1.0f || fabs(scrollDirection.y) > 1.0f));

    mCanChangeDirection = (scrolling && !mScrolling) || mPassedCentreThisFrame || (!isOutsideView && mWasOutsideView) || bWrappedOffScreen;

    if(mCanChangeDirection)
    {
      // figure out if we have changed direction
      if((mWrapped != mWasWrapped) && (fabs(scrollDirection.x) > 1.0f || fabs(scrollDirection.y) || (!isOutsideView && mWasOutsideView)))
      {
        if( fabs(scrollDirection.x) > 1.0f )
        {
          if(scrollDirection.x < 0.0f)
          {
            scrollDirection.x += (mScrollMax.x - mScrollMin.x) / mPageSize.x;
          }
          else
          {
            scrollDirection.x -= (mScrollMax.x - mScrollMin.x) / mPageSize.x;
          }
        }
        if( fabs(scrollDirection.y) > 1.0f )
        {
          if(scrollDirection.y < 0.0f)
          {
            scrollDirection.y += (mScrollMax.y - mScrollMin.y) / mPageSize.y;
          }
          else
          {
            scrollDirection.y -= (mScrollMax.y - mScrollMin.y) / mPageSize.y;
          }
        }
      }

      // clear direction flags
      mDirectionFlags &= ~Toolkit::ScrollView::DirectionFlagMask_Direction;
      if(scrollDirection.x < 0.0f)
      {
        mDirectionFlags |= Toolkit::ScrollView::DirectionFlagLeft;
      }
      else if( scrollDirection.x > 0.0f )
      {
        mDirectionFlags |= Toolkit::ScrollView::DirectionFlagRight;
      }

      if(scrolling && !mScrolling)
      {
        // have started moving
        if(((mDirectionFlags & Toolkit::ScrollView::DirectionFlagLeft)
           && (mScrollAmount.x > 0.0f))
           || ((mDirectionFlags & Toolkit::ScrollView::DirectionFlagRight)
               && (mScrollAmount.x < 0.0f)))
        {
          // started moving towards the screen, allow transition change
          mDirectionFlags = (mDirectionFlags & ~Toolkit::ScrollView::DirectionFlagMask_Transition) | Toolkit::ScrollView::DirectionFlagTransitionOn;
        }
        else if(((mDirectionFlags & Toolkit::ScrollView::DirectionFlagLeft)
                && (mScrollAmount.x < 0.0f))
                || ((mDirectionFlags & Toolkit::ScrollView::DirectionFlagRight)
                    && (mScrollAmount.x > 0.0f)))
        {
          // started moving away from screen, allow transition change
          mDirectionFlags  = (mDirectionFlags & ~Toolkit::ScrollView::DirectionFlagMask_Transition) | Toolkit::ScrollView::DirectionFlagTransitionOff;
        }
      }
      else
      {
        // have changed direction
        if((((mDirectionFlags & Toolkit::ScrollView::DirectionFlagLeft)
           && mScrollAmount.x > 0.0f)
            || ((mDirectionFlags & Toolkit::ScrollView::DirectionFlagRight)
                && mScrollAmount.x < 0.0f))
           && (isOutsideView || (!isOutsideView && mWasOutsideView) || bWrappedOffScreen))
        {
          // went from moving away to moving towards and can change direction
          mDirectionFlags = (mDirectionFlags & ~Toolkit::ScrollView::DirectionFlagMask_Transition) | Toolkit::ScrollView::DirectionFlagTransitionOn;
        }
        else if((((mDirectionFlags & Toolkit::ScrollView::DirectionFlagLeft)
                && (mScrollAmount.x < 0.0f))
                 || ((mDirectionFlags & Toolkit::ScrollView::DirectionFlagRight)
                     && (mScrollAmount.x > 0.0f)))
                && (isOutsideView || mPassedCentreThisFrame || bWrappedOffScreen))
        {
          // went from moving towards to moving away and can change direction
          mDirectionFlags = (mDirectionFlags & (~Toolkit::ScrollView::DirectionFlagMask_Transition)) | Toolkit::ScrollView::DirectionFlagTransitionOff;
        }
      }
      // now set up current values depending on direction
      if(mFlags & Toolkit::ScrollViewCustomEffect::FlagTranslate)
      {
        // want to translate by specific amount
        if((mFlags & Toolkit::ScrollViewCustomEffect::FlagTranslateIn)
           && (mDirectionFlags & Toolkit::ScrollView::DirectionFlagTransitionOn))
        {
          mCurrentTranslation = mTranslateIn;
        }
        else if((mFlags & Toolkit::ScrollViewCustomEffect::FlagTranslateOut)
           && (mDirectionFlags & Toolkit::ScrollView::DirectionFlagTransitionOff))
        {
          mCurrentTranslation = mTranslateOut;
        }
        else if(!(mFlags & (Toolkit::ScrollViewCustomEffect::FlagTranslateIn | Toolkit::ScrollViewCustomEffect::FlagTranslateOut)))
        {
          // using same value for both transitions
          mCurrentTranslation = mTranslateIn;
        }
        else
        {
          // no value to use
          mCurrentTranslation = Vector3::ZERO;
        }
      }

      if(mFlags & Toolkit::ScrollViewCustomEffect::FlagRotate)
      {
        // want to rotate around an origin
        if(mFlags & Toolkit::ScrollViewCustomEffect::FlagRotateAngleForcedOrigin)
        {
          // the angle forces the origin position depending on page size
          // also the page spacing is implemented by setting the 'fake' origin far enough back to add a small gap between pages
          // use rotation origin since it isnt needed otherwise
          mCurrentGlobalOrigin = mGlobalOriginIn;
        }
        else
        {
          mCurrentGlobalRotation = mGlobalRotateIn;
          if((mFlags & Toolkit::ScrollViewCustomEffect::FlagRotateOut)
             && (mDirectionFlags & Toolkit::ScrollView::DirectionFlagTransitionOff))
          {
            mCurrentGlobalRotation = mGlobalRotateOut;
          }
        }
      }

      // now set up current values depending on direction
      if(mFlags & Toolkit::ScrollViewCustomEffect::FlagSwingAngle)
      {
        // want to translate by specific amount
        if((mFlags & Toolkit::ScrollViewCustomEffect::FlagSwingAngleIn)
           && (mDirectionFlags & Toolkit::ScrollView::DirectionFlagTransitionOn))
        {
          // moving towards centre of screen and have a value for that
          mCurrentSwingAngle = mSwingAngleIn;
          mCurrentSwingAxis = mSwingAxisIn;
        }
        else if((mFlags & Toolkit::ScrollViewCustomEffect::FlagSwingAngleOut)
           && (mDirectionFlags & Toolkit::ScrollView::DirectionFlagTransitionOff))
        {
          // moving away from centre of screen and have a value for that
          mCurrentSwingAngle = mSwingAngleOut;
          mCurrentSwingAxis = mSwingAxisOut;
        }
        else if(!(mFlags & (Toolkit::ScrollViewCustomEffect::FlagSwingAngleIn | Toolkit::ScrollViewCustomEffect::FlagSwingAngleOut)))
        {
          // using same value for both transitions
          mCurrentSwingAngle = mSwingAngleIn;
          mCurrentSwingAxis = mSwingAxisIn;
        }
        else
        {
          // no value to use
          mCurrentSwingAngle = 0.0f;
          mCurrentSwingAxis = Vector3(0.0f, -1.0f, 0.0f);
        }

        if(mFlags & Toolkit::ScrollViewCustomEffect::FlagSwingAnchor)
        {
          // want to translate by specific amount
          if((mFlags & Toolkit::ScrollViewCustomEffect::FlagSwingAnchorIn)
             && (mDirectionFlags & Toolkit::ScrollView::DirectionFlagTransitionOn))
          {
            mCurrentSwingAnchor = mSwingAnchorIn;
          }
          else if((mFlags & Toolkit::ScrollViewCustomEffect::FlagSwingAnchorOut)
             && (mDirectionFlags & Toolkit::ScrollView::DirectionFlagTransitionOff))
          {
            mCurrentSwingAnchor = mSwingAnchorOut;
          }
          else if(!(mFlags & (Toolkit::ScrollViewCustomEffect::FlagSwingAnchorIn | Toolkit::ScrollViewCustomEffect::FlagSwingAnchorOut)))
          {
            // using same value for both transitions
            mCurrentSwingAnchor = mSwingAnchorIn;
          }
          else
          {
            // no value to use
            mCurrentSwingAnchor = Vector3(0,0,0);
          }
          if(mDirectionFlags & Toolkit::ScrollView::DirectionFlagLeft)
          {
            mCurrentSwingAnchor *= -1.0f;
          }
        }
      }

      // now set up current values depending on direction
      if(mFlags & Toolkit::ScrollViewCustomEffect::FlagOpacityThreshold)
      {
        mCurrentOpacity = mOpacityThresholdIn;
        if((mFlags & Toolkit::ScrollViewCustomEffect::FlagOpacityThresholdOut)
           && (mDirectionFlags & Toolkit::ScrollView::DirectionFlagTransitionOff))
        {
          mCurrentOpacity = mOpacityThresholdOut;
        }
        if(mFlags & Toolkit::ScrollViewCustomEffect::FlagOpacityAlphaFunctionMask)
        {
          // need to adjust using alpha functions
          if( (mFlags & Toolkit::ScrollViewCustomEffect::FlagOpacityAlphaFunctionIn)
             && (mDirectionFlags & Toolkit::ScrollView::DirectionFlagTransitionOn) )
          {
            mCurrentOpacityAlphaFunction = mOpacityAlphaFunctionIn;
          }
          else if( (mFlags & Toolkit::ScrollViewCustomEffect::FlagOpacityAlphaFunctionOut)
                  && (mDirectionFlags & Toolkit::ScrollView::DirectionFlagTransitionOff) )
          {
            mCurrentOpacityAlphaFunction = mOpacityAlphaFunctionOut;
          }
          else
          {
            mCurrentOpacityAlphaFunction = NULL;
          }
        }
      }
    }

    // if user panning OR any form of scroll direction (animated included) set panning to true
    mPanning = panning;
    mScrolling = scrolling;
    mWasOutsideView = isOutsideView;
    mWasStraightOnView = mIsStraightOnView;

    if(!(mFlags & Toolkit::ScrollViewCustomEffect::FlagSwingAngle))
    {
      return current;
    }
    Vector3 amount(mScrollAmount);
    if(mFlags & Toolkit::ScrollViewCustomEffect::FlagSwingAngleAlphaFunctionMask)
    {
      // need to apply alpha function
      if((mFlags & Toolkit::ScrollViewCustomEffect::FlagSwingAngleAlphaFunctionIn)
         && (mDirectionFlags & Toolkit::ScrollView::DirectionFlagTransitionOn))
      {
        amount.x = mSwingAlphaFunctionIn(fabs(mScrollAmount.x));
        amount.y = mSwingAlphaFunctionIn(fabs(mScrollAmount.y));
        if(mScrollAmount.x < 0)
        {
          amount.x *= -1.0f;
        }
        if(mScrollAmount.y < 0)
        {
          amount.y *= -1.0f;
        }
      }
      else if((mFlags & Toolkit::ScrollViewCustomEffect::FlagSwingAngleAlphaFunctionOut)
             && (mDirectionFlags & Toolkit::ScrollView::DirectionFlagTransitionOff))
      {
        amount.x = mSwingAlphaFunctionOut(fabs(mScrollAmount.x));
        amount.y = mSwingAlphaFunctionOut(fabs(mScrollAmount.y));
        if(mScrollAmount.x < 0)
        {
          amount.x *= -1.0f;
        }
        if(mScrollAmount.y < 0)
        {
          amount.y *= -1.0f;
        }
      }
    }

    // TODO - swing angle seems very slightly off... SORT IT!!
    //Quaternion rotation = Quaternion::Slerp(current, mCurrentSwingAngle, mScrollAmount.x);
    return Quaternion(mCurrentSwingAngle * amount.x, mCurrentSwingAxis) * current; // Quaternion::Lerp(current, mCurrentSwingAngle, mScrollAmount.x);
  }

  /**
   * @param[in] current The current color of this Actor
   * @param[in] scrollAmountProperty The scroll-view's position property (SCROLL_POSITION_PROPERTY_NAME)
   * @return The new color of this Actor.
   */
  Vector4 ColorConstraint(const Vector4& current,
                          const PropertyInput& scrollAmountProperty)
  {
    if(mSkipConstraints)
    {
      if(!mIsStraightOnView)
      {
        // will be off screen, set alpha to 0 to stop drawing it
        return Vector4(current.r, current.g, current.b, 0.0f);
      }
      return current;
    }

    if( !(mFlags & Toolkit::ScrollViewCustomEffect::FlagOpacityThreshold) )
    {
      return current;
    }

    float amount = fabsf(mScrollAmount.x);
    if((mFlags & Toolkit::ScrollViewCustomEffect::FlagOpacityAlphaFunctionMask)
       && mCurrentOpacityAlphaFunction)
    {
      amount = mCurrentOpacityAlphaFunction(amount);
    }
    Vector4 newColour(current.r, current.g, current.b, fmaxf(((1.0f - amount) / (1.0f - mCurrentOpacity)), 0.0f));
    return newColour;
  }

  /**
   * @brief PositionConstraint2
   * @param current
   * @param scrollPositionProperty
   * @param startPagePosition
   * @param startDirection
   * @return
   */
  Vector3 PositionConstraint(const Vector3& current,
                             const PropertyInput& scrollAmountProperty,
                             const PropertyInput& anchorProperty,
                             const PropertyInput& rotationProperty)
  {
    if(mSkipConstraints)
    {
      return mPosition;
    }

    Vector3 newPosition;

    if(mFlags & Toolkit::ScrollViewCustomEffect::FlagRotateAngleForcedOrigin)
    {
      Quaternion qx(mScrollAmount.x * mCurrentGlobalOrigin.x, Vector3(0, 1, 0));
      Quaternion qy(mScrollAmount.y * mCurrentGlobalOrigin.y, Vector3(1, 0, 0));

      float thetaBx = (Math::PI - mCurrentGlobalOrigin.x) * 0.5f;
      float radiusx = ((mPageSize.width + mPageSpacing.width) * 0.5f) * tanf(thetaBx);
      Vector3 originPositionVec = Vector3(0, 0, radiusx);
      Vector3 horizontalPos = qx.Rotate(originPositionVec);
      newPosition.x = horizontalPos.x;
      newPosition.z = radiusx - horizontalPos.z;
      // need to create an origin based on current horizontal/vertical scrolling page size
      //Vector2 thetaA(mScrollAmount.x * mCurrentGlobalOrigin.x, mScrollAmount.y * mCurrentGlobalOrigin.y);
      float thetaBy = (Math::PI - mCurrentGlobalOrigin.y) * 0.5f;
      float radiusy = ((mPageSize.height + mPageSpacing.height) * 0.5f) * tanf(thetaBy);
      originPositionVec = Vector3(0, 0, radiusy);
      horizontalPos = qy.Rotate(originPositionVec);
      newPosition.y = horizontalPos.y;
      if(mDirectionFlags & (Toolkit::ScrollView::DirectionFlagUp | Toolkit::ScrollView::DirectionFlagDown))
      {
        newPosition.z = radiusy - horizontalPos.z;
      }

      //Vector3 vRadius(sinf(thetaA.x) * radius, sinf(thetaA.y) * radius, z);
      //newPosition = Vector3(vRadius.x, vRadius.y, -vRadius.z + radius);
    }
    else if(mFlags & Toolkit::ScrollViewCustomEffect::FlagRotate)
    {
      // rotate around our origin which is relative to the scene
      Vector3 vec = newPosition - mCurrentGlobalOrigin;
      newPosition -= vec;
      vec = mCurrentGlobalRotation.Rotate(vec);
      newPosition += vec;
    }

    if(mFlags & Toolkit::ScrollViewCustomEffect::FlagTranslate)
    {
      //Vector3 spacing(mPageSpacing.x, 0, 0);
      Vector3 amount(mScrollAmount);
      amount.z = fabs(mScrollAmount.x);
      if(mFlags & Toolkit::ScrollViewCustomEffect::FlagTranslationAlphaFunctionMask)
      {
        // need to apply alpha function
        if((mFlags & Toolkit::ScrollViewCustomEffect::FlagTranslationAlphaFunctionIn)
           && (mDirectionFlags & Toolkit::ScrollView::DirectionFlagTransitionOn))
        {
          amount.x = mTranslateAlphaFunctionIn(fabs(mScrollAmount.x));
          amount.y = mTranslateAlphaFunctionIn(fabs(mScrollAmount.y));
          amount.z = mTranslateAlphaFunctionIn(fabs(mScrollAmount.x));
          if(mScrollAmount.x < 0)
          {
            amount.x *= -1.0f;
          }
          if(mScrollAmount.y < 0)
          {
            amount.y *= -1.0f;
          }
        }
        else if((mFlags & Toolkit::ScrollViewCustomEffect::FlagTranslationAlphaFunctionOut)
               && (mDirectionFlags & Toolkit::ScrollView::DirectionFlagTransitionOff))
        {
          amount.x = mTranslateAlphaFunctionOut(fabs(mScrollAmount.x));
          amount.y = mTranslateAlphaFunctionOut(fabs(mScrollAmount.y));
          amount.z = mTranslateAlphaFunctionIn(fabs(mScrollAmount.x));
          if(mScrollAmount.x < 0)
          {
            amount.x *= -1.0f;
          }
          if(mScrollAmount.y < 0)
          {
            amount.y *= -1.0f;
          }
        }
      }
      newPosition += mCurrentTranslation * amount; // (mCurrentTranslation + ((!(mFlags & Toolkit::ScrollViewCustomEffect::FlagRotateAngleForcedOrigin)) ? (spacing * 0.5f) : Vector3(0,0,0))) * mScrollAmount;
    }

    if(mFlags & Toolkit::ScrollViewCustomEffect::FlagSwingAnchor)
    {
      // rotate around our anchor point which is local to our actor
      Quaternion rotation(mCurrentSwingAngle * mScrollAmount.x, mCurrentSwingAxis);
      Vector3 offset = mCurrentSwingAnchor * mPageSize;
      newPosition += offset;
      offset = rotation.Rotate(-offset);
      newPosition += offset;
    }

    return newPosition;
  }

  // input parameters

  Property::Index mScrollAmountProperty;
  Property::Index mAnchorProperty;
  uint          mFlags;                         ///< flags describing functionality, set automatically depending on functions called during effect setup
  Vector2       mPageSpacing;                   ///< space between pages... kinda obvious really
  Vector3       mTranslateIn;                   ///< translation offset to use when scrolling a page onto the screen
  Vector3       mTranslateOut;                  ///< translation offset to use when scrolling a page off the screen
  Quaternion    mGlobalRotateIn;                ///< rotates the page's position around a point
  Quaternion    mGlobalRotateOut;               ///< rotates the page's position around a point
  Vector3       mGlobalOriginIn;                ///< the point to rotate a page around when scrolling onto screen
  Vector3       mGlobalOriginOut;               ///< the point to rotate a page around when scrolling off screen
  float         mSwingAngleIn;                  ///< angle to rotate a page around its anchor when scrolling onto screen
  Vector3       mSwingAxisIn;                   ///< angle to rotate a page around its anchor when scrolling off screen
  float         mSwingAngleOut;                 ///< angle to rotate a page around its anchor when scrolling onto screen
  Vector3       mSwingAxisOut;                  ///< angle to rotate a page around its anchor when scrolling off screen
  Vector3       mSwingAnchorIn;                 ///< the page anchor point to use when scrolling onto screen
  Vector3       mSwingAnchorOut;                ///< the page anchor point to use when scrolling off screen
  float         mOpacityThresholdIn;            ///< the point at which opacity will change as page scrolls onto screen
  float         mOpacityThresholdOut;           ///< the point at which opacity will change as page scrolls off screen
  AlphaFunction mGlobalRotateAlphaFunctionIn;
  AlphaFunction mGlobalRotateAlphaFunctionOut;
  AlphaFunction mSwingAlphaFunctionIn;
  AlphaFunction mSwingAlphaFunctionOut;
  AlphaFunction mSwingAnchorAlphaFunctionIn;
  AlphaFunction mSwingAnchorAlphaFunctionOut;
  AlphaFunction mTranslateAlphaFunctionIn;
  AlphaFunction mTranslateAlphaFunctionOut;
  AlphaFunction mOpacityAlphaFunctionIn;
  AlphaFunction mOpacityAlphaFunctionOut;

  // constraint update params
  // taken from property inputs every constraint update
  Vector3       mPagePos;
  Vector3       mScrollPos;
  Vector3       mScrollMin;
  Vector3       mScrollMax;
  Vector3       mPageSize;
  bool          mWrap:1;
  bool          mPanning:1;
  bool          mScrolling:1;
  bool          mWasOutsideView:1;
  bool          mIsStraightOnView:1;
  bool          mWasStraightOnView:1;
  bool          mWrapped:1;               ///< whether the scroll page was wrapped this frame
  bool          mWasWrapped:1;            ///< whether the scroll page was wrapped last frame
  bool          mCanChangeDirection:1;
  bool          mSkipConstraints:1;       ///< whether we can skip the main constraints
  bool          mPassedCentreThisFrame:1; ///< true if control has moved passed centre of screen
  bool          mForceDirectionUpdate:1;
  bool          mDirectionChanged:1;

  // calculated each constraint update depending on flags set
  int           mDirectionFlags;
  int           mLastDirectionFlags;
  Vector2       mLastScrollPosition;

  Vector3       mPosition;
  Vector3       mScrollAmount;
  Vector3       mLastScrollAmount;
  Vector3       mCurrentTranslation;
  Quaternion    mCurrentGlobalRotation;
  Vector3       mCurrentGlobalOrigin;
  float         mCurrentSwingAngle;
  Vector3       mCurrentSwingAxis;
  Vector3       mCurrentSwingAnchor;
  float         mCurrentOpacity;
  AlphaFunction mCurrentOpacityAlphaFunction;
};

typedef IntrusivePtr<ScrollViewCustomEffectInfo> ScrollViewCustomEffectInfoPtr;

} // unnamed namespace

const std::string ScrollViewCustomEffect::SCROLL_AMOUNT_PROPERTY_STRING( "scroll-amount" );
const std::string ScrollViewCustomEffect::ANCHOR_POINT_PROPERTY_STRING( "custom-anchor-point" );

ScrollViewCustomEffect::ScrollViewCustomEffect() :
  mFlags(0),
  mPageSpacing(0,0),
  mTranslateIn(0,0,0),
  mTranslateOut(0,0,0),
  mGlobalRotateIn(0.0f, Vector3(0, 1.0f, 0.0f)),
  mGlobalRotateOut(0.0f, Vector3(0, 1.0f, 0.0f)),
  mGlobalOriginIn(0,0,0),
  mGlobalOriginOut(0,0,0),
  mSwingAngleIn(0.0f),
  mSwingAxisIn(0.0f, 1.0f, 0.0f),
  mSwingAngleOut(0.0f),
  mSwingAxisOut(0.0f, 1.0f, 0.0f),
  mSwingAnchorIn(0,0,0),
  mSwingAnchorOut(0,0,0),
  mOpacityThresholdIn(0),
  mOpacityThresholdOut(0),
  mGlobalRotateAlphaFunctionIn(NULL),
  mGlobalRotateAlphaFunctionOut(NULL),
  mSwingAlphaFunctionIn(NULL),
  mSwingAlphaFunctionOut(NULL),
  mSwingAnchorAlphaFunctionIn(NULL),
  mSwingAnchorAlphaFunctionOut(NULL),
  mTranslateAlphaFunctionIn(NULL),
  mTranslateAlphaFunctionOut(NULL),
  mOpacityAlphaFunctionIn(NULL),
  mOpacityAlphaFunctionOut(NULL)
{

}

ScrollViewCustomEffect::~ScrollViewCustomEffect()
{
}

void ScrollViewCustomEffect::SetPageSpacing(const Vector2& spacing)
{
  mPageSpacing = spacing;
}

void ScrollViewCustomEffect::SetPageTranslation(const Vector3& translation)
{
  mFlags = (mFlags & ~Toolkit::ScrollViewCustomEffect::FlagTranslateMask) | Toolkit::ScrollViewCustomEffect::FlagTranslate;
  mTranslateIn = mTranslateOut = translation;
}

void ScrollViewCustomEffect::SetPageTranslation(const Vector3& translationIn, const Vector3& translationOut)
{
  // set flags describing translation with separate in out translation
  mFlags = (mFlags & ~Toolkit::ScrollViewCustomEffect::FlagTranslateMask)
            | Toolkit::ScrollViewCustomEffect::FlagTranslate
            | Toolkit::ScrollViewCustomEffect::FlagTranslateIn
            | Toolkit::ScrollViewCustomEffect::FlagTranslateOut;

  mTranslateIn = translationIn;
  mTranslateOut = translationOut;
}

void ScrollViewCustomEffect::SetPageTranslationIn(const Vector3& translation)
{
  mFlags = (mFlags & (~Toolkit::ScrollViewCustomEffect::FlagTranslateMask | Toolkit::ScrollViewCustomEffect::FlagTranslateOut))
            | Toolkit::ScrollViewCustomEffect::FlagTranslate
            | Toolkit::ScrollViewCustomEffect::FlagTranslateIn;

  mTranslateIn = translation;
}

void ScrollViewCustomEffect::SetPageTranslationOut(const Vector3& translation)
{
  mFlags = (mFlags & (~Toolkit::ScrollViewCustomEffect::FlagTranslateMask | Toolkit::ScrollViewCustomEffect::FlagTranslateIn))
            | Toolkit::ScrollViewCustomEffect::FlagTranslate
            | Toolkit::ScrollViewCustomEffect::FlagTranslateOut;

  mTranslateOut = translation;
}

void ScrollViewCustomEffect::SetPageTranslateAlphaFunction(AlphaFunction func)
{
  if(func)
  {
    mFlags |= Toolkit::ScrollViewCustomEffect::FlagTranslationAlphaFunctionMask;
  }
  else
  {
    mFlags &= ~Toolkit::ScrollViewCustomEffect::FlagTranslationAlphaFunctionMask;
  }
  mTranslateAlphaFunctionIn = mTranslateAlphaFunctionOut = func;
}

void ScrollViewCustomEffect::SetPageTranslateAlphaFunction(AlphaFunction funcIn, AlphaFunction funcOut)
{
  if(funcIn)
  {
    mFlags |= Toolkit::ScrollViewCustomEffect::FlagTranslationAlphaFunctionIn;
  }
  else
  {
    mFlags &= ~Toolkit::ScrollViewCustomEffect::FlagTranslationAlphaFunctionIn;
  }
  if(funcOut)
  {
    mFlags |= Toolkit::ScrollViewCustomEffect::FlagTranslationAlphaFunctionOut;
  }
  else
  {
    mFlags &= ~Toolkit::ScrollViewCustomEffect::FlagTranslationAlphaFunctionOut;
  }
  mTranslateAlphaFunctionIn = funcIn;
  mTranslateAlphaFunctionOut = funcOut;
}

void ScrollViewCustomEffect::SetPageTranslateAlphaFunctionIn(AlphaFunction func)
{
  if(func)
  {
    mFlags |= Toolkit::ScrollViewCustomEffect::FlagTranslationAlphaFunctionIn;
  }
  else
  {
    mFlags &= ~Toolkit::ScrollViewCustomEffect::FlagTranslationAlphaFunctionIn;
  }
  mTranslateAlphaFunctionIn = func;
}

void ScrollViewCustomEffect::SetPageTranslateAlphaFunctionOut(AlphaFunction func)
{
  if(func)
  {
    mFlags |= Toolkit::ScrollViewCustomEffect::FlagTranslationAlphaFunctionOut;
  }
  else
  {
    mFlags &= ~Toolkit::ScrollViewCustomEffect::FlagTranslationAlphaFunctionOut;
  }
  mTranslateAlphaFunctionOut = func;
}

void ScrollViewCustomEffect::SetGlobalPageRotation(float angle, const Vector3& axis)
{
  // set flags describing translation with separate in out translation
  mFlags = (mFlags & ~Toolkit::ScrollViewCustomEffect::FlagRotateMask) // reset rotate flags
            | Toolkit::ScrollViewCustomEffect::FlagRotate;             // set new rotate flag

  mGlobalRotateIn = mGlobalRotateOut = Quaternion(angle, axis);
}

void ScrollViewCustomEffect::SetAngledOriginPageRotation(const Vector3& angle)
{
  mFlags = (mFlags & ~Toolkit::ScrollViewCustomEffect::FlagRotateMask)
            | Toolkit::ScrollViewCustomEffect::FlagRotate
            | Toolkit::ScrollViewCustomEffect::FlagRotateAngleForcedOrigin;

  // set this angle into global originin for now, the flag will let us know what value to use in constraints
  mGlobalOriginIn = angle;
}

void ScrollViewCustomEffect::SetGlobalPageRotation(float angleIn, const Vector3& axisIn, float angleOut, const Vector3& axisOut)
{
  // set flags describing translation with separate in out translation
  mFlags = (mFlags & ~Toolkit::ScrollViewCustomEffect::FlagRotateMask)
            | Toolkit::ScrollViewCustomEffect::FlagRotate
            | Toolkit::ScrollViewCustomEffect::FlagRotateIn
            | Toolkit::ScrollViewCustomEffect::FlagRotateOut;

  mGlobalRotateIn = Quaternion(angleIn, axisIn);
  mGlobalRotateOut = Quaternion(angleOut, axisOut);
}

void ScrollViewCustomEffect::SetGlobalPageRotationIn(float angle, const Vector3& axis)
{
  // set flags describing translation with separate in out translation
  mFlags = (mFlags & (~Toolkit::ScrollViewCustomEffect::FlagRotateMask | Toolkit::ScrollViewCustomEffect::FlagRotateOut)) // reset all rotation flags except RotateOut, since they may be calling these functions separately
            | Toolkit::ScrollViewCustomEffect::FlagRotate
            | Toolkit::ScrollViewCustomEffect::FlagRotateIn;

  mGlobalRotateIn = Quaternion(angle, axis);
}

void ScrollViewCustomEffect::SetGlobalPageRotationOut(float angle, const Vector3& axis)
{
  // set flags describing translation with separate in out translation
  mFlags = (mFlags & (~Toolkit::ScrollViewCustomEffect::FlagRotateMask | Toolkit::ScrollViewCustomEffect::FlagRotateIn)) // reset all rotation flags except RotateOut, since they may be calling these functions separately
            | Toolkit::ScrollViewCustomEffect::FlagRotate
            | Toolkit::ScrollViewCustomEffect::FlagRotateOut;

  mGlobalRotateOut = Quaternion(angle, axis);
}

void ScrollViewCustomEffect::SetGlobalPageRotationOrigin(const Vector3& origin)
{
  // set flags describing translation with separate in out translation
  mFlags = (mFlags & ~Toolkit::ScrollViewCustomEffect::FlagRotateOriginMask) // reset all rotation flags
            | Toolkit::ScrollViewCustomEffect::FlagRotateOrigin;

  mGlobalOriginIn = mGlobalOriginOut = origin;
}

void ScrollViewCustomEffect::SetGlobalPageRotationOrigin(const Vector3& originIn, const Vector3& originOut)
{
  // set flags describing translation with separate in out translation
  mFlags = (mFlags & ~Toolkit::ScrollViewCustomEffect::FlagRotateOriginMask) // reset all rotation flags
            | Toolkit::ScrollViewCustomEffect::FlagRotateOrigin
            | Toolkit::ScrollViewCustomEffect::FlagRotateOriginIn
            | Toolkit::ScrollViewCustomEffect::FlagRotateOriginOut;

  mGlobalOriginIn = originIn;
  mGlobalOriginOut = originOut;
}

void ScrollViewCustomEffect::SetGlobalPageRotationOriginIn(const Vector3& origin)
{
  // set flags describing translation with separate in out translation
  mFlags = (mFlags & (~Toolkit::ScrollViewCustomEffect::FlagRotateOriginMask | Toolkit::ScrollViewCustomEffect::FlagRotateOriginOut)) // reset all rotation flags except RotateOut, since they may be calling these functions separately
            | Toolkit::ScrollViewCustomEffect::FlagRotateOrigin
            | Toolkit::ScrollViewCustomEffect::FlagRotateOriginIn;

  mGlobalOriginIn = origin;
}

void ScrollViewCustomEffect::SetGlobalPageRotationOriginOut(const Vector3& origin)
{
  // set flags describing translation with separate in out translation
  mFlags = (mFlags & (~Toolkit::ScrollViewCustomEffect::FlagRotateOriginMask | Toolkit::ScrollViewCustomEffect::FlagRotateOriginIn)) // reset all rotation flags except RotateOut, since they may be calling these functions separately
            | Toolkit::ScrollViewCustomEffect::FlagRotateOrigin
            | Toolkit::ScrollViewCustomEffect::FlagRotateOriginOut;

  mGlobalOriginOut = origin;
}

void ScrollViewCustomEffect::SetSwingAngle(const float angle, const Vector3& axis)
{
  // set flags describing translation with separate in out translation
  mFlags = (mFlags & ~Toolkit::ScrollViewCustomEffect::FlagSwingAngleMask) // reset rotate flags
            | Toolkit::ScrollViewCustomEffect::FlagSwingAngle;             // set new rotate flag

  mSwingAngleIn = mSwingAngleOut = angle;
  mSwingAxisIn = mSwingAxisOut = axis;
}

void ScrollViewCustomEffect::SetSwingAngle(float angleIn, const Vector3& axisIn, float angleOut, const Vector3& axisOut)
{
  // set flags describing translation with separate in out translation
  mFlags = (mFlags & ~Toolkit::ScrollViewCustomEffect::FlagSwingAngleMask)
            | Toolkit::ScrollViewCustomEffect::FlagSwingAngle
            | Toolkit::ScrollViewCustomEffect::FlagSwingAngleIn
            | Toolkit::ScrollViewCustomEffect::FlagSwingAngleOut;

  mSwingAngleIn = angleIn;
  mSwingAngleOut = angleOut;
  mSwingAxisIn = axisIn;
  mSwingAxisOut = axisOut;
}

void ScrollViewCustomEffect::SetSwingAngleIn(float angle, const Vector3& axis)
{
  // set flags describing translation with separate in out translation
  mFlags = (mFlags & (~Toolkit::ScrollViewCustomEffect::FlagSwingAngleMask | Toolkit::ScrollViewCustomEffect::FlagSwingAngleOut)) // reset all rotation flags except RotateOut, since they may be calling these functions separately
            | Toolkit::ScrollViewCustomEffect::FlagSwingAngle
            | Toolkit::ScrollViewCustomEffect::FlagSwingAngleIn;

  mSwingAngleIn = angle;
  mSwingAxisIn = axis;
}

void ScrollViewCustomEffect::SetSwingAngleOut(float angle, const Vector3& axis)
{
  // set flags describing translation with separate in out translation
  mFlags = (mFlags & (~Toolkit::ScrollViewCustomEffect::FlagSwingAngleMask | Toolkit::ScrollViewCustomEffect::FlagSwingAngleIn)) // reset all rotation flags except RotateOut, since they may be calling these functions separately
            | Toolkit::ScrollViewCustomEffect::FlagSwingAngle
            | Toolkit::ScrollViewCustomEffect::FlagSwingAngleOut;

  mSwingAngleOut = angle;
  mSwingAxisOut = axis;
}

void ScrollViewCustomEffect::SetSwingAngleAlphaFunction(AlphaFunction func)
{
  if(func)
  {
    mFlags |= Toolkit::ScrollViewCustomEffect::FlagSwingAngleAlphaFunctionMask;
  }
  else
  {
    mFlags &= ~Toolkit::ScrollViewCustomEffect::FlagSwingAngleAlphaFunctionMask;
  }
  mSwingAlphaFunctionIn = mSwingAlphaFunctionOut = func;
}

void ScrollViewCustomEffect::SetSwingAngleAlphaFunction(AlphaFunction funcIn, AlphaFunction funcOut)
{
  if(funcIn)
  {
    mFlags |= Toolkit::ScrollViewCustomEffect::FlagSwingAngleAlphaFunctionIn;
  }
  else
  {
    mFlags &= ~Toolkit::ScrollViewCustomEffect::FlagSwingAngleAlphaFunctionIn;
  }
  if(funcOut)
  {
    mFlags |= Toolkit::ScrollViewCustomEffect::FlagSwingAngleAlphaFunctionOut;
  }
  else
  {
    mFlags &= ~Toolkit::ScrollViewCustomEffect::FlagSwingAngleAlphaFunctionOut;
  }
  mSwingAlphaFunctionIn = funcIn;
  mSwingAlphaFunctionOut = funcOut;
}

void ScrollViewCustomEffect::SetSwingAngleAlphaFunctionIn(AlphaFunction func)
{
  if(func)
  {
    mFlags |= Toolkit::ScrollViewCustomEffect::FlagSwingAngleAlphaFunctionIn;
  }
  else
  {
    mFlags &= ~Toolkit::ScrollViewCustomEffect::FlagSwingAngleAlphaFunctionIn;
  }
  mSwingAlphaFunctionIn = func;
}

void ScrollViewCustomEffect::SetSwingAngleAlphaFunctionOut(AlphaFunction func)
{
  if(func)
  {
    mFlags |= Toolkit::ScrollViewCustomEffect::FlagSwingAngleAlphaFunctionOut;
  }
  else
  {
    mFlags &= ~Toolkit::ScrollViewCustomEffect::FlagSwingAngleAlphaFunctionOut;
  }
  mSwingAlphaFunctionOut = func;
}

void ScrollViewCustomEffect::SetSwingAnchor(const Vector3& anchor)
{
  mFlags = (mFlags & ~Toolkit::ScrollViewCustomEffect::FlagSwingAnchorMask)
            | Toolkit::ScrollViewCustomEffect::FlagSwingAnchor;
  mSwingAnchorIn = mSwingAnchorOut = anchor;
}

void ScrollViewCustomEffect::SetSwingAnchor(const Vector3& anchorIn, const Vector3& anchorOut)
{
  // set flags describing translation with separate in out translation
  mFlags = (mFlags & ~Toolkit::ScrollViewCustomEffect::FlagSwingAnchorMask)
            | Toolkit::ScrollViewCustomEffect::FlagSwingAnchor
            | Toolkit::ScrollViewCustomEffect::FlagSwingAnchorIn
            | Toolkit::ScrollViewCustomEffect::FlagSwingAnchorOut;

  mSwingAnchorIn = anchorIn;
  mSwingAnchorOut = anchorOut;
}

void ScrollViewCustomEffect::SetSwingAnchorIn(const Vector3& anchor)
{
  mFlags = (mFlags & (~Toolkit::ScrollViewCustomEffect::FlagSwingAnchorMask | Toolkit::ScrollViewCustomEffect::FlagSwingAnchorOut))
            | Toolkit::ScrollViewCustomEffect::FlagSwingAnchor
            | Toolkit::ScrollViewCustomEffect::FlagSwingAnchorIn;

  mSwingAnchorIn = anchor;
}

void ScrollViewCustomEffect::SetSwingAnchorOut(const Vector3& anchor)
{
  mFlags = (mFlags & (~Toolkit::ScrollViewCustomEffect::FlagSwingAnchorMask | Toolkit::ScrollViewCustomEffect::FlagSwingAnchorIn))
            | Toolkit::ScrollViewCustomEffect::FlagSwingAnchor
            | Toolkit::ScrollViewCustomEffect::FlagSwingAnchorOut;

  mSwingAnchorOut = anchor;
}

void ScrollViewCustomEffect::SetSwingAnchorAlphaFunction(AlphaFunction func)
{
  if(func)
  {
    mFlags |= Toolkit::ScrollViewCustomEffect::FlagSwingAnchorAlphaFunctionMask;
  }
  else
  {
    mFlags &= ~Toolkit::ScrollViewCustomEffect::FlagSwingAnchorAlphaFunctionMask;
  }
  mSwingAnchorAlphaFunctionIn = mSwingAnchorAlphaFunctionOut = func;
}

void ScrollViewCustomEffect::SetSwingAnchorAlphaFunction(AlphaFunction funcIn, AlphaFunction funcOut)
{
  if(funcIn)
  {
    mFlags |= Toolkit::ScrollViewCustomEffect::FlagSwingAnchorAlphaFunctionIn;
  }
  else
  {
    mFlags &= ~Toolkit::ScrollViewCustomEffect::FlagSwingAnchorAlphaFunctionIn;
  }
  if(funcOut)
  {
    mFlags |= Toolkit::ScrollViewCustomEffect::FlagSwingAnchorAlphaFunctionOut;
  }
  else
  {
    mFlags &= ~Toolkit::ScrollViewCustomEffect::FlagSwingAnchorAlphaFunctionOut;
  }
  mSwingAnchorAlphaFunctionIn = funcIn;
  mSwingAnchorAlphaFunctionOut = funcOut;
}

void ScrollViewCustomEffect::SetSwingAnchorAlphaFunctionIn(AlphaFunction func)
{
  if(func)
  {
    mFlags |= Toolkit::ScrollViewCustomEffect::FlagSwingAnchorAlphaFunctionIn;
  }
  else
  {
    mFlags &= ~Toolkit::ScrollViewCustomEffect::FlagSwingAnchorAlphaFunctionIn;
  }
  mSwingAnchorAlphaFunctionIn = func;
}

void ScrollViewCustomEffect::SetSwingAnchorAlphaFunctionOut(AlphaFunction func)
{
  if(func)
  {
    mFlags |= Toolkit::ScrollViewCustomEffect::FlagSwingAnchorAlphaFunctionOut;
  }
  else
  {
    mFlags &= ~Toolkit::ScrollViewCustomEffect::FlagSwingAnchorAlphaFunctionOut;
  }
  mSwingAnchorAlphaFunctionOut = func;
}

void ScrollViewCustomEffect::SetOpacityThreshold(float thresh)
{
  mFlags = (mFlags & ~Toolkit::ScrollViewCustomEffect::FlagOpacityThresholdMask)
            | Toolkit::ScrollViewCustomEffect::FlagOpacityThreshold
            | Toolkit::ScrollViewCustomEffect::FlagOpacityThresholdIn
            | Toolkit::ScrollViewCustomEffect::FlagOpacityThresholdOut;

  mOpacityThresholdIn = mOpacityThresholdOut = thresh;
}

void ScrollViewCustomEffect::SetOpacityThreshold(float threshIn, float threshOut)
{
  // set flags describing translation with separate in out translation
  mFlags = (mFlags & ~Toolkit::ScrollViewCustomEffect::FlagOpacityThresholdMask)
            | Toolkit::ScrollViewCustomEffect::FlagOpacityThreshold
            | Toolkit::ScrollViewCustomEffect::FlagOpacityThresholdIn
            | Toolkit::ScrollViewCustomEffect::FlagOpacityThresholdOut;

  mOpacityThresholdIn = threshIn;
  mOpacityThresholdOut = threshOut;
}

void ScrollViewCustomEffect::SetOpacityThresholdIn(float thresh)
{
  mFlags = (mFlags & (~Toolkit::ScrollViewCustomEffect::FlagOpacityThresholdMask | Toolkit::ScrollViewCustomEffect::FlagOpacityThresholdOut))
            | Toolkit::ScrollViewCustomEffect::FlagOpacityThreshold
            | Toolkit::ScrollViewCustomEffect::FlagOpacityThresholdIn;

  mOpacityThresholdIn = thresh;
}

void ScrollViewCustomEffect::SetOpacityThresholdOut(float thresh)
{
  mFlags = (mFlags & (~Toolkit::ScrollViewCustomEffect::FlagOpacityThresholdMask | Toolkit::ScrollViewCustomEffect::FlagOpacityThresholdIn))
            | Toolkit::ScrollViewCustomEffect::FlagOpacityThreshold
            | Toolkit::ScrollViewCustomEffect::FlagOpacityThresholdOut;

  mOpacityThresholdOut = thresh;
}

void ScrollViewCustomEffect::SetOpacityAlphaFunction(AlphaFunction func)
{
  if(func)
  {
    mFlags |= Toolkit::ScrollViewCustomEffect::FlagOpacityAlphaFunctionMask;
  }
  else
  {
    mFlags &= ~Toolkit::ScrollViewCustomEffect::FlagOpacityAlphaFunctionMask;
  }
  mOpacityAlphaFunctionIn = mOpacityAlphaFunctionOut = func;
}

void ScrollViewCustomEffect::SetOpacityAlphaFunction(AlphaFunction funcIn, AlphaFunction funcOut)
{
  if(funcIn)
  {
    mFlags |= Toolkit::ScrollViewCustomEffect::FlagOpacityAlphaFunctionIn;
  }
  else
  {
    mFlags &= ~Toolkit::ScrollViewCustomEffect::FlagOpacityAlphaFunctionIn;
  }
  if(funcOut)
  {
    mFlags |= Toolkit::ScrollViewCustomEffect::FlagOpacityAlphaFunctionOut;
  }
  else
  {
    mFlags &= ~Toolkit::ScrollViewCustomEffect::FlagOpacityAlphaFunctionOut;
  }
  mOpacityAlphaFunctionIn = funcIn;
  mOpacityAlphaFunctionOut = funcOut;
}

void ScrollViewCustomEffect::SetOpacityAlphaFunctionIn(AlphaFunction func)
{
  if(func)
  {
    mFlags |= Toolkit::ScrollViewCustomEffect::FlagOpacityAlphaFunctionIn;
  }
  else
  {
    mFlags &= ~Toolkit::ScrollViewCustomEffect::FlagOpacityAlphaFunctionIn;
  }
  mOpacityAlphaFunctionIn = func;
}

void ScrollViewCustomEffect::SetOpacityAlphaFunctionOut(AlphaFunction func)
{
  if(func)
  {
    mFlags |= Toolkit::ScrollViewCustomEffect::FlagOpacityAlphaFunctionOut;
  }
  else
  {
    mFlags &= ~Toolkit::ScrollViewCustomEffect::FlagOpacityAlphaFunctionOut;
  }
  mOpacityAlphaFunctionOut = func;
}

void ScrollViewCustomEffect::ApplyToPage( Actor page, Vector3 pageSize)
{
  // may have already called register for these properties, so check before registering
  Dali::Toolkit::ScrollView scrollView = GetScrollView();
  Property::Index scrollPropertyIndex = page.GetPropertyIndex(SCROLL_AMOUNT_PROPERTY_STRING);
  if(scrollPropertyIndex == Property::INVALID_INDEX)
  {
    scrollPropertyIndex = page.RegisterProperty(SCROLL_AMOUNT_PROPERTY_STRING, Vector3::ZERO);
  }

  Property::Index anchorPropertyIndex = page.GetPropertyIndex(ANCHOR_POINT_PROPERTY_STRING);
  if(anchorPropertyIndex == Property::INVALID_INDEX)
  {
    anchorPropertyIndex = page.RegisterProperty(ANCHOR_POINT_PROPERTY_STRING, Vector3::ZERO);
  }

  ScrollViewCustomEffectInfoPtr info(new ScrollViewCustomEffectInfo(
                                                                     mFlags,
                                                                     scrollPropertyIndex,
                                                                     anchorPropertyIndex,
                                                                     mPageSpacing,
                                                                     mTranslateIn, mTranslateOut,
                                                                     mGlobalRotateIn, mGlobalRotateOut,
                                                                     mGlobalOriginIn, mGlobalOriginOut,
                                                                     mSwingAngleIn, mSwingAxisIn, mSwingAngleOut, mSwingAxisOut,
                                                                     mSwingAnchorIn - AnchorPoint::CENTER, mSwingAnchorOut - AnchorPoint::CENTER,
                                                                     mOpacityThresholdIn, mOpacityThresholdOut,
                                                                     mGlobalRotateAlphaFunctionIn, mGlobalRotateAlphaFunctionOut,
                                                                     mSwingAlphaFunctionIn, mSwingAlphaFunctionOut,
                                                                     mSwingAnchorAlphaFunctionIn, mSwingAnchorAlphaFunctionOut,
                                                                     mTranslateAlphaFunctionIn, mTranslateAlphaFunctionOut,
                                                                     mOpacityAlphaFunctionIn, mOpacityAlphaFunctionOut));

  ScrollViewCustomEffectInfo effectInfo( *info );
  Property::Index scrollAmountProperty = effectInfo.mScrollAmountProperty;
  Property::Index anchProperty = effectInfo.mAnchorProperty;
  // Apply constraints to this actor //
  Constraint constraint;
  constraint = Constraint::New<Vector3>( scrollAmountProperty,
                                            LocalSource(Actor::POSITION),
                                            Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollView::SCROLL_POSITION_PROPERTY_NAME ) ),
                                            Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollView::SCROLL_POSITION_MIN_PROPERTY_NAME ) ),
                                            Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollView::SCROLL_POSITION_MAX_PROPERTY_NAME ) ),
                                            Source(scrollView, Actor::SIZE ),
                                            Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollView::SCROLL_WRAP_PROPERTY_NAME ) ),
                                            boost::bind( &ScrollViewCustomEffectInfo::ScrollAmountConstraint, info, _1, _2, _3, _4, _5, _6, _7) );

  constraint.SetRemoveAction( Constraint::Discard );
  page.ApplyConstraint( constraint );

  constraint = Constraint::New<Quaternion>( Actor::ROTATION,
                                         Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollView::SCROLL_POSITION_PROPERTY_NAME ) ),
                                         Source( scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollView::SCROLL_PANNING_PROPERTY_NAME ) ),
                                         Source( scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollView::SCROLL_SCROLLING_PROPERTY_NAME ) ),
                                         boost::bind( &ScrollViewCustomEffectInfo::PageDirectionAndRotationConstraint, info, _1, _2, _3, _4) );

  constraint.SetRemoveAction( Constraint::Discard );
  page.ApplyConstraint( constraint );

  constraint = Constraint::New<Vector4>( Actor::COLOR,
                                         Source(page, scrollAmountProperty ),
                                         boost::bind( &ScrollViewCustomEffectInfo::ColorConstraint, info, _1, _2) );

  constraint.SetRemoveAction( Constraint::Discard );
  page.ApplyConstraint( constraint );

  constraint = Constraint::New<Vector3>( Actor::POSITION,
                                         Source(page, scrollAmountProperty ),
                                         Source(page, anchProperty ),
                                         LocalSource(Actor::ROTATION),
                                         boost::bind( &ScrollViewCustomEffectInfo::PositionConstraint, info, _1, _2, _3, _4) );

  constraint.SetRemoveAction( Constraint::Discard );
  page.ApplyConstraint( constraint );
}

void ScrollViewCustomEffect::OnAttach(Toolkit::ScrollView& scrollView)
{
}

void ScrollViewCustomEffect::OnDetach(Toolkit::ScrollView& scrollView)
{
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
