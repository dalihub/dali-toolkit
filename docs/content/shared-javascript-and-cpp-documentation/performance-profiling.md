/**
 *

# Performance Profiling  {#performanceprofiling}


DALi has many mechanisms for analysing performance including kernel, system and network logging.


## Background
The Dali rendering pipeline has 2 stages.

Each stage is typically run once per frame.

- Update
  - Run animations
  - Run constraints
  - Run physics
  - Update the scene-graph
- Render
  - Upload 3D data using OpenGL ( textures, vertex buffers etc).
  - Draw the scene using OpenGL
  

Update produces data - **Writes** final object positions to a buffer
  
Render consumes data - **Reads** object positions from a buffer and draws with OpenGL

![ ](../assets/img/performance/update-render.png)
![ ](update-render.png)

  
One reason for having 2 buffers is to allow both tasks to overlap and run in parallel in certain situations.
E.g. if rendering is taking a long time (due to a texture upload), the Update thread can start work producing
data for the next frame. The aim being to take advantage of multi-core CPU's.
  
To run at a solid 60 FPS (16 milliseconds per frame), it is recommended to stay below the following times:
  
 - Update: 4 milliseconds
 - Render: 4 milliseconds
  
This will leave enough time for the output to be composited (if the system uses a compositor) and to avoid using
too much CPU power.
  
The main Dali application thread which deals with event processing is independent of the update / render threads.
This means animations won't stop if the main thread decides to do a long operation like downloading a file from the internet.
  

## Time Stamp Logging

This type of logging is used for recording individual time stamped events.
  
Setting DALI_PERFORMANCE_TIMESTAMP_OUTPUT environment variable will enable time stamps.

Tools such as Tizen dynamic analyser and StageHand can be used to provide a GUI display of
the output.


The log options are:

|  Bit |  Function                | Example      |
|------|--------------------------|--------------|
|   0  |  log markers to DALi log (dlog on Tizen) | DALI_PERFORMANCE_TIMESTAMP_OUTPUT=1 dali-demo |
|   1  |  log markers to kernel trace ( logs to ftrace )| DALI_PERFORMANCE_TIMESTAMP_OUTPUT=2 dali-demo |
|   2  |  log markers to system trace ( ttrace on Tizen for Tizen analyser) | DALI_PERFORMANCE_TIMESTAMP_OUTPUT=4 dali-demo |
|   3  |  log markers to network client (tcp port 3001+) | DALI_PERFORMANCE_TIMESTAMP_OUTPUT=8 dali-demo |

  

~~~
DALI_PERFORMANCE_TIMESTAMP_OUTPUT=1 dali-demo
INFO: DALI: 1134155.500142 (seconds), V_SYNC
INFO: DALI: 1134155.500167 (seconds), UPDATE_START
INFO: DALI: 1134155.500214 (seconds), PROCESS_EVENT_END
INFO: DALI: 1134155.500659 (seconds), UPDATE_END
INFO: DALI: 1134155.508039 (seconds), PROCESS_EVENT_START
INFO: DALI: 1134155.508295 (seconds), PROCESS_EVENT_END
INFO: DALI: 1134155.511109 (seconds), RENDER_START
INFO: DALI: 1134155.511548 (seconds), RENDER_END
INFO: DALI: 1134155.516899 (seconds), V_SYNC
INFO: DALI: 1134155.516945 (seconds), UPDATE_START
INFO: DALI: 1134155.517462 (seconds), UPDATE_END
INFO: DALI: 1134155.527884 (seconds), RENDER_START
INFO: DALI: 1134155.528108 (seconds), PROCESS_EVENT_START
INFO: DALI: 1134155.528327 (seconds), RENDER_END
INFO: DALI: 1134155.528358 (seconds), PROCESS_EVENT_END
INFO: DALI: 1134155.528388 (seconds), PROCESS_EVENT_START
INFO: DALI: 1134155.528749 (seconds), PROCESS_EVENT_END
INFO: DALI: 1134155.533672 (seconds), V_SYNC
~~~

### Markers that are logged

| Marker | Description
|--------|-------------
| V_SYNC.| The heart beat which represents Dali should start creating a new frame if anything has changed. Runs at display refresh rate, typically 60Hz |
| UPDATE_START | Dali update task has started |
| UPDATE_START | Dali update task has finished |
| RENDER_START | Dali render task has started |
| RENDER_END | Dali render task has finished |
| PROCESS_EVENT_START | Dali main thread processing events (e.g. in response to a touch event or a timer) |
| PROCESS_EVENT_START | Dali main thread processing events finished |
| SWAP_START | glSwapBuffers started (todo) |
| SWAP_END | glSwapBuffers end  (todo) |
| PAUSE  | Application paused |
| RESUME | Application resumed |

### Custom time stamps for application developers

A developer can output custom markers using the PerformanceLogger API (C++ only currently)

~~~
PerformanceLogger logger = PerformanceLogger::New("MyMarker");
logger.AddMarker(PerformanceLogger::START_EVENT);

// do stuff

logger.AddMarker(PerformanceLogger::END_EVENT);
~~~

## Statistics logging

Statistics logging uses Dali log output which on Tizen is dlog, but this can also be used on desktop by redirecting stderr to a file.

Setting DALI_LOG_PERFORMANCE_STATS environment variable will enable time stamps.

The log options are:

|  Bit |  Function                | Example      |
|------|--------------------------|--------------|
|   0  |  log all statistics to the DALi log | DALI_LOG_PERFORMANCE_STATS=1 dali-demo |
|   1  |  log update and render statistics to the DALi log| DALI_LOG_PERFORMANCE_STATS=2 dali-demo |
|   2  |  log event (main) task statistics to the DALi log| DALI_LOG_PERFORMANCE_STATS=4 dali-demo |
|   3  |  log custom marker statistics to the DALi log | DALI_LOG_PERFORMANCE_STATS=8 dali-demo |

Example output
~~~
$ export DALI_LOG_PERFORMANCE_STATS=1
$ dali-demo

 Event, min 0.04 ms, max 5.27 ms, total (0.1 secs), avg 0.28 ms, std dev 0.73 ms
 Update, min 0.29 ms, max 0.91 ms, total (0.5 secs), avg 0.68 ms, std dev 0.15 ms
 Render, min 0.33 ms, max 0.97 ms, total (0.6 secs), avg 0.73 ms, std dev 0.17 ms
 TableViewInit, min 76.55 ms, max 76.55 ms, total (0.1 secs), avg 76.55 ms, std dev 0.00 ms
~~~

If nothing is animating Dali will enter a paused state to save power. At this
point nothing will be logged.

### Custom statistics for application developers

This is identical to the custom timestamp example.
~~~
PerformanceLogger logger = PerformanceLogger::New("MyMarker");
logger.AddMarker(PerformanceLogger::START_EVENT);

// do stuff

logger.AddMarker(PerformanceLogger::END_EVENT);
~~~


## Application profiling

 The main application thread in Dali is used to process and respond to events such as touch, key, mouse, gestures and timers.

Example:
~~~
$ export DALI_LOG_PERFORMANCE_STATS=4
$ dali-demo
$
$ ...
$ INFO: DALI:  Event, min 0.04 ms, max 5.27 ms, total (0.1 secs), avg 0.28 ms, std dev 0.73 ms
~~~

Inside the event processing, the application may be listening for certain events.
For example when an actor is touched, some application code may be run in an OnTouchEvent callback.
By checking the max times you can check for any spikes that occur when interacting with the application.

Example:
~~~
$ INFO: DALI: Event , min 0.10 ms, max 500.01 ms, total (6.4 secs), avg 20.83 ms

- Something has taken 500 ms = 1/2 second during event processing.
- Need to investigate what the application is doing for 1/2 a second.
~~~


## Using ftrace for timestamp logging

~~~
DALI_PERFORMANCE_TIMESTAMP_OUTPUT=2 dali-demo
~~~

Ftrace is a kernel tracer designed to help developers find out what is going on inside the kernel.
It can be used for analysing how long Dali takes to perform different tasks and
what Dali is doing in relation to other system processes / interrupts.
  
On Tizen if the kernel has been built with ftrace enabled, then Dali can log out to ftrace.
This gives exact time stamps of the main events in Dali.
Current markers that are logged:



### Checking ftrace is working on Linux

Documentation for ftrace:
Follow these instructions to ensure the debugfs has been mounted, and the kernel you are using
has been built with ftrace enabled.

https://www.kernel.org/doc/Documentation/trace/ftrace.txt

To check ftrace is working:
~~~
$ cd /sys/kernel/debug/tracing
$ echo 1 > tracing_enabled    (enabled tracing)
$ echo "test" > trace_marker
$ echo 0 > tracing_enabled    (disable tracing)
$ cat trace
#
#          TASK-PID    CPU#    TIMESTAMP  FUNCTION
#             | |       |          |         |
         <...>-2539  [001] 267964.345607: tracing_mark_write: test


If the message did not get added to the trace, then check the write permissions on trace_marker file. E.g.
$ chmod ugoa+w trace_marker
~~~
To view Dali markers in trace file

~~~
$ export DALI_LOG_PERFORMANCE=2
$ dali-demo
$
$ cat /sys/kernel/debug/tracing/trace

  <...>-3330  [000] 785155.216611: tracing_mark_write: SPI_EV_DALI_V_SYNC
  <...>-3328  [003] 785155.216644: tracing_mark_write: SPI_EV_DALI_UPDATE_START
  <...>-3328  [003] 785155.217045: tracing_mark_write: SPI_EV_DALI_UPDATE_END
  <...>-3329  [001] 785155.227418: tracing_mark_write: SPI_EV_DALI_RENDER_START
  <...>-3329  [001] 785155.227807: tracing_mark_write: SPI_EV_DALI_RENDER_END
  <...>-3330  [000] 785155.233336: tracing_mark_write: SPI_EV_DALI_V_SYNC
  <...>-3328  [002] 785155.233374: tracing_mark_write: SPI_EV_DALI_UPDATE_START
  <...>-3328  [002] 785155.233672: tracing_mark_write: SPI_EV_DALI_UPDATE_END
  <...>-3329  [001] 785155.235161: tracing_mark_write: SPI_EV_DALI_RENDER_START
  <...>-3329  [001] 785155.235475: tracing_mark_write: SPI_EV_DALI_RENDER_END
  <...>-3330  [000] 785155.250029: tracing_mark_write: SPI_EV_DALI_V_SYNC
  <...>-3328  [003] 785155.250065: tracing_mark_write: SPI_EV_DALI_UPDATE_START
  <...>-3328  [003] 785155.250330: tracing_mark_write: SPI_EV_DALI_UPDATE_END
  <...>-3329  [001] 785155.252860: tracing_mark_write: SPI_EV_DALI_RENDER_START
  <...>-3329  [001] 785155.253178: tracing_mark_write: SPI_EV_DALI_RENDER_END
  <...>-3329  [001] 785155.264508: tracing_mark_write: SPI_EV_DALI_RENDER_START
  <...>-3329  [001] 785155.265006: tracing_mark_write: SPI_EV_DALI_RENDER_END
~~~
@class _Guide_Performance_Profiling
*/


