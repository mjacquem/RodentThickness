/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkSolveTransportImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2005/09/30 17:01:19 $
  Version:   $Revision: 1.1 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkSolveTransportImageFilter_h
#define __itkSolveTransportImageFilter_h

#include "itkImageToImageFilter.h"
#include <map>
#include <itkFEMLinearSystemWrapperVNL.h>
#include <itkImage.h>
#include <itkImageRegionConstIterator.h>
#include <itkImageFileReader.h>
#include <itkConstNeighborhoodIterator.h>
#include <itkNeighborhoodIterator.h>
#include <iostream>
#include "itkCovariantVector.h"

namespace itk
{

/** \class SolveTransportImageFilter
 * \brief Computes the solution to Laplace's equation \Delta u = 0
 * to be subsequently used for the definition of distances
 * \ingroup ImageToImageFilter
 */
  template <class TLabelMapImage, class TInputImage, class TVectorFieldValueType=float>
class ITK_EXPORT SolveTransportImageFilter:
    public ImageToImageFilter<TLabelMapImage,TInputImage>
{
public:
  /** Standard class typedefs. */
  typedef SolveTransportImageFilter         Self;
  typedef ImageToImageFilter<TLabelMapImage,TInputImage>  Superclass;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);  

  /** Run-time type information (and related methods). */
  itkTypeMacro(SolveTransportImageFilter, ImageToImageFilter);

  /** typedefs needed for managing the image regions */
  typedef typename TInputImage::RegionType      RegionType;
  typedef typename TInputImage::SizeType        SizeType;
  typedef typename TInputImage::IndexType       StartType;
  typedef typename TInputImage::ConstPointer    InputImageConstPointer;
  typedef typename TLabelMapImage::ConstPointer LabelMapImageConstPointer;
  typedef typename TInputImage::Pointer         OutputImagePointer;

  typedef typename itk::ImageRegionConstIterator< TInputImage > ConstIteratorType;

  typedef typename itk::ImageRegionConstIterator< TLabelMapImage > ConstLabelMapIteratorType;

  typedef typename itk::ConstNeighborhoodIterator< TInputImage > ConstNeighborhoodIteratorType;
  typedef typename itk::ConstNeighborhoodIterator< TLabelMapImage > ConstLabelMapNeighborhoodIteratorType;

  typedef typename itk::NeighborhoodIterator< TInputImage > NeighborhoodIteratorType;
  typedef typename ConstNeighborhoodIteratorType::RadiusType RadiusType;

  typedef typename TInputImage::IndexType IndexType;
  
  typedef typename itk::Image<CovariantVector<TVectorFieldValueType, ::itk::GetImageDimension<TInputImage>::ImageDimension>,  ::itk::GetImageDimension<TInputImage>::ImageDimension > VectorFieldType;
  typedef typename VectorFieldType::Pointer VectorFieldPointer;
  typedef typename VectorFieldType::ConstPointer VectorFieldConstPointer;
  typedef typename VectorFieldType::PixelType VectorType;

  typedef typename TLabelMapImage::PixelType LabelMapPixelType;

  typedef typename itk::ImageRegionConstIterator< VectorFieldType > ConstVectorFieldIteratorType;

  itkGetMacro( SolutionDomainId, int);
  itkSetMacro( SolutionDomainId, int);

  const VectorFieldType* GetVectorField() const;
  void SetVectorField( const VectorFieldType * );

  const TInputImage* GetInitialValueField() const;
  void SetInitialValueField( const TInputImage* );

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

  itkGetMacro( ReverseVectorField, bool );
  itkSetMacro( ReverseVectorField, bool );
  itkBooleanMacro( ReverseVectorField );

  itkSetMacro( OutputSolutionOnBoundary, bool );
  itkGetMacro( OutputSolutionOnBoundary, bool );
  itkBooleanMacro( OutputSolutionOnBoundary );

  itkSetMacro( ComputeDistanceToInterface, bool );
  itkGetMacro( ComputeDistanceToInterface, bool );
  itkBooleanMacro( ComputeDistanceToInterface );

  itkGetMacro( IntegrationConstant, double );
  itkSetMacro( IntegrationConstant, double );

  itkGetMacro( InitialValue, double );
  itkSetMacro( InitialValue, double );

  itkGetMacro( OriginId, int );
  itkSetMacro( OriginId, int );

  /** Extract dimension from input image. */
  itkStaticConstMacro(InputImageDimension, unsigned int,
                      TInputImage::ImageDimension);
  itkStaticConstMacro(OutputImageDimension, unsigned int,
                      TInputImage::ImageDimension);

protected:
  SolveTransportImageFilter();
  ~SolveTransportImageFilter() {};
  void PrintSelf(std::ostream& os, Indent indent) const;

  /** This method defines the metadata of the output image */
  void GenerateOutputInformation();
  
  /** This method performs the operation of copying pixels from the input image
   * into the outpu image. */
  void GenerateData();
  int flowsOut( IndexType );
  void normalizeVector( VectorType & );

private:
  SolveTransportImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  int m_OriginId;
  bool m_ReverseVectorField;
  double m_IntegrationConstant;
  int m_SolutionDomainId;
  double m_InitialValue;
  bool m_OutputSolutionOnBoundary;
  bool m_ComputeDistanceToInterface;

};

  
} // end namespace itk
  
#ifndef ITK_MANUAL_INSTANTIATION
#include "itkSolveTransportImageFilter.hxx"
#endif
  
#endif

