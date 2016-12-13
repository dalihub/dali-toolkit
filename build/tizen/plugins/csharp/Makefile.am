# Copyright (c) 2016 Samsung Electronics Co., Ltd.

# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at

# http://www.apache.org/licenses/LICENSE-2.0

# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

dali_swig_dir = ../../../../plugins/dali-swig

BUILT_SOURCES = \
          $(dali_swig_dir)/automatic/cpp/dali_wrap.cpp \
          $(dali_swig_dir)/automatic/cpp/dali_wrap.h \
          $(dali_swig_dir)/manual/cpp/keyboard_focus_manager_wrap.cpp \
          $(dali_swig_dir)/manual/cpp/view-wrapper-impl-wrap.cpp \
          $(dali_swig_dir)/manual/cpp/callbackbase_wrap.cpp

all-local:
gbs-local: libNDalic.so NDali.dll

libNDalic.so: $(dali_swig_dir)/automatic/cpp/dali_wrap.o $(dali_swig_dir)/manual/cpp/keyboard_focus_manager_wrap.o $(dali_swig_dir)/manual/cpp/view-wrapper-impl-wrap.o $(dali_swig_dir)/manual/cpp/callbackbase_wrap.o
	$(CXX) -shared $(dali_swig_dir)/automatic/cpp/dali_wrap.o $(dali_swig_dir)/manual/cpp/keyboard_focus_manager_wrap.o $(dali_swig_dir)/manual/cpp/view-wrapper-impl-wrap.o $(dali_swig_dir)/manual/cpp/callbackbase_wrap.o -o $(dali_swig_dir)/libNDalic.so $(DALICORE_LIBS) $(DALIADAPTOR_LIBS) $(DALITOOLKIT_LIBS)

$(dali_swig_dir)/automatic/cpp/dali_wrap.o: $(BUILT_SOURCES)
	$(CXX) -c -fpic $(CXXFLAGS) $(DALICORE_CFLAGS) $(DALIADAPTOR_CFLAGS) $(DALITOOLKIT_CFLAGS) $(dali_swig_dir)/automatic/cpp/dali_wrap.cpp -o $(dali_swig_dir)/automatic/cpp/dali_wrap.o

$(dali_swig_dir)/manual/cpp/keyboard_focus_manager_wrap.o: $(BUILT_SOURCES)
	$(CXX) -c -fpic $(CXXFLAGS) $(DALICORE_CFLAGS) $(DALIADAPTOR_CFLAGS) $(DALITOOLKIT_CFLAGS) $(dali_swig_dir)/manual/cpp/keyboard_focus_manager_wrap.cpp -o $(dali_swig_dir)/manual/cpp/keyboard_focus_manager_wrap.o

$(dali_swig_dir)/manual/cpp/view-wrapper-impl-wrap.o: $(BUILT_SOURCES)
	$(CXX) -c -fpic $(CXXFLAGS) $(DALICORE_CFLAGS) $(DALIADAPTOR_CFLAGS) $(DALITOOLKIT_CFLAGS) $(dali_swig_dir)/manual/cpp/view-wrapper-impl-wrap.cpp -o $(dali_swig_dir)/manual/cpp/view-wrapper-impl-wrap.o

$(dali_swig_dir)/manual/cpp/callbackbase_wrap.cpp.o: $(BUILT_SOURCES)
	$(CXX) -c -fpic $(CXXFLAGS) $(DALICORE_CFLAGS) $(DALIADAPTOR_CFLAGS) $(DALITOOLKIT_CFLAGS) $(dali_swig_dir)/manual/cpp/callbackbase_wrap.cpp -o $(dali_swig_dir)/manual/cpp/callbackbase_wrap.o

NDali.dll: $(BUILT_SOURCES)
#	mcs -nologo -target:library -out:$(dali_swig_dir)/NDali.dll $(dali_swig_dir)/automatic/csharp/*.cs $(dali_swig_dir)/manual/csharp/*.cs

$(BUILT_SOURCES): $(dali_swig_dir)/SWIG/*.i
	rm -f $(dali_swig_dir)/automatic/csharp/*.cs
	rm -f $(dali_swig_dir)/automatic/cpp/*.cpp
	$(SWIG) -version
	$(SWIG) -csharp -c++ -outdir $(dali_swig_dir)/automatic/csharp \
            $(DALI_CFLAGS) $(DALICORE_CFLAGS) $(DALIADAPTOR_CFLAGS) $(DALITOOLKIT_CFLAGS) -namespace Dali -o $(dali_swig_dir)/automatic/cpp/dali_wrap.cpp $(dali_swig_dir)/SWIG/dali.i

gbs-install: gbs-local
	-$(INSTALL) -d $(DESTDIR)
	-$(INSTALL) $(dali_swig_dir)/libNDalic.so $(DESTDIR)
	-$(INSTALL) $(dali_swig_dir)/NDali.dll $(DESTDIR)

EXTRA_DIST = \
    swig.cmd

