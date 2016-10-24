using Dali;
using System;
using System.Runtime.InteropServices;
using System.Collections.Generic;


namespace FirstScreen
{
    public class ScrollContainer
    {
        private View _container;
        private Actor _itemRoot;
        private Vector3 _itemSize;
        private List<View> _itemList;
        private int _itemCount;
        private int _focusedItem;
        private PanGestureDetector _panGestureDetector;
        private float _scrollDisplacement;
        private float _currentScrollPosition;
        private float _padding;
        private float _width;
        private float _height;
        private bool _isFocused;
        private float _marginX;
        private float _marginY;
        private float _offsetY;
        private float _offsetX;
        private Stage _stage;
        private Vector2 _stageSize;
        private ImageView _shadowBorder;
        private ImageView _spotLight;
        private Animation _spotLightAnimation;
        private Animation _showAnimation;
        private Animation _hideAnimation;
        private Animation _focusAnimation;
        private Animation _scrollAnimation;
        private Animation _focusTransitionAnimation;
        private Path _circularPath;
        private bool _shouldHide;

        public ScrollContainer()
        {
            _itemSize = new Vector3(0.0f, 0.0f, 0.0f);
            _padding = 0.0f;
            _width = 0.0f;
            _height = 0.0f;
            _currentScrollPosition = 0.0f;
            _itemCount = 0;
            _focusedItem = -1;
            _isFocused = false;
            _marginX = 50.0f;
            _marginY = 0.0f;
            _offsetY = 0.0f;
            _offsetX = 0.0f;

            _shouldHide = true;

            _container = new View();
            _itemRoot = new Actor();
            _container.Add(_itemRoot);

            _itemList = new List<View>();

            if (_panGestureDetector == null)
            {
                _panGestureDetector = new PanGestureDetector();
                _panGestureDetector.Attach(_container);

                _panGestureDetector.Detected += OnPan;
            }

            _container.ParentOrigin = NDalic.ParentOriginTopLeft;
            _container.AnchorPoint = NDalic.AnchorPointTopLeft;
            _itemRoot.ParentOrigin = NDalic.ParentOriginTopLeft;
            _itemRoot.AnchorPoint = NDalic.AnchorPointTopLeft;

            _container.WidthResizePolicy = "FILL_TO_PARENT";
            _container.HeightResizePolicy = "FILL_TO_PARENT";
            _itemRoot.WidthResizePolicy = "FILL_TO_PARENT";
            _itemRoot.HeightResizePolicy = "FILL_TO_PARENT";

            _stage = Stage.GetCurrent();
            _stageSize = _stage.GetSize();

            _spotLightAnimation = new Animation(5.0f);
            _focusTransitionAnimation = new Animation (0.35f);
            _focusAnimation = new Animation (0.35f);
            _focusAnimation.SetEndAction(Animation.EndAction.BakeFinal);
            _scrollAnimation = new Animation (0.35f);
            _scrollAnimation.SetEndAction(Animation.EndAction.BakeFinal);
        }

        public bool IsFocused
        {
            get
            {
                return _isFocused;
            }
        }

        public Dali.View Container
        {
            get
            {
                return _container;
            }
        }

        public int ItemCount
        {
            get
            {
                return _itemCount;
            }
        }

        public Actor ItemRoot
        {
            get
            {
                return _itemRoot;
            }
        }

        public Vector3 ItemSize
        {
            get
            {
                return _itemSize;
            }

            set
            {
                _itemSize = value;

                Vector3 topLeft = new Vector3(-0.25f * _itemSize.width, -0.25f * _itemSize.height, 0.0f);
                Vector3 topRight = new Vector3(0.25f * _itemSize.width, -0.25f * _itemSize.height, 0.0f);
                Vector3 bottomRight = new Vector3(0.25f * _itemSize.width, 0.25f * _itemSize.height, 0.0f);
                Vector3 bottomLeft = new Vector3(-0.25f * _itemSize.width, 0.25f * _itemSize.height, 0.0f);

                _circularPath = new Path();
                _circularPath.AddPoint(topLeft);
                _circularPath.AddPoint(topRight);
                _circularPath.AddPoint(bottomRight);
                _circularPath.AddPoint(bottomLeft);
                _circularPath.AddPoint(topLeft);
                _circularPath.GenerateControlPoints(0.5f);
            }
        }

