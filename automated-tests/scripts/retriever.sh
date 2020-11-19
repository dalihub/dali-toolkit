#!/bin/bash

USAGE=$(cat <<EOF
Usage note: retriever.sh [option] [directory] test-files
Options:
  none    retrieve TC names with corresponding startup and cleanup functions
  -f      retrieve TC name with corresponding "set" and "purpose" clauses
  -anum   retrieve automatic TC number
  -mnum   retrieve manual TC number

In case of TC in form of "int tc_name()" script will abort.
("int tc_name(void)" is a proper function signature)
EOF
)

function get_tc_files {
    CMAKE_FILE="$DIR/CMakeLists.txt"
    if [ ! -e $CMAKE_FILE ]; then
        echo "File $CMAKE_FILE not found. Aborting..."
        exit 1
    fi

    TC_FILES=$(cat $CMAKE_FILE | awk -vDIR="$DIR" '
    BEGIN {
        flag = 0;
        files = "";
    }
    /^SET\(TC_SOURCES/ {
        flag = 1;
        next;
    }
    /\)/ {
        if (flag == 1)
            exit;
    }
    !/^ *#/ {
        if (flag == 1) {
            if (files == "")
                files = DIR "/" $1;
            else
                files = files " " DIR "/" $1;
        }
    }
    END {
        print files;
    }')
}



function tc_names {
    if [[ -z "$1" ]]; then
        exit
    fi

    awk '
    BEGIN {
        OFS = ",";
        start_fun = "NULL";
        clean_fun = "NULL";
        err_flag = 0;
        tc_list = "";
    }
    BEGINFILE {
        start_fun = "NULL";
        clean_fun = "NULL";
    }
    /^void .*startup\(void\)/ {
        gsub(/^void /, "");
        gsub(/\(void\)$/,"");
        start_fun = $0
    }
    /^void .*cleanup\(void\)/ {
        gsub(/^void /, "");
        gsub(/\(void\)$/,"");
        clean_fun = $0
    }
    /^int .*\(\)/ {
        print "Warning: function with empty argument list -- \"" $0 "\" in " FILENAME ":" FNR;
        err_flag = 1;
    }
    /^int .*\(void\)/ {
        gsub(/^int /, "");
        gsub(/\(void\).*/,"");
        if (tc_list != "") tc_list = tc_list "\n";
        tc_list = tc_list $0 OFS start_fun OFS clean_fun
    }
    END {
        if (err_flag) {
            exit 1
        } else {
            print tc_list
        }
    }
    ' $*
}

function tc_anum {
    awk '
    BEGIN {
        count = 0;
        err_flag = 0;
    }
    /^int .*\(\)/ {
        print "Warning: function with empty argument list -- \"" $0 "\" in " FILENAME ":" FNR;
        err_flag = 1;
    }
    /^int .*\(void\)$/ {
        count++;
    }
    END {
        if (err_flag) {
            exit 1
        } else {
            print count
        }
    }
    ' $*
}

function tc_mnum {
    # TODO: fix this hardcoded value
    echo 0
}

function tc_fullinfo {
    awk '
    BEGIN {
        OFS=",";
        purpose = "";
        set = "default";
        err_flag = 0;
        tc_list = "";
    }
    /^\/\/& set:/ {
        set = $3;
        next;
    }
    /^\/\/& purpose:/ {
        purpose = $3;
        for (i = 4; i <= NF; i++) {
            purpose = purpose " " $i;
        }
        next;
    }
    /^int .*\(\)/ {
        print "Warning: function with empty argument list -- \"" $0 "\" in " FILENAME ":" FNR;
        err_flag = 1;
    }
    /^int .*\(void\)$/ {
        gsub(/^int /, "");
        gsub(/\(void\)$/,"");
        if (tc_list != "") tc_list = tc_list "\n";
        tc_list = tc_list $0 OFS set OFS purpose;
        purpose = "";
        next
    }
    END {
        if (err_flag) {
            exit 1
        } else {
            print tc_list
        }
    }
    ' $*
}

TEMP=`getopt -o f::,a:,m: --long full::,anum:,mnum: \
     -n 'retriever.sh' -- "$@"`

if [ $? != 0 ] ; then echo "Terminating..." >&2 ; exit 1 ; fi

# Note the quotes around `$TEMP': they are essential!
eval set -- "$TEMP"

opt_full=false
opt_full_dir=
opt_anum=
opt_mnum=

while true ; do
    case "$1" in
        -f|--full) opt_full_dir="$2" ; opt_full="true" ; shift 2 ;;
        -a|--anum) opt_anum="$2" ; shift 2 ;;
        -m|--mnum) opt_mnum="$2" ; shift 2 ;;
        --) shift ; break ;;
        *) echo -e $USAGE ; exit 1 ;;
    esac
done

#echo "###Retriever" >& 2
#echo opt_full: {$opt_full} >& 2
#echo opt_full_dir: {$opt_full_dir} >& 2

DIR=.
if [ -n "$opt_anum" ] ; then
    DIR=$opt_anum
    echo opt_anum: DIR=$DIR >& 2
elif [ -n "$opt_mnum" ] ; then
    DIR=$opt_mnum
    echo opt_mnum: DIR=$DIR >& 2
elif [ "$opt_full" == "true" ] ; then
    if [ -n $opt_full_dir ] ; then
        DIR=$opt_full_dir
        echo opt_full: DIR=$DIR >& 2
    fi
fi


# get filename from first argument
if [[ $# == 1 && -f $DIR/$1 ]] ; then
    FILE=$1
    shift;
fi

#echo "Dir: $DIR  File: $FILE" >& 2

# populate $TC_FILES with files declared in CMakeLists.txt
if [[ -z $FILE ]]; then
    if [[ ! $DIR == "." || $# == 0 ]] ; then
        #echo Get tc files: DIR=$DIR >& 2
        get_tc_files $DIR
    else
        TC_FILES=$(
            for i in $* ; do
                echo $DIR/$i
            done
            )
    fi
    if [ $? != 0 ]; then
        exit 1
    fi
    echo "Got all files in `pwd`" >& 2
    #echo "TC_FILES: $TC_FILES" >& 2
else
    TC_FILES="$DIR/$FILE"
    echo "TC_FILES: $TC_FILES" >& 2
fi



# run appropriate subcommand
if [ "$opt_full" == "true" ] ; then
    tc_fullinfo $TC_FILES
elif [ -n "$opt_anum" ] ; then
    tc_anum $TC_FILES
elif [ -n "$opt_mnum" ] ; then
    tc_mnum $TC_FILES
else
    tc_names $TC_FILES
fi
