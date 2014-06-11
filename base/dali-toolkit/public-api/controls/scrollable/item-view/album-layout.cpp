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

#include <algorithm>
#include <dali-toolkit/public-api/controls/scrollable/item-view/album-layout.h>

using namespace Dali;
using namespace Dali::Toolkit;
using namespace std;

namespace // unnamed namespace
{
const float DEFAULT_SCROLL_SPEED_FACTOR = 0.005f;
const float DEFAULT_MAXIMUM_SWIPE_SPEED = 3.0f;
const float DEFAULT_ITEM_FLICK_ANIMATION_DURATION = 0.25f;

const float SELECTED_RIGHT = 2.5f;
const float SELECTED_LEFT = 3.5f;
const float SELECTED_CENTER = 3.0f;

const float LAYOUT_POSITION_NAGATIVE_1 = -1.0f;
const float LAYOUT_POSITION_0 = 0.0f;
const float LAYOUT_POSITION_2 = 2.0f;
const float LAYOUT_POSITION_3 = 3.0f;
const float LAYOUT_POSITION_4 = 4.0f;
const float LAYOUT_POSITION_6 = 6.0f;
const float LAYOUT_POSITION_7 = 7.0f;

const Vector3 POSITION_0 = Vector3(850.0f,-250.0f,0.0f);
const Vector3 POSITION_1 = Vector3(700.0f,50.0f,0.0f);
const Vector3 POSITION_2 = Vector3(440.0f,227.0f,0.0f);
const Vector3 POSITION_4 = Vector3(-440.0f,227.0f,0.0f);
const Vector3 POSITION_5 = Vector3(-700.0f,50.0f,0.0f);
const Vector3 POSITION_6 = Vector3(-850.0f,-250.0f,0.0f);

const float ROTATION_0 = Math::PI/6.0f;
const float ROTATION_1 = 0.0f;
const float ROTATION_2 = Math::PI/6.0f;
const float ROTATION_4 = -Math::PI/6.0f;
const float ROTATION_5 = 0.0f;
const float ROTATION_6 = -Math::PI/6.0f;

const float SCALE = 1.0f;

const Vector2 COLOR = Vector2(1.0f,1.0f);

const Vector3 SELECTED_ITEM_POSITION = Vector3( 0.0f,-80.0f,140.0f);
const float SELECETED_ITEM_SCALE = 1.72f;
const Vector2 SELECTED_ITEM_COLOR = Vector2(1.0f,1.0f);
const Vector3 VIRTUAL_ITEM_POSITION_RIGHT = Vector3( 280.0f,130.0f,130.0f);
const Vector3 VIRTUAL_ITEM_POSITION_LEFT = Vector3( -280.0f,130.0f,130.0f);
const float ROTATION_X = Math::PI/4.0f;
const float SCALE_RIGHT = 1.0f;
const float SCALE_LEFT = 1.0f;
const Vector2 COLOR_RIGHT = Vector2(1.0f,1.0f);
const Vector2 COLOR_LEFT = Vector2(1.0f,1.0f);
const Vector3 POSITION_RIGHT = Vector3(710.0f,-450.0f,0.0f);
const Vector3 POSITION_LEFT = Vector3(-710.0f,-450.0f,0.0f);
const float ROTATION_RIGHT = -Math::PI / 6.0f;
const float ROTATION_LEFT = Math::PI / 6.0f;

const float ALBUM_HIGHT = 7.0f;
const float ALPHA = 1.1f;
const float ALPHA_OF_SIZE = 0.35f;
const float LINE_OF_BOTTOM = 360.0f;

const float CHANCE_OF_RANDOM_ROTATION_OF_STACK = 0.5f;
const float RANGE_OF_RANDOM_ROTATION_OF_STACK = 0.5f;

const float THRESHHOLD_OF_MOVING = 0.02f;
const int NUM_OF_FRAME_FILTERED = 5;

const unsigned int SPREAD_ITEM_NUM = 6;

typedef enum
{
 SCROLL_LEFT = 1,
 SCROLL_NONE = 0,
 SCROLL_RIGHT = -1
}ScrollDirection;

struct DefaultItemSizeFunction
{
  Vector3 operator()(const Vector3& layoutSize)
  {
    float width = layoutSize.height * ALPHA_OF_SIZE;
    return Vector3(width, width, width);
  }
};

struct AlbumScaleConstraint
{
  AlbumScaleConstraint(const float scaleRight, const float scaleLeft, const float selectedItemScale, vector<float> scaleSpread)
  {
     mScaleRight = scaleRight;
     mScaleLeft = scaleLeft;
     mSelectedItemScale = selectedItemScale;

    if(scaleSpread.size() == SPREAD_ITEM_NUM)
    {
      mScaleVecSpread = scaleSpread;
    }
  }

