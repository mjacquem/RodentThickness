#include "vtkUnstructuredGrid.h"
#include "vtkDoubleArray.h"
#include "vtkPoints.h"
#include "vtkPointData.h"
#include "vtkPolyDataWriter.h"
#include "vtkUnstructuredGridWriter.h"

#include "itkImageCommon.h"
#include "itkVector.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageRegionConstIteratorWithIndex.h"

#include <vector>
#include <iostream>

#define VTK_CREATE(type, name) \
	type *name = type::New()

//  vtkSmartPointer<type> name = vtkSmartPointer<type>::New()

using namespace std;

typedef itk::Point<double,3> PointType;
typedef itk::Vector<double,3> VectorType;
typedef itk::Image<VectorType,3> VectorImageType;
typedef itk::Image<unsigned short,3> LabelMapType;
typedef itk::Image<double,3> ThicknessMapType;
typedef itk::ImageRegionConstIteratorWithIndex<LabelMapType> LabelMapIteratorType;
typedef itk::ImageRegionConstIteratorWithIndex<VectorImageType> VectorIteratorType;

/**
 * Read boundary label map
 * Read laplacian gradient image
 * Find region of interest based on boundary label map
 * Construct unstructured grid 
 **/
int main (int argc, char* argv[]) {
  if (argc < 4) {
    cout << argv[0] << " boundary-label-map laplacian-gradient-image thickness-image vtk-grid-output" << endl; return 0;
  } 

  VTK_CREATE(vtkUnstructuredGrid, grid);
  VTK_CREATE(vtkPoints, gridPoints);
  VTK_CREATE(vtkDoubleArray, gridVectors);
  VTK_CREATE(vtkDoubleArray, gridScalars);
 
  gridVectors->SetNumberOfComponents(3);
  gridScalars->SetNumberOfComponents(1);

  int ret = 0;
  VectorImageType::Pointer laplacianImage = ReadImageT<VectorImageType>(argv[2], ret);
  LabelMapType::Pointer boundaryImage = ReadImageT<LabelMapType>(argv[1], ret);
  ThicknessMapType::Pointer thicknessImage = ReadImageT<ThicknessMapType>(argv[3], ret);

  cout << "Iterating pixels... " << endl;
  LabelMapIteratorType it(boundaryImage, boundaryImage->GetBufferedRegion());
  while (!it.IsAtEnd()) {
    LabelMapType::PixelType domainValue = it.Get();
    if (domainValue == 3) {
      LabelMapType::IndexType domainIndex = it.GetIndex();

      PointType domainPoint;
      boundaryImage->TransformIndexToPhysicalPoint(domainIndex, domainPoint);
      gridPoints->InsertNextPoint(domainPoint[0], domainPoint[1], domainPoint[2]);

      VectorType domainVector = laplacianImage->GetPixel(domainIndex);
      gridVectors->InsertNextTuple3(domainVector[0], domainVector[1], domainVector[2]);

      ThicknessMapType::PixelType thicknessValue = thicknessImage->GetPixel(domainIndex);
      gridScalars->InsertNextValue(thicknessValue);
    }
    ++it;
  }

  grid->SetPoints(gridPoints);
  grid->GetPointData()->SetVectors(gridVectors);
  grid->GetPointData()->SetScalars(gridScalars);

  vtkUnstructuredGridWriter* writer = vtkUnstructuredGridWriter::New();
  writer->SetFileName(argv[4]);
  writer->SetInput(grid);
  writer->Write();
 
  return EXIT_SUCCESS;
} 
