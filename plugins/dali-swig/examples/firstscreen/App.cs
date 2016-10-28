using Dali;
using System;
using System.Runtime.InteropServices;
using System.Collections.Generic;

namespace FirstScreen
{
    public class FirstScreenApp
    {
        private Application _application;                  // Reference to Dali Application.
        private Stage _stage;                              // Reference to Dali stage.
        private Vector2 _stageSize;                        // Reference to Dali stage size.

        private View _topContainer;                        // Top Container added to the Stage will contain Poster ScrollContainers.
        private View _bottomContainer;                     // Bottom Container added to the Stage will contain Menu ScrollContainer.
        private int _currentPostersContainerID;            // Current Poster Container ID visible on the Top Container / Stage.
        private int _totalPostersContainers;               // Number of Poster ScrollContainers to be added on Top Container.
        private List<ScrollContainer> _postersContainer;   // List collection of Poster ScrollContainers used on the Top Container in this demo application.
        private ScrollContainer _menuContainer;            // Menu ScrollContainer used on the Bottom Container in this demo application.
        private Layer _bottomClipLayer;                    // Clip layer (Dali Clip Layer instance) used for Bottom Container.
        private Layer _topClipLayer;                       // Clip layer (Dali Clip Layer instance) used for Top Container.

        private FocusEffect _focusEffect;                  // FocusEffect is used to apply Focus animation effect on any supplied item/image.
        private string _imagePath;                         // Contains the physical location of all images used in the demo application.

        private ImageView _keyboardFocusIndicator;         // Reference to the ImageView (Keyboard Focus indicator) applied to the focused item on ScrollContainer.
        private ImageView _launcherSeparator;              // Reference to the ImageView used for launcher separation (Launcher consists of three image icons on left of Menu ScrollContainer).
        private ImageView[] launcherIcon;                  // ImageViews used for launcher Icons.
        private Animation _showBottomContainerAnimation;   // Animation used to show/unhide Bottom Container (Menu ScrollContainer) when it is focused.
        private Animation _hideBottomContainerAnimation;   // Animation used to hide Bottom Container (Menu ScrollContainer) when it is focused.
        private Animation _showAnimation;                  // Animation used to move Poster scrollContainer from bottom to top and make it non-transparent.
        private Animation _hideAnimation;                  // Animation used to make the unused specified Poster scrollContainer transparent.
        private ScrollContainer _hideScrollContainer;      // The unused Poster scrollContainer which needs to be transparent.
        KeyboardFocusManager _keyboardFocusManager;        // Reference to Dali KeyboardFocusManager.

        private FirstScreenApp(Application application)
        {
            _application = application;
            _application.Initialized += OnInitialize;
        }

        public static void Run()
        {
            FirstScreenApp tVApp = new FirstScreenApp(Application.NewApplication());
            tVApp.MainLoop();
        }

        private void MainLoop()
        {
            _application.MainLoop();
        }

        // Create Items for Poster ScrollContainer
        private void CreatePosters()
        {
            for (int j = 0; j < _totalPostersContainers; j++)
            {
                View posterContainer = _postersContainer[j].Container;
                for (int i = 0; i < Constants.PostersItemsCount; i++)
                {
                    if (j % _totalPostersContainers == 0)
                    {
                        View item = new ImageView(_imagePath + "/poster"+j+"/"+ (i % 6)+ ".jpg");
                        item.SetName ("poster-item-" + _postersContainer[j].ItemCount);
                        _postersContainer[j].Add(item);
                    }
                    else
                    {
                        View item = new ImageView(_imagePath + "/poster"+j+"/"+ (i % 6)+ ".jpg");
                        item.SetName ("poster-item-" + _postersContainer[j].ItemCount);
                        _postersContainer[j].Add(item);
                    }
                }

                if (j == 0)
                {
                    Show(_postersContainer[j]);
                }
                else
                {
                    Hide(_postersContainer[j]);
                }

                _postersContainer[j].SetFocused(false);
            }

            _currentPostersContainerID = 0;
        }