  Vector3 operator()(const Vector3& current, const float& layoutPosition, const float& scrollSpeed, const Vector3& layoutSize)
  {
    int begin = 0;
    int end = 0;
    float beginScale = 0.0f;
    float endScale = 0.0f;
    float scale = 0.0f;
    float pos = layoutPosition + SELECTED_CENTER;

    if(pos <= LAYOUT_POSITION_NAGATIVE_1)/*items of right stack*/
    {
      scale = mScaleRight;
    }
    else if(pos > LAYOUT_POSITION_NAGATIVE_1 && pos < LAYOUT_POSITION_0)/*items between -1.0f and 0.0f*/
    {
      beginScale = mScaleVecSpread.at(0);
      endScale = mScaleRight;

      scale = (endScale - beginScale) * fabs(pos) + beginScale;
    }
    else if(pos >= LAYOUT_POSITION_0 && pos < SELECTED_RIGHT)/*items between 0.0f and center*/
    {
      if(int(pos) < pos)
      {
        begin = int(pos);
        end = int(pos) + 1;

        beginScale = mScaleVecSpread.at(begin);
        endScale = mScaleVecSpread.at(end);

        scale = (endScale - beginScale) * fabs(pos - int(pos)) + beginScale;
      }
      else
      {
        scale = mScaleVecSpread.at(int(pos));
      }
    }
    else if(pos >= SELECTED_RIGHT && pos <= SELECTED_LEFT)/*items of center*/
    {
      scale = mSelectedItemScale;
    }
    else if(pos > SELECTED_LEFT && pos <= LAYOUT_POSITION_6)/*items between center and 6.0f*/
    {
      if(int(pos) < pos)
      {
        begin = int(pos)-1;
        end = int(pos);

        beginScale = mScaleVecSpread.at(begin);
        endScale = mScaleVecSpread.at(end);

        scale = (endScale - beginScale) * fabs(pos - int(pos)) + beginScale;
      }
      else
      {
        scale = mScaleVecSpread.at(int(pos)-1);
      }
    }
    else if(pos > LAYOUT_POSITION_6 && pos < LAYOUT_POSITION_7)/*items between 6.0f and 7.0f*/
    {
      beginScale = mScaleVecSpread.at(5);
      endScale = mScaleLeft;

      scale = (endScale - beginScale) * fabs(pos - int(pos)) + beginScale;
    }
    else if(pos >= LAYOUT_POSITION_7)/*items of left stack*/
    {
      scale = mScaleLeft;
    }
    else
    {
      scale = 1.0f;
    }

    return Vector3(scale,scale,1.0f);
  }

