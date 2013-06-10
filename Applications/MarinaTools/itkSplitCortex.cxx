#include "itkImageCommon.h"
#include "itkImage.h"
#include "itkImageRegionConstIteratorWithIndex.h"
#include "iostream"
#include "vector"

typedef itk::Image<unsigned short,3> ImageType;
typedef itk::ImageRegionConstIteratorWithIndex<ImageType> IteratorType;

using namespace std;

int main(int argc, char* argv[]) {
  int ret = 0;
  ImageType::Pointer guideImage = ReadImageT<ImageType>(argv[1], ret);
  ImageType::Pointer baseImage = ReadImageT<ImageType>(argv[2], ret);

  IteratorType it(guideImage, guideImage->GetBufferedRegion());

  ImageType::SizeType size = guideImage->GetBufferedRegion().GetSize();
  cout << "Size: " << size << endl;

  vector<ImageType::IndexType> maxIndex;
  int prevSliceZ = 0;
  ImageType::IndexType mxxIndex;
  mxxIndex[0] = 9999;

  for (int z = 0; z < size[1]; z++) {
    for (int y = 0; y < size[2]; y++) {
      for (int x = 0; x < size[0]; x++) {
        ImageType::IndexType idx;
        idx[0] = x; idx[2] = y; idx[1] = z;
        if (guideImage->GetPixel(idx) == 3) {
          if (mxxIndex[0] > idx[0]) {
            mxxIndex = idx;
          }
        }
      }
    }
    if (mxxIndex[0] >= size[0]) {
      continue;
    }
    maxIndex.push_back(mxxIndex);
    mxxIndex[0] = size[0];
  }


  for (int i = 0; i < maxIndex.size(); i++) {
    ImageType::IndexType idx = maxIndex[i];
    ImageType::IndexType baseIdx;
    for (int y = 0; y < size[2]; y++) {
      for (int x = 0; x < idx[0]; x++) {
        baseIdx[0] = x;
        baseIdx[2] = y;
        baseIdx[1] = idx[1];
        if (baseImage->GetPixel(baseIdx) == 3) {
          baseImage->SetPixel(baseIdx, 4);
        }
      }
    }
  }

  WriteImageT<ImageType>(argv[3], baseImage);
  return 0;
}
