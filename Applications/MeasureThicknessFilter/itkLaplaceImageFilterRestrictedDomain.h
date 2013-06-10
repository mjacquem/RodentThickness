/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkLaplaceImageFilterRestrictedDomain.h,v $
  Language:  C++
  Date:      $Date: 2006-03-27 17:01:04 $
  Version:   $Revision: 1.31 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkLaplaceImageFilterRestrictedDomain_h_
#define __itkLaplaceImageFilterRestrictedDomain_h_

#include "itkLaplaceFunctionRestrictedDomain.h"
#include "itkMacro.h"
#include "itkDenseFiniteDifferenceImageFilter.h"
#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIterator.h"

#define ITK_LEAN_AND_MEAN

namespace itk {

/**
 * \class LaplaceImageFilterRestrictedDomain
 *
 * this filter transports image information
 * subject to an externally defined flow field
 * */

template <class TInputImage, class TLabelImage, class TOutputImage>
class ITK_EXPORT LaplaceImageFilterRestrictedDomain
  : public DenseFiniteDifferenceImageFilter<TInputImage, TOutputImage>
{
public:
  /** Standard class typedefs. */
  typedef LaplaceImageFilterRestrictedDomain Self;
  typedef DenseFiniteDifferenceImageFilter<TInputImage, TOutputImage> Superclass;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self> ConstPointer;
  typedef typename Superclass::PixelType  PixelType;
  typedef                      PixelType  ScalarValueType;

  typedef typename TInputImage::SpacingType SpacingType;
  typedef typename Superclass::TimeStepType TimeStepType;

  typedef typename itk::ImageRegionConstIterator< TOutputImage > ConstIteratorType;

  /** Extract superclass image dimension. */
  itkStaticConstMacro(ImageDimension, unsigned int,
                      Superclass::ImageDimension);

  /** Standard method for creation through object factory. */
  itkNewMacro(Self);

  /** Run-time information. */
  itkTypeMacro(LaplaceImageFilterRestrictedDomain,
               DenseFiniteDifferenceImageFilter);
  
  itkSetMacro( TerminalNumberOfIterations, unsigned int );
  itkGetMacro( TerminalNumberOfIterations, unsigned int );


  void SetNeumannId( const int &i )
  {
    m_NeumannId = i;
    m_LaplaceFunctionRestrictedDomain->SetNeumannId( i );
  }

  const int &GetNeumannId()
  {
    return m_NeumannId;
  }

  void SetSolutionDomainId( const int &i )
  {
    m_SolutionDomainId = i;
    m_LaplaceFunctionRestrictedDomain->SetSolutionDomainId( i );
  }

  const int &GetSolutionDomainId()
  {
    return m_SolutionDomainId;
  }

  void SetTargetId( const int &i )
  {
    m_TargetId = i;
    m_LaplaceFunctionRestrictedDomain->SetTargetId( i );
  }

  const int &GetTargetId()
  {
    return m_TargetId;
  }

  void SetOriginId( const int &i )
  {
    m_OriginId = i;
    m_LaplaceFunctionRestrictedDomain->SetOriginId( i );
  }

  const int &GetOriginId()
  {
    return m_OriginId;
  }

  void SetTimeStep(const TimeStepType &t)
  { m_TimeStep = t; }

  const TimeStepType &GetTimeStep() const
  { return m_TimeStep; }

  void SetSpacing( const SpacingType st )
  {
    this->m_Spacing = st;
    this->GetDifferenceFunction()->SetSpacing( this->m_Spacing );
  }

  SpacingType & GetSpacing()
  {
    return this->m_Spacing;
  }

  virtual void ApplyUpdate(const TimeStepType& dt);

  virtual void SetLabelImage( const TLabelImage *LabelImage)
  {
    this->SetNthInput(1, const_cast<TLabelImage *>( LabelImage ) );
    m_LaplaceFunctionRestrictedDomain->SetLabelImage( LabelImage );
  }
  
  const TLabelImage* GetLabelImage(void) { return this->m_LaplaceFunctionRestrictedDomain->GetLabelImage(); }


  /** Supplies the halting criteria for this class of filters.  The
   * algorithm will stop after a user-specified number of iterations. */
  virtual bool Halt()
  {

    std::cout << "iteration = " << m_CurrentIteration << std::endl;

    return ( m_NeedsToHalt );
  }
  
  /** Extract superclass information. */
  typedef typename Superclass::UpdateBufferType UpdateBufferType;
  
#ifdef ITK_USE_CONCEPT_CHECKING
  /** Begin concept checking */
  itkConceptMacro(OutputHasNumericTraitsCheck,
    (Concept::HasNumericTraits<typename TOutputImage::PixelType>));
  /** End concept checking */
#endif

protected:
  LaplaceImageFilterRestrictedDomain();
  ~LaplaceImageFilterRestrictedDomain() {}
  void PrintSelf(std::ostream& os, Indent indent) const;

  virtual void InitializeIteration();
  virtual void Initialize();
  void PreInitialize();
  
private:
  LaplaceImageFilterRestrictedDomain(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  TimeStepType m_TimeStep;
  unsigned int m_TerminalNumberOfIterations;
  unsigned int m_CurrentIteration;
  bool m_NeedsToHalt;

  int m_OriginId;
  int m_TargetId;
  int m_SolutionDomainId;
  int m_NeumannId;

  SpacingType m_Spacing;
  LaplaceFunctionRestrictedDomain<UpdateBufferType,TLabelImage> *m_LaplaceFunctionRestrictedDomain;
};

} // end namspace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkLaplaceImageFilterRestrictedDomain.hxx"
#endif

#undef ITK_LEAN_AND_MEAN

#endif