  float mScaleRight;
  float mScaleLeft;
  float mSelectedItemScale;
  vector<float> mScaleVecSpread;
};

Vector3 CalculatePosition(Vector3 pos, float rotateX)
{
  pos.z = pos.z - fabs(pos.y - LINE_OF_BOTTOM) * sinf(rotateX) / cosf(rotateX);
  return pos;
}

Vector3 CalculateStackPosition(Vector3 pos, float rotateX, int num, bool left)
{
  pos.z = pos.z - fabs(pos.y - LINE_OF_BOTTOM) * sinf(rotateX) / cosf(rotateX);

  if(left)
  {
    pos.x = pos.x + num * ALPHA;
  }
  else
  {
    pos.x = pos.x - num * ALPHA;
  }

  pos.y -= num * ALBUM_HIGHT * sinf(rotateX);
  pos.z += num * ALBUM_HIGHT * cosf(rotateX);

  return pos;
}

Vector3 GetPosition(float layoutPos, Vector3 posRight, Vector3 posLeft, Vector3 posSelectedItem, vector<Vector3> posVecSpread, float rotateX)
{
  int begin =0;
  int end = 0;

  float alpha = 0.0f;

  Vector3 beginPos =  Vector3::ZERO;
  Vector3 endPos = Vector3::ZERO;

  Vector3 pos = Vector3::ZERO;

  if(layoutPos <= LAYOUT_POSITION_NAGATIVE_1)/*items of right stack*/
  {
    if(int(layoutPos) > layoutPos)
    {
      begin = int(layoutPos);
      end = int(layoutPos)-1;

      beginPos = CalculateStackPosition(posRight, rotateX, int(LAYOUT_POSITION_NAGATIVE_1) - int(layoutPos),false);

      endPos = CalculateStackPosition(posRight, rotateX, - int(layoutPos),false);

      alpha = fabs(layoutPos - int(layoutPos));

      pos.x = (endPos.x - beginPos.x) * alpha + beginPos.x;
      pos.y = (endPos.y - beginPos.y) * alpha + beginPos.y;
      pos.z = (endPos.z - beginPos.z) * alpha + beginPos.z;

      return pos;
    }
    else
    {
      return CalculateStackPosition(posRight,rotateX,int(LAYOUT_POSITION_NAGATIVE_1) - int(layoutPos),false);
    }
  }
  else if(layoutPos < LAYOUT_POSITION_0 && layoutPos > LAYOUT_POSITION_NAGATIVE_1)/*items between -1.0f and 0.0f*/
  {
    beginPos = CalculatePosition(posVecSpread.at(int(LAYOUT_POSITION_0)),rotateX);
    endPos = CalculateStackPosition(posRight, rotateX, int(layoutPos),false);

    alpha = -layoutPos;

    pos.x = (endPos.x - beginPos.x) * alpha + beginPos.x;
    pos.y = (endPos.y - beginPos.y) * alpha + beginPos.y;
    pos.z = (endPos.z - beginPos.z) * alpha + beginPos.z;

    return pos;
  }
  else if(layoutPos >= LAYOUT_POSITION_0 && layoutPos <= LAYOUT_POSITION_2)/*items between 0.0f and 2.0f*/
  {
    if(int(layoutPos) < layoutPos)
    {
      begin = int(layoutPos);
      end = int(layoutPos) + 1;

      beginPos = posVecSpread.at(begin);
      endPos = posVecSpread.at(end);

      alpha = fabs(layoutPos - int(layoutPos));

      pos.x = (endPos.x - beginPos.x) * alpha + beginPos.x;
      pos.y = (endPos.y - beginPos.y) * alpha + beginPos.y;
      pos.z = (endPos.z - beginPos.z) * alpha + beginPos.z;
    }
    else
    {
      pos = posVecSpread.at(int(layoutPos));
    }
  }
  else if(layoutPos >LAYOUT_POSITION_2 && layoutPos<SELECTED_RIGHT)/*items between 2.0f and center*/
  {
    beginPos = posVecSpread.at(int(LAYOUT_POSITION_2));
    endPos = VIRTUAL_ITEM_POSITION_RIGHT;

    alpha = (layoutPos - LAYOUT_POSITION_2) / (SELECTED_RIGHT - LAYOUT_POSITION_2);

    pos.x = (endPos.x - beginPos.x) * alpha + beginPos.x;
    pos.y = (endPos.y - beginPos.y) * alpha + beginPos.y;
    pos.z = (endPos.z - beginPos.z) * alpha + beginPos.z;
  }
  else if(layoutPos > SELECTED_LEFT && layoutPos < LAYOUT_POSITION_4)/*items between center and 4.0f*/
  {
    beginPos = posVecSpread.at(int(LAYOUT_POSITION_3));
    endPos = VIRTUAL_ITEM_POSITION_LEFT;

    alpha = (LAYOUT_POSITION_4 - layoutPos) / (LAYOUT_POSITION_4 - SELECTED_LEFT);

    pos.x = (endPos.x - beginPos.x) * alpha + beginPos.x;
    pos.y = (endPos.y - beginPos.y) * alpha + beginPos.y;
    pos.z = (endPos.z - beginPos.z) * alpha + beginPos.z;
  }
  else if(layoutPos >= LAYOUT_POSITION_4 && layoutPos <= LAYOUT_POSITION_6)/*items between 4.0f and 6.0f*/
  {
    if(int(layoutPos) < layoutPos)
    {
      begin = int(layoutPos);
      end = int(layoutPos) + 1;

      beginPos = posVecSpread.at(begin - 1);
      endPos = posVecSpread.at(end - 1);

      alpha = fabs(layoutPos - int(layoutPos));

      pos.x = (endPos.x - beginPos.x) * alpha + beginPos.x;
      pos.y = (endPos.y - beginPos.y) * alpha + beginPos.y;
      pos.z = (endPos.z - beginPos.z) * alpha + beginPos.z;
    }
    else
    {
      pos = posVecSpread.at(int(layoutPos) -1);
    }
  }
  else if(layoutPos > LAYOUT_POSITION_6 && layoutPos < LAYOUT_POSITION_7)/*items between 6.0f and 7.0f*/
  {
    beginPos = CalculatePosition(posVecSpread.at(int(LAYOUT_POSITION_6) - 1),rotateX);
    endPos = CalculateStackPosition(posLeft, rotateX, int(layoutPos) + 1 - int(LAYOUT_POSITION_7),true);

    alpha = fabs(layoutPos - int(layoutPos));

    pos.x = (endPos.x - beginPos.x) * alpha + beginPos.x;
    pos.y = (endPos.y - beginPos.y) * alpha + beginPos.y;
    pos.z = (endPos.z - beginPos.z) * alpha + beginPos.z;

    return pos;
  }
  else if(layoutPos >= LAYOUT_POSITION_7)/*items of left stack*/
  {
    if(int(layoutPos) < layoutPos)
    {
      begin = int(layoutPos);
      end = int(layoutPos) + 1;

      beginPos = CalculateStackPosition(posLeft, rotateX, int(layoutPos) - int(LAYOUT_POSITION_7),true);

      endPos = CalculateStackPosition(posLeft, rotateX, int(layoutPos) + 1 - int(LAYOUT_POSITION_7),true);

      alpha = fabs(layoutPos - int(layoutPos));

      pos.x = (endPos.x - beginPos.x) * alpha + beginPos.x;
      pos.y = (endPos.y - beginPos.y) * alpha + beginPos.y;
      pos.z = (endPos.z - beginPos.z) * alpha + beginPos.z;

      return pos;
    }
    else
    {
      return CalculateStackPosition(posLeft, rotateX, int(layoutPos) - int(LAYOUT_POSITION_7),true);
    }
  }

  return CalculatePosition(pos,rotateX);
}

struct AlbumPositionConstraint
{
  AlbumPositionConstraint(const Vector3 posRight, const Vector3 posLeft, const Vector3 posSelectedItem, const vector<Vector3> posVecSpread, float rotateX)
  {
    mPositionRight = posRight;
    mPositionLeft = posLeft;
    mSelectedItemPosition = posSelectedItem;
    mRotationX = rotateX;

    if(posVecSpread.size() == SPREAD_ITEM_NUM)
    {
      mPositionVecSpread = posVecSpread;
    }
  }

  Vector3 operator()(const Vector3& current, const float& layoutPosition, const float& scrollSpeed, const Vector3& layoutSize)
  {
    float pos = layoutPosition + SELECTED_CENTER;

    /*handle if the item is selected item(in the center)*/
    if(pos >= SELECTED_RIGHT && pos <= SELECTED_LEFT)
    {
      return SELECTED_ITEM_POSITION;
    }

    /*get the spread items position*/
    return GetPosition(pos,mPositionRight,mPositionLeft,mSelectedItemPosition,mPositionVecSpread,mRotationX);
  }