        // Create Items for Menu ScrollContainer
        private void CreateMenu()
        {
            View menuContainer = _menuContainer.Container;
            menuContainer.Position = new Vector3(Constants.LauncherWidth, 0.0f, 0.0f);

            for(int i = 0; i < Constants.MenuItemsCount; i++)
            {
                View menuItem = new ImageView(_imagePath + "/menu/" + i % 7 + ".png");
                menuItem.SetName("menu-item-" + _menuContainer.ItemCount);
                _menuContainer.Add(menuItem);
            }
        }

        private Actor OnKeyboardPreFocusChangeSignal(object source, KeyboardFocusManager.PreFocusChangeEventArgs e)
        {
            if (!e.Current && !e.Proposed)
            {
                return _menuContainer;
            }

            Actor actor = _menuContainer.Container;

            if (e.Direction == View.KeyboardFocus.Direction.UP)
            {
                // Move the Focus to Poster ScrollContainer and hide Bottom Container (Menu ScrollContainer)
                if (_menuContainer.IsFocused)
                {
                    actor = _postersContainer[_currentPostersContainerID].GetCurrentFocusedActor();
                    _menuContainer.SetFocused(false);
                    _postersContainer[_currentPostersContainerID].SetFocused(true);
                    HideBottomContainer();

                    // Also apply Focus animation on Focused item on Poster ScrollContainer
                    FocusAnimation(_postersContainer[_currentPostersContainerID], FocusEffectDirection.BottomToTop);
                }
            }
            else if (e.Direction == View.KeyboardFocus.Direction.DOWN)
            {
                // Show Bottom Container (Menu ScrollContainer) and move the Focus to it
                if (!_menuContainer.IsFocused)
                {
                    ShowBottomContainer();
                    actor = _menuContainer.GetCurrentFocusedActor();
                    _postersContainer[_currentPostersContainerID].SetFocused(false);
                    _menuContainer.SetFocused(true);

                    // Also apply Focus animation on Focused item on Menu ScrollContainer
                    FocusAnimation(_menuContainer, FocusEffectDirection.TopToBottom);
                }
            }
            else
            {
                actor = e.Proposed;
            }

            if (e.Direction == View.KeyboardFocus.Direction.LEFT)
            {
                if (_menuContainer.IsFocused)
                {
                    int id = _menuContainer.FocusedItemID % _totalPostersContainers;
                    if (id != _currentPostersContainerID)
                    {
                        Hide(_postersContainer[_currentPostersContainerID]);
                        _currentPostersContainerID = id;

                        Show(_postersContainer[_currentPostersContainerID]);
                    }
                }
            }
            else if (e.Direction == View.KeyboardFocus.Direction.RIGHT)
            {
                if (_menuContainer.IsFocused)
                {
                    int id = _menuContainer.FocusedItemID % _totalPostersContainers;
                    if (id != _currentPostersContainerID)
                    {
                        Hide(_postersContainer[_currentPostersContainerID]);
                        _currentPostersContainerID = id;
                        Show(_postersContainer[_currentPostersContainerID]);
                    }
                }
            }

            return actor;
        }

        // Perform Focus animation Effect on the current Focused Item on ScrollContainer.
        private void FocusAnimation(ScrollContainer scrollContainer, FocusEffectDirection direction)
        {
            _focusEffect.FocusAnimation(scrollContainer.GetCurrentFocusedActor(), scrollContainer.ItemSize, 1.0f, direction);
        }

