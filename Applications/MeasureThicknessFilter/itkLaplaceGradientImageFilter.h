/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkLaplaceGradientImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2003/09/10 14:28:47 $
  Version:   $Revision: 1.14 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkLaplaceGradientImageFilter_h
#define __itkLaplaceGradientImageFilter_h

#include "itkImageToImageFilter.h"
#include "itkImage.h"
#include "itkCovariantVector.h"
#include <itkImageRegionConstIterator.h>
#include <itkImageFileReader.h>
#include <itkConstNeighborhoodIterator.h>
#include <itkNeighborhoodIterator.h>
#include <iostream>
#include <itkFEMLinearSystemWrapperItpack.h>

namespace itk
{
/** \class LaplaceGradientImageFilter
 * \brief Computes the gradient of an image using directional derivatives.
 *
 * Computes the gradient of an image using directional derivatives.
 * The directional derivative at each pixel location is computed by
 * convolution with a first-order derivative operator.
 *
 * The second template parameter defines the value type used in the
 * derivative operator (defaults to float).  The third template
 * parameter defines the value type used for output image (defaults to
 * float).  The output image is defined as a covariant vector image
 * whose value type is specified as this third template parameter.
 *
 *
 * \sa Image
 * \sa Neighborhood
 * \sa NeighborhoodOperator
 * \sa NeighborhoodIterator
 * 
 * \ingroup LaplaceGradientFilters 
 */
  template <class TLabelMapImage, class TInputImage, class TOperatorValueType=double, class TOutputValueType=double>
class ITK_EXPORT LaplaceGradientImageFilter :
    public ImageToImageFilter< TInputImage,
                               Image<CovariantVector<TOutputValueType, TInputImage::ImageDimension>,  TInputImage::ImageDimension> >
{
public:
  /** Extract dimension from input image. */
  itkStaticConstMacro(InputImageDimension, unsigned int,
                      TInputImage::ImageDimension);
  itkStaticConstMacro(OutputImageDimension, unsigned int,
                      TInputImage::ImageDimension);

  /** Standard class typedefs. */
  typedef LaplaceGradientImageFilter Self;

  /** Convenient typedefs for simplifying declarations. */
  typedef TInputImage InputImageType;
  typedef TLabelMapImage LabelMapImageType;
  typedef typename InputImageType::ConstPointer InputImageConstPointer;
  typedef typename LabelMapImageType::ConstPointer LabelMapImageConstPointer;
  typedef typename InputImageType::Pointer InputImagePointer;
  typedef Image<CovariantVector<TOutputValueType, itkGetStaticConstMacro(OutputImageDimension)>,  itkGetStaticConstMacro(OutputImageDimension)> OutputImageType;
  typedef typename OutputImageType::Pointer OutputImagePointer;

  /** Standard class typedefs. */
  typedef ImageToImageFilter< InputImageType, OutputImageType> Superclass;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(LaplaceGradientImageFilter, ImageToImageFilter);
  
  /** Image typedef support. */
  typedef typename InputImageType::PixelType InputPixelType;
  typedef TOperatorValueType OperatorValueType;
  typedef TOutputValueType OutputValueType;
  typedef CovariantVector<OutputValueType, itkGetStaticConstMacro(OutputImageDimension)> OutputPixelType;
  typedef typename OutputImageType::RegionType OutputImageRegionType;
  
  typedef typename itk::ImageRegionConstIterator< InputImageType > ConstIteratorType;
  typedef typename itk::ConstNeighborhoodIterator< InputImageType > ConstNeighborhoodIteratorType;
  typedef typename itk::ConstNeighborhoodIterator< LabelMapImageType > ConstLabelMapNeighborhoodIteratorType;
  typedef typename itk::NeighborhoodIterator< InputImageType > NeighborhoodIteratorType;
  typedef typename ConstNeighborhoodIteratorType::RadiusType RadiusType;


  /** Set the fixed image. */
  void SetDomainImage( LabelMapImageConstPointer );

  /** Get the fixed image. */
  LabelMapImageConstPointer GetDomainImage(void) const;

  /** Use the image spacing information in calculations. Use this option if you
   *  want derivatives in physical space. Default is UseImageSpacingOn. */
  void SetUseImageSpacingOn()
  { this->SetUseImageSpacing(true); }
  
  /** Ignore the image spacing. Use this option if you want derivatives in
      isotropic pixel space.  Default is UseImageSpacingOn. */
  void SetUseImageSpacingOff()
  { this->SetUseImageSpacing(false); }
  
  /** Set/Get whether or not the filter will use the spacing of the input
      image in its calculations */
  itkSetMacro(UseImageSpacing, bool);
  itkGetMacro(UseImageSpacing, bool);

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
  

protected:
  LaplaceGradientImageFilter();

  virtual ~LaplaceGradientImageFilter() {}
  void PrintSelf(std::ostream& os, Indent indent) const;


  void GenerateOutputInformation();
  void GenerateData();


private:
  LaplaceGradientImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  bool m_UseImageSpacing;

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

  int m_LabelMapIsSet;

  LabelMapImageConstPointer m_LabelMapImage;

};
  
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkLaplaceGradientImageFilter.hxx"
#endif

#endif