  Vector3 mPositionRight;
  Vector3 mPositionLeft;
  Vector3 mSelectedItemPosition;
  vector<Vector3> mPositionVecSpread;
  float mRotationX;
 };

Quaternion GetRotation(float layoutPos, vector<float> rotateVecStack, vector<float> rotateVecSpread, float rotateX)
{
  int begin =0;
  int end = 0;

  float alpha = 0.0f;

  float beginRotation = 0.0f;
  float endRotation = 0.0f;

  float rotation = 0.0f;
  if(layoutPos <= LAYOUT_POSITION_NAGATIVE_1)/*items of right stack*/
  {
    if(int(layoutPos) > layoutPos)
    {
      begin = int(layoutPos) + 1;
      end = int(layoutPos);

      begin *= -1;
      end *= -1;

      beginRotation = rotateVecStack.at(begin);
      endRotation = rotateVecStack.at(end);

      alpha = fabs(layoutPos - int(layoutPos));

      rotation = (endRotation - beginRotation) * alpha + beginRotation;
    }
    else
    {
      rotation = rotateVecStack.at(-int(layoutPos)-1);
    }
  }
  else if(layoutPos > LAYOUT_POSITION_NAGATIVE_1 && layoutPos < LAYOUT_POSITION_0)/*items between -1.0f and 0.0f*/
  {
    begin = 0;
    end = 0;

    beginRotation = rotateVecSpread.at(begin);
    endRotation = rotateVecStack.at(end);

    alpha = fabs(layoutPos);

    rotation = (endRotation - beginRotation) * alpha + beginRotation;
  }
  else if(layoutPos >= LAYOUT_POSITION_0 && layoutPos < LAYOUT_POSITION_3)
  {
    if(int(layoutPos) < layoutPos)
    {
      begin = int(layoutPos);
      end = int(layoutPos) + 1;

      beginRotation = rotateVecSpread.at(begin);
      endRotation = rotateVecSpread.at(end);

      alpha = fabs(layoutPos - int(layoutPos));

      rotation = (endRotation - beginRotation) * alpha + beginRotation;
    }
    else
    {
      rotation = rotateVecSpread.at(int(layoutPos));
    }
  }
  else if(layoutPos > LAYOUT_POSITION_3 && layoutPos <= LAYOUT_POSITION_6)
  {
    if(int(layoutPos) < layoutPos)
    {
      begin = int(layoutPos) - 1;
      end = int(layoutPos);

      beginRotation = rotateVecSpread.at(begin);
      endRotation = rotateVecSpread.at(end);

      alpha = fabs(layoutPos - int(layoutPos));

      rotation = (endRotation - beginRotation) * alpha + beginRotation;
    }
    else
    {
      rotation = rotateVecSpread.at(int(layoutPos)-1);
    }
  }
  else if(layoutPos > LAYOUT_POSITION_6 && layoutPos < LAYOUT_POSITION_7)
  {
    begin = 5;
    end = 0;

    beginRotation = rotateVecSpread.at(begin);
    endRotation = rotateVecStack.at(end);

    alpha = fabs(layoutPos - int(LAYOUT_POSITION_6));

    rotation = (endRotation - beginRotation) * alpha + beginRotation;
  }
  else if(layoutPos >= LAYOUT_POSITION_7)
  {
    if(int(layoutPos) < layoutPos)
    {
      begin = int(layoutPos) - int(LAYOUT_POSITION_7);
      end = int(layoutPos) - int(LAYOUT_POSITION_7) + 1;

      beginRotation = rotateVecStack.at(begin);
      endRotation = rotateVecStack.at(end);

      alpha = fabs(layoutPos - int(layoutPos));

      rotation = (endRotation - beginRotation) * alpha + beginRotation;
    }
    else
    {
      rotation = rotateVecStack.at(int(layoutPos)-int(LAYOUT_POSITION_7));
    }
  }

  return Quaternion(rotateX, Vector3::XAXIS) * Quaternion(rotation, Vector3::ZAXIS);
}

struct AlbumRotationConstraint
{
  AlbumRotationConstraint(const vector<float> rotatVecSpread, const vector<float> rotatVecStack, const float rotateX, int num)
  {
    mRotationX = rotateX;
    mNumOfItems = num;
    mIndex = 0;
    mLastIndex = 0;
    mLeft = SCROLL_NONE;
    mLastPosition = 0.0f;
    mTimes = 0;

    if(rotatVecSpread.size() == SPREAD_ITEM_NUM)
    {
      mRotationVecSpread = rotatVecSpread;
    }

    mRotationVecStack = rotatVecStack;
  }

  Quaternion operator()(const Quaternion& current, const float& layoutPosition, const float& scrollSpeed, const Vector3& layoutSize)
  {
    float pos = layoutPosition + SELECTED_CENTER;

    if(mIndex == mNumOfItems)
    {
      mIndex = 0;
    }

    mIndex ++;

    if(mLastIndex != mIndex)
    {
      mLastIndex = mIndex;

      if(mLeft == SCROLL_RIGHT)
      {
        if(pos > mLastPosition + THRESHHOLD_OF_MOVING)
        {
          mTimes = 0;
          mLeft = SCROLL_LEFT;
        }
        else if(pos < mLastPosition)
        {
          mTimes = 0;
          mLeft = SCROLL_RIGHT;
        }
        else
        {
          mTimes++;
          if(mTimes > NUM_OF_FRAME_FILTERED)
          {
            mTimes = 0;
            mLeft = SCROLL_NONE;
          }
        }
      }
      else if(mLeft == SCROLL_LEFT)
      {
        if(pos > mLastPosition)
        {
          mTimes = 0;
          mLeft = SCROLL_LEFT;
        }
        else if(pos < mLastPosition - THRESHHOLD_OF_MOVING)
        {
          mTimes = 0;
          mLeft = SCROLL_RIGHT;
        }
        else
        {
          mTimes++;
          if(mTimes > NUM_OF_FRAME_FILTERED)
          {
            mTimes = 0;
            mLeft = SCROLL_NONE;
          }
        }
      }
      else
      {
        if(pos < mLastPosition)
        {
          mLeft = SCROLL_RIGHT;
        }
        else if(pos > mLastPosition)
        {
          mLeft = SCROLL_LEFT;
        }
        else
        {
          mLeft = SCROLL_NONE;
        }
      }

      mLastPosition = pos;

     /*rotation for the selected item(center)*/
     if(pos >= SELECTED_RIGHT && pos < SELECTED_LEFT)
     {
       if(mLeft == SCROLL_LEFT)
       {
         return Quaternion(-fabs(SELECTED_CENTER - pos), Vector3::YAXIS);
       }
       else if(mLeft == SCROLL_RIGHT)
       {
         return Quaternion(fabs(pos - SELECTED_CENTER), Vector3::YAXIS);
       }
       else
       {
         return Quaternion(0.0f, Vector3::YAXIS);
       }
     }
    }

    /*rotation for the spread item*/
    return GetRotation(pos,mRotationVecStack,mRotationVecSpread,mRotationX);
  }

