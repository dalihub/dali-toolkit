using Dali;
using System;

namespace FirstScreen
{
    public enum FocusEffectDirection
    {
        TopToBottom,
        BottomToTop
    };

    public interface IFocusEffect
    {
        void FocusAnimation(View parentItem, Vector3 itemSize, float duration, FocusEffectDirection direction);
    }
}

