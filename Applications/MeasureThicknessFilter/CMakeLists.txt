CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(measureThicknessFilter) 

SET(CMAKE_VERBOSE_MAKEFILE ON)

FIND_PACKAGE(ITK) 
IF(ITK_FOUND) 
    INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND) 
    MESSAGE(FATAL_ERROR "ITK not found. Please set ITK_DIR.")
ENDIF(ITK_FOUND) 

FIND_PACKAGE(VTK)
IF (VTK_FOUND)
    INCLUDE (${VTK_USE_FILE})
ELSE(VTK_FOUND)
    MESSAGE(FATAL_ERROR, "VTK not found. Please set VTK_DIR.")
ENDIF (VTK_FOUND)

FIND_PACKAGE(GenerateCLP REQUIRED)
IF(GenerateCLP_FOUND)
  INCLUDE(${GenerateCLP_USE_FILE})
ENDIF(GenerateCLP_FOUND)

SET (measureThicknessFilter_SOURCE measureThicknessFilter.cxx)
SET (measureThicknessFilter_SOURCE_FILES measureThicknessFilter.cxx
itkBoundaryStatisticsImageFilter.h       itkLaplaceImageFilterRestrictedDomain.h    itkSolveLaplacianImageFilter.h
itkBoundaryStatisticsImageFilter.hxx     itkLaplaceImageFilterRestrictedDomain.hxx  itkSolveLaplacianImageFilter.hxx
itkCreateBoundaryConditionImageFilter.h  itkNegateVectorImageFilter.h               itkSolveTransportImageFilter.h
itkNormalizeVectorImageFilter.h            itkSolveTransportImageFilter.hxx
itkPropagateBCFilter.h                     itkTransportFunctionRestrictedDomain.h
itkGradientImageFilter.h                 itkPropagateBCFilter.hxx                   itkTransportFunctionRestrictedDomain.hxx
itkGradientImageFilter.hxx               itkScalarValueDiffusionFilter.h            itkTransportImageFilterRestrictedDomain.h
itkLaplaceFunctionRestrictedDomain.h     itkScalarValueDiffusionFilter.hxx          itkTransportImageFilterRestrictedDomain.hxx
itkLaplaceFunctionRestrictedDomain.hxx   itkScalarValueDiffusionFunction.h          itkZeroImageFilter.h
itkLaplaceGradientImageFilter.h          itkScalarValueDiffusionFunction.hxx
itkLaplaceGradientImageFilter.hxx        itkScalarValueSparseDiffusionFilter.h
)

GENERATECLP(measureThicknessFilter_SOURCE measureThicknessFilter.xml)

ADD_EXECUTABLE(measureThicknessFilter ${measureThicknessFilter_SOURCE_FILES})

TARGET_LINK_LIBRARIES(measureThicknessFilter ${ITK_LIBRARIES})