  vector<float> mRotationVecSpread;
  vector<float> mRotationVecStack;
  float mRotationX;
  Actor mScrollDirectionActor;
  int mLastIndex;
  int mIndex;
  int mNumOfItems;
  int mTimes;
  ScrollDirection mLeft;
  float mLastPosition;
};

struct AlbumColorConstraint
{
  AlbumColorConstraint(const Vector2 colorRight, const Vector2 colorLeft, const Vector2 colorSelectedItem, const vector<Vector2> spreadVecColor, const int stackNum)
  {
    mColorRight = colorRight;
    mColorLeft = colorLeft;
    mSelectedItemColor = colorSelectedItem;
    mStackNum = stackNum;

    if(spreadVecColor.size() == SPREAD_ITEM_NUM)
    {
      mColorVecSpread = spreadVecColor;
    }
  }

  Vector4 operator()(const Vector4& current, const float& layoutPosition, const float& scrollSpeed, const Vector3& layoutSize)
  {
    float black = 1.0f;
    Vector4 color = current;
    float pos = layoutPosition + SELECTED_CENTER;
    Vector2 temp = Vector2(0.0f,0.0f);

    int begin = 0;
    int end = 0;

    Vector2 beginColor = Vector2(0.0f,0.0f);
    Vector2 endColor = Vector2(0.0f,0.0f);

    if(pos <= -mStackNum-1)
    {
      color.w = 0.0f;
      black = 0.0f;
    }
    else if(pos > -mStackNum-1 && pos < -mStackNum)
    {
      beginColor = mColorRight;
      endColor = Vector2(0.0f,0.0f);

      color.w = (endColor.x - beginColor.x) * fabs(pos - int(pos)) + beginColor.x;
      black = (endColor.y - beginColor.y) * fabs(pos - int(pos)) + beginColor.y;
    }
    else if(pos <= LAYOUT_POSITION_NAGATIVE_1 && pos >= -mStackNum)
    {
      color.w = mColorRight.x;
      black = mColorRight.y;
    }
    else if(pos > LAYOUT_POSITION_NAGATIVE_1 && pos < LAYOUT_POSITION_0)
    {
      beginColor = mColorVecSpread.at(int(LAYOUT_POSITION_0));
      endColor = mColorRight;

      color.w = (endColor.x - beginColor.x) * fabs(pos) + beginColor.x;
      black = (endColor.y - beginColor.y) * fabs(pos) + beginColor.y;
    }
    else if(pos >= LAYOUT_POSITION_0 && pos  <= LAYOUT_POSITION_2)
    {
      if(int(pos) < pos)
      {
        begin = int(pos);
        end = int(pos) + 1;

        beginColor = mColorVecSpread.at(begin);
        endColor = mColorVecSpread.at(end);

        temp = (endColor - beginColor) * fabs(pos - int(pos)) + beginColor;

        color.w = temp.x;
        black = temp.y;
      }
      else
      {
        beginColor = mColorVecSpread.at(int(pos));

        color.w = beginColor.x;
        black = beginColor.y;
      }
    }
    else if(pos  > LAYOUT_POSITION_2 && pos < SELECTED_RIGHT)/*items between 2.0f and center*/
    {
      beginColor = mColorVecSpread.at(int(LAYOUT_POSITION_2));
      endColor = Vector2(0.0f,0.0f);

      temp = (endColor - beginColor) * (pos - LAYOUT_POSITION_2)/(SELECTED_RIGHT - LAYOUT_POSITION_2) + beginColor;

      color.w = temp.x;
      black = temp.y;
    }
    else if(pos >= SELECTED_RIGHT && pos <= SELECTED_LEFT)/*items of center*/
    {
      color.w = mSelectedItemColor.x;
      black = mSelectedItemColor.y;
    }
    else if(pos  > SELECTED_LEFT && pos < LAYOUT_POSITION_4)/*items between center and 4.0f*/
    {
      beginColor = Vector2(0.0f,0.0f);
      endColor = mColorVecSpread.at(int(LAYOUT_POSITION_3));

      temp = (endColor - beginColor) * (pos - SELECTED_LEFT)/(LAYOUT_POSITION_4 - SELECTED_LEFT) + beginColor;

      color.w = temp.x;
      black = temp.y;
    }
    else if(pos >= LAYOUT_POSITION_4 && pos <= LAYOUT_POSITION_6)/*items between 4.0f and 6.0f*/
    {
      if(int(pos) < pos)
      {
        begin = int(pos) - 1;
        end = int(pos);

        beginColor = mColorVecSpread.at(begin);
        endColor = mColorVecSpread.at(end);

        temp = (endColor - beginColor) * fabs(pos - int(pos)) + beginColor;

        color.w = temp.x;
        black = temp.y;
      }
      else
      {
        beginColor = mColorVecSpread.at(int(pos) -  1);

        color.w = beginColor.x;
        black = beginColor.y;
      }
    }
    else if(pos > LAYOUT_POSITION_6 && pos < LAYOUT_POSITION_7)/*items between 6.0f and 7.0f*/
    {
      beginColor = mColorVecSpread.at(int(LAYOUT_POSITION_6) - 1);
      endColor = mColorLeft;

      color.w = (endColor.x - beginColor.x) * fabs(pos - int(pos)) + beginColor.x;
      black = (endColor.y - beginColor.y) * fabs(pos - int(pos)) + beginColor.y;
    }
    else if(pos >= LAYOUT_POSITION_7 && pos <= mStackNum + int(LAYOUT_POSITION_7))/*items of left stack*/
    {
      color.w = mColorLeft.x;
      black = mColorLeft.y;
    }
    else if(pos > mStackNum + int(LAYOUT_POSITION_7) && pos < mStackNum + int(LAYOUT_POSITION_7) + 1)
    {
      beginColor = mColorLeft;
      endColor = Vector2(0.0f,0.0f);

      color.w = (endColor.x - beginColor.x) * fabs(pos - int(pos)) + beginColor.x;
      black = (endColor.y - beginColor.y) * fabs(pos - int(pos)) + beginColor.y;
    }
    else if(pos >= mStackNum + int(LAYOUT_POSITION_7) +1)
    {
      color.w = 0.0f;
      black = 0.0f;
    }

    color.r = color.r * black;
    color.g = color.g * black;
    color.b = color.b * black;

    return color;
  }

