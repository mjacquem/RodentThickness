# Make sure this file is included only once
get_filename_component(CMAKE_CURRENT_LIST_FILENAME ${CMAKE_CURRENT_LIST_FILE} NAME_WE)
if(${CMAKE_CURRENT_LIST_FILENAME}_FILE_INCLUDED)
  return()
endif()
set(${CMAKE_CURRENT_LIST_FILENAME}_FILE_INCLUDED 1)


# Include dependent projects if any
set(extProjName spharm-pdm) #The find_package known name

#set(${extProjName}_DEPENDENCIES ${ITK_EXTERNAL_NAME} SlicerExecutionModel VTK BatchMake)
set(${extProjName}_DEPENDENCIES BatchMake VTK SlicerExecutionModel ${ITK_EXTERNAL_NAME})


SlicerMacroCheckExternalProjectDependency(${extProjName})
set(proj ${extProjName}) #This local name


ExternalProject_Add(spharm-pdm
  SVN_REPOSITORY "http://www.nitrc.org/svn/spharm-pdm"
  SVN_REVISION -r "194"
  SVN_USERNAME slicerbot
  SVN_PASSWORD slicer
  SOURCE_DIR ${proj}
  BINARY_DIR ${proj}-build
  CMAKE_GENERATOR ${gen}
  CMAKE_ARGS
  ${CMAKE_OSX_EXTERNAL_PROJECT_ARGS}
  ${COMMON_EXTERNAL_PROJECT_ARGS}
  -DCOMPILE_ImageMath:BOOL=ON
  -DCOMPILE_SegPostProcessCLP:BOOL=ON
  -DCOMPILE_GenParaMeshCLP:BOOL=ON
  -DCOMPILE_MetaMeshTools:BOOL=OFF
  -DCOMPILE_RadiusToMesh:BOOL=OFF
  -DCOMPILE_ParaToSPHARMMeshCLP:BOOL=ON
  -DCOMPILE_ShapeAnalysisModule:BOOL=OFF
  -DCOMPILE_StatNonParamTestPDM:BOOL=OFF
  -DCOMPILE_shapeAnalysisMANCOVA_Wizard:BOOL=OFF
  -DCOMPILE_shapeworks:BOOL=ON
  -DCOMPILE_ParticleModule:BOOL=OFF
  -DUSE_SYSTEM_ITK:BOOL=ON
  -DUSE_SYSTEM_SlicerExecutionModel:BOOL=OFF
  -DUSE_SYSTEM_VTK:BOOL=ON
  -DUSE_SYSTEM_BatchMake:BOOL=ON
  -DUSE_SYSTEM_CLAPACK:BOOL=OFF
  -DUSE_SYSTEM_LAPACK:BOOL=OFF
 # -DBoost_DIR:PATH=${Boost_DIR}
  -DITK_DIR:PATH=${ITK_DIR}
  -DVTK_DIR:PATH=${VTK_DIR}
  -DSlicerExecutionModel_DIR:PATH=${SlicerExecutionModel_DIR}
  -DBatchMake_DIR:PATH=${BatchMake_DIR}
  INSTALL_COMMAND ""
  DEPENDS
      ${${proj}_DEPENDENCIES}
  )
set(${extProjName}_DIR ${CMAKE_BINARY_DIR}/${proj}-build)
#SlicerMacroEmptyExternalProject(${proj} "${${proj}_DEPENDENCIES}")
