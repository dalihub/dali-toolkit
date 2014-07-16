/*! \page resource-tracking Resource Tracking
 *
 *
 * <h2 class="pg">Enable Logging</h2>
 * Setting DALI_ENABLE_LOG environment variable to RESOURCE_LOG will enable resource usage logging in Dali applications.<br>
 *
 * On target resource logging utilizes dlog, but this can also be used on desktop by redirecting stderr to a file.<br>
 *
 * The generated information includes any image files that are loaded with their dimensions,<br>
 *  GPU memory consumption, CPU RAM used and details of texture atlases created.
 *
 * <h2 class="pg">Viewing Resource Logs</h2>
 * dalireslog.sh is installed as part of the dali-adaptor package and can be found in the adaptors/tizen/scripts folder.<br>
 * The script shows a summary of memory used by resources.
 *
 * USAGE:
 * ./dalireslog.sh [FILE]<br>
 * if FILE isn't specified, the script will try to use dlogutil.
 *
 * Example:
 *
 * &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;sh-4.1$ ./dalireslog.sh<br>
 *
 * &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<i>On a separate terminal:</i><br>
 * &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;sh-4.1$ DALI_ENABLE_LOG=RESOURCE_LOG /opt/apps/com.samsung.dali-demo/bin/album.example
 *
 * Example on desktop:<br><br>
 * &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;jon-doe\@ws-1234$ DALI_ENABLE_LOG=RESOURCE_LOG blind-effect.example 2>/home/SERILOCAL/john.doe/log.txt<br>
 *
 * &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<i>On a separate terminal:</i><br>
 * &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;dalireslog.sh ~/log.txt
 *
 * Displayed information:<br>
 *
 * <ul>
 * <li>3D - amount of GPU memory used by application.<br>
 * <li>MEM Atlas - amount of GPU memory used by texture atlases (usually this refers to font atlases). <br>
 * <li>Number of atlases - how many texture atlases are present in memory.<br>
 * </ul>
 * A list of files is displayed in the main view, with different color codes representing different states:<br>
 *
 * <ul>
 * <li>CPU - resource is in memory, but hasn't been uploaded to a GL texture.<br>
 * <li>GPU - resource has been uploaded to a GL texture, bitmap buffer discarded.<br>
 * <li>CPUGPU - resource has been uploaded to a GL texture, but still present in CPU memory as well.<br>
 * <li>DISCARDED - resource has been discarded, memory freed up.
 * </ul>
 */
