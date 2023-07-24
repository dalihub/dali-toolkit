prefix=@PREFIX@
exec_prefix=@EXEC_PREFIX@
apiversion=@BULLET_VERSION@
libdir=@LIB_DIR@
includedir=@DEV_INCLUDE_PATH@

Name: bullet
Description: Bullet Continuous Collision Detection and Physics Library
Version: ${apiversion}
Requires:
Libs: -L${libdir} -lbullet3
Cflags: -I${includedir}