  int mStackNum;
  Vector2 mColorRight;
  Vector2 mColorLeft;
  Vector2 mSelectedItemColor;
  vector<Vector2> mColorVecSpread;
};

struct AlbumVisibilityConstraintPortrait
{
  AlbumVisibilityConstraintPortrait()
  {
  }

  bool operator()(const bool& current, const float& layoutPosition, const float& scrollSpeed, const Vector3& layoutSize)
  {
    return true;
  }
};

} // unnamed namespace

namespace Dali
{

namespace Toolkit
{

struct AlbumLayout::Impl
{
  Impl()
  : mItemSizeFunction(DefaultItemSizeFunction()),
    mScrollSpeedFactor(DEFAULT_SCROLL_SPEED_FACTOR),
    mMaximumSwipeSpeed(DEFAULT_MAXIMUM_SWIPE_SPEED),
    mItemFlickAnimationDuration(DEFAULT_ITEM_FLICK_ANIMATION_DURATION),
    mNumOfItems(0)
  {
    /*Initialize the variables*/
    mSelectedItemScale = SELECETED_ITEM_SCALE;
    mRotationX = ROTATION_X;
    mScaleRight = SCALE_RIGHT;
    mScaleLeft = SCALE_LEFT;
    mRotationRight = ROTATION_RIGHT;
    mRotationLeft = ROTATION_LEFT;
    mSelectedItemColor = SELECTED_ITEM_COLOR;
    mSelectedItemPosition = SELECTED_ITEM_POSITION;
    mColorRight = COLOR_RIGHT;
    mColorLeft = COLOR_LEFT;
    mPositionRight = POSITION_RIGHT;
    mPositionLeft = POSITION_LEFT;
    mStackNum = 50;

    /*Initialize the position of spread items*/
    mPositionVecSpread.push_back(POSITION_0);
    mPositionVecSpread.push_back(POSITION_1);
    mPositionVecSpread.push_back(POSITION_2);
    mPositionVecSpread.push_back(POSITION_4);
    mPositionVecSpread.push_back(POSITION_5);
    mPositionVecSpread.push_back(POSITION_6);

    /*Initialize the rotation of spread items*/
    mRotationVecSpread.push_back(ROTATION_0);
    mRotationVecSpread.push_back(ROTATION_1);
    mRotationVecSpread.push_back(ROTATION_2);
    mRotationVecSpread.push_back(ROTATION_4);
    mRotationVecSpread.push_back(ROTATION_5);
    mRotationVecSpread.push_back(ROTATION_6);

    /*Initialize the scale of spread items*/
    for(unsigned int i=0; i<SPREAD_ITEM_NUM; i++)
    {
      mScaleVecSpread.push_back(SCALE);
    }

    /*Initialize the color of spread items*/
    for(unsigned int i=0; i<SPREAD_ITEM_NUM; i++)
    {
      mColorVecSpread.push_back(COLOR);
    }
  }

  void SetPosition(vector<Vector3> positionList)
  {
    if(positionList.size() == SPREAD_ITEM_NUM)
    {
      mPositionVecSpread = positionList;
    }
  }

  vector<Vector3> GetPosition() const
  {
    return mPositionVecSpread;
  }

  void SetColor(vector<Vector2> colorList)
  {
    if(colorList.size() == SPREAD_ITEM_NUM)
    {
      mColorVecSpread = colorList;
    }
  }

  vector<Vector2> GetColor() const
  {
    return mColorVecSpread;
  }

  void SetScale(vector<float> scaleList)
  {
    if(scaleList.size() == SPREAD_ITEM_NUM)
    {
      mScaleVecSpread = scaleList;
    }
  }

  vector<float> GetScale() const
  {
    return mScaleVecSpread;
  }

  void SetRotationX(float rotat_x)
  {
    mRotationX = rotat_x;
  }

  float GetRotationX() const
  {
    return mRotationX;
  }

  void SetRotationZ(vector<float> rotationList)
  {
    if(rotationList.size() == SPREAD_ITEM_NUM)
    {
      mRotationVecSpread = rotationList;
    }
  }

  vector<float> GetRotationZ() const
  {
    return mRotationVecSpread;
  }

