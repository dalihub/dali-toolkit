#!/bin/bash

TEMP=`getopt -o dhsSmfqp: --long debug,help,failnorerun,quiet,serial,tct,modules,prefix: -n 'execute.sh' -- "$@"`

if [ $? != 0 ] ; then echo "Terminating..." >&2 ; exit 1 ; fi

# Note the quotes around `$TEMP': they are essential!
eval set -- "$TEMP"

function usage
{
    echo -e "Usage: execute.sh [-d][-s|-S|-r][-q] [module|testcase]"
    echo -e "       execute.sh\t\tExecute test cases from all modules in parallel"
    echo -e "       execute.sh -f \tExecute test cases from all modules in parallel without rerunning failed test cases"
    echo -e "       execute.sh -d <testcase>\tDebug testcase"
    echo -e "       execute.sh -p <prefix>\tExecute all testcases named with <prefix>"
    echo -e "       execute.sh [module]\tExecute test cases from the given module in parallel"
    echo -e "       execute.sh -s [module]\t\tExecute test cases in serial using Testkit-Lite"
    echo -e "       execute.sh -S [module]\t\tExecute test cases in serial"
    echo -e "       execute.sh -q|--quiet ...\tExecute test cases, but don't write output"
    echo -e "       execute.sh <testcase>\tFind and execute the given test case"
    exit 2
}

opt_tct=0
opt_serial=""
opt_modules=0
opt_debug=0
opt_noFailedRerun="";
opt_quiet="";
opt_match="";
while true ; do
    case "$1" in
        -h|--help)     usage ;;
        -d|--debug)    opt_debug=1 ; shift ;;
        -s|--tct)      opt_tct=1 ; shift ;;
        -f|--nofailedrerun) opt_noFailedRerun="-f" ; shift ;;
        -S|--serial)   opt_serial="-s" ; shift ;;
        -q|--quiet)    opt_quiet="-q" ; shift ;;
        -p|--prefix)   opt_match="-m $2" ; shift 2;;
        -m|--modules)  opt_modules=1 ; shift ;;
        --) shift; break;;
        *) echo "Internal error $1!" ; exit 1 ;;
    esac
done

function execute_tct
{
    scripts/tctestsgen.sh $1 `pwd` desktop $2
    testkit-lite -f `pwd`/tests.xml -o tct-${1}-core-tests.xml  -A --comm localhost
    scripts/add_style.pl $1
}

function summary_start
{
    start=`date +"%Y-%m-%d_%H_%M_%S"`
    cat > summary.xml <<EOF
<?xml version="1.0" encoding="UTF-8"?>
<?xml-stylesheet type="text/xsl" href="./style/summary.xsl"?>
<result_summary plan_name="Core">
  <other xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:type="xs:string" />
  <summary test_plan_name="Dali">
    <start_at>$start</start_at>
    <end_at>$start</end_at>
  </summary>
EOF
}

function summary_end
{
    cat >> summary.xml <<EOF
</result_summary>
EOF
}

function output_start
{
    start=`date +"%Y-%m-%d_%H_%M_%S"`
    cat > tct-${1}-core-tests.xml <<EOF
<?xml version="1.0" encoding="UTF-8"?>
<?xml-stylesheet type="text/xsl" href="./style/testresult.xsl"?>
<test_definition>
<environment build_id="" device_id="localhost" device_model="" device_name="N/A" host="Ubuntu" manufacturer="" resolution="N/A" screen_size="N/A"><other /></environment>
<summary test_plan_name="Empty test_plan_name"><start_at>$start</start_at><end_at>$start</end_at></summary>
  <suite category="Core APIs" name="tct-$1-core-tests">
    <set name="default" set_debug_msg="automated-tests.auto.suite_1_set_1.dlog">
EOF
}


function output_end
{
    cat >> tct-${1}-core-tests.xml <<EOF
    </set>
  </suite>
</test_definition>
EOF
}

modules=`ls -1 build/src | grep -v CMakeFiles | grep -v cmake_install.cmake | grep -v Makefile`

if [ $opt_modules == 1 ] ; then
    modules= get_modules
    echo $modules
    exit 0
fi

# Clean up old test results
rm -f tct*core-tests.xml

# Clean up old coverage data
[ -d ../build/tizen ] && rm -f ../build/tizen/dali/.libs/*.gcda
[ -d ../build/tizen-cmake ] && find ../build/tizen-cmake/ -name \*.gcda -exec rm {} \;

find build \( -name "*.gcda" \) -exec rm '{}' \;

ASCII_BOLD="\e[1m"
ASCII_RESET="\e[0m"

if [ -f summary.xml ] ; then unlink summary.xml ; fi

if [ $opt_tct == 1 ] ; then
    # Use Test-kit lite
    # Run all test case executables serially, create XML output
    if [ -n "$1" ] ; then
        execute_tct $1 $*
    else
        for mod in $modules
        do
            if [ $mod != 'common' ] && [ $mod != 'manual' ]; then

                echo -ne "$ASCII_BOLD"
                echo -e "Executing $mod$ASCII_RESET"
                execute_tct $mod $*
            fi
        done
    fi

    scripts/summarize.pl

else
    # Execute test cases using own test harness

    if [ -z "$1" ] ; then
        # No arguments:
        # Execute each test executable in turn (by default, runs tests in parallel)
        summary_start
        for mod in $modules
        do
            echo -e "$ASCII_BOLD"
            echo -e "Executing $mod$ASCII_RESET"
            output_start $mod
            dbus-launch build/src/$mod/tct-$mod-core $opt_serial $opt_noFailedRerun $opt_quiet $opt_match
            output_end $mod
        done
        summary_end

    elif [ -f "build/src/$1/tct-$1-core" ] ; then
        # First argument is an executable filename - execute only that with any
        # remaining arguments
        summary_start
        module=$1
        shift;
        output_start ${module}
        dbus-launch build/src/$module/tct-$module-core $opt_serial $opt_noFailedRerun $opt_quiet $opt_match $*
        output_end ${module}
        summary_end

    else
        # First argument is not an executable. Is it a test case name?
        # Try executing each executable with the test case name until success/known failure
        for mod in $modules
        do
            output=`build/src/$mod/tct-$mod-core $1`
            ret=$?
            if [ $ret -ne 6 ] ; then
                if [ $opt_debug -ne 0 ] ; then
                    echo DEBUGGING:
                    dbus-launch gdb --args build/src/$mod/tct-$mod-core $1

                else
                    echo $output
                    if [ $ret -eq 0 ] ; then echo -e "\nPassed" ; fi
                fi
                exit $ret
            fi
        done
        echo $1 not found
    fi

    # Kill off any dangling or sleeping dbus sessions that we would have created
    cgroup=$(awk -F ':' '$2 == "name=systemd" { print $3 }' /proc/self/cgroup)
    if [ -n "$cgroup" ] ; then
        for pid in $(cat /sys/fs/cgroup/systemd/$cgroup/tasks 2>/dev/null); do
            comm=$(cat /proc/$pid/comm 2>/dev/null)
            case "$comm" in
            dbus-daemon|dbus-launch)
                kill $pid
                ;;
            esac
        done
    fi
fi

if [ -f summary.xml ] ; then
    scripts/output_summary.pl
fi

exit $?
