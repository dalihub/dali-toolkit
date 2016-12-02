/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

using System;
using System.Runtime.InteropServices;
using Dali;

namespace MyCSharpExample
{
    class Example
    {
        // This is simple structure to contain Control name and implement state at once
        // name : control name
        // isImplemented : the state which the control is implemented in public or not
        private struct Item
        {
            public String name;
            public bool isImplemented;

            public Item(String name, bool isImplemented)
            {
                this.name = name;
                this.isImplemented = isImplemented;
            }
        }

        private Dali.Application _application;
        private TableView _contentContainer;
        private Stage _stage;

        // List of items
        private Item[] mViewList = {
            new Item("PushButton", false),  new Item("DropDown", false),    new Item("Toggle", false),
            new Item("InputField", false),  new Item("AnimateGif", false),  new Item("Loading", false),
            new Item("ProgressBar", false), new Item("CheckBox", false),    new Item("RadioButton", true),
            new Item("Tooltip", false),     new Item("Popup", false),       new Item("Toast", false),
            new Item("ItemView", false),    new Item("CheckBox", true)
        };

        public Example(Dali.Application application)
        {
            _application = application;
            _application.Initialized += OnInitialize;
        }

        public void OnInitialize(object source, AUIApplicationInitEventArgs e)
        {
            Console.WriteLine("Customized Application Initialize event handler");
            _stage = Stage.GetCurrent();
            _stage.BackgroundColor = Color.White;

            // Top label
            TextLabel topLabel = new TextLabel();
            topLabel.SetResizePolicy(ResizePolicyType.FILL_TO_PARENT, DimensionType.WIDTH);
            topLabel.SetResizePolicy(ResizePolicyType.SIZE_RELATIVE_TO_PARENT, DimensionType.HEIGHT);
            topLabel.AnchorPoint = NDalic.AnchorPointTopCenter;
            topLabel.ParentOrigin = NDalic.ParentOriginTopCenter;
            topLabel.SetSizeModeFactor(new Vector3( 0.0f, 0.1f, 0.0f ) );
            topLabel.BackgroundColor = new Color(43.0f / 255.0f, 145.0f / 255.0f, 175.0f / 255.0f, 1.0f);
            topLabel.TextColor = NDalic.WHITE;
            topLabel.Text = " DALi Views";
            topLabel.HorizontalAlignment = "BEGIN";
            topLabel.VerticalAlignment = "CENTER";
            topLabel.PointSize = 42.0f;
            _stage.Add(topLabel);

            // Grid container to contain items. Use tableView because FlexContainer support focus navigation just two direction ( up/down or left/right )
            _contentContainer = new TableView(6, 5);
            _contentContainer.SetResizePolicy(ResizePolicyType.FILL_TO_PARENT, DimensionType.WIDTH);
            _contentContainer.SetResizePolicy(ResizePolicyType.SIZE_RELATIVE_TO_PARENT, DimensionType.HEIGHT);
            _contentContainer.SetSizeModeFactor(new Vector3( 0.0f, 0.9f, 0.0f ) );
            _contentContainer.AnchorPoint = NDalic.AnchorPointBottomCenter;
            _contentContainer.ParentOrigin = NDalic.ParentOriginBottomCenter;
            _contentContainer.SetRelativeHeight(0, 0.07f);
            _contentContainer.SetRelativeHeight(1, 0.26f);
            _contentContainer.SetRelativeHeight(2, 0.07f);
            _contentContainer.SetRelativeHeight(3, 0.26f);
            _contentContainer.SetRelativeHeight(4, 0.07f);
            _contentContainer.SetRelativeHeight(5, 0.26f);
            _contentContainer.SetKeyboardFocusable(true);
            _stage.Add(_contentContainer);

            CreateContent();

            KeyboardFocusManager.Get().PreFocusChange += OnPreFocusChange;
        }