  void SetCenterPosition(Vector3 pos)
  {
    mSelectedItemPosition = pos;
  }

  Vector3 GetCenterPosition() const
  {
    return mSelectedItemPosition;
  }

  void SetCenterScale(float scale)
  {
    mSelectedItemScale = scale;
  }

  float GetCenterScale() const
  {
    return mSelectedItemScale;
  }

  void SetCenterColor(Vector2 color)
  {
    mSelectedItemColor = color;
  }

  Vector2 GetCenterColor() const
  {
    return mSelectedItemColor;
  }

  void SetStackPosition(Vector3 rightPos, Vector3 leftPos)
  {
    mPositionRight = rightPos;
    mPositionLeft = leftPos;
  }

  Vector3 GetRightStackPosition() const
  {
    return mPositionRight;
  }

  Vector3 GetLeftStackPosition() const
  {
    return mPositionLeft;
  }

  void SetStackScale(float rightScale, float leftScale)
  {
    mScaleRight = rightScale;
    mScaleLeft = leftScale;
  }

  float GetRightStackScale() const
  {
    return mScaleRight;
  }

  float GetLeftStackScale() const
  {
    return mScaleLeft;
  }

  void SetStackColor(Vector2 rightColor, Vector2 leftColor)
  {
    mColorRight = rightColor;
    mColorLeft = leftColor;
  }

  Vector2 GetRightStackColor() const
  {
    return mColorRight;
  }

  Vector2 GetLeftStackColor() const
  {
    return mColorLeft;
  }

  void SetNumOfItems(int num)
  {
    mNumOfItems = num;

    /*Initialize the rotation of stack items*/
    for(int i=0; i<mNumOfItems; i++)
    {
      if(Random::Chance(CHANCE_OF_RANDOM_ROTATION_OF_STACK))
      {
        mRotationVecStack.push_back(Random::Range(-RANGE_OF_RANDOM_ROTATION_OF_STACK,RANGE_OF_RANDOM_ROTATION_OF_STACK));
      }
      else
      {
        mRotationVecStack.push_back(0.0f);
      }
    }
  }

  int GetNumOfItems() const
  {
    return mNumOfItems;
  }

  void SetStackNum(int num)
  {
    mStackNum = num;
  }

  int GetStackNum() const
  {
    return mStackNum;
  }

  ItemSizeFunction mItemSizeFunction;

  float mScrollSpeedFactor;
  float mMaximumSwipeSpeed;
  float mItemFlickAnimationDuration;

  Vector3 mSelectedItemPosition;/*position of selected item*/
  float mSelectedItemScale;/*scale of selected item*/
  Vector2 mSelectedItemColor;/*color of selected item*/

  float mRotationX;/*rotation around X*/

  vector<Vector3> mPositionVecSpread;/*positions of the spread items*/
  vector<float> mRotationVecSpread;/*rotations of the spread items*/
  vector<float> mScaleVecSpread;/*scales of the spread items*/
  vector<Vector2> mColorVecSpread;/*colors of the spread items*/

  vector<float> mRotationVecStack;/*rotations of the stack items*/

  float mRotationRight;/*rotation of right album stack*/
  float mRotationLeft;/*rotation of left album stack*/

  float mScaleRight;/*scale of right album stack*/
  float mScaleLeft;/*scale of left album stack*/

  Vector2 mColorRight;/*color of right album stack*/
  Vector2 mColorLeft;/*color of left album stack*/

  Vector3 mPositionRight;/*position of right album stack*/
  Vector3 mPositionLeft;/*position of left album stack*/

