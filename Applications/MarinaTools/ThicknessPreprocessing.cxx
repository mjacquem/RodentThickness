#include <iostream>

#include "itkImageCommon.h"

#include "itkBinaryBallStructuringElement.h"
#include "itkGrayscaleDilateImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkAntiAliasBinaryImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"

#include "ThicknessPreprocessingCLP.h"


typedef itk::Image<short,3> LabelMapType;
typedef itk::Image<double,3> ImageType;
typedef itk::BinaryBallStructuringElement<double,3> StructuringElementType;
typedef itk::GrayscaleDilateImageFilter<ImageType,ImageType,StructuringElementType> DilateFilterType;
typedef itk::ImageRegionConstIteratorWithIndex<LabelMapType> IteratorType;

typedef itk::AntiAliasBinaryImageFilter<LabelMapType,ImageType> AntiAliasFilterType;
typedef itk::RescaleIntensityImageFilter<ImageType,LabelMapType> RescaleFilterType;
typedef itk::BinaryThresholdImageFilter<LabelMapType,LabelMapType> ThresholdFilterType;

using namespace std;

LabelMapType::Pointer applyAntiAliasFilter(LabelMapType::Pointer img) {
  cout << "Applying Anti-Alias Filter ... ";
  AntiAliasFilterType::Pointer antiAliasFilter = AntiAliasFilterType::New();
  antiAliasFilter->SetInput(img);
  antiAliasFilter->SetMaximumRMSError(0.001);
  antiAliasFilter->SetNumberOfIterations(100);
  antiAliasFilter->SetNumberOfLayers(2);
  antiAliasFilter->Update();
  cout << " done." << endl;

  cout << "Applying Rescale Filter ... ";
  RescaleFilterType::Pointer rescaleFilter = RescaleFilterType::New();
  rescaleFilter->SetInput(antiAliasFilter->GetOutput());
  rescaleFilter->SetOutputMinimum(0);
  rescaleFilter->SetOutputMaximum(255);
  cout << " done." << endl;

  cout << "Applying Threshold Filter ... ";
  ThresholdFilterType::Pointer threshFilter = ThresholdFilterType::New();
  threshFilter->SetInput(rescaleFilter->GetOutput());
  threshFilter->SetUpperThreshold(255);
  threshFilter->SetLowerThreshold(127);
  threshFilter->SetOutsideValue(0);
  threshFilter->SetInsideValue(1);
  threshFilter->Update();
  cout << " done." << endl;

  return threshFilter->GetOutput();
}

LabelMapType::Pointer extractLabel(LabelMapType::Pointer img, int extractingLabel) {
  cout << "Extracting label [" << extractingLabel << "] ...";
  ThresholdFilterType::Pointer extractFilter = ThresholdFilterType::New();
  extractFilter->SetInput(img);
  extractFilter->SetUpperThreshold(extractingLabel);
  extractFilter->SetLowerThreshold(extractingLabel);
  extractFilter->SetOutsideValue(0);
  extractFilter->SetInsideValue(1);
  extractFilter->Update();
  cout << " done." << endl;
  return extractFilter->GetOutput();
}

int main(int argc, char* argv[]) {
  PARSE_ARGS;

  int ret = 0;
  LabelMapType::Pointer boundaryMap = ReadImageT<LabelMapType>(inputBoundaryLabelMap.c_str(), ret);
  LabelMapType::Pointer solutionDomainImage = extractLabel(boundaryMap, solutionDomainLabel);
  LabelMapType::Pointer antiAliasedImage = applyAntiAliasFilter(solutionDomainImage);

  IteratorType it(antiAliasedImage, antiAliasedImage->GetBufferedRegion());
  
  while (!it.IsAtEnd()) {
    LabelMapType::IndexType idx = it.GetIndex();
    LabelMapType::PixelType before = solutionDomainImage->GetPixel(idx);
    LabelMapType::PixelType after = it.Get();
    if (after == 1 &&  before == 0) {
      boundaryMap->SetPixel(idx, static_cast<LabelMapType::PixelType>(solutionDomainLabel));
    }
    ++it;
  }

  if (antiAliasedSolutionLabel != "") {
    WriteImageT<LabelMapType>(antiAliasedSolutionLabel.c_str(), antiAliasedImage);
  }
  WriteImageT<LabelMapType>(outputBoundaryLabelMap.c_str(), boundaryMap);
  return 0;
}
