#include "itkImageCommon.h"
#include "itkImage.h"
#include "itkImageRegionConstIteratorWithIndex.h"

using namespace std;

typedef itk::Image<unsigned short, 3> ImageType;
typedef itk::ImageRegionConstIteratorWithIndex<ImageType> IteratorType;

int main(int argc, char* argv[]) {
  if (argc < 3) {
    cout << argv[0] << " out-img label1 label2 ..." << endl;
  }

  int ret = 0;
  ImageType::Pointer label = ReadImageT<ImageType>(argv[2], ret);

  int labelValue = 1;
  for (int i = 3; i < argc; i++) {
    ImageType::Pointer anotherLabel = ReadImageT<ImageType>(argv[i], ret);
    
    IteratorType it(anotherLabel, anotherLabel->GetBufferedRegion());
    while (!it.IsAtEnd()) {
      labelValue = (int) it.Get();
      if (labelValue > 0) {
        label->SetPixel(it.GetIndex(), static_cast<ImageType::PixelType>(labelValue));
      }
      ++it;
    }
  }

  WriteImageT<ImageType>(argv[1], label);
}