  int mNumOfItems;/*num of items*/
  int mStackNum;/*num of items of stack*/
};

AlbumLayoutPtr AlbumLayout::New()
{
  return AlbumLayoutPtr(new AlbumLayout());
}

AlbumLayout::~AlbumLayout()
{
  delete mImpl;
}

void AlbumLayout::SetItemSizeFunction(ItemSizeFunction function)
{
  mImpl->mItemSizeFunction = function;
}

AlbumLayout::ItemSizeFunction AlbumLayout::GetItemSizeFunction() const
{
  return mImpl->mItemSizeFunction;
}

void AlbumLayout::SetScrollSpeedFactor(float scrollSpeed)
{
  mImpl->mScrollSpeedFactor = scrollSpeed;
}

void AlbumLayout::SetMaximumSwipeSpeed(float speed)
{
  mImpl->mMaximumSwipeSpeed = speed;
}

void AlbumLayout::SetItemFlickAnimationDuration(float durationSeconds)
{
  mImpl->mItemFlickAnimationDuration = durationSeconds;
}

float AlbumLayout::GetScrollSpeedFactor() const
{
  return mImpl->mScrollSpeedFactor;
}

float AlbumLayout::GetMaximumSwipeSpeed() const
{
  return mImpl->mMaximumSwipeSpeed;
}

float AlbumLayout::GetItemFlickAnimationDuration() const
{
  return mImpl->mItemFlickAnimationDuration;
}

float AlbumLayout::GetMinimumLayoutPosition(unsigned int numberOfItems, Vector3 layoutSize) const
{
  return - static_cast<float>(numberOfItems) + 1;
}

float AlbumLayout::GetClosestAnchorPosition(float layoutPosition) const
{
  return round(layoutPosition);
}

float AlbumLayout::GetItemScrollToPosition(unsigned int itemId) const
{
  return -(static_cast<float>(itemId));
}

ItemRange AlbumLayout::GetItemsWithinArea(float firstItemPosition, Vector3 layoutSize) const
{
  return ItemRange(0, mImpl->mNumOfItems);
}

unsigned int AlbumLayout::GetReserveItemCount(Vector3 layoutSize) const
{
  return 0;
}

bool AlbumLayout::GetItemSize(unsigned int itemId, Vector3 layoutSize, Vector3& itemSize) const
{
  itemSize = mImpl->mItemSizeFunction(layoutSize);
  return true;
}

void AlbumLayout::GetResizeAnimation(Animation& animation, Actor actor, Vector3 size, float durationSeconds) const
{
  if(animation)
  {
    animation.Resize(actor, size);
  }
}

bool AlbumLayout::GetPositionConstraint(unsigned int itemId, ItemLayout::Vector3Function& constraint) const
{
  constraint = AlbumPositionConstraint(mImpl->mPositionRight,mImpl->mPositionLeft,mImpl->mSelectedItemPosition,mImpl->mPositionVecSpread,mImpl->mRotationX);
  return true;
}

bool AlbumLayout::GetRotationConstraint(unsigned int itemId, ItemLayout::QuaternionFunction& constraint) const
{
  constraint = AlbumRotationConstraint(mImpl->mRotationVecSpread,mImpl->mRotationVecStack,mImpl->mRotationX,mImpl->mNumOfItems);
  return true;
}

bool AlbumLayout::GetScaleConstraint(unsigned int itemId, ItemLayout::Vector3Function& constraint) const
{
  constraint = AlbumScaleConstraint(mImpl->mScaleRight,mImpl->mScaleLeft,mImpl->mSelectedItemScale,mImpl->mScaleVecSpread);
  return true;
}

bool AlbumLayout::GetColorConstraint(unsigned int itemId, ItemLayout::Vector4Function& constraint) const
{
  constraint = AlbumColorConstraint(mImpl->mColorRight,mImpl->mColorLeft,mImpl->mSelectedItemColor,mImpl->mColorVecSpread,mImpl->mStackNum);
  return true;
}

bool AlbumLayout::GetVisibilityConstraint(unsigned int itemId, ItemLayout::BoolFunction& constraint) const
{
  constraint = AlbumVisibilityConstraintPortrait();
  return true;
}

Degree AlbumLayout::GetScrollDirection() const
{
  Degree scrollDirection(0);

  scrollDirection = -90.0f;

  return scrollDirection;
}

void AlbumLayout::SetNumOfItems(int num)
{
  mImpl->SetNumOfItems(num);
}

int AlbumLayout::GetNumOfItems() const
{
  return mImpl->GetNumOfItems();
}

void AlbumLayout::SetStackNum(int num)
{
  mImpl->SetStackNum(num);
}

int AlbumLayout::GetStackNum() const
{
  return mImpl->GetStackNum();
}

void AlbumLayout::SetPosition(vector<Vector3> positionList)
{
  mImpl->SetPosition(positionList);
}

vector<Vector3> AlbumLayout::GetPosition() const
{
  return mImpl->GetPosition();
}

void AlbumLayout::SetScale(vector<float> scaleList)
{
  mImpl->SetScale(scaleList);
}

vector<float> AlbumLayout::GetScale() const
{
  return mImpl->GetScale();
}

void AlbumLayout::SetColor(vector<Vector2> colorList)
{
  mImpl->SetColor(colorList);
}

vector<Vector2> AlbumLayout::GetColor() const
{
  return mImpl->GetColor();
}

void AlbumLayout::SetRotationX(float rotation)
{
  mImpl->SetRotationX(rotation);
}

float AlbumLayout::GetRotationX() const
{
  return mImpl->GetRotationX();
}

void AlbumLayout::SetRotationZ(vector<float> rotationList)
{
  mImpl->SetRotationZ(rotationList);
}

vector<float> AlbumLayout::GetRotationZ() const
{
  return mImpl->GetRotationZ();
}

void AlbumLayout::SetCenterPosition(Vector3 pos)
{
  mImpl->SetCenterPosition(pos);
}

Vector3 AlbumLayout::GetCenterPosition() const
{
  return mImpl->GetCenterPosition();
}

void AlbumLayout::SetCenterScale(float scale)
{
  mImpl->SetCenterScale(scale);
}

float AlbumLayout::GetCenterScale() const
{
  return mImpl->GetCenterScale();
}

void AlbumLayout::SetCenterColor(Vector2 color)
{
  mImpl->SetCenterColor(color);
}

Vector2 AlbumLayout::GetCenterColor() const
{
  return mImpl->GetCenterColor();
}

void AlbumLayout::SetStackPosition(Vector3 rightPos, Vector3 leftPos)
{
  mImpl->SetStackPosition(rightPos, leftPos);
}

Vector3 AlbumLayout::GetRightStackPosition() const
{
  return mImpl->GetRightStackPosition();
}

Vector3 AlbumLayout::GetLeftStackPosition() const
{
  return mImpl->GetLeftStackPosition();
}

void AlbumLayout::SetStackScale(float rightScale, float leftScale)
{
  mImpl->SetStackScale(rightScale,leftScale);
}

float AlbumLayout::GetRightStackScale() const
{
  return mImpl->GetRightStackScale();
}

float AlbumLayout::GetLeftStackScale() const
{
  return mImpl->GetLeftStackScale();
}

void AlbumLayout::SetStackColor(Vector2 rightColor, Vector2 leftColor)
{
  mImpl->SetStackColor(rightColor, leftColor);
}

Vector2 AlbumLayout::GetRightStackColor() const
{
  return mImpl->GetRightStackColor();
}

Vector2 AlbumLayout::GetLeftStackColor() const
{
  return mImpl->GetLeftStackColor();
}

AlbumLayout::AlbumLayout()
: mImpl(NULL)
{
  mImpl = new Impl();
}

} // namespace Toolkit

} // namespace Dali
