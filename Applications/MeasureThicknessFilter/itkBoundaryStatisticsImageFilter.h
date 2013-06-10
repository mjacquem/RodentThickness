/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkSolveTransportImageFiltert.h,v $
  Language:  C++
  Date:      $Date: 2005/09/30 17:01:19 $
  Version:   $Revision: 1.1 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkBoundaryStatisticsImageFilter_h
#define __itkBoundaryStatisticsImageFilter_h

#include "itkImageToImageFilter.h"
#include <itkImage.h>
#include <itkImageRegionConstIterator.h>
#include <itkImageFileReader.h>
#include <itkConstNeighborhoodIterator.h>
#include <itkNeighborhoodIterator.h>
#include <iostream>

namespace itk
{

/** \class BoundaryStatisticsImageFilter
 * \brief Computes the solution to Laplace's equation \Delta u = 0
 * to be subsequently used for the definition of distances
 * \ingroup ImageToImageFilter
 */
  template <class TLabelMapImage, class TInputImage>
class ITK_EXPORT BoundaryStatisticsImageFilter:
    public ImageToImageFilter<TInputImage,TInputImage>
{
public:
  /** Standard class typedefs. */
  typedef BoundaryStatisticsImageFilter         Self;
  typedef ImageToImageFilter<TInputImage,TInputImage>  Superclass;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);  

  /** Run-time type information (and related methods). */
  itkTypeMacro(BoundaryStatisticsImageFilter, ImageToImageFilter);

  /** typedefs needed for managing the image regions */
  typedef typename TInputImage::RegionType      RegionType;
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
  
  typedef typename TLabelMapImage::PixelType LabelMapPixelType;
  typedef typename TInputImage::PixelType PixelType;

  itkGetMacro( SolutionDomainId, int);
  itkSetMacro( SolutionDomainId, int);

  itkGetMacro( DirichletLowId, int);
  itkSetMacro( DirichletLowId, int);

  itkGetMacro( DirichletHighId, int);
  itkSetMacro( DirichletHighId, int);

  itkGetMacro( Mean, double );
  itkGetMacro( Std, double );
  itkGetMacro( Min, double );
  itkGetMacro( Max, double );
  itkGetMacro( Volume, double );
  itkGetMacro( NumberOfVolumeVoxels, long int );
  itkGetMacro( NumberOfSurfaceVoxels, long int );

  const TLabelMapImage* GetLabelMapImage() const;
  void SetLabelMapImage( const TLabelMapImage* );

  /** Extract dimension from input image. */
  itkStaticConstMacro(InputImageDimension, unsigned int,
                      TInputImage::ImageDimension);
  itkStaticConstMacro(OutputImageDimension, unsigned int,
                      TInputImage::ImageDimension);

  const std::vector<double>& GetStatisticsVector() const;

protected:
  BoundaryStatisticsImageFilter();
  ~BoundaryStatisticsImageFilter() {};
  void PrintSelf(std::ostream& os, Indent indent) const;

  /** This method defines the metadata of the output image */
  void GenerateOutputInformation();

  /** This method performs the operation of copying pixels from the input image
   * into the outpu image. */
  void GenerateData();

private:
  BoundaryStatisticsImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  int m_SolutionDomainId;
  int m_DirichletLowId;
  int m_DirichletHighId;

  double m_Mean;
  double m_Std;
  double m_Max;
  double m_Min;
  double m_Volume;
  long int m_NumberOfSurfaceVoxels;
  long int m_NumberOfVolumeVoxels;

  std::vector<double> vStatisticsValues;

};

  
} // end namespace itk
  
#ifndef ITK_MANUAL_INSTANTIATION
#include "itkBoundaryStatisticsImageFilter.hxx"
#endif
  
#endif

