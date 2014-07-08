#
# Copyright (c) 2014 Samsung Electronics Co., Ltd.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

SUBDIRS = dali-toolkit

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = dali-toolkit.pc

MAINTAINERCLEANFILES = \
    aclocal.m4 \
    autom4te.cache \
    config.guess \
    config.sub \
    configure \
    depcomp \
    install-sh \
    ltmain.sh \
    missing \
    `find "$(srcdir)" -type f -name Makefile.in -print` \
    `find . \( -name "*.gcov" -o -name "*.gcno" -o -name "*.gcda" \) -print`

CLEANFILES = \
	`find . \( -name "*.gcov" -o -name "*.gcno" -o -name "*.gcda" \) -print`

COVERAGE_DIR=.cov
COVERAGE_OUTPUT_DIR=doc/coverage

cov_data:
	@test -z $(COVERAGE_DIR) || mkdir -p $(COVERAGE_DIR)
	@rm -f $(COVERAGE_DIR)/*
	@cp dali-toolkit/.libs/*.gcda dali-toolkit/.libs/*.gcno  $(COVERAGE_DIR)
	@for i in `find $(COVERAGE_DIR) -name "libdali_toolkit_la-*.gcda" -o -name "libdali_toolkit_la-*.gcno"` ;\
		do mv $$i `echo $$i | sed s/libdali_toolkit_la-//` ; echo $$i ; done
	@cd $(COVERAGE_DIR) ; lcov --base-directory . --directory . -c -o dali.info
	@cd $(COVERAGE_DIR) ; lcov --remove dali.info "*boost*" "/usr/include/*" "*/dali-env/*" -o dali.info
	@test -z $(COVERAGE_OUTPUT_DIR) || mkdir -p $(COVERAGE_OUTPUT_DIR)

coverage: cov_data
	@genhtml -o $(COVERAGE_OUTPUT_DIR) $(COVERAGE_DIR)/dali.info

reset_coverage:
	@lcov -z --directory `pwd`
