using Dali;
using System;
using System.Runtime.InteropServices;
using System.Collections.Generic;


namespace FirstScreen
{
    public class ScrollContainer : CustomView
    {
        private View _container;                      // View Container will be the first item added to ScrollContainer and parent to all the items added to the ScrollContainer.
        private Vector3 _itemSize;                    // Size of the item / images added to the ScrollContainer.
        private List<View> _itemList;                 // List collection of View items/images  added to the ScrollContainer.
        private int _itemCount;                       // Number of items / images  added to the ScrollContainer.
        private int _focusedItem;                     // Index of currently focused View item / image  on the ScrollContainer.
        private float _scrollDisplacement;            // Used for horizontal pan displacement.
        private float _currentScrollPosition;         // Used for horizontal scroll position.
        private float _gap;                           // Used for gap / padding between items / images on the ScrollContainer.
        private float _width;                         // Width of the ScrollContainer.
        private float _height;                        // Height of the ScrollContainer.
        private bool _isFocused;                      // Flag to check if ScrollContainer is enabled or not.
        private float _marginX;                       // Extra horizontal margin is used to add an extra gap between items / images after a focused and scaled item / image.
        private float _marginY;                       // Extra vertical margin (not used at the moment).
        private float _offsetYFactor;                 // Vertical Position offset Factor of item height.
        private float _offsetX;                       // Horizontal Position offset of ScrollContainer.
        private Stage _stage;                         // Reference to Dali stage.
        private Vector2 _stageSize;                   // Reference to Dali stage size.
        private ImageView _shadowBorder;              // Reference to Shadow border ImageView applied to the focused item on ScrollContainer.
        private ImageView _spotLight;                 // Reference to SpotLight ImageView applied to the focused item on ScrollContainer.
        private Animation _spotLightAnimation;        // SpotLight Animation applied to the focused item on ScrollContainer.
        private Animation _focusAnimation;            // Focused position animation on ScrollContainer.
        private Animation _scrollAnimation;           // Scroll animation on items of ScrollContainer.
        private Animation _focusTransitionAnimation;  // Focus Transition (scaling /unscaling) animation on items of ScrollContainer.
        private Path _circularPath;                   // Circular path used for SpotLight Animation applied to the focused item on ScrollContainer.