        // Perform Show animation on ScrollContainer (used only for Poster Container)
        private void Show(ScrollContainer scrollContainer)
        {
            scrollContainer.Add(scrollContainer.Container);

            _hideScrollContainer = null;
            _showAnimation = new Animation (0.35f);

            // This animation will move Poster scrollContainer from bottom to top and make it non-transparent.
            _showAnimation.AnimateTo(new Dali.Property(scrollContainer.Container, Actor.Property.COLOR_ALPHA), new Dali.Property.Value(1.0f));

            scrollContainer.Container.PositionY = scrollContainer.Container.Position.y + 200.0f;
            float targetPositionY = scrollContainer.Container.Position.y - 200.0f;
            _showAnimation.AnimateTo(new Dali.Property(scrollContainer.Container, Actor.Property.POSITION_Y), new Dali.Property.Value(targetPositionY),
                                     new AlphaFunction(AlphaFunction.BuiltinFunction.LINEAR));

            _showAnimation.Play();
        }

        // Perform Hide animation on ScrollContainer (used only for Poster Container)
        private void Hide(ScrollContainer scrollContainer)
        {
            if (_hideAnimation)
            {
                _hideAnimation.Clear();
                _hideAnimation.Reset();
            }

            float duration = 0.35f;
            _hideAnimation = new Animation(duration);

            _hideAnimation.AnimateTo(new Dali.Property(scrollContainer.Container, Actor.Property.COLOR_ALPHA), new Dali.Property.Value(0.0f),
                                     new AlphaFunction(AlphaFunction.BuiltinFunction.LINEAR), new TimePeriod(0.0f, duration * 0.75f));

            _hideAnimation.Finished += OnHideAnimationFinished;

            _hideScrollContainer = scrollContainer;
            _hideAnimation.Play();
        }

        // This removes all the items from the specified unused Poster ScrollContainer (hence Stage) to improve performance.
        private void OnHideAnimationFinished(object source, Animation.FinishedEventArgs e)
        {
            if (_hideScrollContainer)
            {
                _hideScrollContainer.Remove(_hideScrollContainer.Container);
            }
        }

        // Hide Bottom Container (Menu ScrollContainer) when it is not focused
        private void HideBottomContainer()
        {
            _topClipLayer.ClippingBox = new RectInteger(0,
                                                        Convert.ToInt32(_stageSize.height * Constants.TopContainerPositionFactor),
                                                        Convert.ToInt32((_stageSize.width)),
                                                        Convert.ToInt32((_stageSize.height * Constants.TopClipLayerExpandHeightFactor)));  // X, Y, Width, Height

            _hideBottomContainerAnimation.AnimateTo(new Property(_bottomContainer, Actor.Property.POSITION),
                                                    new Property.Value(new Vector3(0.0f, _stageSize.height * Constants.BottomContainerHidePositionFactor, 0.0f)),
                                                    new AlphaFunction(AlphaFunction.BuiltinFunction.EASE_OUT_SINE));
            _hideBottomContainerAnimation.Play();
        }

        // Show (unhide) Bottom Container (Menu ScrollContainer) when it is focused
        private void ShowBottomContainer()
        {
            _topClipLayer.ClippingBox = new RectInteger(0,
                                                        Convert.ToInt32(_stageSize.height * Constants.TopContainerPositionFactor),
                                                        Convert.ToInt32((_stageSize.width)),
                                                        Convert.ToInt32((_stageSize.height * Constants.TopClipLayerHeightFactor)));  // X, Y, Width, Height

            _showBottomContainerAnimation.AnimateTo(new Property(_bottomContainer, Actor.Property.POSITION),
                                                    new Property.Value(new Vector3(0.0f, _stageSize.height * Constants.BottomContainerShowPositionFactor, 0.0f)),
                                                    new AlphaFunction(AlphaFunction.BuiltinFunction.EASE_OUT_SINE));
            _showBottomContainerAnimation.Play();
        }

