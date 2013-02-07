CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(RodentThickness)  # name of the project

# Slicer3
find_package(Slicer3 REQUIRED)
include(${Slicer3_USE_FILE})

# Default install prefix
slicer3_set_default_install_prefix_for_external_projects()

FIND_PACKAGE(GenerateCLP REQUIRED)
IF(GenerateCLP_FOUND)
	INCLUDE(${GenerateCLP_USE_FILE})
ELSE(GenerateCLP_FOUND)
	MESSAGE(FATAL_ERROR "GenerateCLP not found. Please set GenerateCLP_DIR")
ENDIF(GenerateCLP_FOUND)

FIND_PACKAGE(BatchMake REQUIRED)
IF(BatchMake_FOUND)
	INCLUDE(${BatchMake_USE_FILE})
ELSE(BatchMake_FOUND)
	MESSAGE(FATAL_ERROR "BatchMake not found. Please set BatchMake_DIR")
ENDIF(BatchMake_FOUND)

FIND_PACKAGE(ITK)
IF(ITK_FOUND)
	INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
	MESSAGE(FATAL_ERROR "ITK not found. Please set ITK_DIR")
ENDIF(ITK_FOUND)

FIND_PACKAGE(Qt4 REQUIRED) #/tools/devel/linux/Qt4/Qt4.8.1/Qt4.8.1_linux64/bin/qmake
IF(QT_USE_FILE)
	INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${QT_INCLUDE_DIR})
	INCLUDE(${QT_USE_FILE})
	ADD_DEFINITIONS(-DQT_GUI_LIBS -DQT_CORE_LIB -DQT3_SUPPORT)
ELSE(QT_USE_FILE)
	MESSAGE(FATAL_ERROR, "QT not found. Please set QT_DIR.")
ENDIF(QT_USE_FILE)

QT4_WRAP_CPP(QtProject_HEADERS_MOC GuiCSV.h)
QT4_WRAP_UI(UI_FILES guicsv.ui)

include_directories(${CMAKE_BINARY_DIRECTORY})
#####################

SET(sources RodentThickness.cxx GuiCSV.h GuiCSV.cxx ScriptRunner.h ScriptRunner.cxx StatisticalpartRunner.h StatisticalpartRunner.cxx ${QtProject_HEADERS_MOC} ${UI_FILES}) # define the variable "sources" that contains the names of files
GENERATECLP(sources RodentThickness.xml)
ADD_EXECUTABLE(RodentThickness ${sources})  # add the files contained by "sources" to the project

TARGET_LINK_LIBRARIES(RodentThickness ${QT_LIBRARIES} ${ITK_LIBRARIES} ${BatchMake_LIBRARIES})







