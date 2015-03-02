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

// CLASS HEADER
#include <dali-toolkit/internal/controls/cluster/cluster-style-impl.h>

// EXTERNAL INCLUDES
#include <dali/public-api/animation/animation.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/cluster/cluster.h>

namespace Dali
{

namespace // unnamed namespace
{
// Cluster style one

const unsigned int STYLE_1_CHILDREN_NUMBER = 8;

const float STYLE_1_BACKGROUND_IMAGE_OFFSET_Z = 1.0f;

const float STYLE_1_CHILD_OFFSET_Z = 2.0f;
const float STYLE_1_CHILD_GAP_FACTOR = 0.03f;
const float STYLE_1_CHILD_SIZE_FACTOR[] = { 0.4f, 0.15f, 0.25f, 0.15f, 0.4f, 0.15f, 0.25f, 0.15f };
const Vector3 STYLE_1_CHILD_POSITION_FACTOR[] = { Vector3(0.5f - STYLE_1_CHILD_SIZE_FACTOR[0] - STYLE_1_CHILD_GAP_FACTOR * 0.5f,
                                                            0.5f - STYLE_1_CHILD_SIZE_FACTOR[0] - STYLE_1_CHILD_GAP_FACTOR * 0.5f,
                                                            STYLE_1_CHILD_OFFSET_Z),
                                                    Vector3(0.5f + STYLE_1_CHILD_GAP_FACTOR * 0.5f,
                                                            0.5f - STYLE_1_CHILD_SIZE_FACTOR[1] - STYLE_1_CHILD_SIZE_FACTOR[2] - STYLE_1_CHILD_GAP_FACTOR * 1.5f,
                                                            STYLE_1_CHILD_OFFSET_Z + 0.5f),
                                                    Vector3(0.5f + STYLE_1_CHILD_GAP_FACTOR * 0.5f,
                                                            0.5f - STYLE_1_CHILD_SIZE_FACTOR[2] - STYLE_1_CHILD_GAP_FACTOR * 0.5f,
                                                            STYLE_1_CHILD_OFFSET_Z + 1.0f),
                                                    Vector3(0.5f + STYLE_1_CHILD_SIZE_FACTOR[2] + STYLE_1_CHILD_GAP_FACTOR * 1.5f,
                                                            0.5f - STYLE_1_CHILD_SIZE_FACTOR[3] - STYLE_1_CHILD_GAP_FACTOR * 0.5f,
                                                            STYLE_1_CHILD_OFFSET_Z + 1.5f),
                                                    Vector3(0.5f + STYLE_1_CHILD_GAP_FACTOR * 0.5f,
                                                            0.5f + STYLE_1_CHILD_GAP_FACTOR * 0.5f,
                                                            STYLE_1_CHILD_OFFSET_Z + 2.0f),
                                                    Vector3(0.5f - STYLE_1_CHILD_SIZE_FACTOR[5] - STYLE_1_CHILD_GAP_FACTOR * 0.5f,
                                                            0.5f + STYLE_1_CHILD_SIZE_FACTOR[6] + STYLE_1_CHILD_GAP_FACTOR * 1.5f,
                                                            STYLE_1_CHILD_OFFSET_Z + 2.5f),
                                                    Vector3(0.5f - STYLE_1_CHILD_SIZE_FACTOR[6] - STYLE_1_CHILD_GAP_FACTOR * 0.5f,
                                                            0.5f + STYLE_1_CHILD_GAP_FACTOR * 0.5f,
                                                            STYLE_1_CHILD_OFFSET_Z + 3.0f),
                                                    Vector3(0.5f - STYLE_1_CHILD_SIZE_FACTOR[6] - STYLE_1_CHILD_SIZE_FACTOR[7] - STYLE_1_CHILD_GAP_FACTOR * 1.5f,
                                                            0.5f + STYLE_1_CHILD_GAP_FACTOR * 0.5f,
                                                            STYLE_1_CHILD_OFFSET_Z + 3.5f) };

const Vector3 STYLE_1_TITLE_SIZE_FACTOR = Vector3(0.3f, 0.11f, 1.0f);
const Vector3 STYLE_1_TITLE_POSITION_FACTOR = Vector3(0.5f - STYLE_1_CHILD_SIZE_FACTOR[0] - STYLE_1_CHILD_GAP_FACTOR * 0.5f,
                                                        0.5f - STYLE_1_CHILD_SIZE_FACTOR[0] - STYLE_1_CHILD_GAP_FACTOR * 0.5f - STYLE_1_TITLE_SIZE_FACTOR.height + 0.02f,
                                                        0.0f);
const Vector3 STYLE_1_TITLE_POSITION_OFFSET = Vector3(0.0f, 0.0f, 8.0f);

// Cluster style two

const unsigned int STYLE_2_CHILDREN_NUMBER = 6;

const float STYLE_2_BACKGROUND_IMAGE_OFFSET_Z = 1.0f;

const float STYLE_2_CHILD_OFFSET_Z = 2.0f;
const float STYLE_2_CHILD_GAP_FACTOR = 0.03f;
const float STYLE_2_CHILD_SIZE_FACTOR[] = { 0.4f, 0.25f, 0.15f, 0.4f, 0.25f, 0.15f };
const Vector3 STYLE_2_CHILD_POSITION_FACTOR[] = { Vector3(0.5f - STYLE_2_CHILD_SIZE_FACTOR[0] - STYLE_2_CHILD_GAP_FACTOR * 0.5f,
                                                            0.5f - STYLE_2_CHILD_SIZE_FACTOR[0] * 0.75f,
                                                            STYLE_2_CHILD_OFFSET_Z),
                                                    Vector3(0.5f + STYLE_2_CHILD_GAP_FACTOR * 0.5f,
                                                            0.5f - STYLE_2_CHILD_SIZE_FACTOR[1] - STYLE_2_CHILD_SIZE_FACTOR[3] * 0.25f - STYLE_2_CHILD_GAP_FACTOR,
                                                            STYLE_2_CHILD_OFFSET_Z + 0.5f),
                                                    Vector3(0.5f + STYLE_2_CHILD_SIZE_FACTOR[1] + STYLE_2_CHILD_GAP_FACTOR * 1.5f,
                                                            0.5f - STYLE_2_CHILD_SIZE_FACTOR[2] - STYLE_2_CHILD_SIZE_FACTOR[3] * 0.25f - STYLE_2_CHILD_GAP_FACTOR,
                                                            STYLE_2_CHILD_OFFSET_Z + 1.0f),
                                                    Vector3(0.5f + STYLE_2_CHILD_GAP_FACTOR * 0.5f,
                                                            0.5f - STYLE_2_CHILD_SIZE_FACTOR[3] * 0.25f,
                                                            STYLE_2_CHILD_OFFSET_Z + 1.5f),
                                                    Vector3(0.5f - STYLE_2_CHILD_SIZE_FACTOR[4] - STYLE_2_CHILD_GAP_FACTOR * 0.5f,
                                                            0.5f + STYLE_2_CHILD_SIZE_FACTOR[0] * 0.25f + STYLE_2_CHILD_GAP_FACTOR,
                                                            STYLE_2_CHILD_OFFSET_Z + 2.0f),
                                                    Vector3(0.5f - STYLE_2_CHILD_SIZE_FACTOR[4] - STYLE_2_CHILD_SIZE_FACTOR[5] - STYLE_2_CHILD_GAP_FACTOR * 1.5f,
                                                            0.5f + STYLE_2_CHILD_SIZE_FACTOR[0] * 0.25f + STYLE_2_CHILD_GAP_FACTOR,
                                                            STYLE_2_CHILD_OFFSET_Z + 2.5f) };

const Vector3 STYLE_2_TITLE_SIZE_FACTOR = Vector3(0.3f, 0.11f, 1.0f);
const Vector3 STYLE_2_TITLE_POSITION_FACTOR = Vector3(0.5f - STYLE_2_CHILD_SIZE_FACTOR[0] - STYLE_2_CHILD_GAP_FACTOR * 0.5f,
                                                        0.5f - STYLE_2_CHILD_SIZE_FACTOR[0] * 0.75f - STYLE_2_TITLE_SIZE_FACTOR.height + 0.02f,
                                                        0.0f);
const Vector3 STYLE_2_TITLE_POSITION_OFFSET = Vector3(0.0f, 0.0f, 8.0f);

// Cluster style three

const unsigned int STYLE_3_CHILDREN_NUMBER = 6;

const Vector3 STYLE_3_TITLE_SIZE_FACTOR = Vector3(0.4f, 0.15f, 1.0f);

const float STYLE_3_BACKGROUND_IMAGE_OFFSET_Z = 1.0f;

const float STYLE_3_CHILD_OFFSET_Z = 2.0f;
const float STYLE_3_CHILD_GAP_FACTOR = 0.03f;
const float STYLE_3_CHILD_SIZE_FACTOR[] = { 0.4f, 0.4f, 0.15f, 0.25f, 0.25f, 0.15f };
const float STYLE_3_CHILD_POSITION_OFFSET_Y = (1.0f - STYLE_3_CHILD_SIZE_FACTOR[0] - STYLE_3_CHILD_SIZE_FACTOR[3] - STYLE_3_CHILD_GAP_FACTOR - STYLE_3_TITLE_SIZE_FACTOR.height) * 0.5f;
const Vector3 STYLE_3_CHILD_POSITION_FACTOR[] = { Vector3(0.5f - STYLE_3_CHILD_SIZE_FACTOR[0] - STYLE_3_CHILD_GAP_FACTOR * 0.5f,
                                                              0.5f - STYLE_3_CHILD_SIZE_FACTOR[0] - STYLE_3_CHILD_GAP_FACTOR * 0.5f + STYLE_3_CHILD_POSITION_OFFSET_Y,
                                                              STYLE_3_CHILD_OFFSET_Z),
                                                      Vector3(0.5f + STYLE_3_CHILD_GAP_FACTOR * 0.5f,
                                                              0.5f - STYLE_3_CHILD_SIZE_FACTOR[1] - STYLE_3_CHILD_GAP_FACTOR * 0.5f + STYLE_3_CHILD_POSITION_OFFSET_Y,
                                                              STYLE_3_CHILD_OFFSET_Z + 0.5f),
                                                      Vector3(0.5f + STYLE_3_CHILD_SIZE_FACTOR[3] + STYLE_3_CHILD_GAP_FACTOR * 1.5f,
                                                              0.5f + STYLE_3_CHILD_GAP_FACTOR * 0.5f + STYLE_3_CHILD_POSITION_OFFSET_Y,
                                                              STYLE_3_CHILD_OFFSET_Z + 1.0f),
                                                      Vector3(0.5f + STYLE_3_CHILD_GAP_FACTOR * 0.5f,
                                                              0.5f + STYLE_3_CHILD_GAP_FACTOR * 0.5f + STYLE_3_CHILD_POSITION_OFFSET_Y,
                                                              STYLE_3_CHILD_OFFSET_Z + 1.5f),
                                                      Vector3(0.5f - STYLE_3_CHILD_SIZE_FACTOR[4] - STYLE_3_CHILD_GAP_FACTOR * 0.5f,
                                                              0.5f + STYLE_3_CHILD_GAP_FACTOR * 0.5f + STYLE_3_CHILD_POSITION_OFFSET_Y,
                                                              STYLE_3_CHILD_OFFSET_Z + 2.0f),
                                                      Vector3(0.5f - STYLE_3_CHILD_SIZE_FACTOR[4] - STYLE_3_CHILD_SIZE_FACTOR[5] - STYLE_3_CHILD_GAP_FACTOR * 1.5f,
                                                              0.5f + STYLE_3_CHILD_GAP_FACTOR * 0.5f + STYLE_3_CHILD_POSITION_OFFSET_Y,
                                                              STYLE_3_CHILD_OFFSET_Z + 2.5f) };

const Vector3 STYLE_3_TITLE_POSITION_FACTOR = Vector3(0.5f - STYLE_3_CHILD_SIZE_FACTOR[0] - STYLE_3_CHILD_GAP_FACTOR * 0.5f,
                                                          0.5f - STYLE_3_CHILD_SIZE_FACTOR[0] - STYLE_3_CHILD_GAP_FACTOR * 0.5f + STYLE_3_CHILD_POSITION_OFFSET_Y - STYLE_3_TITLE_SIZE_FACTOR.height + 0.02f,
                                                          0.0f);
const Vector3 STYLE_3_TITLE_POSITION_OFFSET = Vector3(0.0f, 0.0f, 8.0f);

// Cluster style four

const unsigned int STYLE_4_CHILDREN_NUMBER = 6;

const float STYLE_4_BACKGROUND_IMAGE_OFFSET_Z = 1.0f;

const float STYLE_4_CHILD_OFFSET_Z = 2.0f;
const float STYLE_4_CHILD_GAP_FACTOR = 0.03f;
const float STYLE_4_CHILD_SIZE_FACTOR[] = { 0.4f, 0.22f, 0.13f, 0.4f, 0.22f, 0.13f };
const Vector3 STYLE_4_CHILD_POSITION_FACTOR[] = { Vector3(0.5f - STYLE_4_CHILD_SIZE_FACTOR[0] * 0.9f,
                                                             0.5f - STYLE_4_CHILD_SIZE_FACTOR[0] - STYLE_4_CHILD_GAP_FACTOR * 0.5f,
                                                             STYLE_4_CHILD_OFFSET_Z),
                                                     Vector3(0.5f + STYLE_4_CHILD_SIZE_FACTOR[0] * 0.1f + STYLE_4_CHILD_GAP_FACTOR,
                                                             0.5f - STYLE_4_CHILD_SIZE_FACTOR[1] - STYLE_4_CHILD_GAP_FACTOR * 0.5f,
                                                             STYLE_4_CHILD_OFFSET_Z + 0.5f),
                                                     Vector3(0.5f + STYLE_4_CHILD_SIZE_FACTOR[0] * 0.1f + STYLE_4_CHILD_SIZE_FACTOR[1] + STYLE_4_CHILD_GAP_FACTOR * 2.0f,
                                                             0.5f - STYLE_4_CHILD_SIZE_FACTOR[2] - STYLE_4_CHILD_GAP_FACTOR * 0.5f,
                                                             STYLE_4_CHILD_OFFSET_Z + 1.0f),
                                                     Vector3(0.5f - STYLE_4_CHILD_SIZE_FACTOR[3] * 0.1f,
                                                             0.5f + STYLE_4_CHILD_GAP_FACTOR * 0.5f,
                                                             STYLE_4_CHILD_OFFSET_Z + 1.5f),
                                                     Vector3(0.5f - STYLE_4_CHILD_SIZE_FACTOR[3] * 0.1f - STYLE_4_CHILD_SIZE_FACTOR[4] - STYLE_4_CHILD_GAP_FACTOR,
                                                             0.5f + STYLE_4_CHILD_GAP_FACTOR * 0.5f,
                                                             STYLE_4_CHILD_OFFSET_Z + 2.0f),
                                                     Vector3(0.5f - STYLE_4_CHILD_SIZE_FACTOR[3] * 0.1f - STYLE_4_CHILD_SIZE_FACTOR[4] - STYLE_4_CHILD_SIZE_FACTOR[5] - STYLE_4_CHILD_GAP_FACTOR * 2.0f,
                                                             0.5f + STYLE_4_CHILD_GAP_FACTOR * 0.5f,
                                                             STYLE_4_CHILD_OFFSET_Z + 2.5f) };

const Vector3 STYLE_4_TITLE_SIZE_FACTOR = Vector3(0.3f, 0.11f, 1.0f);
const Vector3 STYLE_4_TITLE_POSITION_FACTOR = Vector3(0.5f - STYLE_4_CHILD_SIZE_FACTOR[0] * 0.9f,
                                                         0.5f - STYLE_4_CHILD_SIZE_FACTOR[0] - STYLE_4_CHILD_GAP_FACTOR * 0.5f - STYLE_4_TITLE_SIZE_FACTOR.height + 0.02f,
                                                         0.0f);
const Vector3 STYLE_4_TITLE_POSITION_OFFSET = Vector3(0.0f, 0.0f, 8.0f);
const unsigned int CLUSTER_RANDOM_SEED(0x17eac9f3);         ///< Random seed for cluster data.

const int STYLE_RANDOM_CHILDREN_NUMBER = 16;

Vector3 FirstOrderEquation( const Vector3& source, const Vector3& relative, const Vector3& offset = Vector3::ZERO  )
{
  return source * relative + offset;
}

// random data generator //////////////////////////////////////////////////////

const unsigned int GEN_RAND_CONST = 0x15d9a373;

unsigned int genRandom(unsigned int& seed, unsigned int offset)
{
  unsigned int shft = offset&31;

  offset++;

  seed^= (seed << (shft) | seed >> (32 - shft)) * (offset * GEN_RAND_CONST);

  return seed;
}

float genRandomFloat(unsigned int& seed, unsigned int offset, float min, float max)
{
  const float f = static_cast<float>(genRandom(seed, offset)) / 0xffffffff;
  return f * (max - min) + min;
}

} // unnamed namespace

namespace Toolkit
{

namespace Internal
{

// ClusterStyle ///////////////////////////////////////////////////////////////

ClusterStyle::ClusterStyle()
: mMaxChildren(0),
  mTitlePositionRelative(Vector3::ONE),
  mTitlePositionOffset(Vector3::ZERO),
  mTitleSize(Vector3::ONE),
  mBackgroundPositionRelative(Vector3::ONE),
  mBackgroundPositionOffset(Vector3::ZERO),
  mBackgroundSize(Vector3::ONE),
  mClusterSize( Vector3::ZERO )
{
}

ClusterStyle::~ClusterStyle()
{

}

void ClusterStyle::SetMaximumNumberOfChildren(unsigned int maxChildren)
{
  mMaxChildren = maxChildren;
}

unsigned int ClusterStyle::GetMaximumNumberOfChildren() const
{
  return mMaxChildren;
}

void ClusterStyle::ApplyStyleToBackground(Actor background, AlphaFunction alpha, const TimePeriod& durationSeconds)
{
  Apply( background,
         FirstOrderEquation( GetClusterSize(), mBackgroundPositionRelative, mBackgroundPositionOffset ),
         FirstOrderEquation( GetClusterSize(), mBackgroundSize ),
         alpha,
         durationSeconds);
}

void ClusterStyle::ApplyStyleToTitle(Actor title, AlphaFunction alpha, const TimePeriod& durationSeconds)
{
  Apply( title,
         FirstOrderEquation( GetClusterSize(), mTitlePositionRelative, mTitlePositionOffset ),
         FirstOrderEquation( GetClusterSize(), mTitleSize ),
         alpha,
         durationSeconds);
}

void ClusterStyle::SetTitleProperties(const Vector3& relativePosition,
                                      const Vector3& offsetPosition,
                                      const Vector3& size)
{
  mTitlePositionRelative = relativePosition;
  mTitlePositionOffset = offsetPosition;
  mTitleSize = size;
}

void ClusterStyle::SetBackgroundProperties(const Vector3& relativePosition,
                                           const Vector3& offsetPosition,
                                           const Vector3& size)
{
  mBackgroundPositionRelative = relativePosition;
  mBackgroundPositionOffset = offsetPosition;
  mBackgroundSize = size;
}

void ClusterStyle::SetClusterSize( const Vector3& clusterSize )
{
  mClusterSize = clusterSize;
}

Vector3 ClusterStyle::GetClusterSize() const
{
  return mClusterSize;
}

void ClusterStyle::Apply( Actor actor,
                          const Vector3& position,
                          const Vector3& size,
                          AlphaFunction alpha,
                          const TimePeriod& durationSeconds)
{
  float animationDuration = durationSeconds.delaySeconds + durationSeconds.durationSeconds;
  if( animationDuration > 0.f )
  {
    Animation animation = Animation::New(animationDuration);
    animation.MoveTo( actor, position, alpha, durationSeconds.delaySeconds, durationSeconds.durationSeconds );
    animation.Resize( actor, size, alpha, durationSeconds.delaySeconds, durationSeconds.durationSeconds );
    animation.Play();
  }
  else
  {
    actor.SetPosition( position );
    actor.SetSize( size );
  }
}

void ClusterStyle::Apply( Actor actor,
                          const Vector3& position,
                          const Vector3& size,
                          const Quaternion& rotation,
                          const Vector3& scale,
                          AlphaFunction alpha,
                          const TimePeriod& durationSeconds)
{
  float animationDuration = durationSeconds.delaySeconds + durationSeconds.durationSeconds;
  if( animationDuration > 0.f )
  {
    Animation animation = Animation::New(animationDuration);
    animation.MoveTo( actor, position, alpha, durationSeconds.delaySeconds, durationSeconds.durationSeconds );
    animation.Resize( actor, size, alpha, durationSeconds.delaySeconds, durationSeconds.durationSeconds );
    animation.RotateTo( actor, rotation, alpha, durationSeconds.delaySeconds, durationSeconds.durationSeconds );
    animation.ScaleTo( actor, scale, alpha, durationSeconds.delaySeconds, durationSeconds.durationSeconds );
    animation.Play();
  }
  else
  {
    actor.SetPosition( position );
    actor.SetSize( size );
    actor.SetRotation( rotation );
    actor.SetScale( scale );
  }
}

// ClusterStyleStandard ///////////////////////////////////////////////////////

ClusterStylePtr ClusterStyleStandard::New(StyleType style)
{
  ClusterStylePtr impl( new ClusterStyleStandard(style) );

  return impl;
}

ClusterStyleStandard::ClusterStyleStandard(StyleType style)
: ClusterStyle(),
  mSizes(NULL),
  mPositions(NULL)
{
  switch(style)
  {
    case Toolkit::ClusterStyleStandard::ClusterStyle1:
    {
      SetMaximumNumberOfChildren(STYLE_1_CHILDREN_NUMBER);
      SetSizes(STYLE_1_CHILD_SIZE_FACTOR);
      SetPositions(STYLE_1_CHILD_POSITION_FACTOR);
      SetTitleProperties(STYLE_1_TITLE_POSITION_FACTOR,
                         STYLE_1_TITLE_POSITION_OFFSET,
                         STYLE_1_TITLE_SIZE_FACTOR);
      SetBackgroundProperties(Vector3::ZERO,
                              Vector3(0.0f, 0.0f, STYLE_1_BACKGROUND_IMAGE_OFFSET_Z),
                              Vector3::ONE);
      break;
    }
    case Toolkit::ClusterStyleStandard::ClusterStyle2:
    {
      SetMaximumNumberOfChildren(STYLE_2_CHILDREN_NUMBER);
      SetSizes(STYLE_2_CHILD_SIZE_FACTOR);
      SetPositions(STYLE_2_CHILD_POSITION_FACTOR);
      SetTitleProperties(STYLE_2_TITLE_POSITION_FACTOR,
                         STYLE_2_TITLE_POSITION_OFFSET,
                         STYLE_2_TITLE_SIZE_FACTOR);
      SetBackgroundProperties(Vector3::ZERO,
                              Vector3(0.0f, 0.0f, STYLE_2_BACKGROUND_IMAGE_OFFSET_Z),
                              Vector3::ONE);
      break;
    }
    case Toolkit::ClusterStyleStandard::ClusterStyle3:
    {
      SetMaximumNumberOfChildren(STYLE_3_CHILDREN_NUMBER);
      SetSizes(STYLE_3_CHILD_SIZE_FACTOR);
      SetPositions(STYLE_3_CHILD_POSITION_FACTOR);
      SetTitleProperties(STYLE_3_TITLE_POSITION_FACTOR,
                         STYLE_3_TITLE_POSITION_OFFSET,
                         STYLE_3_TITLE_SIZE_FACTOR);
      SetBackgroundProperties(Vector3::ZERO,
                              Vector3(0.0f, 0.0f, STYLE_3_BACKGROUND_IMAGE_OFFSET_Z),
                              Vector3::ONE);
      break;
    }
    case Toolkit::ClusterStyleStandard::ClusterStyle4:
    {
      SetMaximumNumberOfChildren(STYLE_4_CHILDREN_NUMBER);
      SetSizes(STYLE_4_CHILD_SIZE_FACTOR);
      SetPositions(STYLE_4_CHILD_POSITION_FACTOR);
      SetTitleProperties(STYLE_4_TITLE_POSITION_FACTOR,
                         STYLE_4_TITLE_POSITION_OFFSET,
                         STYLE_4_TITLE_SIZE_FACTOR);
      SetBackgroundProperties(Vector3::ZERO,
                              Vector3(0.0f, 0.0f, STYLE_4_BACKGROUND_IMAGE_OFFSET_Z),
                              Vector3::ONE);
      break;
    }
    default:
    {
      DALI_ASSERT_ALWAYS(false && "Invalid Style");
      break;
    }
  } // end switch
}

void ClusterStyleStandard::SetSizes(const float *sizes)
{
  mSizes = sizes;
}

void ClusterStyleStandard::SetPositions(const Vector3 *positions)
{
  mPositions = positions;
}

void ClusterStyleStandard::ApplyStyle(Actor child, unsigned int index, AlphaFunction alpha, const TimePeriod& durationSeconds)
{
  if(mPositions)
  {
    const float& size = mSizes[index];
    // counter top-left parent origin and top-left anchor point.
    const Vector3 position = mPositions[index] - Vector3(0.5f, 0.5f, 0.0f) + Vector3(size, size, 0.0f) * 0.5f;

    Apply( child,
           FirstOrderEquation( GetClusterSize(), Vector3(position.x, position.y, 0.0f), Vector3(0.0f, 0.0f, position.z) ),
           FirstOrderEquation( GetClusterSize(), Vector3::ONE * size ),
           Quaternion(),
           Vector3::ONE,
           alpha,
           durationSeconds);
  }
}

// ClusterStyleRandom /////////////////////////////////////////////////////////

ClusterStylePtr ClusterStyleRandom::New()
{
  ClusterStylePtr impl( new ClusterStyleRandom() );

  return impl;
}

ClusterStyleRandom::ClusterStyleRandom()
: ClusterStyle()
{
  SetMaximumNumberOfChildren(STYLE_RANDOM_CHILDREN_NUMBER);
  SetTitleProperties(Vector3::ZERO,
                     Vector3::ZERO,
                     Vector3::ONE);
  SetBackgroundProperties(Vector3::ZERO,
                          Vector3(0.0f, 0.0f, 0.0f),
                          Vector3::ONE);
}

void ClusterStyleRandom::ApplyStyle(Actor child, unsigned int index, AlphaFunction alpha, const TimePeriod& durationSeconds)
{
  unsigned int seed = CLUSTER_RANDOM_SEED;
  const float size = 0.5f;
  const float rotation = genRandomFloat(seed, index, -1.0f, 1.0f) * Math::PI * 0.1; // +/- 18 degrees
  const Vector3 position(genRandomFloat(seed, index, -0.1f, 0.1f),
                         genRandomFloat(seed, index, -0.1f, 0.1f),
                         0.0f);

  Property::Index depthProperty = child.GetPropertyIndex(Toolkit::Cluster::CLUSTER_ACTOR_DEPTH);
  float depthPropertyValue = child.GetProperty<float>( depthProperty );

  Apply( child,
         FirstOrderEquation( GetClusterSize(), Vector3(position.x, position.y, 0.0f), Vector3(0.0f, 0.0f, depthPropertyValue) ),
         FirstOrderEquation( GetClusterSize(), Vector3::ONE * size),
         Quaternion(rotation, Vector3::ZAXIS),
         Vector3::ONE,
         alpha,
         durationSeconds);
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
