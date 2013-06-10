CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(MarinaTools) 

GENERATECLP(MeshPointsIntensitySampling.cxx MeshPointsIntensitySampling.xml)
ADD_EXECUTABLE(MeshPointsIntensitySampling MeshPointsIntensitySampling.cxx MeshPointsIntensitySamplingCLP.h vtkPolyDataToitkMesh.cxx vtkPolyDataToitkMesh.h)
TARGET_LINK_LIBRARIES(MeshPointsIntensitySampling ${ITK_LIBRARIES} vtkFiltering vtkIO vtkGraphics)

GENERATECLP(main.cxx BinToDistPreprocessing.xml)
ADD_EXECUTABLE(BinaryToDistanceMap main.cxx BinToDistPreprocessingCLP.h)
TARGET_LINK_LIBRARIES(BinaryToDistanceMap ${ITK_LIBRARIES})