        public float Padding
        {
            get
            {
                return _padding;
            }

            set
            {
                _padding = value;
            }
        }

        public float MarginX
        {
            get
            {
                return _marginX;
            }

            set
            {
                _marginX = value;
            }
        }

        public float OffsetY
        {
            get
            {
                return _offsetY;
            }

            set
            {
                _offsetY = value;
            }
        }

        public float OffsetX
        {
            get
            {
                return _offsetX;
            }

            set
            {
                _offsetX = value;
            }
        }

        public float MarginY
        {
            get
            {
                return _marginY;
            }

            set
            {
                _marginY = value;
            }
        }

        public float Width
        {
            get
            {
                return _width;
            }

            set
            {
                _width = value;
            }
        }

        public float Height
        {
            get
            {
                return _height;
            }

            set
            {
                _height = value;
            }
        }

        public ImageView ShadowBorder
        {
            get
            {
                return _shadowBorder;
            }

            set
            {
                _shadowBorder = value;
            }
        }

        public ImageView SpotLight
        {
            get
            {
                return _spotLight;
            }

            set
            {
                _spotLight = value;
            }
        }

        public int FocusedItemID
        {
            get
            {
                if (_focusedItem < 0)
                {
                    _focusedItem = 0;
                }

                return _focusedItem;
            }
        }

        public Actor GetCurrentFocusedActor()
        {
            if (_focusedItem < 0)
            {
                _focusedItem = 0;
            }

            return _itemList[_focusedItem];
        }

        public void AddItem(View item)
        {
            item.AnchorPoint = NDalic.AnchorPointBottomCenter;
            item.ParentOrigin = NDalic.ParentOriginBottomCenter;

            item.Size = _itemSize;
            item.SetKeyboardFocusable(true);
            item.Position = GetItemPosition(_itemCount, _currentScrollPosition);

            item.Name = _itemCount.ToString();

//          item.ClippingMode = "CLIP_CHILDREN";

            _itemRoot.Add(item);
            _itemList.Add(item);
            _panGestureDetector.Attach(item);
            _itemCount++;
        }

        // Perform Show animation on ScrollContainer (used only for Poster Container)
        public void Show()
        {
            Container.Add(ItemRoot);

            _shouldHide = false;
            _showAnimation = new Animation (0.35f);

            _showAnimation.AnimateTo(new Property(_container, Actor.Property.COLOR_ALPHA), new Property.Value(1.0f));

            _container.PositionY = _container.Position.y + 200.0f;
            float targetPositionY = _container.Position.y - 200.0f;
            _showAnimation.AnimateTo(new Property(_container, Actor.Property.POSITION_Y), new Property.Value(targetPositionY),
                                     new AlphaFunction(AlphaFunction.BuiltinFunction.LINEAR));

            _showAnimation.Play();
        }

        // Perform Hide animation on ScrollContainer (used only for Poster Container)
        public void Hide()
        {
            if (_hideAnimation)
            {
                _hideAnimation.Clear();
                _hideAnimation.Reset();
            }

            float duration = 0.35f;
            _hideAnimation = new Animation(duration);

            _hideAnimation.AnimateTo(new Property(_container, Actor.Property.COLOR_ALPHA), new Property.Value(0.0f),
                                     new AlphaFunction(AlphaFunction.BuiltinFunction.LINEAR), new TimePeriod(0.0f, duration * 0.75f));

            _hideAnimation.Finished += OnHideAnimationFinished;

            _shouldHide = true;
            _hideAnimation.Play();
        }

