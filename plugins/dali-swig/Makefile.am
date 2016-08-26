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

CLEANFILES = cpp/dali_wrap.* csharp/*.cs *.so *.dll \
             examples/*.so examples/*.dll examples/*.exe

BUILT_SOURCES = cpp/dali_wrap.cpp cpp/dali_wrap.h

if HAVE_MCS
if BUILD_MCS

all-local: libNDalic.so NDali.dll

libNDalic.so: cpp/dali_wrap.o
	g++ -shared cpp/dali_wrap.o -o libNDalic.so $(DALICORE_LIBS) $(DALIADAPTOR_LIBS) $(DALITOOLKIT_LIBS)

cpp/dali_wrap.o: $(BUILT_SOURCES)
	g++ -c -fpic $(CXXFLAGS) $(DALICORE_CFLAGS) $(DALIADAPTOR_CFLAGS) $(DALITOOLKIT_CFLAGS) cpp/dali_wrap.cpp -o cpp/dali_wrap.o

NDali.dll: $(BUILT_SOURCES)
	$(MCS) -nologo -target:library -out:NDali.dll csharp/*.cs

check-local: examples/dali-test.exe \
             examples/hello-world.exe \
             examples/scroll-view.exe \
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
	rm -f csharp/*.cs
	$(SWIG) -csharp -c++ -outdir csharp \
            $(DALI_CFLAGS) $(DALICORE_CFLAGS) $(DALIADAPTOR_CFLAGS) $(DALITOOLKIT_CFLAGS) -namespace Dali -o cpp/dali_wrap.cpp SWIG/dali.i
	./property-wrapper.rb
	./constructor-generator.rb

dist-hook: $(BUILT_SOURCES)
	mkdir -p $(distdir)/cpp
	cp ./cpp/dali_wrap.cpp $(distdir)/cpp
	cp ./cpp/dali_wrap.h $(distdir)/cpp
	cp ./cpp/DaliWrapper.h ./cpp/DaliWrapper.cpp $(distdir)/cpp
	cp ./cpp/stdafx.h $(distdir)/cpp
	mkdir -p $(distdir)/csharp
	cp ./csharp/*.csproj $(distdir)/csharp
	cp ./csharp/*.cs $(distdir)/csharp
	mkdir -p $(distdir)/examples
	cp ./examples/*.csproj $(distdir)/examples
	cp ./examples/*.cs $(distdir)/examples

EXTRA_DIST = \
    swig.cmd

