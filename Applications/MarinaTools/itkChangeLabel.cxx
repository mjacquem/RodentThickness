#include <iostream>

#include "itkImageCommon.h"
#include "itkChangeLabelImageFilter.h"

using namespace std;

typedef itk::Image<unsigned short,3> ImageType;
typedef itk::ChangeLabelImageFilter<ImageType,ImageType> ChangeLabelFilterType;

int main(int argc, char* argv[]) {
  if (argc < 5) {
    cout << argv[0] << " in-image out-image old-label1 new-label1 old-label2 new-label2 ... " << endl;
    return 1;
  }

  int ret = 0;
  const char* inImageName = argv[1];
  const char* outImageName = argv[2];

  ImageType::Pointer inImage = ReadImageT<ImageType>(inImageName, ret);
  ChangeLabelFilterType::Pointer filter = ChangeLabelFilterType::New();
  ChangeLabelFilterType::ChangeMapType changeMap;

  for (int i = 3; i < argc; i+=2) {
    ImageType::PixelType oldLabel = static_cast<ImageType::PixelType>(atoi(argv[i]));
    ImageType::PixelType newLabel = static_cast<ImageType::PixelType>(atoi(argv[i+1]));

    cout << "Mapping " << oldLabel << " => " << newLabel << endl;
    changeMap.insert(pair<ImageType::PixelType,ImageType::PixelType>(oldLabel, newLabel));
  }

  filter->SetChangeMap(changeMap);
  filter->SetInput(inImage);
  filter->Update();

  WriteImageT<ImageType>(outImageName, filter->GetOutput());
}
