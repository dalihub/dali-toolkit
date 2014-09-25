/*! \page performance-profiling Performance Profiling
 *
 * <h2 class="pg">Enable Logging</h2>
 *
 * Setting DALI_LOG_PERFORMANCE environment variable will enable performance profiling <br>
 * It uses a bit mask to decide what to log. <br>
 * The log options are:<br>
 * \code
 * Bit 0 = Log update and render threads  (e.g.  DALI_LOG_PERFORMANCE=1 dali-demo)
 * Bit 1 = Log event process time         (e.g.  DALI_LOG_PERFORMANCE=2 dali-demo)
 * Bit 2 = Log Dali markers to trace file (e.g.  DALI_LOG_PERFORMANCE=4 dali-demo)
 *
 * To log both update, render and event times, then combine bits 0 and 1.<br>
 * DALI_LOG_PERFORMANCE=3 dali-demo
 * \endcode
 *
 *
 * <h2 class="pg">Background</h2>
 * The Dali rendering pipeline has 2 stages.
 * Each stage is typically run once per frame.
 * <h3> 1. Update </h3>
 * <ul>
 * <li> Run animations</li>
 * <li> Run constraints</li>
 * <li> Run physics</li>
 * <li> Update the scene-graph</li>
 * </ul>
 * <h3> 2. Render </h3>
 * <ul>
 * <li> Upload 3D data using OpenGL ( textures, vertex buffers etc).</li>
 * <li> Draw the scene using OpenGL</li>
 * </ul>
 *
 *
 * To run at 60 FPS (16 milliseconds per frame), it is recommended to stay below the following times:
 * <ul>
 * <li> Update: 4 milliseconds</li>
 * <li> Render: 4 milliseconds</li>
 * </ul>
 *
 * This will leave enough time for the output to be composited (if the system uses a compositor) and to avoid using
 * too much CPU power.
 * The main Dali application thread which deals with event processing is independent of the update / render threads. <br>
 * This means animations won't stop if the main thread decides to do a long operation like downloading a file from the internet.
 *
 * Performance logging uses Dali log output which on Tizen is dlog, but this can also be used on desktop by redirecting stderr to a file.<br>
 *
 * Example:
 * \code
 * $ export DALI_LOG_PERFORMANCE=1
 * $ dali-demo
 * $
 * $ ...
 * $ I/DALI ( 5692): slp-logging.cpp: LogMessage(40) > Update , min 0.59 ms, max 6.43 ms, total (3.4 secs), avg 1.26 ms
 * $ I/DALI ( 5692): slp-logging.cpp: LogMessage(40) > Render , min 1.67 ms, max 5.01 ms, total (4.5 secs), avg 3.71 ms
 * \endcode
 *
 * If nothing is animating Dali will enter a paused state to save power. At this
 * point nothing will be logged.
 *
 *
 *  <h2 class="pg">Application profiling</h2>
 *
 *  The main application thread in Dali is used to process and respond to events such as touch, key, mouse, gestures and timers. <br>
 *  The time taken to process events can be measured by setting DALI_LOG_PERFORMANCE environment variable to 2 <br>
 *
 * Example:
 * \code
 * $ export DALI_LOG_PERFORMANCE=2
 * $ dali-demo
 * $
 * $ ...
 * $ INFO: DALI: Event , min 0.01 ms, max 14.99 ms, total (2.4 secs), avg 1.83 ms
 * \endcode
 *
 * Inside the event processing, the application may be listening for certain events. <br>
 * For example when an actor is touched, some application code may be run in an OnTouchEvent callback. <br>
 * By checking the max times you can check for any spikes that occur when interacting with the application.
 *
 * Example:
 * \code
 * $ INFO: DALI: Event , min 0.10 ms, max 500.01 ms, total (6.4 secs), avg 20.83 ms
 *
 * - Something has taken 500 ms = 1/2 second during event processing.
 * - Need to investigate what the application is doing for 1/2 a second.
 * \endcode
 *
 *
 * <h2 class="pg">Logging performance markers to Kernel trace file</h2>
 *
 * Ftrace is a kernel tracer designed to help developers find out what is going on inside the kernel.<br>
 * It can be used for analysing how long Dali takes to perform different tasks and <br>
 * what Dali is doing in relation to other system processes / interrupts.
 *
 * On Tizen if the kernel has been built with ftrace enabled, then Dali can log out to ftrace.<br>
 * This gives exact time stamps of the main events in Dali.
 * Current markers that are logged:
 * <ul>
 * <li> DALI_V_SYNC. The heart beat which represents Dali should start creating a new frame if anything has changed.<br>
 * Typically runs 60 Frames per second, depending on display refresh rate.
 * <li> DALI_UPDATE_START. Dali update task has started.
 * <li> DALI_UPDATE_START. Dali update task has finished
 * <li> DALI_RENDER_START. Dali render task has started
 * <li> DALI_RENDER_END. Dali render task has finished
 * </ul>
 *
 * <h3> Checking ftrace is working on Linux</h3>
 *
 * Documentation for ftrace:
 * Follow these instructions to ensure the debugfs has been mounted, and the kernel you are using
 * has been built with ftrace enabled. <br>
 * https://www.kernel.org/doc/Documentation/trace/ftrace.txt
 *
 * To check ftrace is working:
 * \code
 * $ cd /sys/kernel/debug/tracing
 * $ echo 1 > tracing_enabled    (enabled tracing)
 * $ echo "test" > trace_marker
 * $ echo 0 > tracing_enabled    (disable tracing)
 * $ cat trace
 * #
 * #          TASK-PID    CPU#    TIMESTAMP  FUNCTION
 * #             | |       |          |         |
 *          <...>-2539  [001] 267964.345607: tracing_mark_write: test
 *
 *
 * If the message did not get added to the trace, then check the write permissions on trace_marker file. E.g.
 * $ chmod ugoa+w trace_marker
 * \endcode
 *
 * To view Dali markers in trace file<br>
 *
 * \code
 * $ export DALI_LOG_PERFORMANCE=4
 * $ dali-demo
 * $
 * $ cat /sys/kernel/debug/tracing/trace
 *
 *   <...>-3330  [000] 785155.216611: tracing_mark_write: SPI_EV_DALI_V_SYNC
 *   <...>-3328  [003] 785155.216644: tracing_mark_write: SPI_EV_DALI_UPDATE_START
 *   <...>-3328  [003] 785155.217045: tracing_mark_write: SPI_EV_DALI_UPDATE_END
 *   <...>-3329  [001] 785155.227418: tracing_mark_write: SPI_EV_DALI_RENDER_START
 *   <...>-3329  [001] 785155.227807: tracing_mark_write: SPI_EV_DALI_RENDER_END
 *   <...>-3330  [000] 785155.233336: tracing_mark_write: SPI_EV_DALI_V_SYNC
 *   <...>-3328  [002] 785155.233374: tracing_mark_write: SPI_EV_DALI_UPDATE_START
 *   <...>-3328  [002] 785155.233672: tracing_mark_write: SPI_EV_DALI_UPDATE_END
 *   <...>-3329  [001] 785155.235161: tracing_mark_write: SPI_EV_DALI_RENDER_START
 *   <...>-3329  [001] 785155.235475: tracing_mark_write: SPI_EV_DALI_RENDER_END
 *   <...>-3330  [000] 785155.250029: tracing_mark_write: SPI_EV_DALI_V_SYNC
 *   <...>-3328  [003] 785155.250065: tracing_mark_write: SPI_EV_DALI_UPDATE_START
 *   <...>-3328  [003] 785155.250330: tracing_mark_write: SPI_EV_DALI_UPDATE_END
 *   <...>-3329  [001] 785155.252860: tracing_mark_write: SPI_EV_DALI_RENDER_START
 *   <...>-3329  [001] 785155.253178: tracing_mark_write: SPI_EV_DALI_RENDER_END
 *   <...>-3329  [001] 785155.264508: tracing_mark_write: SPI_EV_DALI_RENDER_START
 *   <...>-3329  [001] 785155.265006: tracing_mark_write: SPI_EV_DALI_RENDER_END
 * \endcode
 */
