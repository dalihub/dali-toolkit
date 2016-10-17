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

CLEANFILES = automatic/cpp/dali_wrap.* automatic/csharp/*.cs *.so *.dll \
             manual/cpp/*.o \
             examples/*.so examples/*.dll examples/*.exe

BUILT_SOURCES = automatic/cpp/dali_wrap.cpp automatic/cpp/dali_wrap.h

if HAVE_MCS
if BUILD_MCS

all-local: libNDalic.so NDali.dll

libNDalic.so: automatic/cpp/dali_wrap.o manual/cpp/keyboard_focus_manager_wrap.o manual/cpp/view-wrapper-impl-wrap.o
	g++ -shared automatic/cpp/dali_wrap.o manual/cpp/keyboard_focus_manager_wrap.o manual/cpp/view-wrapper-impl-wrap.o -o libNDalic.so $(DALICORE_LIBS) $(DALIADAPTOR_LIBS) $(DALITOOLKIT_LIBS)

automatic/cpp/dali_wrap.o: $(BUILT_SOURCES)
	g++ -c -fpic $(CXXFLAGS) $(DALICORE_CFLAGS) $(DALIADAPTOR_CFLAGS) $(DALITOOLKIT_CFLAGS) automatic/cpp/dali_wrap.cpp -o automatic/cpp/dali_wrap.o

manual/cpp/keyboard_focus_manager_wrap.o: $(BUILT_SOURCES)
	g++ -c -fpic $(CXXFLAGS) $(DALICORE_CFLAGS) $(DALIADAPTOR_CFLAGS) $(DALITOOLKIT_CFLAGS) manual/cpp/keyboard_focus_manager_wrap.cpp -o manual/cpp/keyboard_focus_manager_wrap.o

manual/cpp/view-wrapper-impl-wrap.o: $(BUILT_SOURCES)
	g++ -c -fpic $(CXXFLAGS) $(DALICORE_CFLAGS) $(DALIADAPTOR_CFLAGS) $(DALITOOLKIT_CFLAGS) manual/cpp/view-wrapper-impl-wrap.cpp -o manual/cpp/view-wrapper-impl-wrap.o

NDali.dll: $(BUILT_SOURCES)
	$(MCS) -nologo -target:library -out:NDali.dll automatic/csharp/*.cs manual/csharp/*.cs

check-local: examples/dali-test.exe \
             examples/hello-world.exe \
             examples/scroll-view.exe \
             examples/custom-control.exe \
             examples/spin-control.exe \
             examples/libNDalic.so examples/NDali.dll

examples/%.exe: examples/%.cs
	$(MCS) -nologo -target:exe -out:$@ -reference:NDali.dll $<

examples/libNDalic.so: libNDalic.so
	cd examples && ln -sf ../libNDalic.so

examples/NDali.dll: NDali.dll
	cd examples && ln -sf ../NDali.dll

endif
endif

# use swig to generate the CS wrapper code
# then call our property-wrapper to inject DALi property getters / setters
$(BUILT_SOURCES): SWIG/*.i
	rm -f automatic/csharp/*.cs
	$(SWIG) -csharp -c++ -outdir automatic/csharp \
            $(DALI_CFLAGS) $(DALICORE_CFLAGS) $(DALIADAPTOR_CFLAGS) $(DALITOOLKIT_CFLAGS) -namespace Dali -o automatic/cpp/dali_wrap.cpp SWIG/dali.i
	./property-wrapper.rb
	./constructor-generator.rb

dist-hook: $(BUILT_SOURCES)
	mkdir -p $(distdir)/automatic/cpp
	cp ./automatic/cpp/*.cpp $(distdir)/automatic/cpp
	cp ./automatic/cpp/*.h $(distdir)/automatic/cpp
	mkdir -p $(distdir)/manual/cpp
	cp ./manual/cpp/*.cpp $(distdir)/manual/cpp
	cp ./manual/cpp/*.h $(distdir)/manual/cpp
	mkdir -p $(distdir)/automatic/csharp
	cp ./automatic/csharp/*.cs $(distdir)/automatic/csharp
	mkdir -p $(distdir)/manual/csharp
	cp ./manual/csharp/*.cs $(distdir)/manual/csharp
	mkdir -p $(distdir)/examples
	cp ./examples/*.csproj $(distdir)/examples
	cp ./examples/*.cs $(distdir)/examples


