# Locate SDL_image library
# This module defines
# SDLIMAGE_LIBRARY, the name of the library to link against
# SDLIMAGE_FOUND, if false, do not try to link to SDL
# SDLIMAGE_INCLUDE_DIR, where to find SDL/SDL.h
#
# $SDLDIR is an environment variable that would
# correspond to the ./configure --prefix=$SDLDIR
# used in building SDL.
#
# Created by Eric Wing. This was influenced by the FindSDL.cmake 
# module, but with modifications to recognize OS X frameworks and 
# additional Unix paths (FreeBSD, etc).

#=============================================================================
# Copyright 2005-2009 Kitware, Inc.
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of CMake, substitute the full
#  License text for the above reference.)

FIND_PATH(SDL2IMAGE_INCLUDE_DIR SDL_image.h
  HINTS
  $ENV{SDL2IMAGEDIR}
  $ENV{SDL2DIR}
  PATH_SUFFIXES include
  PATHS
  ~/Library/Frameworks
  /Library/Frameworks
  /usr/local/include/SDL2
  /usr/include/SDL2
  /usr/local/include/SDL12
  /usr/local/include/SDL11 # FreeBSD ports
  /usr/include/SDL12
  /usr/include/SDL11
  /usr/local/include
  /usr/include
  /sw/include/SDL2 # Fink
  /sw/include
  /opt/local/include/SDL2 # DarwinPorts
  /opt/local/include
  /opt/csw/include/SDL2 # Blastwave
  /opt/csw/include 
  /opt/include/SDL2
  /opt/include
  /Users/matti/Entwicklung/SDL/include/SDL2
  /home/matti/Entwicklung/SDL/include/SDL2
)

FIND_LIBRARY(SDL2IMAGE_LIBRARY 
  NAMES SDL2_image
  HINTS
  $ENV{SDL2IMAGEDIR}
  $ENV{SDL2DIR}
  PATH_SUFFIXES lib64 lib
  PATHS
  ~/Library/Frameworks
  /Library/Frameworks
  /usr/local
  /usr
  /sw
  /opt/local
  /opt/csw
  /opt
  /Users/matti/Entwicklung/SDL/lib/
  /home/matti/Entwicklung/SDL/lib/
)

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(SDL2IMAGE
                                  REQUIRED_VARS SDL2IMAGE_LIBRARY SDL2IMAGE_INCLUDE_DIR)
