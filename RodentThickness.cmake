CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

set(MODULE_NAME ${EXTENSION_NAME}) # Do not use 'project()'
set(MODULE_TITLE ${MODULE_NAME})

include(${CMAKE_CURRENT_SOURCE_DIR}/Common.cmake)


FIND_PACKAGE(SlicerExecutionModel REQUIRED)
IF(SlicerExecutionModel_FOUND)
	INCLUDE(${SlicerExecutionModel_USE_FILE})
	INCLUDE(${GenerateCLP_USE_FILE})
ELSE(SlicerExecutionModel_FOUND)
	MESSAGE(FATAL_ERROR "SlicerExecutionModel not found. Please set SlicerExecutionModel_DIR")
ENDIF(SlicerExecutionModel_FOUND)

FIND_PACKAGE(BatchMake REQUIRED)
IF(BatchMake_FOUND)
	INCLUDE(${BatchMake_USE_FILE})
ELSE(BatchMake_FOUND)
	MESSAGE(FATAL_ERROR "BatchMake not found. Please set BatchMake_DIR")
ENDIF(BatchMake_FOUND)

#-----------------------------------------------------------------------------
set(expected_ITK_VERSION_MAJOR ${ITK_VERSION_MAJOR})
find_package(ITK NO_MODULE REQUIRED)
if(${ITK_VERSION_MAJOR} VERSION_LESS ${expected_ITK_VERSION_MAJOR})
  # Note: Since ITKv3 doesn't include a ITKConfigVersion.cmake file, let's check the version
  #       explicitly instead of passing the version as an argument to find_package() command.
  message(FATAL_ERROR "Could not find a configuration file for package \"ITK\" that is compatible "
                      "with requested version \"${expected_ITK_VERSION_MAJOR}\".\n"
                      "The following configuration files were considered but not accepted:\n"
                      "  ${ITK_CONFIG}, version: ${ITK_VERSION_MAJOR}.${ITK_VERSION_MINOR}.${ITK_VERSION_PATCH}\n")
endif()

include(${ITK_USE_FILE})

FIND_PACKAGE(VTK)
IF (VTK_FOUND)
    INCLUDE (${VTK_USE_FILE})
ELSE(VTK_FOUND)
    MESSAGE(FATAL_ERROR, "VTK not found. Please set VTK_DIR.")
ENDIF (VTK_FOUND)

FIND_PACKAGE(Qt4 REQUIRED) #/tools/devel/linux/Qt4/Qt4.8.1/Qt4.8.1_linux64/bin/qmake
IF(QT_USE_FILE)
	INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${QT_INCLUDE_DIR})
	INCLUDE(${QT_USE_FILE})
	ADD_DEFINITIONS(-DQT_GUI_LIBS -DQT_CORE_LIB -DQT3_SUPPORT)
ELSE(QT_USE_FILE)
	MESSAGE(FATAL_ERROR, "QT not found. Please set QT_DIR.")
ENDIF(QT_USE_FILE)

#======================================================================================
include(ExternalProject)
include(SlicerMacroEmptyExternalProject)
include(SlicerMacroCheckExternalProjectDependency)


set( proj ${LOCAL_PROJECT_NAME} )
option(USE_SPHARM-PDM "Build SPHARM-PDM" ON)
if(USE_SPHARM-PDM)
 #include(${CMAKE_CURRENT_LIST_DIR}/SuperBuild/External_Spharm-pdm.cmake)
 set(${LOCAL_PROJECT_NAME}_DEPENDENCIES spharm-pdm)
endif()

include(${CMAKE_CURRENT_SOURCE_DIR}/SetExternalProjectOptions.cmake)
#python test


add_subdirectory(Applications)


#======================================================================================
#======================================================================================
# Testing for 

#  set(TestingSRCdirectory ${CMAKE_CURRENT_SOURCE_DIR}/Testing)
#  set(TestingBINdirectory ${CMAKE_CURRENT_BINARY_DIR}/Testing)
#  set(TestDataFolder ${TestingSRCdirectory}/Data)
#  add_library(RodentThicknessLibrary STATIC ${RTsources}) # STATIC is also the default
#  set_target_properties(RodentThicknessLibrary PROPERTIES COMPILE_FLAGS "-Dmain=ModuleEntryPoint")
#  target_link_libraries(RodentThicknessLibrary ${QT_LIBRARIES} ${ITK_LIBRARIES} ${BatchMake_LIBRARIES})
#  set_target_properties(RodentThicknessLibrary PROPERTIES LABELS RodentThickness)
 
  # Create Tests
#  include(CTest)
#  add_subdirectory( ${TestingSRCdirectory} ) # contains a CMakeLists.txt
# include_directories( ${TestingSRCdirectory} ) # contains a CMakeLists.txt





