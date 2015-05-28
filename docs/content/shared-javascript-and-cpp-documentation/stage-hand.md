/**
 *
# Stagehand Visual Debugger for DALi {#stagehand}

![ ](../assets/img/stage-hand/blocks.png)
![ ](blocks.png)

## Introduction

Stagehand  is an open source tool that allows a developer to:

- Connect to a DALi application running on:
 - Tizen
 - Ubuntu
- View a wireframe of the current scene on top of a screen shot
- Modify properties in the scene
- Monitor performance of the application

  
Full source code is available from http://github.com/gamoeba/stagehand.git

Stagehand is released under the Apache 2 licence.

## Installing StageHand on Ubuntu

http://www.gamoeba.com/stagehand/

Install both files on the website. Start with the QT dependencies first.


## How it works

Stagehand connects to DALi via the network using a TCP/IP connection.
  
![ ](../assets/img/stage-hand/inner-workings.png)
![ ](inner-workings.png)

## Network setup

To enable network control on DALi, an environment variable must be set:

~~~
$ export DALI_NETWORK_CONTROL=1 to enable
~~~
Or run the application with the variable set.
~~~
$ DALI_NETWORK_CONTROL=1 /usr/apps/com.samsung.dali-demo/bin/dali-demo
~~~

Check what network port the application is using. It will be from port 3031 onwards.
  
If running DALi on desktop, just type netstat -tlnp. On Tizen log into the device and run netstat. E.g.
~~~
$ su

$ netstat -tlpn
~~~

![ ](../assets/img/stage-hand/netstat.png)
![ ](netstat.png)


## Connecting with Stagehand

Click the settings icons below
  
![ ](../assets/img/stage-hand/stagehand-settings.png)
![ ](stagehand-settings.png)


### Connecting to DALi running on Tizen

Here we are connecting to a device running SDB
  
![ ](../assets/img/stage-hand/stagehand-tizen-connection.png)
![ ](stagehand-tizen-connection.png)

### Connect to DALi running on Desktop ( Ubuntu )
  
![ ](../assets/img/stage-hand/stagehand-ubuntu-connection.png)
![ ](stagehand-ubuntu-connection.png)

## Using Stagehand

Click Refresh to load the current scene from the device.
  
![ ](../assets/img/stage-hand/stagehand-refesh.png)
![ ](stagehand-refesh.png)

Click screen shot to get overlay current screen
  
![ ](../assets/img/stage-hand/stagehand-screenshot.png)
![ ](stagehand-screenshot.png)

To cycle through the actor hierachy, keep clicking the same spot. Alternatively, select using the actor tree.

  
![ ](../assets/img/stage-hand/stagehand-mainscreen.png)
![ ](stagehand-mainscreen.png)


Click the save icon to save the scene
  
![ ](../assets/img/stage-hand/stagehand-save.png)
![ ](stagehand-save.png)

Zooming in and out
  
![ ](../assets/img/stage-hand/stagehand-zoom.png)
![ ](stagehand-zoom.png)

Modifying the scene
  
![ ](../assets/img/stage-hand/stagehand-modify.png)
![ ](stagehand-modify.png)

## Performance monitoring

To enable performance option. Edit
  
~/.stagehand/stagehand.ini
  
Set perfmode=on

Restart application. Click performance
  
![ ](../assets/img/stage-hand/stagehand-performance.png)
![ ](stagehand-performance.png)

## Trouble shooting

You can manually check the network is enabled on the DALi application using netcat.

In the example below we are connecting to a Tizen device using SDB port forwarding.
  
After running nc localhost 3031 we type help
  
![ ](../assets/img/stage-hand/stagehand-netcat.png)
![ ](stagehand-netcat.png)


## Tizen smack


If you can't connect to a DALi application, but using netstat you can see a port is open.
  
Check the smack log for errors:
~~~
tail -f -n 10 /var/log/audit/audit.log | grep 'internet'
~~~

If it is being blocked, you temporarily enable it by editing:
  
~~~
 /etc/smack/accesses2.d/ your-app-name

then add: system::user_internet …..app-name rw
~~~
## Tizen Emulator connection problem


Make sure  DALi application is run using launch_app:
~~~
launch_app [APP_ID] __AUL_SDK__ DEBUG __DLP_DEBUG_ARG__ :10003
~~~

@class _Guide_Visual_Debugger