        // First screen demo Application initialisation
        private void OnInitialize(object source, AUIApplicationInitEventArgs e)
        {
            _hideScrollContainer = null;
            _stage = Stage.GetCurrent();
            _stageSize = _stage.GetSize();
            //_stage.SetBackgroundColor(NDalic.TRANSPARENT);

            _totalPostersContainers = Constants.TotalPostersContainers;
            _imagePath = "./images/"; // Desktop
            //_imagePath = "/home/owner/apps_rw/org.tizen.firstscreen/res/images/"; // Target

            _postersContainer = new List<ScrollContainer> ();
            _menuContainer = new ScrollContainer ();

            _hideBottomContainerAnimation = new Animation(0.25f);
            _showBottomContainerAnimation = new Animation(0.25f);

            // Create a Top Container for poster items
            _topContainer = new View();
            _topContainer.Size = new Vector3(_stageSize.width, _stageSize.height * Constants.TopContainerHeightFactor, 0.0f);
            _topContainer.Position = new Vector3(0.0f, _stageSize.y * Constants.TopContainerPositionFactor, 0.0f);
            _topContainer.ParentOrigin = NDalic.ParentOriginTopLeft;
            _topContainer.AnchorPoint = NDalic.AnchorPointTopLeft;

            // Add a background to Top container
            Property.Map visual = new Property.Map();
            visual.Insert(NDalic.VISUAL_PROPERTY_TYPE, new Property.Value((int)VisualType.IMAGE));
            visual.Insert(NDalic.IMAGE_VISUAL_URL, new Property.Value(_imagePath + "/background.png"));
            _topContainer.Background = visual;
            _topContainer.Name = "TopControl";

            // Create a Bottom Container
            _bottomContainer = new View();
            _bottomContainer.Size = new Vector3(_stageSize.width, _stageSize.height * Constants.BottomContainerHeightFactor, 0.0f);
            _bottomContainer.Position = new Vector3(0.0f, _stageSize.height * Constants.BottomContainerHidePositionFactor, 0.0f);
            _bottomContainer.ParentOrigin = NDalic.ParentOriginTopLeft;
            _bottomContainer.AnchorPoint = NDalic.AnchorPointTopLeft;

            // Add a background to Bottom Container
            visual = new Property.Map();
            visual.Insert(NDalic.VISUAL_PROPERTY_TYPE, new Property.Value((int)VisualType.IMAGE));
            visual.Insert(NDalic.IMAGE_VISUAL_URL, new Property.Value(_imagePath + "/background.png"));
            _bottomContainer.Background = visual;
            _bottomContainer.Name = "BottomControl";

            // Add both Top and Bottom Containers to Stage
            _stage.Add(_topContainer);
            _stage.Add(_bottomContainer);

            // Add a clip layer to Top Container
            _topClipLayer = new Layer();
            _topClipLayer.AnchorPoint = NDalic.AnchorPointBottomCenter;
            _topClipLayer.ParentOrigin = NDalic.ParentOriginBottomCenter;
            _topClipLayer.ClippingEnable = true;
            _topClipLayer.ClippingBox = new RectInteger(0,
                                                        Convert.ToInt32(_stageSize.height * Constants.TopContainerPositionFactor),
                                                        Convert.ToInt32((_stageSize.width)),
                                                        Convert.ToInt32((_stageSize.height * Constants.TopClipLayerHeightFactor)));  // X, Y, Width, Height
            _topContainer.Add(_topClipLayer);

            // Create a SpotLight for items / images of both Poster and Menu ScrollContainers
            ImageView spotLight = new ImageView(_imagePath + "/highlight_spot.png");
            spotLight.WidthResizePolicy = "USE_NATURAL_SIZE";
            spotLight.HeightResizePolicy = "USE_NATURAL_SIZE";
            spotLight.ParentOrigin = NDalic.ParentOriginCenter;
            spotLight.AnchorPoint = NDalic.AnchorPointCenter;
            spotLight.Name = "spotLight";

            // Create a shadowBorder for items / images of Poster ScrollContainers
            ImageView shadowBorder = new ImageView(_imagePath + "/thumbnail_shadow.9.png");
            shadowBorder.ParentOrigin = NDalic.ParentOriginCenter;
            shadowBorder.AnchorPoint = NDalic.AnchorPointCenter;
            shadowBorder.WidthResizePolicy = "SIZE_FIXED_OFFSET_FROM_PARENT";
            shadowBorder.HeightResizePolicy = "SIZE_FIXED_OFFSET_FROM_PARENT";
            shadowBorder.SetSizeModeFactor(new Vector3(32.0f, 41.0f, 0.0f));
            shadowBorder.Name = "poster shadowBorder";

            // Create Poster Containers and add them to Top Clip layer
            for (int i = 0; i < _totalPostersContainers; i++)
            {
                _postersContainer.Add(new ScrollContainer());
                _postersContainer[i].Container.Name = "poster" + i;
                if (i == 0)
                {
                    _postersContainer[i].ItemSize = new Vector3((_stageSize.width * Constants.Poster0ItemWidthFactor) - Constants.PostersContainerPadding,
                                                                _stageSize.height * Constants.PostersItemHeightFactor, 0.0f);
                }
                else
                {
                    _postersContainer[i].ItemSize = new Vector3((_stageSize.width * Constants.Poster1ItemWidthFactor) - Constants.PostersContainerPadding,
                                                                _stageSize.height * Constants.PostersItemHeightFactor, 0.0f);
                }
                _postersContainer[i].Gap = Constants.PostersContainerPadding;
                _postersContainer[i].MarginX = Constants.PostersContainerMargin;
                _postersContainer[i].OffsetYFator = Constants.PostersContainerOffsetYFactor;
                _postersContainer[i].Width = _stageSize.width;
                _postersContainer[i].Height = _stageSize.height * Constants.PostersContainerHeightFactor;
                _postersContainer[i].ShadowBorder = shadowBorder;
                _postersContainer[i].ShadowBorder.Position = new Vector3(0.0f, 4.0f, 0.0f);
                _postersContainer[i].SpotLight = spotLight;
                _topClipLayer.Add(_postersContainer[i]);
            }

            // Add a clip layer to Bottom Container
            _bottomClipLayer = new Layer();
            _bottomClipLayer.AnchorPoint = NDalic.AnchorPointBottomCenter;
            _bottomClipLayer.ParentOrigin = NDalic.ParentOriginBottomCenter;
            _bottomClipLayer.ClippingEnable = true;
            _bottomClipLayer.ClippingBox = new RectInteger(Convert.ToInt32(Constants.LauncherWidth),
                                                           Convert.ToInt32(_stageSize.height * Constants.BottomContainerShowPositionFactor),
                                                           Convert.ToInt32((_stageSize.width)),
                                                           Convert.ToInt32((_stageSize.height - (_stageSize.height * Constants.BottomClipLayerHeightFactor))));  // X, Y, Width, Height
            _bottomContainer.Add(_bottomClipLayer);

            // Add Launcher items to Bottom Container. Launcher is used to display three images on left of Menu ScrollContainer
            launcherIcon = new ImageView[Convert.ToInt32(Constants.LauncherItemsCount)];
            for (int launcherIndex = 0; launcherIndex < Constants.LauncherItemsCount; launcherIndex++)
            {
                launcherIcon[launcherIndex] = new ImageView(_imagePath + "/" + launcherIndex + "-normal.png");
                launcherIcon[launcherIndex].Name = "launcherIcon" + launcherIndex;
                launcherIcon[launcherIndex].WidthResizePolicy = "USE_NATURAL_SIZE";
                launcherIcon[launcherIndex].HeightResizePolicy = "USE_NATURAL_SIZE";
                launcherIcon[launcherIndex].ParentOrigin = NDalic.ParentOriginCenterLeft;
                launcherIcon[launcherIndex].AnchorPoint = NDalic.AnchorPointCenterLeft;
                launcherIcon[launcherIndex].Position = new Vector3(Constants.LauncherIconWidth * launcherIndex + Constants.LauncherLeftMargin, 0.0f, 0.0f);
                _bottomContainer.Add(launcherIcon[launcherIndex]);
            }

            // Add a shadow seperator image between last Launcher icon and Menu ScrollContainer
            _launcherSeparator = new ImageView(_imagePath + "/focus_launcher_shadow_n.png");
            _launcherSeparator.Name = "launcherSeparator";
            _launcherSeparator.WidthResizePolicy = "USE_NATURAL_SIZE";
            _launcherSeparator.HeightResizePolicy = "FILL_TO_PARENT";
            _launcherSeparator.ParentOrigin = NDalic.ParentOriginCenterLeft;
            _launcherSeparator.AnchorPoint = NDalic.AnchorPointCenterLeft;
            _launcherSeparator.Position = new Vector3(Constants.LauncherIconWidth * Constants.LauncherItemsCount + Constants.LauncherLeftMargin, 0.0f, 0.0f);
            _bottomContainer.Add(_launcherSeparator);

            // Create Menu Container and add it to Bottom Clip Layer
            Vector3 menuItemSize = new Vector3((_stageSize.width * Constants.MenuItemWidthFactor) - Constants.MenuContainerPadding,
                                        _stageSize.height * Constants.MenuItemHeightFactor, 0.0f);
            _menuContainer.Container.Name = "menu";
            _menuContainer.ItemSize = menuItemSize;
            _menuContainer.Gap = Constants.MenuContainerPadding;
            _menuContainer.MarginX = Constants.MenuContainerMargin;
            _menuContainer.OffsetYFator = Constants.MenuContainerOffsetYFactor;
            _menuContainer.OffsetX = Constants.LauncherWidth;
            _menuContainer.Width = _stageSize.width - Constants.LauncherWidth;
            _menuContainer.Height = _stageSize.height * Constants.MenuContainerHeightFactor;
            _menuContainer.ShadowBorder = new ImageView(_imagePath + "/focus_launcher_shadow.9.png");
            _menuContainer.ShadowBorder.Name = "_menuContainer.ShadowBorder";
            _menuContainer.ShadowBorder.Size = new Vector3(_menuContainer.ItemSize.width + 40.0f, _menuContainer.ItemSize.height + 50.0f, 0.0f);
            _menuContainer.ShadowBorder.Position = new Vector3(0.0f, 5.0f, 0.0f);
            _menuContainer.ShadowBorder.ParentOrigin = NDalic.ParentOriginCenter;
            _menuContainer.ShadowBorder.AnchorPoint = NDalic.AnchorPointCenter;
            _menuContainer.SpotLight = spotLight;
            _bottomClipLayer.Add(_menuContainer);

            CreatePosters(); // Create Items for Poster ScrollContainer
            CreateMenu();    // Create Items for Menu ScrollContainer

            // Initialize PreFocusChange event of KeyboardFocusManager
            _keyboardFocusManager = KeyboardFocusManager.Get();
            _keyboardFocusManager.PreFocusChange += OnKeyboardPreFocusChangeSignal;

            _keyboardFocusIndicator = new ImageView(_imagePath + "/highlight_stroke.9.png");
            _keyboardFocusIndicator.ParentOrigin = NDalic.ParentOriginCenter;
            _keyboardFocusIndicator.AnchorPoint = NDalic.AnchorPointCenter;
            _keyboardFocusIndicator.WidthResizePolicy = "FILL_TO_PARENT";
            _keyboardFocusIndicator.HeightResizePolicy = "FILL_TO_PARENT";

            _keyboardFocusManager.SetFocusIndicatorActor(_keyboardFocusIndicator);

            _keyboardFocusManager.SetAsFocusGroup(_menuContainer, true);
            _keyboardFocusManager.SetAsFocusGroup(_postersContainer[0], true);
            _keyboardFocusManager.SetAsFocusGroup(_postersContainer[1], true);
            _keyboardFocusManager.SetFocusGroupLoop(true);

            _focusEffect = new FocusEffect();

            // Move Fcous to Bottom Container (Menu ScrollContainer)
            ShowBottomContainer();
            _menuContainer.SetFocused(true);
        }
    }
}


