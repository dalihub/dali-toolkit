<!--
/**-->

# Resource Tracking {#resourcetracking}

## Enable Logging

Setting DALI_ENABLE_LOG environment variable to RESOURCE_LOG will enable resource usage logging in DALi applications.
On target resource logging utilizes dlog, but this can also be used on desktop by redirecting stderr to a file.
The generated information includes any image files that are loaded with their dimensions,
GPU memory consumption, CPU RAM used and details of texture atlases created.

## Viewing Resource Logs

dalireslog.sh is installed as part of the DALi Adaptor package and can be found in the adaptors/tizen/scripts folder.
The script shows a summary of memory used by resources.
USAGE:
./dalireslog.sh [FILE]
if FILE isn't specified, the script will try to use dlogutil.

### Example:
~~~{.bash}
sh-4.1$ ./dalireslog.sh

**On a separate terminal:**

sh-4.1$ DALI_ENABLE_LOG=RESOURCE_LOG /opt/apps/com.samsung.dali-demo/bin/album.example
~~~
Example on desktop:
~~~{.bash}
jon-doe\@ws-1234$ DALI_ENABLE_LOG=RESOURCE_LOG blind-effect.example 2>/home/SERILOCAL/john.doe/log.txt

**On a separate terminal:**

dalireslog.sh ~/log.txt

~~~

### Displayed information:

|  | |
|--|-|
| 3D | amount of GPU memory used by application |
| MEM Atlas | amount of GPU memory used by texture atlases (usually this refers to font atlases)
| Number of atlases | how many texture atlases are present in memory.|

A list of files is displayed in the main view, with different color codes representing different states:

| | |
|-|-|
|CPU | resource is in memory, but hasn't been uploaded to a GL texture.|
|GPU | resource has been uploaded to a GL texture, bitmap buffer discarded.|
|CPUGPU | resource has been uploaded to a GL texture, but still present in CPU memory as well.|
|DISCARDED | resource has been discarded, memory freed up |

@class _Guide_ResourceTracking
 */
