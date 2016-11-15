using Dali;
using System;
using System.Runtime.InteropServices;
using System.Collections.Generic;

namespace FirstScreen
{
    public class FirstScreenApp
    {
        private int _currentPostersContainerID;
        private int _totalPostersContainers;

        private Application _application;
        private Stage _stage;
        private Vector2 _stageSize;

        private List<ScrollContainer> _postersContainer;

        private ScrollContainer _menuContainer;
        private Vector3 _menuItemSize;

        private Layer _bottomClipLayer;
        private Layer _topClipLayer;
        private View _topContainer;
        private View _bottomContainer;

        private FocusEffect _focusEffect;
        private string _imagePath;

        private ImageView _keyboardFocusIndicator;
        private ImageView _launcherSeparator;
        private ImageView[] launcherIcon;
        private Animation _showBottomContainerAnimation;
        private Animation _hideBottomContainerAnimation;

        public FirstScreenApp(Application application)
        {
            _application = application;
            _application.Initialized += OnInitialize;
        }

        public static void Run()
        {
            FirstScreenApp tVApp = new FirstScreenApp(Application.NewApplication());
            tVApp.MainLoop();
        }

        public void MainLoop()
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
                        _postersContainer[j].AddItem(item);
                    }
                    else
                    {
                        View item = new ImageView(_imagePath + "/poster"+j+"/"+ (i % 6)+ ".jpg");
                        item.SetName ("poster-item-" + _postersContainer[j].ItemCount);
                        _postersContainer[j].AddItem(item);
                    }
                }

                if (j == 0)
                {
                    _postersContainer[j].Show();
                }
                else
                {
                    _postersContainer[j].Hide();
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
                _menuContainer.AddItem(menuItem);
            }
        }

        private Actor OnKeyboardPreFocusChangeSignal(object source, KeyboardFocusManager.PreFocusChangeEventArgs e)
        {
            Actor actor = _menuContainer.ItemRoot;

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
                    _postersContainer[_currentPostersContainerID].FocusAnimation(_focusEffect, FocusEffectDirection.BottomToTop);
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
                    _menuContainer.FocusAnimation(_focusEffect, FocusEffectDirection.TopToBottom);
                }
            }
            else if (e.Direction == View.KeyboardFocus.Direction.LEFT)
            {
                if (_menuContainer.IsFocused)
                {
                    // Move the Focus to the left item/image of currently focused item on Menu ScrollContainer
                    actor = _menuContainer.FocusPrevious();

                    int id = _menuContainer.FocusedItemID % _totalPostersContainers;
                    if (id != _currentPostersContainerID)
                    {
                        _postersContainer[_currentPostersContainerID].Hide();
                        _currentPostersContainerID = id;

                        _postersContainer[_currentPostersContainerID].Show();
                    }
                }
                else
                {
                    // Move the Focus to the left item/image of currently focused item on Poster ScrollContainer
                    actor = _postersContainer[_currentPostersContainerID].FocusPrevious();
                }
            }
            else if (e.Direction == View.KeyboardFocus.Direction.RIGHT)
            {
                if (_menuContainer.IsFocused)
                {
                    // Move the Focus to the right item/image of currently focused item on Menu ScrollContainer
                    actor = _menuContainer.FocusNext();

                    int id = _menuContainer.FocusedItemID % _totalPostersContainers;
                    if (id != _currentPostersContainerID)
                    {
                        _postersContainer[_currentPostersContainerID].Hide();
                        _currentPostersContainerID = id;
                        _postersContainer[_currentPostersContainerID].Show();
                    }
                }
                else
                {
                    // Move the Focus to the right item/image of currently focused item on Poster ScrollContainer
                    actor = _postersContainer[_currentPostersContainerID].FocusNext();
                }
            }

            return actor;
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
            _stage = Stage.GetCurrent();
            _stageSize = _stage.GetSize();
//            _stage.SetBackgroundColor(NDalic.TRANSPARENT);

            _totalPostersContainers = Constants.TotalPostersContainers;
            _imagePath = "./images/"; // Desktop
//            _imagePath = "/home/owner/apps_rw/org.tizen.firstscreen/res/images/"; // Target

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
                    _postersContainer[i].ItemSize = new Vector3(_stageSize.width * Constants.Poster1ItemWidthFactor,
                                                                _stageSize.height * Constants.PostersItemHeightFactor, 0.0f);
                }
                _postersContainer[i].Padding = Constants.PostersContainerPadding;
                _postersContainer[i].MarginX = Constants.PostersContainerMargin;
                _postersContainer[i].OffsetY = Constants.PostersContainerOffsetYFactor;
                _postersContainer[i].Width = _stageSize.width;
                _postersContainer[i].Height = _stageSize.height * Constants.PostersContainerHeightFactor;
                _postersContainer[i].ShadowBorder = shadowBorder;
                _postersContainer[i].ShadowBorder.Position = new Vector3(0.0f, 4.0f, 0.0f);
                _postersContainer[i].SpotLight = spotLight;
                _topClipLayer.Add(_postersContainer[i].Container);
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
            _menuItemSize = new Vector3((_stageSize.width * Constants.MenuItemWidthFactor) - Constants.MenuContainerPadding,
                                        _stageSize.height * Constants.MenuItemHeightFactor, 0.0f);
            _menuContainer.Container.Name = "menu";
            _menuContainer.ItemSize = _menuItemSize;
            _menuContainer.Padding = Constants.MenuContainerPadding;
            _menuContainer.MarginX = Constants.MenuContainerMargin;
            _menuContainer.OffsetY = Constants.MenuContainerOffsetYFactor;
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
            _bottomClipLayer.Add(_menuContainer.Container);

            CreatePosters(); // Create Items for Poster ScrollContainer
            CreateMenu();    // Create Items for Menu ScrollContainer

            // Initialize PreFocusChange event of KeyboardFocusManager
            KeyboardFocusManager keyboardFocusManager = KeyboardFocusManager.Get();
            keyboardFocusManager.PreFocusChange += OnKeyboardPreFocusChangeSignal;

            _keyboardFocusIndicator = new ImageView(_imagePath + "/highlight_stroke.9.png");
            _keyboardFocusIndicator.ParentOrigin = NDalic.ParentOriginCenter;
            _keyboardFocusIndicator.AnchorPoint = NDalic.AnchorPointCenter;
            _keyboardFocusIndicator.WidthResizePolicy = "FILL_TO_PARENT";
            _keyboardFocusIndicator.HeightResizePolicy = "FILL_TO_PARENT";

            keyboardFocusManager.SetFocusIndicatorActor(_keyboardFocusIndicator);

            _focusEffect = new FocusEffect();

            // Move Fcous to Bottom Container (Menu ScrollContainer)
            ShowBottomContainer();
            _menuContainer.SetFocused(true);
        }
    }
}

