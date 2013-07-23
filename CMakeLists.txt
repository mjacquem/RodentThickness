cmake_minimum_required(VERSION 2.8.3)
cmake_policy(VERSION 2.8.3)

set(LOCAL_PROJECT_NAME RodentThickness)

set(Executablebin ${CMAKE_CURRENT_BINARY_DIR}/../bin)
option(${LOCAL_PROJECT_NAME}_SUPERBUILD "Build ${LOCAL_PROJECT_NAME} and the projects it depends on via SuperBuild.cmake." ON)
find_program(pathexecpython 
       NAMES python
       PATHS ${CMAKE_CURRENT_BINARY_DIR} # Here ${CMAKE_CURRENT_BINARY_DIR} is the inner build directory 
	)


#-----------------------------------------------------------------------------
# Superbuild script
#-----------------------------------------------------------------------------
if(${LOCAL_PROJECT_NAME}_SUPERBUILD)   # Default = Superbuild CMakeLists
  project(SuperBuild_${LOCAL_PROJECT_NAME})
  include("${CMAKE_CURRENT_SOURCE_DIR}/Superbuild.cmake")
  return()
else()
  project(${LOCAL_PROJECT_NAME})
  include("${CMAKE_CURRENT_SOURCE_DIR}/${LOCAL_PROJECT_NAME}.cmake") # For the RodentThickness external project
  return()
endif()

message(FATAL_ERROR "You should never reach this point !")