        public View Focus(int itemId)
        {
            if (itemId < 0)
            {
                itemId = 0;
            }
            else if (itemId >= _itemList.Count)
            {
                itemId = _itemList.Count - 1;
            }

            _itemList[itemId].Add(_shadowBorder);
            _itemList[itemId].Add(_spotLight);

            // Perform Spot Light animation
            if(_focusedItem != itemId && _spotLight != null)
            {
                _spotLightAnimation.Clear();
                _spotLightAnimation.Animate( _spotLight, _circularPath, new Vector3(0.0f, 0.0f, 0.0f) );
                _spotLightAnimation.SetLooping(true);
                _spotLightAnimation.Play();
            }

            _focusedItem = itemId;

            Vector3 itemPosition = GetItemPosition(_focusedItem, _currentScrollPosition);

            _focusAnimation.Clear();

            float relativeItemPositionX = itemPosition.x - _itemSize.width * 0.5f + (_stageSize.width * 0.5f) + _offsetX;
            if (relativeItemPositionX < _marginX + _offsetX + _padding)
            {
                float amount = _marginX + _offsetX + _padding - relativeItemPositionX;
                Scroll(amount, itemId + 1); // Perform Scroll animation
            }
            else if (relativeItemPositionX + _itemSize.width + _padding + _marginX > _stageSize.width)
            {
                float amount = relativeItemPositionX + _marginX + _padding + _itemSize.width - _stageSize.width;
                Scroll(-amount, itemId - 1); // Perform Scroll animation
            }
            else
            {
                for (int i = 0; i < _itemList.Count; ++i)
                {
                    Vector3 targetPosition = GetItemPosition(i, _currentScrollPosition);
                    _focusAnimation.AnimateTo(new Property(_itemList[i], Actor.Property.POSITION),
                                              new Property.Value(targetPosition),
                                              new AlphaFunction(AlphaFunction.BuiltinFunction.EASE_OUT_SINE));
                }
            }

            for (int i = 0; i < _itemList.Count; ++i)
            {
                SetupItemRenderer(_itemList[i], false);

                // Perform Focus animation
                if (i == _focusedItem)
                {
                    _focusAnimation.AnimateTo(new Property(_itemList[i], Actor.Property.SCALE),
                                              new Property.Value(new Vector3(1.2f, 1.2f, 1.2f)),
                                              new AlphaFunction(AlphaFunction.BuiltinFunction.EASE_OUT_SINE));
                }
                else
                {
                    _focusAnimation.AnimateTo(new Property(_itemList[i], Actor.Property.SCALE),
                                              new Property.Value(new Vector3(1.0f, 1.0f, 1.0f)),
                                              new AlphaFunction(AlphaFunction.BuiltinFunction.EASE_OUT_SINE));
                }
            }

            _focusAnimation.Play();

            if (_isFocused && _focusedItem >= 0)
            {
                SetupItemRenderer(_itemList[_focusedItem], true);
                SetupSpotLightRenderer();
            }

            return _itemList[_focusedItem];
        }

        // Perform EddenEffect animation on Focused Item specified
        public void FocusAnimation(FocusEffect focusEffect, FocusEffectDirection direction)
        {
            focusEffect.FocusAnimation(_itemList[_focusedItem], _itemSize, 1.0f, direction);
        }

        public void SetFocused(bool focused)
        {
            _isFocused = focused;

            // Perform Focus animation if the ScrollContainer is not focused already
            if (!_isFocused)
            {
                Actor parent = _shadowBorder.GetParent();
                if (parent)
                {
                    parent.Remove(_shadowBorder);
                }

                parent = _spotLight.GetParent();
                if (parent)
                {
                    parent.Remove(_spotLight);
                }

                _focusTransitionAnimation.Clear();

                for (int i = 0; i < _itemList.Count; ++i)
                {
                    SetupItemRenderer(_itemList[i], false);

                    Vector3 targetPosition = GetItemPosition(i, _currentScrollPosition);
                    _focusTransitionAnimation.AnimateTo(new Property(_itemList[i], Actor.Property.POSITION),
                                                        new Property.Value(targetPosition),
                                                        new AlphaFunction(AlphaFunction.BuiltinFunction.EASE_OUT_SINE));

                    _focusTransitionAnimation.AnimateTo(new Property(_itemList[i], Actor.Property.SCALE),
                                                        new Property.Value(new Vector3(1.0f, 1.0f, 1.0f)),
                                                        new AlphaFunction(AlphaFunction.BuiltinFunction.EASE_OUT_SINE));
                }

                _focusTransitionAnimation.Play();
            }
            else
            {
                Focus(_focusedItem);
            }
        }

        // Obtain ID of first visible item/image on the screen of the ScrollContainer
        public int GetFirstVisibleItemId()
        {
            int firstItemId = -1;

            if (_isFocused)
            {
                firstItemId = (int)Math.Floor((-1.0 * _currentScrollPosition + _marginX * 2.0f) / (_itemSize.x + _padding));
            }
            else
            {
                firstItemId = (int)Math.Floor(-1.0 * _currentScrollPosition / (_itemSize.x + _padding));
            }

            if (firstItemId < 0)
            {
                firstItemId = 0;
            }

            return firstItemId;
        }