        public ScrollContainer() : base(ViewWrapperImpl.CustomViewBehaviour.DISABLE_STYLE_CHANGE_SIGNALS |
                                        ViewWrapperImpl.CustomViewBehaviour.REQUIRES_KEYBOARD_NAVIGATION_SUPPORT)
        {

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

        public float Gap
        {
            get
            {
                return _gap;
            }

            set
            {
                _gap = value;
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

        public float OffsetYFator
        {
            get
            {
                return _offsetYFactor;
            }

            set
            {
                _offsetYFactor = value;
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

        // This override method is called automatically after the Control has been initialized.
        // Any second phase initialization is done here.
        public override void OnInitialize()
        {
            _itemSize = new Vector3(0.0f, 0.0f, 0.0f);
            _gap = 0.0f;
            _width = 0.0f;
            _height = 0.0f;
            _currentScrollPosition = 0.0f;
            _itemCount = 0;
            _focusedItem = -1;
            _isFocused = false;
            _marginX = 50.0f;
            _marginY = 0.0f;
            _offsetYFactor = 0.0f;
            _offsetX = 0.0f;

            _container = new View();
            this.Add(_container);

            _itemList = new List<View>();

            this.ParentOrigin = NDalic.ParentOriginTopLeft;
            this.AnchorPoint = NDalic.AnchorPointTopLeft;
            this.WidthResizePolicy = "FILL_TO_PARENT";
            this.HeightResizePolicy = "FILL_TO_PARENT";
            this.SetKeyboardFocusable(true);

            _container.ParentOrigin = NDalic.ParentOriginTopLeft;
            _container.AnchorPoint = NDalic.AnchorPointTopLeft;
            _container.WidthResizePolicy = "FILL_TO_PARENT";
            _container.HeightResizePolicy = "FILL_TO_PARENT";

            _stage = Stage.GetCurrent();
            _stageSize = _stage.GetSize();

            _spotLightAnimation = new Animation(Constants.SpotLightDuration);
            _focusTransitionAnimation = new Animation(Constants.FocusTransitionDuration);
            _focusAnimation = new Animation(Constants.FocusDuration);
            _focusAnimation.SetEndAction(Animation.EndAction.BakeFinal);
            _scrollAnimation = new Animation(Constants.ScrollDuration);
            _scrollAnimation.SetEndAction(Animation.EndAction.BakeFinal);

            EnableGestureDetection(Gesture.Type.Pan);
        }

        // This will be invoked automatically if an item/image is added to the ScrollContainer
        public override void OnChildAdd(Actor actor)
        {
            View item = View.DownCast(actor);

            if (item is View && item != _container)
            {
                item.AnchorPoint = NDalic.AnchorPointBottomCenter;
                item.ParentOrigin = NDalic.ParentOriginBottomCenter;

                item.Size = _itemSize;
                item.SetKeyboardFocusable(true);
                item.Position = GetItemPosition(_itemCount, _currentScrollPosition);

                item.Name = _itemCount.ToString();

                //item.ClippingMode = "CLIP_CHILDREN";

                _container.Add(item);
                _itemList.Add(item);

                _itemCount++;
                item.SetKeyboardFocusable(true);
            }
        }

        // This will be invoked automatically if an item/image is removed from the ScrollContainer
        public override void OnChildRemove(Actor actor)
        {
            View item = View.DownCast(actor);

            if (item is View && item != _container)
            {
                _container.Remove(item);

                _itemCount--;
                _itemList.Remove(item);
            }
        }

        // This override function supports two dimensional keyboard navigation.
        // This function returns the next keyboard focusable actor in ScrollContainer control towards the given direction.
        public override Actor GetNextKeyboardFocusableActor(Actor currentFocusedActor, View.KeyboardFocus.Direction direction, bool loopEnabled)
        {
            if (direction == View.KeyboardFocus.Direction.LEFT)
            {
                return FocusPrevious(loopEnabled);
            }
            else if (direction == View.KeyboardFocus.Direction.RIGHT)
            {
                return FocusNext(loopEnabled);
            }
            else
            {
                return currentFocusedActor;
            }
        }

        // This override function is invoked before chosen focusable actor will be focused.
        // This allows the application to preform any actions (i.e. Scroll and SpotLight animations) before the focus is actually moved to the chosen actor.
        public override void OnKeyboardFocusChangeCommitted(Actor commitedFocusableActor)
        {
            Focus(_focusedItem);
        }

        // This override function is invoked whenever a pan gesture is detected on this control.
        // Perform Scroll Animation based upon pan gesture velocity / speed.
        public override void OnPan(PanGesture pan)
        {
            switch (pan.state)
            {
            case Gesture.State.Started:
                _scrollDisplacement = 0.0f;
                break;

            case Gesture.State.Continuing:
                _scrollDisplacement = pan.displacement.x;
                break;

            case Gesture.State.Finished:
            case Gesture.State.Cancelled:
                float absScrollDistance = _scrollDisplacement;
                if (absScrollDistance < 0.0f)
                    absScrollDistance = 0.0f - absScrollDistance;

                float scrollSpeed = pan.velocity.x * pan.velocity.x + pan.velocity.y * pan.velocity.y;
                float maxScrollSpeed = 40.0f;  // TBD
                if (scrollSpeed > maxScrollSpeed)
                    scrollSpeed = maxScrollSpeed;

                if (absScrollDistance > 1.0f && scrollSpeed > 0.05f) // Threshold TBD
                {
                    if (_scrollDisplacement > 0.0f) // scroll constant distance in constant speed.
                    {
                        Scroll((_itemSize.x + _gap) * 2, GetFirstVisibleItemId());
                    }
                    else
                    {
                        Scroll(-(_itemSize.x + _gap) * 2, GetFirstVisibleItemId());
                    }
                }
                break;
            }
        }

        // This function returns current focused actor
        public View GetCurrentFocusedActor()
        {
            if (_focusedItem < 0)
            {
                _focusedItem = 0;
            }

            return _itemList[_focusedItem];
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
                    _focusTransitionAnimation.AnimateTo(new Dali.Property(_itemList[i], Actor.Property.POSITION),
                                                        new Dali.Property.Value(targetPosition),
                                                        new AlphaFunction(AlphaFunction.BuiltinFunction.EASE_OUT_SINE));

                    _focusTransitionAnimation.AnimateTo(new Dali.Property(_itemList[i], Actor.Property.SCALE),
                                                        new Dali.Property.Value(new Vector3(1.0f, 1.0f, 1.0f)),
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
        private int GetFirstVisibleItemId()
        {
            int firstItemId = -1;

            if (_isFocused)
            {
                firstItemId = (int)Math.Floor((-1.0 * _currentScrollPosition + _marginX * 2.0f) / (_itemSize.x + _gap));
            }
            else
            {
                firstItemId = (int)Math.Floor(-1.0 * _currentScrollPosition / (_itemSize.x + _gap));
            }

            if (firstItemId < 0)
            {
                firstItemId = 0;
            }

            return firstItemId;
        }

        // Obtain ID of last visible item/image on the screen of the ScrollContainer
        private int GetLastVisibleItemId()
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
        private Actor FocusNext(bool loopEnabled)
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

            if (nextItem >= _itemList.Count)
            {
                if (loopEnabled)
                {
                    nextItem = 0;
                }
                else
                {
                    nextItem = _itemList.Count - 1;
                }
            }

            _focusedItem = nextItem;
            return _itemList[_focusedItem];
        }

        // Obtain Previous item/image (left of the currently focused item) of the ScrollContainer
        private Actor FocusPrevious(bool loopEnabled)
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

            if (previousItem < 0)
            {
                if (loopEnabled)
                {
                    previousItem = _itemList.Count - 1;
                }
                else
                {
                    previousItem = 0;
                }
            }

            _focusedItem = previousItem;
            return _itemList[_focusedItem];
        }

        // Perform ScrollAnimation on each item
        private void Scroll(float amount, int baseItem)
        {
            float tagetScrollPosition = _currentScrollPosition + amount;
            float totalItemSize = _itemList.Count * (_itemSize.width + _gap) + _gap + (_marginX * 2.0f);

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
                _scrollAnimation.AnimateTo(new Dali.Property(_itemList[i], Actor.Property.POSITION),
                                           new Dali.Property.Value(targetPosition),
                                           new AlphaFunction(AlphaFunction.BuiltinFunction.EASE_OUT_SINE));
            }

            _currentScrollPosition = tagetScrollPosition;
            _scrollAnimation.Play();
        }

        // This function uses ItemId as next FocusedItem and preforms Scroll and SpotLight animations on that item.
        private void Focus(int itemId)
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
            if (relativeItemPositionX < _marginX + _offsetX + _gap)
            {
                float amount = _marginX + _offsetX + _gap - relativeItemPositionX;
                Scroll(amount, itemId + 1); // Perform Scroll animation
            }
            else if (relativeItemPositionX + _itemSize.width + _gap + _marginX > _stageSize.width)
            {
                float amount = relativeItemPositionX + _marginX + _gap + _itemSize.width - _stageSize.width;
                Scroll(-amount, itemId - 1); // Perform Scroll animation
            }
            else
            {
                // Perform animation when item is focused
                for (int i = 0; i < _itemList.Count; ++i)
                {
                    Vector3 targetPosition = GetItemPosition(i, _currentScrollPosition);
                    _focusAnimation.AnimateTo(new Dali.Property(_itemList[i], Actor.Property.POSITION),
                                              new Dali.Property.Value(targetPosition),
                                              new AlphaFunction(AlphaFunction.BuiltinFunction.EASE_OUT_SINE));
                }
            }

            for (int i = 0; i < _itemList.Count; ++i)
            {
                SetupItemRenderer(_itemList[i], false);

                // Perform scale animation on Focused item
                if (i == _focusedItem)
                {
                    _focusAnimation.AnimateTo(new Dali.Property(_itemList[i], Actor.Property.SCALE),
                                              new Dali.Property.Value(new Vector3(1.2f, 1.2f, 1.2f)),
                                              new AlphaFunction(AlphaFunction.BuiltinFunction.EASE_OUT_SINE));
                }
                else
                {
                    _focusAnimation.AnimateTo(new Dali.Property(_itemList[i], Actor.Property.SCALE),
                                              new Dali.Property.Value(new Vector3(1.0f, 1.0f, 1.0f)),
                                              new AlphaFunction(AlphaFunction.BuiltinFunction.EASE_OUT_SINE));
                }
            }

            _focusAnimation.Play();

            if (_isFocused && _focusedItem >= 0)
            {
                SetupItemRenderer(_itemList[_focusedItem], true);
                SetupSpotLightRenderer();
            }
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
                    float positionX = (_itemSize.width * itemId) + (_gap * (itemId + 1)) + scrollPosition + (_itemSize.width * 0.5f) - (_stageSize.width * 0.5f);
                    return new Vector3(positionX, -_itemSize.height * _offsetYFactor, 0.0f);
                }
                else if (_focusedItem == itemId)
                {
                    float positionX = (_itemSize.width * itemId) + (_gap * (itemId + 1)) + scrollPosition + _marginX + (_itemSize.width * 0.5f) - (_stageSize.width * 0.5f);
                    return new Vector3(positionX, -_itemSize.height * _offsetYFactor, 0.0f);
                }
                else
                {
                    float positionX = (_itemSize.width * itemId) + (_gap * (itemId + 1)) + scrollPosition + _marginX * 2.0f + (_itemSize.width * 0.5f) - (_stageSize.width * 0.5f);
                    return new Vector3(positionX, -_itemSize.height * _offsetYFactor, 0.0f);
                }
            }
            else
            {
                float positionX = (_itemSize.width * itemId) + (_gap * (itemId + 1)) + scrollPosition + (_itemSize.width * 0.5f) - (_stageSize.width * 0.5f);
                return new Vector3(positionX, -_itemSize.height * _offsetYFactor, 0.0f);
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

