#.rst:
# FindGLESv2
# ----------
#
# FindModule for GLESv2 and GLU.
#
# Result Variables
# ^^^^^^^^^^^^^^^^
#
# This module sets the following variables:
#
# ``GLESv2_FOUND``
#  True, if the system has GLESv2.
# ``GLESv2_XMESA_FOUND``
#  True, if the system has XMESA.
# ``GLESv2_GLU_FOUND``
#  True, if the system has GLU.
# ``GLESv2_INCLUDE_DIR``
#  Path to the GLESv2 include directory.
# ``GLESv2_LIBRARIES``
#  Paths to the GLESv2 and GLU libraries.
#
# If you want to use just GL you can use these values:
#
# ``GLESv2_gl_LIBRARY``
#  Path to the GLESv2 library.
# ``GLESv2_glu_LIBRARY``
#  Path to the GLU library.
#
set(_GLESv2_REQUIRED_VARS GLESv2_gl_LIBRARY)

find_path(GLESv2_INCLUDE_DIR GLES2/gl2.h
  /usr/share/doc/NVIDIA_GLX-1.0/include
  /usr/openwin/share/include
  /opt/graphics/GLESv2/include /usr/X11R6/include
  ${_GLESv2_INCLUDE_PATH}
)
list(APPEND _GLESv2_REQUIRED_VARS GLESv2_INCLUDE_DIR)

find_library(GLESv2_gl_LIBRARY
  NAMES GLESv2
  PATHS /opt/graphics/GLESv2/lib
        /usr/openwin/lib
        /usr/shlib /usr/X11R6/lib
        ${_GLESv2_LIB_PATH}
)

unset(_GLESv2_INCLUDE_PATH)
unset(_GLESv2_LIB_PATH)

if(GLESv2_gl_LIBRARY)

    set( GLESv2_LIBRARIES  ${GLESv2_gl_LIBRARY} ${GLESv2_LIBRARIES})

endif()

# handle the QUIETLY and REQUIRED arguments and set GLESv2_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GLESv2 REQUIRED_VARS ${_GLESv2_REQUIRED_VARS})
unset(_GLESv2_REQUIRED_VARS)

mark_as_advanced(
  GLESv2_INCLUDE_DIR
  GLESv2_gl_LIBRARY
)
