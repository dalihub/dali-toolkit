using Dali;
using System;

namespace FirstScreen
{
    public enum EdenEffectDirection
    {
        TopToBottom,
        BottomToTop
    };

    public interface IEdenEffect
    {
        void FocusAnimation(View parentItem, Vector3 itemSize, float duration, EdenEffectDirection direction);
    }
}

