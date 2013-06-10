/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkSolveLaplacianImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2005/09/30 17:01:19 $
  Version:   $Revision: 1.1 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkSolveLaplacianImageFilter_h
#define __itkSolveLaplacianImageFilter_h

#include "itkImageToImageFilter.h"
#include <map>
#include <itkFEMLinearSystemWrapperVNL.h>
//#include <itkFEMLinearSystemWrapperItpack.h>
#include <itkImage.h>
#include <itkImageRegionConstIterator.h>
#include <itkImageFileReader.h>
#include <itkConstNeighborhoodIterator.h>
#include <itkNeighborhoodIterator.h>
#include <iostream>

namespace itk
{

/** \class SolveLaplacianImageFilter
 * \brief Computes the solution to Laplace's equation \Delta u = 0
 * to be subsequently used for the definition of distances
 * \ingroup ImageToImageFilter
 */
  template <class TInputImage, class TOutputImage>
class ITK_EXPORT SolveLaplacianImageFilter:
    public ImageToImageFilter<TInputImage,TOutputImage>
{
public:
  /** Standard class typedefs. */
  typedef SolveLaplacianImageFilter         Self;
  typedef ImageToImageFilter<TInputImage,TOutputImage>  Superclass;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);  

  /** Run-time type information (and related methods). */
  itkTypeMacro(SolveLaplacianImageFilter, ImageToImageFilter);

  /** typedefs needed for managing the image regions */
  typedef typename TInputImage::RegionType      InputRegionType;
  typedef typename TOutputImage::RegionType     OutputRegionType;

  typedef typename TInputImage::PixelType       InputImagePixelType;
  typedef typename TOutputImage::PixelType       OutputImagePixelType;
  

  typedef typename TInputImage::SizeType        SizeType;
  typedef typename TInputImage::IndexType       StartType;

  typedef typename TInputImage::ConstPointer    InputImageConstPointer;
  typedef typename TOutputImage::Pointer         OutputImagePointer;

  typedef typename TInputImage::RegionType      RegionType;

  typedef typename itk::ImageRegionConstIterator< TInputImage > ConstInputImageIteratorType;
  typedef typename itk::ImageRegionConstIterator< TOutputImage > ConstOutputImageIteratorType;

  typedef typename itk::ConstNeighborhoodIterator< TInputImage > ConstInputImageNeighborhoodIteratorType;
  typedef typename itk::ConstNeighborhoodIterator< TOutputImage > ConstOutputImageNeighborhoodIteratorType;
  typedef typename itk::NeighborhoodIterator< TOutputImage > OutputImageNeighborhoodIteratorType;

  // typedef typename itk::NeighborhoodIterator< TInputImage > NeighborhoodIteratorType;

  typedef typename ConstInputImageNeighborhoodIteratorType::RadiusType RadiusType;

  typedef typename TInputImage::IndexType IndexType;

  struct globalIndxLtFcn {
    bool operator()( const IndexType& o1,  const IndexType& o2 ) const
    {

      for ( int iI=0; iI<InputImageDimension; iI++ ) {
	if ( o1[iI]<o2[iI] ) return 1;
	if ( o1[iI]>o2[iI] ) return 0;
      }
      return 0;

    }
  };

  itkGetMacro( DirichletLowId, int);
  itkSetMacro( DirichletLowId, int);

  itkGetMacro( DirichletHighId, int);
  itkSetMacro( DirichletHighId, int);

  itkGetMacro( NeumannId, int);
  itkSetMacro( NeumannId, int);

  itkGetMacro( DirichletMeanId, int);
  itkSetMacro( DirichletMeanId, int);

  itkGetMacro( SolutionDomainId, int);
  itkSetMacro( SolutionDomainId, int);

  itkGetMacro( DirichletLowVal, double);
  itkSetMacro( DirichletLowVal, double);

  itkGetMacro( DirichletMeanVal, double);
  itkSetMacro( DirichletMeanVal, double);

  itkGetMacro( DirichletHighVal, double);
  itkSetMacro( DirichletHighVal, double);

  itkGetMacro( NeumannVal, double);
  itkSetMacro( NeumannVal, double);

  itkGetMacro( SolutionDomainVal, double);
  itkSetMacro( SolutionDomainVal, double);

  itkGetMacro( KernelWidth, double);
  itkSetMacro( KernelWidth, double);

  /** Extract dimension from input image. */
  itkStaticConstMacro(InputImageDimension, unsigned int,
                      TInputImage::ImageDimension);
  itkStaticConstMacro(OutputImageDimension, unsigned int,
                      TInputImage::ImageDimension);

protected:
  SolveLaplacianImageFilter();
  ~SolveLaplacianImageFilter() {};
  void PrintSelf(std::ostream& os, Indent indent) const;

  /** This method defines the metadata of the output image */
  void GenerateOutputInformation();
  
  /** This method performs the operation of copying pixels from the input image
   * into the outpu image. */
  void GenerateData();
  double kernelValue( double );

private:
  SolveLaplacianImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  int m_DirichletLowId;
  int m_DirichletMeanId;
  int m_DirichletHighId;
  int m_NeumannId;
  int m_SolutionDomainId;

  double m_DirichletLowVal;
  double m_DirichletMeanVal;
  double m_DirichletHighVal;
  double m_NeumannVal;
  double m_SolutionDomainVal;

  double m_KernelWidth;

};

  
} // end namespace itk
  
#ifndef ITK_MANUAL_INSTANTIATION
#include "itkSolveLaplacianImageFilter.hxx"
#endif
  
#endif

