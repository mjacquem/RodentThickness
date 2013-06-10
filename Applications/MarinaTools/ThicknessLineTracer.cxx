#include <iostream>
#include <math.h>

#include "itkImageCommon.h"
#include "itkConstNeighborhoodIterator.h"
#include "itkPoint.h"

#include "vtkPolyData.h"
#include "vtkPolyDataReader.h"
#include "vtkPolyDataWriter.h"
#include "vtkPoints.h"
#include "vtkAppendPolyData.h"
#include "vtkIdList.h"

typedef itk::Point<double,3> PointType;
typedef itk::Image<double,3> ImageType;
typedef itk::ConstNeighborhoodIterator<ImageType> IteratorType;
typedef std::vector<ImageType::IndexType> IndexVector;

PointType newPointType(double* pointCoord) {
  PointType aPoint; 
  for (int i = 0; i < 3; i++) {
    aPoint[i] = pointCoord[i];
  }
  return aPoint;
}

bool subtraceIsoPath(IteratorType& it, ImageType::IndexType& curIndex, ImageType::PixelType curValue, IndexVector& outIndexVector) {
  // loop over neighbor and find next index to add
  //
  double mindiff = 0.1;
  bool foundNext = false;

  /*
  cout << "entering at " << curIndex << " with ";
  for (int i = 0; i < outIndexVector.size(); i++) {
    cout << outIndexVector[i];
  }
  cout << endl;
  */

  ImageType::IndexType closestIndex;
  ImageType::PixelType closestValue;

  it.SetLocation(curIndex);
  const unsigned int neighborSize = it.Size();

  for (int i = 0; i < neighborSize; i++) {
    ImageType::PixelType neighborValue = it.GetPixel(i); 
    if (neighborValue < 0) {
      continue;
    }
    double diff = abs(static_cast<double>(curValue - neighborValue));
    if (diff < mindiff) {
      ImageType::IndexType neighborIndex = it.GetIndex(i);
      if (curIndex == neighborIndex) {
        continue;
      }
      bool foundDup = false;
      for (int j = 0; j < outIndexVector.size(); j++) {
        if (neighborIndex == outIndexVector[j]) {
          foundDup = true;
          break;
        }
      }
      if (!foundDup) {
        mindiff = diff;
        closestIndex = neighborIndex;
        closestValue = neighborValue;
        foundNext = true;
      }
    }
  }

  if (foundNext) {
    outIndexVector.push_back(closestIndex);
    subtraceIsoPath(it, closestIndex, closestValue, outIndexVector);
  }

  return foundNext;
}

void traceIsoPath(ImageType::Pointer image, ImageType::IndexType& index, IndexVector& outIndexVector) {
  ImageType::SizeType radius;
  radius.Fill(1);
  IteratorType it(radius, image, image->GetBufferedRegion());
  ImageType::PixelType curValue = image->GetPixel(index);

  outIndexVector.push_back(index);
  subtraceIsoPath(it, index, curValue, outIndexVector);
}

vtkPoints* computePhysicalPointsFromIndexPath(ImageType::Pointer image, IndexVector& pathIndex) {
  vtkPoints* points = vtkPoints::New();

  cout << "Found path (" << pathIndex.size() << ") : " << endl;
  for (IndexVector::iterator it = pathIndex.begin(); it != pathIndex.end(); it++) {
    ImageType::IndexType index = *it;
    PointType point;
    image->TransformIndexToPhysicalPoint(index, point);
    points->InsertNextPoint(point[0], point[1], point[2]);
    cout << point << " ";
  }
  cout << endl;

  return points;
}

vtkPolyData* constructPathObject(vtkPoints* points) {
  vtkPolyData* pathObj = vtkPolyData::New();
  pathObj->Allocate();
  pathObj->SetPoints(points);
  for (int i = 1; i < points->GetNumberOfPoints(); i++) {
    vtkIdList* idList = vtkIdList::New();
    idList->Allocate(2);
    idList->InsertId(0, i-1);
    idList->InsertId(1, i);
    pathObj->InsertNextCell(VTK_LINE, idList);
  }
  return pathObj;
}

int main(int argc, char* argv[]) {
  int ret = 0;
  vtkPolyDataReader* reader = vtkPolyDataReader::New();
  reader->SetFileName(argv[1]);
  reader->Update(); 
  vtkPolyData* mesh = reader->GetOutput();
  vtkPoints* points = mesh->GetPoints();

  ImageType::Pointer thicknessImage = ReadImageT<ImageType>(argv[2], ret);

  vtkAppendPolyData* appender = vtkAppendPolyData::New();
  for (int i = 0; i < points->GetNumberOfPoints(); i++) {
    double samplePointArray[3];
    points->GetPoint(i, samplePointArray);

    PointType samplePoint = newPointType(samplePointArray);
    ImageType::IndexType sampleIndex;
    thicknessImage->TransformPhysicalPointToIndex(samplePoint, sampleIndex);

    IndexVector pathIndex;
    cout << "Begin trace at " << sampleIndex << endl;
    traceIsoPath(thicknessImage, sampleIndex, pathIndex);
    vtkPoints* pathPoints = computePhysicalPointsFromIndexPath(thicknessImage, pathIndex);

    if (pathPoints->GetNumberOfPoints() > 5) {
      vtkPolyData* pathObject = constructPathObject(pathPoints);
      appender->AddInput(pathObject);
    }
  }
  appender->Update();
  vtkPolyDataWriter* writer = vtkPolyDataWriter::New();
  writer->SetFileName("lines.vtk");
  writer->SetInput(appender->GetOutput());
  writer->Write();
}
