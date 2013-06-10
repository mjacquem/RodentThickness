#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>

#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>

int main(int argc, char* argv[]) {
  if (argc < 5) {
    printf("usage: %s in-nrrd out-nrrd x-spacing y-spacing z-spacing [[x-origin y-origin z-origin] | [ 1 ]]\n", argv[0]);
    return 1;
  }

  double origin[3] = { 0, 0, 0 };
  bool boolZeroOrigin = false;
  bool boolResetOrigin = false;

  if (argc > 6 && argc < 8) {
    if (1 == atoi(argv[6])) {
      boolZeroOrigin = true;
    }
  } else if (argc > 7 && argc < 10) {
    boolResetOrigin = true;
    for (int i = 0; i < 3; i++) {
      origin[i] = atof(argv[i + 6]);
    }
  }

  typedef itk::Image<double,3> ImageType;
  typedef itk::ImageFileReader<ImageType> ImageReaderType;
  typedef itk::ImageFileWriter<ImageType> ImageWriterType;

  ImageReaderType::Pointer reader = ImageReaderType::New();
  reader->SetFileName(argv[1]);
  reader->Update();

  ImageType::Pointer img = reader->GetOutput();
  double spacing[3];
  for (int d = 0; d < 3; d++) {
    spacing[d] = atof(argv[d + 3]);
  }
  img->SetSpacing(spacing);
  if (boolZeroOrigin) {
    std::cout << "resetting the origin to center" << std::endl;
    img->SetOrigin(origin);
  } else if (boolResetOrigin) {
    std::cout << "resetting the origin to " << origin[0] << ", " << origin[1] << "," << origin[2] << std::endl;
    img->SetOrigin(origin);
  }

  ImageWriterType::Pointer writer = ImageWriterType::New();
  writer->SetInput(img);
  writer->SetFileName(argv[2]);
  writer->UseCompressionOn();
  writer->Write();
  return 0;
}
