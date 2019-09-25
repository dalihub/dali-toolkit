# Run doxygen to generate extremely verbose documentation and print the time it took to do so.

LOG_FILE="build.`date +%Y_%m_%d_%H_%M_%S`.log"

export START_TIME=`date +%s`
ionice -c 3 nice doxygen dali-internal.doxy  2>&1 | tee "$LOG_FILE"
export END_TIME=`date +%s`

let "BUILD_TIME = $END_TIME - $START_TIME"
let "BUILD_TIME_MINS = $BUILD_TIME / 60"
let "BUILD_TIME_EXTRA_SECONDS = $BUILD_TIME % 60"
BUILD_TIME_MSG="Build ran from $START_TIME until $END_TIME, $BUILD_TIME_MINS minutes and $BUILD_TIME_EXTRA_SECONDS seconds, finishing: `date`."
echo "$BUILD_TIME_MSG"

WARNINGS=`egrep ": warning: " < $LOG_FILE | wc -l`
echo -e "\nWARNING COUNT: $WARNINGS"