        // Obtain ID of last visible item/image on the screen of the ScrollContainer
        public int GetLastVisibleItemId()
        {
            int lastItemId = -1;

            if (_isFocused)
            {
                lastItemId = (int)Math.Ceiling(((_width - _currentScrollPosition - _marginX * 2.0f) / _itemSize.x) - 1);
            }
            else
            {
                lastItemId = (int)Math.Ceiling(((_width - _currentScrollPosition) / _itemSize.x) - 1);
            }

            if (lastItemId >= _itemList.Count)
            {

                lastItemId = _itemList.Count - 1;
            }

            return lastItemId;
        }

        // Obtain Next item/image (Right of the currently focused item) of the ScrollContainer
        public Actor FocusNext()
        {
            int nextItem = -1;

            if (_focusedItem < GetFirstVisibleItemId() || _focusedItem > GetLastVisibleItemId())
            {
                nextItem = GetFirstVisibleItemId();
            }
            else
            {
                nextItem = _focusedItem + 1;
            }

            return Focus(nextItem);
        }

        // Obtain Previous item/image (left of the currently focused item) of the ScrollContainer
        public Actor FocusPrevious()
        {
            int previousItem = -1;

            if (_focusedItem < GetFirstVisibleItemId() || _focusedItem > GetLastVisibleItemId())
            {
                previousItem = GetFirstVisibleItemId();
            }
            else
            {
                previousItem = _focusedItem - 1;
            }

            return Focus(previousItem);
        }

        private void OnHideAnimationFinished(object source, Animation.FinishedEventArgs e)
        {
            var currentParent =  ItemRoot.GetParent();
            if (_shouldHide && currentParent != null)
            {
                Container.Remove(ItemRoot);
            }
        }

        private void OnPan(object source, PanGestureDetector.DetectedEventArgs e)
        {
            switch (e.PanGesture.state)
            {
            case Gesture.State.Started:
                _scrollDisplacement = 0.0f;
                break;

            case Gesture.State.Continuing:
                _scrollDisplacement = e.PanGesture.displacement.x;
                break;

            case Gesture.State.Finished:
            case Gesture.State.Cancelled:
                float absScrollDistance = _scrollDisplacement;
                if (absScrollDistance < 0.0f)
                    absScrollDistance = 0.0f - absScrollDistance;

                float scrollSpeed = e.PanGesture.velocity.x * e.PanGesture.velocity.x + e.PanGesture.velocity.y * e.PanGesture.velocity.y;
                float maxScrollSpeed = 40.0f;  // TBD
                if (scrollSpeed > maxScrollSpeed)
                    scrollSpeed = maxScrollSpeed;

                if (absScrollDistance > 1.0f && scrollSpeed > 0.05f) // Threshold TBD
                {
                    if (_scrollDisplacement > 0.0f) // scroll constant distance in constant speed.
                    {
                        Scroll((_itemSize.x + _padding) * 2, GetFirstVisibleItemId());
                    }
                    else
                    {
                        Scroll(-(_itemSize.x + _padding) * 2, GetFirstVisibleItemId());
                    }
                }
                break;
            }
        }

        // Perform ScrollAnimation on each item
        private void Scroll(float amount, int baseItem)
        {
            float tagetScrollPosition = _currentScrollPosition + amount;
            float totalItemSize = _itemList.Count * (_itemSize.width + _padding) + _padding + (_marginX * 2.0f);

            float maxScrollPosition = _width - totalItemSize;

            if (tagetScrollPosition < maxScrollPosition)
            {
                tagetScrollPosition = maxScrollPosition;
            }
            if (tagetScrollPosition > 0.0f)
            {
                tagetScrollPosition = 0.0f;
            }

            _scrollAnimation.Clear();

            for (int i = 0; i < _itemList.Count; ++i)
            {
                Vector3 targetPosition = GetItemPosition(i, tagetScrollPosition);
                _scrollAnimation.AnimateTo(new Property(_itemList[i], Actor.Property.POSITION),
                                           new Property.Value(targetPosition),
                                           new AlphaFunction(AlphaFunction.BuiltinFunction.EASE_OUT_SINE));
            }

            _currentScrollPosition = tagetScrollPosition;
            _scrollAnimation.Play();
        }

