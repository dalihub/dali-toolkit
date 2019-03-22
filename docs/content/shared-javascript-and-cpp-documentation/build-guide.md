<!--
/**-->
# Build Guide {#build-guide}

## Ubuntu {#build-ubuntu}

These instructions explain how to build the DALi library for the Ubuntu 14.04 desktop environment.

### Minimum Requirements

+ Ubuntu 14.04
+ Ensure ALL sources are selected:
 + Go to Ubuntu Settings and then to "Software & Updates".
 + In the "Ubuntu Software" tab, ensure ALL software sources are ticked. (This is required because we install some community-maintained free & open-source software).

### Creating a DALi environment

DALi provides a script to set up your desktop environment. This script can be found in the dali-core repository.

+ Fetch ALL 4 repositories from tizen.org.
+ In the parent directory of these repositories, run the following command:
  ~~~{.sh}
  dali-core/build/scripts/dali_env -c
  ~~~
  This will also download any dependencies that the dali repositories require.

+ You can save the environment variables to a file:
  ~~~{.sh}
  dali-env/opt/bin/dali_env -s > setenv
  ~~~

The last few steps only need to be done once.

You will have to source your environment variables every time you open up a new terminal (or you can add to .bashrc if you prefer).
You can do this by sourcing the '''setenv''' script you created above:
~~~{.sh}
. setenv
~~~

### Building the repositories

#### dali-core
~~~{.sh}
cd dali-core/build/tizen
autoreconf --install
./configure --prefix=$DESKTOP_PREFIX
make install -j8
~~~

#### dali-adaptor
~~~{.sh}
cd dali-adaptor/build/tizen
autoreconf --install
./configure --prefix=$DESKTOP_PREFIX --enable-profile=UBUNTU --enable-gles=20
make install -j8
~~~

#### dali-toolkit
~~~{.sh}
cd dali-toolkit/build/tizen
autoreconf --install
./configure --prefix=$DESKTOP_PREFIX
make install -j8
~~~

#### dali-demo
~~~{.sh}
cd dali-demo/build/tizen
cmake -DCMAKE_INSTALL_PREFIX=$DESKTOP_PREFIX .
make install -j8
~~~

### Running dali-demo

Ensure you have sourced your environment as mentioned above and then just run:
~~~{.sh}
dali-demo
~~~

*/
