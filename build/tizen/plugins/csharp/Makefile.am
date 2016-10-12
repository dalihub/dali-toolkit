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

CLEANFILES = $(dali_swig_dir)/automatic/cpp/dali_wrap.* $(dali_swig_dir)/automatic/csharp/*.cs $(dali_swig_dir)/*.so $(dali_swig_dir)/*.dll \
             $(dali_swig_dir)/manual/cpp/*.o \
             $(dali_swig_dir)/examples/*.so $(dali_swig_dir)/examples/*.dll $(dali_swig_dir)/examples/*.exe

BUILT_SOURCES = $(dali_swig_dir)/automatic/cpp/dali_wrap.cpp

all-local: libNDalic.so NDali.dll
gbs-local: libNDalic.so

libNDalic.so: $(dali_swig_dir)/automatic/cpp/dali_wrap.o $(dali_swig_dir)/manual/cpp/keyboard_focus_manager_wrap.o
	$(CXX) -shared $(dali_swig_dir)/automatic/cpp/dali_wrap.o $(dali_swig_dir)/manual/cpp/keyboard_focus_manager_wrap.o -o $(dali_swig_dir)/libNDalic.so $(DALICORE_LIBS) $(DALIADAPTOR_LIBS) $(DALITOOLKIT_LIBS)

$(dali_swig_dir)/automatic/cpp/dali_wrap.o: $(BUILT_SOURCES)
	$(CXX) -c -fpic $(CXXFLAGS) $(DALICORE_CFLAGS) $(DALIADAPTOR_CFLAGS) $(DALITOOLKIT_CFLAGS) $(dali_swig_dir)/automatic/cpp/dali_wrap.cpp -o $(dali_swig_dir)/automatic/cpp/dali_wrap.o

$(dali_swig_dir)/manual/cpp/keyboard_focus_manager_wrap.o: $(dali_swig_dir)/manual/cpp/keyboard_focus_manager_wrap.cpp
	$(CXX) -c -fpic $(CXXFLAGS) $(DALICORE_CFLAGS) $(DALIADAPTOR_CFLAGS) $(DALITOOLKIT_CFLAGS) $(dali_swig_dir)/manual/cpp/keyboard_focus_manager_wrap.cpp -o $(dali_swig_dir)/manual/cpp/keyboard_focus_manager_wrap.o

NDali.dll: $(BUILT_SOURCES)
	$(MCS) -nologo -target:library -out:$(dali_swig_dir)/NDali.dll $(dali_swig_dir)/automatic/csharp/*.cs $(dali_swig_dir)/manual/csharp/*.cs


# use swig to generate the CS wrapper code
# then call our property-wrapper to inject DALi property getters / setters
if ENABLE_RUBY_FLAG

$(BUILT_SOURCES): $(dali_swig_dir)/SWIG/*.i
	rm -f $(dali_swig_dir)/automatic/csharp/*.cs
	rm -f $(dali_swig_dir)/automatic/cpp/dali-wrap.*
	$(SWIG) -csharp -c++ -outdir $(dali_swig_dir)/automatic/csharp \
            $(DALI_CFLAGS) $(DALICORE_CFLAGS) $(DALIADAPTOR_CFLAGS) $(DALITOOLKIT_CFLAGS) -namespace Dali -o $(dali_swig_dir)/automatic/cpp/dali_wrap.cpp $(dali_swig_dir)/SWIG/dali.i
	cd $(dali_swig_dir) && ./property-wrapper.rb
	cd $(dali_swig_dir) && ./constructor-generator.rb

else

$(BUILT_SOURCES): $(dali_swig_dir)/SWIG/*.i
	rm -f $(dali_swig_dir)/automatic/csharp/*.cs
	rm -f $(dali_swig_dir)/automatic/cpp/dali-wrap.*
	$(SWIG) -version
	$(SWIG) -csharp -c++ -outdir $(dali_swig_dir)/automatic/csharp \
            $(DALI_CFLAGS) $(DALICORE_CFLAGS) $(DALIADAPTOR_CFLAGS) $(DALITOOLKIT_CFLAGS) -namespace Dali -o $(dali_swig_dir)/automatic/cpp/dali_wrap.cpp $(dali_swig_dir)/SWIG/dali.i
endif


dist-hook: $(BUILT_SOURCES)
	mkdir -p $(distdir)/automatic/cpp
	cp $(dali_swig_dir)/automatic/cpp/*.cpp $(distdir)/automatic/cpp
	cp $(dali_swig_dir)/automatic/cpp/*.h $(distdir)/automatic/cpp
	mkdir -p $(distdir)/manual/cpp
	cp $(dali_swig_dir)/manual/cpp/*.cpp $(distdir)/manual/cpp
	cp $(dali_swig_dir)/manual/cpp/*.h $(distdir)/manual/cpp
	mkdir -p $(distdir)/automatic/csharp
	cp $(dali_swig_dir)/automatic/csharp/*.cs $(distdir)/automatic/csharp
	mkdir -p $(distdir)/manual/csharp
	cp $(dali_swig_dir)/manual/csharp/*.cs $(distdir)/manual/csharp
	mkdir -p $(distdir)/examples
	cp $(dali_swig_dir)/examples/*.csproj $(distdir)/examples
	cp $(dali_swig_dir)/examples/*.cs $(distdir)/examples

gbs-install: gbs-local
	-$(INSTALL) -d $(DESTDIR)
	-$(INSTALL) $(dali_swig_dir)/libNDalic.so $(DESTDIR)

EXTRA_DIST = \
    swig.cmd

