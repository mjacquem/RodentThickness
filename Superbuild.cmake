#-----------------------------------------------------------------------------
set(verbose FALSE)
#-----------------------------------------------------------------------------

#-----------------------------------------------------------------------------
enable_language(C)
enable_language(CXX)

#-----------------------------------------------------------------------------
enable_testing()
include(CTest)

#-----------------------------------------------------------------------------
include(${CMAKE_CURRENT_SOURCE_DIR}/Common.cmake)
#-----------------------------------------------------------------------------
#If it is build as an extension
#-----------------------------------------------------------------------------


if( RodentThickness_BUILD_SLICER_EXTENSION )
  set( USE_SYSTEM_VTK ON )
  set( BUILD_SHARED_LIBS OFF )
  set(EXTENSION_SUPERBUILD_BINARY_DIR ${${EXTENSION_NAME}_BINARY_DIR} )
  unsetForSlicer(NAMES CMAKE_MODULE_PATH CMAKE_C_COMPILER CMAKE_CXX_COMPILER ITK_DIR SlicerExecutionModel_DIR VTK_DIR QT_QMAKE_EXECUTABLE ITK_VERSION_MAJOR CMAKE_CXX_FLAGS CMAKE_C_FLAGS )
  find_package(Slicer REQUIRED)
  include(${Slicer_USE_FILE})
  unsetAllForSlicerBut( NAMES VTK_DIR QT_QMAKE_EXECUTABLE )
  resetForSlicer(NAMES CMAKE_MODULE_PATH CMAKE_C_COMPILER CMAKE_CXX_COMPILER ITK_VERSION_MAJOR CMAKE_CXX_FLAGS CMAKE_C_FLAGS)
  set( NIRAL_UTILITIES_DEPENDS niral_utilities)
endif()
#-----------------------------------------------------------------------------
# Git protocole option
#-----------------------------------------------------------------------------
option(USE_GIT_PROTOCOL "If behind a firewall turn this off to use http instead." ON)
set(git_protocol "git")
if(NOT USE_GIT_PROTOCOL)
  set(git_protocol "http")
endif()

find_package(Git REQUIRED)

# I don't know who removed the Find_Package for QT, but it needs to be here
# in order to build VTK if ${LOCAL_PROJECT_NAME}_USE_QT is set.
if(${LOCAL_PROJECT_NAME}_USE_QT AND NOT RodentThickness_BUILD_SLICER_EXTENSION)
    find_package(Qt4 REQUIRED)
endif()


# Compute -G arg for configuring external projects with the same CMake generator:
if(CMAKE_EXTRA_GENERATOR)
  set(gen "${CMAKE_EXTRA_GENERATOR} - ${CMAKE_GENERATOR}")
else()
  set(gen "${CMAKE_GENERATOR}")
endif()


# With CMake 2.8.9 or later, the UPDATE_COMMAND is required for updates to occur.
# For earlier versions, we nullify the update state to prevent updates and
# undesirable rebuild.
option(FORCE_EXTERNAL_BUILDS "Force rebuilding of external project (if they are updated)" OFF)
if(CMAKE_VERSION VERSION_LESS 2.8.9 OR NOT FORCE_EXTERNAL_BUILDS)
  set(cmakeversion_external_update UPDATE_COMMAND)
  set(cmakeversion_external_update_value "" )
else()
  set(cmakeversion_external_update LOG_UPDATE )
  set(cmakeversion_external_update_value 1)
endif()



#-----------------------------------------------------------------------------
# Superbuild option(s)
#-----------------------------------------------------------------------------
option(BUILD_STYLE_UTILS "Build uncrustify, cppcheck, & KWStyle" OFF)
CMAKE_DEPENDENT_OPTION(
  USE_SYSTEM_Uncrustify "Use system Uncrustify program" OFF
  "BUILD_STYLE_UTILS" OFF
  )
CMAKE_DEPENDENT_OPTION(
  USE_SYSTEM_KWStyle "Use system KWStyle program" OFF
  "BUILD_STYLE_UTILS" OFF
  )
CMAKE_DEPENDENT_OPTION(
  USE_SYSTEM_Cppcheck "Use system Cppcheck program" OFF
  "BUILD_STYLE_UTILS" OFF
  )

set(EXTERNAL_PROJECT_BUILD_TYPE "Release" CACHE STRING "Default build type for support libraries")

option(USE_SYSTEM_ITK "Build using an externally defined version of ITK" OFF)
option(USE_SYSTEM_SlicerExecutionModel "Build using an externally defined version of SlicerExecutionModel"  OFF)
option(USE_SYSTEM_VTK "Build using an externally defined version of VTK" OFF)
option(USE_SYSTEM_BatchMake "Build using an externally defined version of BatchMake" OFF)


#------------------------------------------------------------------------------
# ${LOCAL_PROJECT_NAME} dependency list
#------------------------------------------------------------------------------
include(ExternalProject)
include(SlicerMacroEmptyExternalProject)
include(SlicerMacroCheckExternalProjectDependency)


set(ITK_EXTERNAL_NAME ITKv${ITK_VERSION_MAJOR})

set(${LOCAL_PROJECT_NAME}_DEPENDENCIES ${ITK_EXTERNAL_NAME} SlicerExecutionModel VTK BatchMake )

if(BUILD_STYLE_UTILS)
  #list(APPEND ${LOCAL_PROJECT_NAME}_DEPENDENCIES Cppcheck KWStyle Uncrustify)
endif()

include(${CMAKE_CURRENT_SOURCE_DIR}/SetExternalProjectOptions.cmake)

#------------------------------------------------------------------------------
# Configure and build
#------------------------------------------------------------------------------
set(proj ${LOCAL_PROJECT_NAME})
ExternalProject_Add(${proj}
  DEPENDS ${${LOCAL_PROJECT_NAME}_DEPENDENCIES}
  DOWNLOAD_COMMAND ""
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
  BINARY_DIR ${LOCAL_PROJECT_NAME}-build
  CMAKE_GENERATOR ${gen}
  CMAKE_ARGS
    --no-warn-unused-cli # HACK Only expected variables should be passed down.
    ${CMAKE_OSX_EXTERNAL_PROJECT_ARGS}
    ${COMMON_EXTERNAL_PROJECT_ARGS}
    -D${LOCAL_PROJECT_NAME}_SUPERBUILD:BOOL=OFF
    -Dpathexecpython:PATH=${pathexecpython}
  INSTALL_COMMAND ""
  )

