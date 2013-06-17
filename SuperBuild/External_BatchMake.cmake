
# Make sure this file is included only once
get_filename_component(CMAKE_CURRENT_LIST_FILENAME ${CMAKE_CURRENT_LIST_FILE} NAME_WE)
if(${CMAKE_CURRENT_LIST_FILENAME}_FILE_INCLUDED)
  return()
endif()
set(${CMAKE_CURRENT_LIST_FILENAME}_FILE_INCLUDED 1)

# Include dependent projects if any
set(extProjName BatchMake) #The find_package known name
set(proj ${extProjName} ) #This local name

#if(${USE_SYSTEM_${extProjName}})
# unset(${extProjName}_DIR CACHE)
#endif()

# Sanity checks
if(DEFINED ${extProjName}_DIR AND NOT EXISTS ${${extProjName}_DIR})
  message(FATAL_ERROR "${extProjName}_DIR variable is defined but corresponds to non-existing directory")
endif()


# Set dependency list
set(${proj}_DEPENDENCIES ${ITK_EXTERNAL_NAME})

SlicerMacroCheckExternalProjectDependency(${proj})

if(NOT DEFINED ${extProjName}_DIR AND NOT ${USE_SYSTEM_${extProjName}})

  # Set CMake OSX variable to pass down the external project
  set(CMAKE_OSX_EXTERNAL_PROJECT_ARGS)
  if(APPLE)
    list(APPEND CMAKE_OSX_EXTERNAL_PROJECT_ARGS
      -DCMAKE_OSX_ARCHITECTURES=${CMAKE_OSX_ARCHITECTURES}
      -DCMAKE_OSX_SYSROOT=${CMAKE_OSX_SYSROOT}
      -DCMAKE_OSX_DEPLOYMENT_TARGET=${CMAKE_OSX_DEPLOYMENT_TARGET})
  endif()

  ### --- Project specific additions here
  set(${proj}_CMAKE_OPTIONS
      -DITK_DIR:PATH=${ITK_DIR}
      -DUSE_FLTK:BOOL=OFF
      -DDASHBOARD_SUPPORT:BOOL=OFF
      -DGRID_SUPPORT:BOOL=ON
      -DUSE_SPLASHSCREEN:BOOL=OFF
    )
  ### --- End Project specific additions
  ## HACK set(${proj}_REPOSITORY "${git_protocol}://batchmake.org/BatchMake.git")
  ## set(${proj}_GIT_TAG origin/master)
  ## Using this temporary repository copy until this version can be properly incorporated.
# set(${proj}_REPOSITORY "${git_protocol}://github.com/hjmjohnson/TempBatchMake.git")
# set(${proj}_GIT_TAG FixInstallBatchMake)

set( BatchMakeCURLCmakeArg "" )
  if( RodentThickness_BUILD_SLICER_EXTENSION )
    find_library( PathToSlicerZlib
    NAMES zlib
    PATHS ${Slicer_HOME}/../zlib-install/lib # ${Slicer_HOME} is <topofbuildtree>/Slicer-build: defined in SlicerConfig.cmake
    PATH_SUFFIXES Debug Release RelWithDebInfo MinSizeRel # For Windows, it can be any one of these
    NO_DEFAULT_PATH
    NO_SYSTEM_ENVIRONMENT_PATH
    )
    set( BatchMakeCURLCmakeArg -DCURL_SPECIAL_LIBZ:PATH=${PathToSlicerZlib} )
  endif( RodentThickness_BUILD_SLICER_EXTENSION )


  set(${proj}_REPOSITORY "${git_protocol}://batchmake.org/BatchMake.git")
  set(${proj}_GIT_TAG "8addbdb62f0135ba01ffe12ddfc32121b6d66ef5")
 # set(${proj}_GIT_TAG "aee6ff198a392e6026730784d9c1c19a1296c465")   8addbdb62f0135ba01ffe12ddfc32121b6d66ef5  1f5bf4f92e8678c34dc6f7558be5e6613804d988
 ExternalProject_Add(${proj}
    GIT_REPOSITORY ${${proj}_REPOSITORY}
    GIT_TAG ${${proj}_GIT_TAG}
    SOURCE_DIR ${proj}
    BINARY_DIR ${proj}-build
    UPDATE_COMMAND ""
    CMAKE_GENERATOR ${gen}
    CMAKE_ARGS
      ${CMAKE_OSX_EXTERNAL_PROJECT_ARGS}
      ${COMMON_EXTERNAL_PROJECT_ARGS}
      -DCMAKE_LIBRARY_OUTPUT_DIRECTORY:PATH=${CMAKE_CURRENT_BINARY_DIR}/BatchMake-build/bin
      -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY:PATH=${CMAKE_CURRENT_BINARY_DIR}/BatchMake-build/bin
      -DCMAKE_RUNTIME_OUTPUT_DIRECTORY:PATH=${CMAKE_CURRENT_BINARY_DIR}/BatchMake-build/bin
      -DCMAKE_BUNDLE_OUTPUT_DIRECTORY:PATH=${CMAKE_CURRENT_BINARY_DIR}/BatchMake-build/bin
      -DBUILD_EXAMPLES:BOOL=OFF
      -DBUILD_TESTING:BOOL=OFF
      -DBUILD_SHARED_LIBS:BOOL=OFF
      ${${proj}_CMAKE_OPTIONS}
      ${BatchMakeCURLCmakeArg}
    INSTALL_COMMAND ""
    PATCH_COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/SuperBuild/BatchMakePatchedZip.c ${CMAKE_CURRENT_BINARY_DIR}/BatchMake/Utilities/Zip/zip.c 
    DEPENDS
      ${${proj}_DEPENDENCIES}
    )
  set(${extProjName}_DIR ${CMAKE_BINARY_DIR}/${proj}-build)
else()
  if(${USE_SYSTEM_${extProjName}})
    find_package(${extProjName} ${ITK_VERSION_MAJOR} REQUIRED)
    if(NOT ${extProjName}_DIR)
      message(FATAL_ERROR "To use the system ${extProjName}, set ${extProjName}_DIR")
    endif()
  endif()
  # The project is provided using ${extProjName}_DIR, nevertheless since other
  # project may depend on ${extProjName}v4, let's add an 'empty' one
  SlicerMacroEmptyExternalProject(${proj} "${${proj}_DEPENDENCIES}")
endif()

list(APPEND ${CMAKE_PROJECT_NAME}_SUPERBUILD_EP_VARS ${extProjName}_DIR:PATH)
