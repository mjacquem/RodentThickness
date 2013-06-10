/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkZeroImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2006-03-30 14:30:37 $
  Version:   $Revision: 1.7 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkZeroImageFilter_h
#define __itkZeroImageFilter_h

#include "itkUnaryFunctorImageFilter.h"
#include "itkConceptChecking.h"

namespace itk
{
  
/** \class ZeroImageFilter
 * \brief Computes the ABS(x) pixel-wise
 * 
 * \ingroup IntensityImageFilters  Multithreaded
 */
namespace Function {  
  
template< class TInput, class TOutput>
class Zero
{
public:
  Zero() {}
  ~Zero() {}
  bool operator!=( const Zero & ) const
  {
    return false;
  }
  bool operator==( const Zero & other ) const
  {
    return !(*this != other);
  }
  inline TOutput operator()( const TInput & A )
  { return (TOutput)( 0 ); }
}; 
}

template <class TInputImage, class TOutputImage>
class ITK_EXPORT ZeroImageFilter :
    public
UnaryFunctorImageFilter<TInputImage,TOutputImage, 
                        Function::Zero< 
  typename TInputImage::PixelType, 
  typename TOutputImage::PixelType>   >
{
public:
  /** Standard class typedefs. */
  typedef ZeroImageFilter  Self;
  typedef UnaryFunctorImageFilter<TInputImage,TOutputImage, 
                                  Function::Zero< typename TInputImage::PixelType, 
                                                 typename TOutputImage::PixelType> >  Superclass;
  typedef SmartPointer<Self>   Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

#ifdef ITK_USE_CONCEPT_CHECKING
  /** Begin concept checking */
  itkConceptMacro(ConvertibleCheck,
    (Concept::Convertible<typename TInputImage::PixelType,
                          typename TOutputImage::PixelType>));
  itkConceptMacro(InputGreaterThanIntCheck,
    (Concept::GreaterThanComparable<typename TInputImage::PixelType, int>));
  /** End concept checking */
#endif

protected:
  ZeroImageFilter() {}
  virtual ~ZeroImageFilter() {}

private:
  ZeroImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

};

} // end namespace itk


#endif
