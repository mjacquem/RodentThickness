#include "itkImageCommon.h"
#include "itkImage.h"
#include "itkImageRegionIterator.h"

typedef itk::Image<double,3> ImageType;

int main(int argc, char* argv[]) {
  if (argc < 3) {
    cout << "usage: " << argv[0] << " in-txt dim-x dim-y dim-z out-image " << endl;
    return 0;
  }

  char* inTxt = argv[1];
  int x = atoi(argv[2]);
  int y = atoi(argv[3]);
  int z = atoi(argv[4]);
  char* outImage = argv[5];

  ImageType::Pointer newImage = NewImageT<ImageType>(x, y, z);
  return 0
}
