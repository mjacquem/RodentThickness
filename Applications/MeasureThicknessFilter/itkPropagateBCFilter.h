/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkPropagateBCFilter.h,v $
  Language:  C++
  Date:      $Date: 2006/03/30 15:36:25 $
  Version:   $Revision: 1.10 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkPropagateBCFilter_h
#define __itkPropagateBCFilter_h

#include "itkImage.h"
#include <itkImageRegionConstIterator.h>
#include <itkImageRegionIterator.h>
#include <itkConstNeighborhoodIterator.h>
#include <itkNeighborhoodIterator.h>
#include <iostream>
#include <itkNeighborhood.h>
#include <itkNeighborhoodIterator.h>
#include <itkConstNeighborhoodIterator.h>
#include <set>
#include <itkImageToImageFilter.h>

namespace itk
{

/** \class PropagateBCFilter
 * \brief 
 *        
 * 
 */
  template <typename TInputImage > 
class ITK_EXPORT PropagateBCFilter:
    public ImageToImageFilter<TInputImage,TInputImage>
{
public:
  /** Standard class typedefs. */
  typedef PropagateBCFilter  Self;
  typedef ImageToImageFilter<TInputImage,TInputImage> Superclass;
  typedef SmartPointer<Self>                   Pointer;
  typedef SmartPointer<const Self>        ConstPointer;
  
  
  /** Pixel Type of the input image */
  typedef TInputImage                                    ImageType;

  typedef typename TInputImage::PixelType                PixelType;
  typedef typename NumericTraits<PixelType>::RealType    RealType;
  typedef typename NumericTraits<PixelType>::ScalarRealType ScalarRealType;

  typedef typename TInputImage::RegionType      RegionType;
  typedef typename TInputImage::SizeType      SizeType;
  typedef typename TInputImage::IndexType       IndexType;
  typedef typename TInputImage::OffsetType       OffsetType;
  

  typedef typename itk::ImageRegionIterator< TInputImage > ImageRegionIteratorType;
  typedef typename itk::ImageRegionConstIterator< TInputImage > ImageRegionConstIteratorType;
  typedef typename itk::NeighborhoodIterator< TInputImage > ImageNeighborhoodIteratorType;
  typedef typename itk::ConstNeighborhoodIterator< TInputImage > ImageConstNeighborhoodIteratorType;
  typedef typename ImageNeighborhoodIteratorType::RadiusType RadiusType;


  /** Image dimension. */
  itkStaticConstMacro(ImageDimension, unsigned int,
                      TInputImage::ImageDimension);


  typedef typename itk::Image< double, ImageDimension > DoubleImageType;
  typedef typename DoubleImageType::Pointer DoubleImagePointer;
  typedef typename itk::ImageRegionIterator< DoubleImageType > DoubleImageRegionIteratorType;
  typedef typename itk::ImageRegionConstIterator< DoubleImageType > DoubleImageRegionConstIteratorType;

  /** Define the image type for internal computations 
      RealType is usually 'double' in NumericTraits. 
      Here we prefer float in order to save memory.  */

  typedef typename NumericTraits< PixelType >::FloatType   InternalRealType;
  typedef Image<InternalRealType, 
                itkGetStaticConstMacro(ImageDimension) >   RealImageType;

  /**  Pointer to the Output Image */
  typedef typename ImageType::Pointer            ImagePointer;                                  
  typedef typename ImageType::ConstPointer       ImageConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

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

  itkGetMacro( BoundaryConditionToDetermineId, int);
  itkSetMacro( BoundaryConditionToDetermineId, int);

  itkGetMacro( NumberOfIterations, int);
  itkSetMacro( NumberOfIterations, int);
  
  itkGetMacro( KernelWidth, double );
  itkSetMacro( KernelWidth, double );

  
  struct globalIndxLtFcn {
    bool operator()( const IndexType& o1,  const IndexType& o2 ) const
    {
      
      for ( int iI=0; iI<ImageDimension; iI++ ) {
	if ( o1[iI]<o2[iI] ) return 1;
	if ( o1[iI]>o2[iI] ) return 0;
      }
      return 0;
      
    }
  };

protected:
  PropagateBCFilter();
  virtual ~PropagateBCFilter() {};
  void PrintSelf(std::ostream& os, Indent indent) const;
  
  /** Generate Data */
  void GenerateData( void );
  void GenerateOutputInformation();
  double kernelValue( double dR );


private:
  PropagateBCFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
  
  int m_DirichletLowId;
  int m_DirichletMeanId;
  int m_DirichletHighId;
  int m_NeumannId;
  int m_SolutionDomainId;
  int m_BoundaryConditionToDetermineId;
  double m_KernelWidth;

  int m_NumberOfIterations;

};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkPropagateBCFilter.hxx"
#endif

#endif




