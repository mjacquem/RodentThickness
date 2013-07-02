# Make sure this file is included only once
get_filename_component(CMAKE_CURRENT_LIST_FILENAME ${CMAKE_CURRENT_LIST_FILE} NAME_WE)
if(${CMAKE_CURRENT_LIST_FILENAME}_FILE_INCLUDED)
  return()
endif()
set(${CMAKE_CURRENT_LIST_FILENAME}_FILE_INCLUDED 1)

# Include dependent projects if any
set(extProjName Rscript) #The find_package known name
set(proj ${extProjName} ) #This local name

#if(${USE_SYSTEM_${extProjName}})
# unset(${extProjName}_DIR CACHE)
#endif()

# Sanity checks
if(DEFINED ${extProjName}_DIR AND NOT EXISTS ${${extProjName}_DIR})
  message(FATAL_ERROR "${extProjName}_DIR variable is defined but corresponds to non-existing directory")
endif()

#set(${proj}_DEPENDENCIES )

#SlicerMacroCheckExternalProjectDependency(${proj})

if(NOT DEFINED ${extProjName}_DIR AND NOT ${USE_SYSTEM_${extProjName}})

  # Set CMake OSX variable to pass down the external project
  set(CMAKE_OSX_EXTERNAL_PROJECT_ARGS)
  if(APPLE)
    list(APPEND CMAKE_OSX_EXTERNAL_PROJECT_ARGS
      -DCMAKE_OSX_ARCHITECTURES=${CMAKE_OSX_ARCHITECTURES}
      -DCMAKE_OSX_SYSROOT=${CMAKE_OSX_SYSROOT}
      -DCMAKE_OSX_DEPLOYMENT_TARGET=${CMAKE_OSX_DEPLOYMENT_TARGET})
  endif()

  # Rscript
  if(WIN32) # If windows, no recompilation so just download binary
    #set(Rscript_DOWNLOAD_ARGS
      #  URL "")

  elseif(APPLE)# Download source code and recompile
    #set(Rscript_DOWNLOAD_ARGS
    #    URL ""
      #  URL_MD5 )
  else()
    set(Rscript_DOWNLOAD_ARGS
        URL "http://cran.cict.fr/src/base/R-2/R-2.15.3.tar.gz"
	URL_MD5 b2f1a5d701f1f90679be0c60e1931a5c)
  endif()
message("[] end download Rscript...")

  ExternalProject_Add(Rscript # Rscript has no CMakeLists.txt # Example : Slicer/SuperBuild/External_python.cmake
    ${Rscript_DOWNLOAD_ARGS}
    DOWNLOAD_DIR ${CMAKE_CURRENT_BINARY_DIR}/Rscript-install
    SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/Rscript
    BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/Rscript-build
    CONFIGURE_COMMAND ""
    CMAKE_GENERATOR ${gen}
    CMAKE_ARGS 
      ${COMMON_BUILD_OPTIONS_FOR_EXTERNALPACKAGES} # So we can give CC to configure*
      ${CMAKE_OSX_EXTERNAL_PROJECT_ARGS}
    INSTALL_COMMAND ""
    BUILD_COMMAND ${CMAKE_COMMAND} -DTOP_BINARY_DIR:PATH=${CMAKE_CURRENT_BINARY_DIR} -P ${CMAKE_CURRENT_SOURCE_DIR}/SuperBuild/InstallRscript.cmake # -DARGNAME:TYPE=VALUE -P <cmake file> = Process script mode
  )
  set(Rscript_DIR ${CMAKE_CURRENT_BINARY_DIR}/Rscript-install)
message("[] after external Rscript...")
else()
  if(${USE_SYSTEM_${extProjName}})
    find_package(${extProjName} ${ITK_VERSION_MAJOR} REQUIRED)
    if(NOT ${extProjName}_DIR)
      message(FATAL_ERROR "To use the system ${extProjName}, set ${extProjName}_DIR")
    endif()
  endif()
  # The project is provided using ${extProjName}_DIR, nevertheless since other
  # project may depend on ${extProjName}v4, let's add an 'empty' one
  #SlicerMacroEmptyExternalProject(${proj} "${${proj}_DEPENDENCIES}")
endif()

list(APPEND ${CMAKE_PROJECT_NAME}_SUPERBUILD_EP_VARS ${extProjName}_DIR:PATH)

