#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkNumericTraits.h"
#include "iostream"

template<class T> typename T::Pointer NewImageT(int w, int h, int d) {
  typename T::Pointer newImage = T::New();
  typename T::IndexType newIndex;
  newIndex.Fill(0);

  typename T::SizeType newSize;
  newSize[0] = w; newSize[1] = h; newSize[2] = d;

  typename T::RegionType newRegion(newIndex, newSize);
  newImage->SetLargestPossibleRegion(newRegion);
  newImage->SetBufferedRegion(newRegion);
  newImage->SetRequestedRegion(newRegion);
  newImage->Allocate();
  newImage->FillBuffer(itk::NumericTraits<ITK_TYPENAME T::PixelType>::Zero);

  return newImage;
}

template<class T> typename T::Pointer ReadImageT(const char* filename, int& ret) {
  std::cout << "Reading " << filename << " ...";
  std::cout.flush();
	typename itk::ImageFileReader<T>::Pointer reader = itk::ImageFileReader<T>::New();
	reader->SetFileName(filename);
	reader->Update();
  std::cout << " done." << std::endl;
	return reader->GetOutput();
}

template<class T> int WriteImageT(const char* filename, typename T::Pointer image) {
  std::cout << "Writing " << filename << " ...";
  std::cout.flush();
	typename itk::ImageFileWriter<T>::Pointer writer = itk::ImageFileWriter<T>::New();
	writer->SetFileName(filename);
	writer->UseCompressionOn();
	writer->SetInput(image);
	writer->Write();
  std::cout << " done." << std::endl;
	return 0;
}
