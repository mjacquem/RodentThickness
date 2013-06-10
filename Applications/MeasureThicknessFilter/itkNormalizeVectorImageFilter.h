/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkNormalizeVectorImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2006/03/30 14:30:37 $
  Version:   $Revision: 1.7 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkNormalizeVectorImageFilter_h
#define __itkNormalizeVectorImageFilter_h

#include "itkUnaryFunctorImageFilter.h"
#include "itkConceptChecking.h"

namespace itk
{
  
/** \class NormalizeVectorImageFilter
 * \brief Computes the ABS(x) pixel-wise
 * 
 * \ingroup IntensityImageFilters  Multithreaded
 */

namespace Functor {  
  
template< class TInput, class TOutput>
class NormalizeVector
{
public:
  NormalizeVector() {}
  ~NormalizeVector() {}
  bool operator!=( const NormalizeVector & ) const
  {
    return false;
  }
  bool operator==( const NormalizeVector & other ) const
  {
    return !(*this != other);
  }
  inline TOutput operator()( const TInput & A ) const
  {
    typedef typename TOutput::ValueType OutputValueType;

    TOutput value;
    OutputValueType norm = 0;
    
    for ( unsigned int k = 0; k < TOutput::Dimension; k++ )
      {
	norm += static_cast<OutputValueType>( A[k]*A[k] );
      }
    norm = sqrt( norm );

    if ( norm>0 ) 
      {

	for( unsigned int k = 0; k < TOutput::Dimension; k++ )
	  { value[k] = static_cast<OutputValueType>( A[k]/norm ); }
	return value;
      }
    else
      {
	return A;
      }
  }
}; 
}

template <class TInputImage, class TOutputImage>
class ITK_EXPORT NormalizeVectorImageFilter :
    public
UnaryFunctorImageFilter<TInputImage,TOutputImage, 
                        Functor::NormalizeVector< 
  typename TInputImage::PixelType, 
  typename TOutputImage::PixelType>   >
{
public:
  /** Standard class typedefs. */
  typedef NormalizeVectorImageFilter  Self;
  typedef UnaryFunctorImageFilter<TInputImage,TOutputImage, 
                                  Functor::NormalizeVector< typename TInputImage::PixelType, 
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
  /** End concept checking */
#endif

protected:
  NormalizeVectorImageFilter() {}
  virtual ~NormalizeVectorImageFilter() {}

private:
  NormalizeVectorImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

};

} // end namespace itk


#endif
