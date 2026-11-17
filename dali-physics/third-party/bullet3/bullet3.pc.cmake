prefix=@PREFIX@
exec_prefix=@EXEC_PREFIX@
apiversion=@BULLET_VERSION@
libdir=@LIB_DIR@
includedir=@DEV_INCLUDE_PATH@

Name: bullet
Description: Bullet Continuous Collision Detection and Physics Library
Version: ${apiversion}
Requires:
Libs: -L${libdir} -l@BULLET3_LIB_NAME@
Cflags: -I${includedir}