        // Callback for KeyboardFocusManager
        private Actor OnPreFocusChange(object source, KeyboardFocusManager.PreFocusChangeEventArgs e)
        {
            if (!e.Proposed && !e.Current)
            {
                e.Proposed = _contentContainer.GetChildAt(1);
            }
            return e.Proposed;
        }

        private void CreateContent()
        {
            for (int i = 0; i < mViewList.Length; i++)
            {
                CreateItem(mViewList[i], i);
            }
        }

        private void CreateItem(Item item, int idx)
        {
            // Make label for item
            TextLabel itemLabel = new TextLabel("    " + item.name);
            itemLabel.Size = new Vector3(_stage.GetSize().width * 0.2f, _stage.GetSize().height * 0.05f, 0.0f);
            itemLabel.HorizontalAlignment = "BEGIN";
            itemLabel.VerticalAlignment = "BOTTOM";
            itemLabel.PointSize = 18.0f;
            _contentContainer.AddChild(itemLabel, new TableView.CellPosition(((uint)idx / 5) * 2, (uint)idx % 5));

            // If item is implemented in public, attach it on stage
            if (item.isImplemented)
            {
                if (item.name.CompareTo("PushButton") == 0)
                {

                }
                if (item.name.CompareTo("DropDown") == 0)
                {

                }
                if (item.name.CompareTo("Toggle") == 0)
                {

                }
                if (item.name.CompareTo("InputField") == 0)
                {

                }
                if (item.name.CompareTo("AnimateGif") == 0)
                {

                }
                if (item.name.CompareTo("Loading") == 0)
                {

                }
                if (item.name.CompareTo("ProgressBar") == 0)
                {

                }
                if (item.name.CompareTo("ScrollBar") == 0)
                {

                }
                if (item.name.CompareTo("CheckBox") == 0)
                {
                    CheckBoxButton checkBoxButton = new CheckBoxButton();
                    checkBoxButton.LabelText = "Yes";

                    _contentContainer.AddChild(checkBoxButton, new TableView.CellPosition(((uint)idx / 5) * 2 + 1, (uint)idx % 5));
                }
                if (item.name.CompareTo("RadioButton") == 0)
                {
                    TableView tableView = new TableView(2, 1);
                    tableView.SetResizePolicy(ResizePolicyType.FILL_TO_PARENT, DimensionType.WIDTH);
                    tableView.SetResizePolicy(ResizePolicyType.FILL_TO_PARENT, DimensionType.HEIGHT);

                    RadioButton rButton = new RadioButton();
                    rButton.LabelText = "Yes";
                    rButton.Selected = true;
                    tableView.AddChild(rButton, new TableView.CellPosition(0, 0));

                    rButton = new RadioButton();
                    rButton.LabelText = "No";

                    tableView.AddChild(rButton, new TableView.CellPosition(1, 0));

                    _contentContainer.AddChild(tableView, new TableView.CellPosition(((uint)idx / 5) * 2 + 1, (uint)idx % 5));
                }
                if (item.name.CompareTo("Tooltip") == 0)
                {

                }
                if (item.name.CompareTo("Popup") == 0)
                {

                }
                if (item.name.CompareTo("Toast") == 0)
                {

                }
                if (item.name.CompareTo("ItemView") == 0)
                {

                }
            }
            else
            {
                ImageView notSupportView = new ImageView("images/not_yet_sign.png");
                notSupportView.Size = new Vector3(_stage.GetSize().width * 0.2f, _stage.GetSize().height * 0.25f, 0.0f);
                notSupportView.SetKeyboardFocusable(true);
                _contentContainer.AddChild(notSupportView, new TableView.CellPosition(((uint)idx / 5) * 2 + 1, (uint)idx % 5));
            }
        }

        public void MainLoop()
        {
            _application.MainLoop();
        }

        /// <summary>
        /// The main entry point for the application.
        /// </summary>

        [STAThread]
        static void Main(string[] args)
        {
            Console.WriteLine("Hello Mono World");

            Example example = new Example(Application.NewApplication());
            example.MainLoop();
        }
    }
}