        // Calculate Position of any item/image of ScrollContainer
        private Vector3 GetItemPosition(int itemId, float scrollPosition)
        {
            if (_isFocused)
            {
                // used (_itemSize.width * 0.5f) because of AnchorPointCenter
                // used (_stageSize.width * 0.5f) because of ParentOriginCenter
                if (_focusedItem > itemId)
                {
                    float positionX = (_itemSize.width * itemId) + (_padding * (itemId + 1)) + scrollPosition + (_itemSize.width * 0.5f) - (_stageSize.width * 0.5f);
                    return new Vector3(positionX, -_itemSize.height * _offsetY, 0.0f);
                }
                else if (_focusedItem == itemId)
                {
                    float positionX = (_itemSize.width * itemId) + (_padding * (itemId + 1)) + scrollPosition + _marginX + (_itemSize.width * 0.5f) - (_stageSize.width * 0.5f);
                    return new Vector3(positionX, -_itemSize.height * _offsetY, 0.0f);
                }
                else
                {
                    float positionX = (_itemSize.width * itemId) + (_padding * (itemId + 1)) + scrollPosition + _marginX * 2.0f + (_itemSize.width * 0.5f) - (_stageSize.width * 0.5f);
                    return new Vector3(positionX, -_itemSize.height * _offsetY, 0.0f);
                }
            }
            else
            {
                float positionX = (_itemSize.width * itemId) + (_padding * (itemId + 1)) + scrollPosition + (_itemSize.width * 0.5f) - (_stageSize.width * 0.5f);
                return new Vector3(positionX, -_itemSize.height * _offsetY, 0.0f);
            }
        }

        // Used for SpotLight animation with clipping
        private void SetupItemRenderer(Actor actor, bool stencilOn)
        {
            if (actor)
            {
                Renderer renderer = actor.GetRendererAt(0);

                if (renderer)
                {
                    // Setup the renderer properties:
                    // The stencil plane is only for stencilling, so disable writing to color buffer.

                    // Enable stencil. Draw to the stencil buffer (only).
                    if (stencilOn)
                    {
                        renderer.RenderMode = (int)RenderModeType.COLOR_STENCIL;
                    }
                    else
                    {
                        renderer.RenderMode = (int)RenderModeType.COLOR;
                    }
                    renderer.StencilFunction = (int)StencilFunctionType.ALWAYS;
                    renderer.StencilFunctionReference = 1;
                    renderer.StencilFunctionMask = 0xFF;
                    renderer.StencilOperationOnFail = (int)StencilOperationType.KEEP;
                    renderer.StencilOperationOnZFail = (int)StencilOperationType.KEEP;
                    renderer.StencilOperationOnZPass = (int)StencilOperationType.REPLACE;
                    renderer.StencilMask = 0xFF;

                    // We don't want to write to the depth buffer.
                    renderer.DepthWriteMode = (int)DepthWriteModeType.OFF;
                    // We don't beed to test the depth buffer.
                    renderer.DepthTestMode = (int)DepthTestModeType.OFF;

                    // This object must be rendered 1st.
                    renderer.DepthIndex = 10;
                }
            }
        }

        // Used for SpotLight animation with clipping
        private void SetupSpotLightRenderer()
        {
            if (_spotLight)
            {
                Renderer renderer = _spotLight.GetRendererAt(0);

                if (renderer)
                {
                    // Setup the renderer properties:
                    // Write to color buffer so soptlight is visible

                    // We use blending to blend the spotlight with the poster image.
                    renderer.BlendMode = (int)BlendModeType.ON;
                    renderer.BlendEquationRgb = (int)BlendEquationType.ADD;
                    renderer.BlendEquationAlpha = (int)BlendEquationType.ADD;
                    renderer.BlendFactorDestRgb = (int)BlendFactorType.ONE;

                    // Enable stencil. Here we only draw to areas within the stencil.
                    renderer.RenderMode = (int)RenderModeType.COLOR_STENCIL;
                    renderer.StencilFunction = (int)StencilFunctionType.EQUAL;
                    renderer.StencilFunctionReference = 1;
                    renderer.StencilFunctionMask = 0xFF;
                    // Don't write to the stencil.
                    renderer.StencilMask = 0x00;

                    // We don't need to write to the depth buffer.
                    renderer.DepthWriteMode = (int)DepthWriteModeType.OFF;
                    // We don't need to test the depth buffer.
                    renderer.DepthTestMode = (int)DepthTestModeType.OFF;

                    // This object must be rendered last.
                    renderer.DepthIndex = 20;
                }
            }
        }
    }
}

