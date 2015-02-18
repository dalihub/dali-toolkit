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

#include <dali-toolkit/public-api/controls/cluster/cluster.h>

// INTERNAL INCLUDES

#include <dali-toolkit/internal/controls/cluster/cluster-impl.h>

namespace Dali
{

namespace Toolkit
{

///////////////////////////////////////////////////////////////////////////////////////////////////
// Cluster
///////////////////////////////////////////////////////////////////////////////////////////////////

const std::string Cluster::CLUSTER_ACTOR_DEPTH( "cluster-actor-depth" );

Cluster::Cluster()
{
}

Cluster::Cluster(const Cluster& cluster)
: Control(cluster)
{
}

Cluster& Cluster::operator =(const Cluster& cluster)
{
  if( &cluster != this )
  {
    Control::operator=( cluster );
  }
  return *this;
}

Cluster::~Cluster()
{
}

Cluster Cluster::New( ClusterStyle& style )
{
  return Internal::Cluster::New(style);
}

Cluster Cluster::DownCast( BaseHandle handle )
{
  return Control::DownCast<Cluster, Internal::Cluster>(handle);
}

void Cluster::AddChild( Actor child )
{
  GetImpl(*this).AddChild( child );
}

void Cluster::AddChild( Actor child, unsigned int positionIndex )
{
  GetImpl(*this).AddChild( child, positionIndex);
}

void Cluster::AddChildAt( Actor child, unsigned int index )
{
  GetImpl(*this).AddChildAt( child, index );
}

void Cluster::AddChildAt( Actor child, unsigned int positionIndex, unsigned int index )
{
  GetImpl(*this).AddChildAt( child, positionIndex, index );
}

Actor Cluster::GetChildAt( unsigned int index )
{
  return GetImpl(*this).GetChildAt(index);
}

Actor Cluster::RemoveChildAt( unsigned int index )
{
  return GetImpl(*this).RemoveChildAt(index);
}

void Cluster::ExpandChild( unsigned int index )
{
  GetImpl(*this).ExpandChild(index);
}

void Cluster::ExpandAllChildren()
{
  GetImpl(*this).ExpandAllChildren();
}

void Cluster::CollapseChild( unsigned int index, bool front )
{
  GetImpl(*this).CollapseChild(index, front);
}

void Cluster::CollapseAllChildren( bool front )
{
  GetImpl(*this).CollapseAllChildren( front );
}

void Cluster::TransformChild( unsigned int index, const Vector3& position, const Vector3& scale, const Quaternion& rotation, AlphaFunction alpha, const TimePeriod& period )
{
  GetImpl(*this).TransformChild( index, position, scale, rotation, alpha, period );
}

void Cluster::RestoreChild( unsigned int index, AlphaFunction alpha, const TimePeriod& period, bool front )
{
  GetImpl(*this).RestoreChild( index, alpha, period, front );
}

void Cluster::SetBackgroundImage( Actor image )
{
  GetImpl(*this).SetBackgroundImage(image);
}

void Cluster::SetTitle( Actor text )
{
  GetImpl(*this).SetTitle(text);
}

void Cluster::SetStyle(ClusterStyle style)
{
  GetImpl(*this).SetStyle(style);
}

ClusterStyle Cluster::GetStyle() const
{
  return GetImpl(*this).GetStyle();
}

unsigned int Cluster::GetExpandedCount() const
{
  return GetImpl(*this).GetExpandedCount();
}

unsigned int Cluster::GetTotalCount() const
{
  return GetImpl(*this).GetTotalCount();
}

Cluster::Cluster( Internal::Cluster& impl )
: Control( impl )
{
}

Cluster::Cluster( Dali::Internal::CustomActor* internal )
: Control( internal )
{
  VerifyCustomActorPointer<Internal::Cluster>(internal);
}

} // namespace Toolkit

} // namespace Dali
