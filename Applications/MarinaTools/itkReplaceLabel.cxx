#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#pragma warning ( disable : 4503 )
#endif

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <set>

#include <string.h>
#include <sys/types.h>
#include <stdlib.h>    // for exit, system
#include <math.h>
#include <errno.h>

#include <itkImage.h>
#include <itkImageFileReader.h> 
#include <itkImageFileWriter.h>
#include <itkExtractImageFilter.h>
#include <itkImageToImageFilter.h>
#include <itkScalarImageKmeansImageFilter.h>
#include <itkNeighborhoodIterator.h>
#include <itkRelabelComponentImageFilter.h>
#include <itkConnectedComponentImageFilter.h>
#include "itkLabelReplaceImageFilter.h"

using namespace std;
using namespace itk;

typedef unsigned char BinaryType;
typedef short PixelType;

enum { ImageDimension = 3 };
typedef Image<BinaryType,ImageDimension>      BinaryImageType;
typedef Image<PixelType,ImageDimension>       ImageType;
typedef ImageType::Pointer                    ImagePointer;
typedef ImageType::RegionType                 ImageRegionType;
typedef ImageRegionIterator< ImageType >      IteratorType;
typedef ImageRegionConstIterator<ImageType>   ConstIteratorType;
typedef NeighborhoodIterator<ImageType>       NeighborhoodIteratorType;

typedef ImageFileReader< ImageType >          VolumeReaderType;
typedef ImageFileWriter< ImageType >          VolumeWriterType;
typedef LabelReplaceImageFilter<ImageType>    ReplaceFilerType;

int main(const int argc, const char **argv)
{
  if (argc < 5) {
    cout << argv[0] << " label1 label2 replacing-label output " << endl;
    return 1;
  }

  char *inputFileName1 = strdup(argv[1]);
  char *inputFileName2 = strdup(argv[2]);
  int label = atoi(argv[3]);
  char *outputFileName = strdup(argv[4]);

  ImagePointer inputImage1, inputImage2 ;

  VolumeReaderType::Pointer imageReader1 = VolumeReaderType::New();
  imageReader1->SetFileName(inputFileName1) ;

  VolumeReaderType::Pointer imageReader2 = VolumeReaderType::New();
  imageReader2->SetFileName(inputFileName2) ;

  try {
    imageReader1->Update();
    inputImage1 = imageReader1->GetOutput();

    imageReader2->Update();
    inputImage2 = imageReader2->GetOutput();
  } catch (ExceptionObject & err) {
    cerr << "ExceptionObject caught!" << endl;
    cerr << err << endl;
    return EXIT_FAILURE;	
  }    
 
  ReplaceFilerType::Pointer filter = ReplaceFilerType::New();
  filter->GetFunctor().SetReplacingLabel(label);
  filter->SetInput1(inputImage1);
  filter->SetInput2(inputImage2);
  filter->Update();

  VolumeWriterType::Pointer writer = VolumeWriterType::New();
  writer->SetFileName(outputFileName);
  writer->UseCompressionOn();
  writer->SetInput(filter->GetOutput());
  writer->Write();

  return 0;
}
