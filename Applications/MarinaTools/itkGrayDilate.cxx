#include <iostream>

#include "itkImageCommon.h"

#include "itkBinaryBallStructuringElement.h"
#include "itkGrayscaleDilateImageFilter.h"


typedef itk::Image<double,3> ImageType;
typedef itk::BinaryBallStructuringElement<double,3> StructuringElementType;
typedef itk::GrayscaleDilateImageFilter<ImageType,ImageType,StructuringElementType> DilateFilterType;

using namespace std;

int main(int argc, char* argv[]) {
  if (argc < 4) {
    cout << argv[0] << " in-image out-image radius " << endl;
    return 1;
  }
  int ret = 0;
  int radius = atoi(argv[3]);
  ImageType::Pointer img = ReadImageT<ImageType>(argv[1], ret);

  StructuringElementType kernel;
  kernel.SetRadius(radius);
  kernel.CreateStructuringElement();

  DilateFilterType::Pointer filter = DilateFilterType::New();
  filter->SetInput(img);
  filter->SetKernel(kernel);
  filter->Update();

  WriteImageT<ImageType>(argv[2], filter->GetOutput());
  return 0;
}
