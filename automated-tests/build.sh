#!/bin/bash


TEMP=`getopt -o r --long rebuild \
     -n 'genmake' -- "$@"`

if [ $? != 0 ] ; then echo "Terminating..." >&2 ; exit 1 ; fi

# Note the quotes around `$TEMP': they are essential!
eval set -- "$TEMP"

opt_rebuild=false

while true ; do
    case "$1" in
        -r|--rebuild) opt_rebuild=true ; shift ;;
        --) shift ; break ;;
        *) shift ;;   # Ignore
    esac
done

if [ false == $opt_rebuild -o ! -d "build" ] ; then
    rm -rf build
    mkdir build
fi

function build
{
    (cd build ; cmake .. -DMODULE=$1 ; make -j7 )
}

if [ -n "$1" ] ; then
  echo BUILDING ONLY $1
  build $1
else
  for mod in `ls -1 src/ | grep -v CMakeList `
  do
    if [ $mod != 'common' ] && [ $mod != 'manual' ]; then
        echo BUILDING $mod
        build $mod
        if [ $? -ne 0 ]; then echo "Build failed" ; exit 1; fi
    fi
  done
fi

echo "Build succeeded"
exit 0
