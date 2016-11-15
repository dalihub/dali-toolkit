// Copyright 2016 by Samsung Electronics, Inc.,
//
// This software is the confidential and proprietary information
// of Samsung Electronics, Inc. ("Confidential Information"). You
// shall not disclose such Confidential Information and shall use
// it only in accordance with the terms of the license agreement
// you entered into with Samsung.
using System;
using Dali;

//------------------------------------------------------------------------------
// <manual-generated />
//
// This file can only run on Tizen target. You should compile it with hello-test.cs, and
// add tizen c# application related library as reference.
//------------------------------------------------------------------------------
namespace Tizen.Applications
{
    /// <summary>
    /// Represents an application that have UI screen. The DaliApplication class has a default stage.
    /// </summary>
    public class DaliApplication : CoreUIApplication
    {
        /// <summary>
        /// The instance of the Dali Application.
        /// </summary>
        /// <remarks>
        /// This application is created before OnCreate() or created event. And the DaliApplication will be terminated when this application is closed.
        /// </remarks>
        protected Dali.Application application;

        /// <summary>
        /// The instance of the Dali Application extension.
        /// </summary>
        protected Dali.ApplicationExtensions applicationExt;

        /// <summary>
        /// Store the stylesheet value.
        /// </summary>
        protected string m_stylesheet;

        /// <summary>
        /// Store the window mode value.
        /// </summary>
        protected Dali.Application.WINDOW_MODE m_windowMode;

        /// <summary>
        /// Store the app mode value.
        /// </summary>
        protected APP_MODE appMode;

        /// <summary>
        /// The instance of the Dali Stage.
        /// </summary>
        public Stage stage { get; private set; }

        /// <summary>
        /// The default constructor.
        /// </summary>
        public DaliApplication():base()
        {
            appMode = APP_MODE.DEFAULT;
        }

        /// <summary>
        /// The constructor with stylesheet.
        /// </summary>
        public DaliApplication(string stylesheet):base()
        {
            //handle the stylesheet
            appMode = APP_MODE.STYLESHEETONLY;
            m_stylesheet = stylesheet;
        }

        /// <summary>
        /// The constructor with stylesheet and window mode.
        /// </summary>
        public DaliApplication(string stylesheet, Dali.Application.WINDOW_MODE windowMode)
            : base()
        {
            //handle the stylesheet and windowMode
            appMode = APP_MODE.STYLESHEETWITHWINDOWMODE;
            m_stylesheet = stylesheet;
            m_windowMode = windowMode;
        }

        /// <summary>
        /// Overrides this method if want to handle behavior before calling OnCreate().
        /// stage property is initialized in this overrided method.
        /// </summary>
        protected override void OnPreCreate()
        {
            switch(appMode)
            {
                case APP_MODE.DEFAULT:
                    application = Dali.Application.NewApplication();
                    break;
                case APP_MODE.STYLESHEETONLY:
                    application = Dali.Application.NewApplication(m_stylesheet);
                    break;
                case APP_MODE.STYLESHEETWITHWINDOWMODE:
                    application = Dali.Application.NewApplication(m_stylesheet, m_windowMode);
                    break;
                default:
                    break;
            }

            applicationExt = new Dali.ApplicationExtensions(application);
            applicationExt.Init();

            stage = Stage.GetCurrent();
            stage.SetBackgroundColor( NDalic.WHITE );
        }

        /// <summary>
        /// Overrides this method if want to handle behavior.
        /// </summary>
        protected override void OnTerminate()
        {
            base.OnTerminate();
            applicationExt.Terminate();
        }

        /// <summary>
        /// Overrides this method if want to handle behavior.
        /// </summary>
        protected override void OnPause()
        {
            base.OnPause();
            applicationExt.Pause();
        }

        /// <summary>
        /// Overrides this method if want to handle behavior.
        /// </summary>
        protected override void OnResume()
        {
            base.OnResume();
            applicationExt.Resume();
        }

        /// <summary>
        /// Overrides this method if want to handle behavior.
        /// </summary>
        protected override void OnLocaleChanged(LocaleChangedEventArgs e)
        {
            base.OnLocaleChanged(e);
            applicationExt.LanguageChange();
        }

        /// <summary>
        /// The mode of creating Dali application.
        /// </summary>
        protected enum APP_MODE
        {
            DEFAULT = 0,
            STYLESHEETONLY = 1,
            STYLESHEETWITHWINDOWMODE = 2
        }
    }
}
