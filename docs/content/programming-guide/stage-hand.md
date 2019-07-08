<!--
/**-->

# Stagehand Visual Debugger for DALi {#stagehand}

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
  
![ ](inner-workings.png)

## Connecting Stagehand to your DALi application

### Preparing DALi

Stagehand connects to DALi via network using a TCP/IP connection, to enable this, your dali-adaptor RPM must be built with a configure option: `--enable-networklogging`

Here is an example dali-adaptor configure line:
~~~
$ CXXFLAGS="-g -O0 -Wno-unused-local-typedefs" CXX="ccache g++" ./configure --prefix=$DESKTOP_PREFIX --enable-debug=yes --enable-profile=UBUNTU --enable-networklogging
~~~

Once this RPM is installed, you can run your DALi application and connect Stagehand to it.
  

### Network setup

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

![ ](netstat.png)


### Connecting with Stagehand

Click the settings icons below
  
![ ](stagehand-settings.png)


### Connecting to DALi running on Tizen

Here we are connecting to a device running SDB
  
![ ](stagehand-tizen-connection.png)

### Connect to DALi running on Desktop ( Ubuntu )
  
![ ](stagehand-ubuntu-connection.png)

## Using Stagehand

Click Refresh to load the current scene from the device.
  
![ ](stagehand-refesh.png)

Click screen shot to get overlay current screen
  
![ ](stagehand-screenshot.png)

To cycle through the actor hierarchy, keep clicking the same spot. Alternatively, select using the actor tree.

  
![ ](stagehand-mainscreen.png)


Click the save icon to save the scene
  
![ ](stagehand-save.png)

Zooming in and out
  
![ ](stagehand-zoom.png)

Modifying the scene
  
![ ](stagehand-modify.png)

## Performance monitoring

To enable performance option. Edit
  
~/.stagehand/stagehand.ini
  
Set perfmode=on

Restart application. Click performance
  
![ ](stagehand-performance.png)

## Trouble shooting

You can manually check the network is enabled on the DALi application using netcat.

In the example below we are connecting to a Tizen device using SDB port forwarding.
  
After running nc localhost 3031 we type help
  
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

