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
        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        delegate void CallbackDelegate(IntPtr data);

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        delegate void ActorCallbackDelegate(IntPtr data);

        private Dali.Application _application;
        private ScrollView _scrollView;
        private ScrollBar _scrollBar;

        public Example(Dali.Application application)
        {
            _application = application;
            _application.Initialized += new Dali.AUIApplicationInitEventHandler(Initialize);
        }


        public void Initialize(object source, AUIApplicationInitEventArgs e)
        {
            CreateScrollView();
        }

        private void CreateScrollView()
        {
            Stage stage = Stage.GetCurrent();
            stage.SetBackgroundColor(NDalic.WHITE);

            // Create a scroll view
            _scrollView = new ScrollView();
            Vector2 stageSize = stage.GetSize();
            _scrollView.Size = new Vector3(stageSize.x, stageSize.y, 0.0f);
            _scrollView.ParentOrigin = NDalic.ParentOriginCenter;
            _scrollView.AnchorPoint = NDalic.AnchorPointCenter;
            stage.Add(_scrollView);

            // Add actors to a scroll view with 3 pages
            int pageRows = 1;
            int pageColumns = 3;
            for(int pageRow = 0; pageRow < pageRows; pageRow++)
            {
                for(int pageColumn = 0; pageColumn < pageColumns; pageColumn++)
                {
                    View pageActor = new View();
                    pageActor.SetResizePolicy(ResizePolicyType.FILL_TO_PARENT, DimensionType.ALL_DIMENSIONS);
                    pageActor.ParentOrigin = NDalic.ParentOriginCenter;
                    pageActor.AnchorPoint = NDalic.AnchorPointCenter;
                    pageActor.Position = new Vector3(pageColumn * stageSize.x, pageRow * stageSize.y, 0.0f);

                    // Add images in a 3x4 grid layout for each page
                    int imageRows = 4;
                    int imageColumns = 3;
                    float margin = 10.0f;
                    Vector3 imageSize = new Vector3((stageSize.x / imageColumns) - margin, (stageSize.y / imageRows) - margin, 0.0f);

                    for(int row = 0; row < imageRows; row++)
                    {
                        for(int column = 0; column < imageColumns;column++)
                        {
                            int imageId = (row * imageColumns + column) % 2 + 1;
                            ImageView imageView = new ImageView("images/image-" + imageId + ".jpg");
                            imageView.ParentOrigin = NDalic.ParentOriginCenter;
                            imageView.AnchorPoint = NDalic.AnchorPointCenter;
                            imageView.Size = imageSize;
                            imageView.Position = new Vector3( margin * 0.5f + (imageSize.x + margin) * column - stageSize.x * 0.5f + imageSize.x * 0.5f,
                                                   margin * 0.5f + (imageSize.y + margin) * row - stageSize.y * 0.5f + imageSize.y * 0.5f, 0.0f );
                            pageActor.Add(imageView);
                        }
                    }

                    _scrollView.Add(pageActor);
                }
            }

            _scrollView.SetAxisAutoLock(true);

            // Set scroll view to have 3 pages in X axis and allow page snapping,
            // and also disable scrolling in Y axis.
            RulerPtr scrollRulerX = new RulerPtr(new FixedRuler(stageSize.width));
            RulerPtr scrollRulerY = new RulerPtr(new DefaultRuler());
            scrollRulerX.SetDomain(new RulerDomain(0.0f, stageSize.width * pageColumns, true));
            scrollRulerY.Disable();
            _scrollView.SetRulerX(scrollRulerX);
            _scrollView.SetRulerY(scrollRulerY);

            // Create a horizontal scroll bar in the bottom of scroll view (which is optional)
            _scrollBar = new ScrollBar();
            _scrollBar.ParentOrigin = NDalic.ParentOriginBottomLeft;
            _scrollBar.AnchorPoint = NDalic.AnchorPointTopLeft;
            _scrollBar.SetResizePolicy(ResizePolicyType.FIT_TO_CHILDREN, DimensionType.WIDTH);
            _scrollBar.SetResizePolicy(ResizePolicyType.FILL_TO_PARENT, DimensionType.HEIGHT);
            _scrollBar.Orientation = new Quaternion( new Radian( new Degree( 270.0f ) ), Vector3.ZAXIS );
            _scrollBar.SetScrollDirection(ScrollBar.Direction.Horizontal);
            _scrollView.Add(_scrollBar);

            // Connect to the OnRelayout signal
            _scrollView.OnRelayoutEvent += new Dali.View.OnRelayoutEventHandler(OnScrollViewRelayout);
        }

        private void OnScrollViewRelayout(object source, View.OnRelayoutEventArgs e)
        {
          // Set the correct scroll bar size after size negotiation of scroll view is done
            _scrollBar.Size = new Vector3(0.0f, _scrollView.GetRelayoutSize(DimensionType.WIDTH), 0.0f);
        }

        public void MainLoop()
        {
            _application.MainLoop ();
        }

        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main(string[] args)
        {
            Example example = new Example(Application.NewApplication());
            example.MainLoop ();
        }
    }
}
