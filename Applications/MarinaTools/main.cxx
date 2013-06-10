#include "BinToDistPreprocessingCLP.h"
#include "itkImage.h"
#include "itkImageFileWriter.h"
#include "itkImageFileReader.h"
#include "itkSignedDanielssonDistanceMapImageFilter.h"
#include "itkDiscreteGaussianImageFilter.h"

#include <fstream>
#include <iostream>
#include <vector>
#include <errno.h>

using namespace std ;
typedef itk::Image < double, 3 > ImageType ;
typedef ImageType::Pointer ImagePointer ;
typedef itk::ImageFileWriter < ImageType > ImageWriterType ;
typedef itk::ImageFileReader < ImageType > ImageReaderType ;
typedef itk::SignedDanielssonDistanceMapImageFilter< ImageType, ImageType > DDMapType ;
typedef itk::DiscreteGaussianImageFilter < ImageType, ImageType > gaussFilterType ;

int main(int argc, const char **argv)
{
  PARSE_ARGS;
  // read the input volume
  ImageReaderType::Pointer reader = ImageReaderType::New () ;

  reader->SetFileName (binaryInput);
  std::cout << "image read" << std::endl ;
  
  ImagePointer fImage = reader->GetOutput () ;

  // now set up the Danielsson filter
  DDMapType::Pointer ddmap = DDMapType::New () ;
  ddmap->SetInput ( fImage ) ;
  ddmap->Update () ;
  std::cout << "distance map computed" << std::endl ;
  
  // gaussian smoothing
  gaussFilterType::Pointer smoothFilter ;
  if ( smoothing > 0 )
    {
      std::cout << "smoothing: " << smoothing << std::endl ;
      smoothFilter = gaussFilterType::New () ;
      smoothFilter->SetInput ( ddmap->GetDistanceMap () ) ;
      smoothFilter->SetVariance ( smoothing ) ;
      smoothFilter->Update () ;
      std::cout << "smoothing completed" << std::endl ;
    }

  // write out the result
  ImageWriterType::Pointer itkWriter = ImageWriterType::New() ;
  itkWriter->SetFileName (output); 
  if ( smoothing > 0 ) 
    itkWriter->SetInput ( smoothFilter->GetOutput() ) ;
  else
    itkWriter->SetInput ( ddmap->GetDistanceMap () ) ;
  itkWriter->Write() ;          
  

  return 0 ;
}
