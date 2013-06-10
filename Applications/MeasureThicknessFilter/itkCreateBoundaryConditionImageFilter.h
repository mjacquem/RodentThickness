/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkSqrtImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2006-03-19 04:36:57 $
  Version:   $Revision: 1.20 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkCreateBoundaryConditionImageFilter_h
#define __itkCreateBoundaryConditionImageFilter_h

#include "itkUnaryFunctorImageFilter.h"
#include "vnl/vnl_math.h"

namespace itk
{
  
/** \class CreateBoundaryConditionImageFilter
 * \brief Computes the vcl_sqrt(x) pixel-wise
 *
 * \ingroup IntensityImageFilters  Multithreaded
 */
namespace Function {  
  
template< class TInput, class TOutput>
class BC
{
public:
  BC() { m_OriginLabel = 1; m_OriginValue = 0; }
  ~BC() {}
  bool operator!=( const BC & ) const
  {
    return false;
  }
  bool operator==( const BC & other ) const
  {
    return !(*this != other);
  }
  inline TOutput operator()( const TInput & A )
  {
    if ( A==m_OriginLabel ) 
      {
      return m_OriginValue;
      }
    else
      {
      return (TOutput)(0);
      }
  }

  void SetOriginLabel( TInput ol ) { m_OriginLabel = ol; };
  TInput GetOriginLabel() { return m_OriginLabel; };
  void SetOriginValue( TOutput ov ) { m_OriginValue = ov; };
  TOutput GetOriginValue() { return m_OriginValue; };

private:
  TInput m_OriginLabel;
  TOutput m_OriginValue;
}; 
}
template <class TInputImage, class TOutputImage>
class ITK_EXPORT CreateBoundaryConditionImageFilter :
    public
UnaryFunctorImageFilter<TInputImage,TOutputImage, 
                        Function::BC< typename TInputImage::PixelType, 
                                        typename TOutputImage::PixelType>   >
{
public:
  /** Standard class typedefs. */
  typedef CreateBoundaryConditionImageFilter  Self;
  typedef UnaryFunctorImageFilter<TInputImage,TOutputImage, 
                                  Function::BC< typename TInputImage::PixelType, 
                                                  typename TOutputImage::PixelType> >  Superclass;
  typedef SmartPointer<Self>   Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  typedef typename TInputImage::PixelType InputPixelType;
  typedef typename TOutputImage::PixelType OutputPixelType;

  itkSetMacro( OriginLabel, InputPixelType );
  itkGetMacro( OriginLabel, InputPixelType );
  itkSetMacro( OriginValue, OutputPixelType );
  itkGetMacro( OriginValue, OutputPixelType );

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  void BeforeThreadedGenerateData() 
  {
    Superclass::BeforeThreadedGenerateData();
    this->GetFunctor().SetOriginLabel( m_OriginLabel );
    this->GetFunctor().SetOriginValue( m_OriginValue );
  };

#ifdef ITK_USE_CONCEPT_CHECKING
  /** Begin concept checking */
  itkConceptMacro(InputConvertibleToDoubleCheck,
                  (Concept::Convertible<typename TInputImage::PixelType, double>));
  itkConceptMacro(DoubleConvertibleToOutputCheck,
                  (Concept::Convertible<double, typename TOutputImage::PixelType>));
  /** End concept checking */
#endif

protected:
  CreateBoundaryConditionImageFilter() {}
  virtual ~CreateBoundaryConditionImageFilter() {}

  InputPixelType m_OriginLabel;
  OutputPixelType m_OriginValue;

private:
  CreateBoundaryConditionImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

};

} // end namespace itk


#endif
