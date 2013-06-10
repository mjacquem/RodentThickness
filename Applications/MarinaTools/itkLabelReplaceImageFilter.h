/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkLabelReplaceImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2009-04-01 14:36:08 $
  Version:   $Revision: 1.8 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkLabelReplaceImageFilter_h
#define __itkLabelReplaceImageFilter_h

#include "itkBinaryFunctorImageFilter.h"
#include "itkNumericTraits.h"


namespace itk
{
  
/** \class LabelReplaceImageFilter
 * \brief Implements a filter that replace given src label with new one
 *
 */
namespace Functor {  
  
template< class TInput1, class TInput2=TInput1, class TOutput=TInput1 >
class Replace
{
private:
  TInput1 _label;

public:
  Replace() : _label(1) {};
  ~Replace() {};
  void SetReplacingLabel(TInput1 label) { _label = label; }
  TInput1  GetReplacingLabel() { return _label; }
  bool operator!=( const Replace & ) const
    {
    return false;
    }
  bool operator==( const Replace & other ) const
    {
    return !(*this != other);
    }
  inline TOutput operator()( const TInput1 & A, const TInput2 & B) const
    {
      return static_cast<TOutput>( B > 0 ? _label : A);
    }
}; 

}
template <class TInputImage1, class TInputImage2=TInputImage1, class TOutputImage=TInputImage1>
class LabelReplaceImageFilter :
    public
BinaryFunctorImageFilter<TInputImage1,TInputImage2,TOutputImage, 
                         Functor::Replace< 
  typename TInputImage1::PixelType, 
  typename TInputImage2::PixelType,
  typename TOutputImage::PixelType>   >


{
public:
  /** Standard class typedefs. */
  typedef LabelReplaceImageFilter                Self;
  typedef Functor::Replace<typename TInputImage1::PixelType, 
          typename TInputImage2::PixelType, 
          typename TOutputImage::PixelType> MyFunctorType;
  typedef BinaryFunctorImageFilter<TInputImage1,TInputImage2,TOutputImage, 
                                   Functor::Replace< 
    typename TInputImage1::PixelType, 
    typename TInputImage2::PixelType,
    typename TOutputImage::PixelType> > Superclass;
  typedef SmartPointer<Self>            Pointer;
  typedef SmartPointer<const Self>      ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Runtime information support. */
  itkTypeMacro(LabelReplaceImageFilter, 
               BinaryFunctorImageFilter);

#ifdef ITK_USE_CONCEPT_CHECKING
  /** Begin concept checking */
  itkConceptMacro(Input1Input2OutputLogicalOperatorsCheck,
    (Concept::LogicalOperators<typename TInputImage1::PixelType,
                               typename TInputImage2::PixelType,
                               typename TOutputImage::PixelType>));
  /** End concept checking */
#endif

protected:
  LabelReplaceImageFilter() {}
  virtual ~LabelReplaceImageFilter() {}

private:
  LabelReplaceImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

};

} // end namespace itk


#endif
