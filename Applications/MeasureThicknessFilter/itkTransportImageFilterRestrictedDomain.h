/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkTransportImageFilterRestrictedDomain.h,v $
  Language:  C++
  Date:      $Date: 2006-03-27 17:01:04 $
  Version:   $Revision: 1.31 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkTransportImageFilterRestrictedDomain_h_
#define __itkTransportImageFilterRestrictedDomain_h_

#include "itkTransportFunctionRestrictedDomain.h"
#include "itkMacro.h"
#include "./itkDenseFiniteDifferenceImageFilter.h"
#include "itkImageRegionConstIterator.h"

#define ITK_LEAN_AND_MEAN

namespace itk {

/**
 * \class TransportImageFilterRestrictedDomain
 *
 * this filter transports image information
 * subject to an externally defined flow field
 * */

template <class TInputImage, class TCovariantVectorImage, class TLabelImage, class TOutputImage>
class ITK_EXPORT TransportImageFilterRestrictedDomain
  : public DenseFiniteDifferenceImageFilter<TInputImage, TOutputImage>
{
public:
  /** Standard class typedefs. */
  typedef TransportImageFilterRestrictedDomain Self;
  typedef DenseFiniteDifferenceImageFilter<TInputImage, TOutputImage> Superclass;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self> ConstPointer;
  typedef typename Superclass::PixelType  PixelType;
  typedef                      PixelType  ScalarValueType;
  typedef typename TCovariantVectorImage::PixelType VectorPixelType;

  typedef typename TInputImage::SpacingType SpacingType;
  typedef typename Superclass::TimeStepType TimeStepType;

  typedef typename itk::ImageRegionConstIterator< TOutputImage > ConstIteratorType;

  /** Extract superclass image dimension. */
  itkStaticConstMacro(ImageDimension, unsigned int,
                      Superclass::ImageDimension);

  /** Standard method for creation through object factory. */
  itkNewMacro(Self);

  /** Run-time information. */
  itkTypeMacro(TransportImageFilterRestrictedDomain,
               DenseFiniteDifferenceImageFilter);
  
  itkSetMacro( TerminalTime, TimeStepType );
  itkGetMacro( TerminalTime, TimeStepType );

  void SetIntegrationConstant( const ScalarValueType &v )
  {
    m_IntegrationConstant = v;
    m_TransportFunctionRestrictedDomain->SetIntegrationConstant( v );
  }

  const ScalarValueType &GetIntegrationConstant()
  {
    return m_IntegrationConstant;
  }

  void SetNeumannId( const int &i )
  {
    m_NeumannId = i;
    m_TransportFunctionRestrictedDomain->SetNeumannId( i );
  }

  const int &GetNeumannId()
  {
    return m_NeumannId;
  }

  void SetSolutionDomainId( const int &i )
  {
    m_SolutionDomainId = i;
    m_TransportFunctionRestrictedDomain->SetSolutionDomainId( i );
  }

  const int &GetSolutionDomainId()
  {
    return m_SolutionDomainId;
  }

  void SetTargetId( const int &i )
  {
    m_TargetId = i;
    m_TransportFunctionRestrictedDomain->SetTargetId( i );
  }

  const int &GetTargetId()
  {
    return m_TargetId;
  }

  void SetOriginId( const int &i )
  {
    m_OriginId = i;
    m_TransportFunctionRestrictedDomain->SetOriginId( i );
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

  void SetAdaptiveTimeStep(const bool &t)
  { m_TransportFunctionRestrictedDomain->SetAdaptiveTimeStep( t ); }

  const TimeStepType &GetAdaptiveTimeStep() const
  { return m_TransportFunctionRestrictedDomain->GetAdaptiveTimeStep(); }

  void AdaptiveTimeStepOn()
  { m_TransportFunctionRestrictedDomain->AdaptiveTimeStepOn(); }

  void AdaptiveTimeStepOff()
  { m_TransportFunctionRestrictedDomain->AdaptiveTimeStepOff(); }

  virtual void ApplyUpdate(const TimeStepType& dt);

  virtual void SetFlowField( const TCovariantVectorImage *FlowField) 
  { 
    this->SetNthInput(1, const_cast<TCovariantVectorImage *>( FlowField )); 
    m_TransportFunctionRestrictedDomain->SetFlowField( FlowField ); 
  }

  const TCovariantVectorImage* GetFlowField(void) { return this->m_TransportFunctionRestrictedDomain->GetFlowField(); }
  virtual void SetLabelImage( const TLabelImage *LabelImage)
  {
    this->SetNthInput(2, const_cast<TLabelImage *>( LabelImage ) );
    m_TransportFunctionRestrictedDomain->SetLabelImage( LabelImage );
  }
  
  const TLabelImage* GetLabelImage(void) { return this->m_TransportFunctionRestrictedDomain->GetLabelImage(); }


  /** Supplies the halting criteria for this class of filters.  The
   * algorithm will stop after a user-specified number of iterations. */
  virtual bool Halt()
  {

    std::cout << "time = " << m_CurrentTime << std::endl;

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
  TransportImageFilterRestrictedDomain();
  ~TransportImageFilterRestrictedDomain() {}
  void PrintSelf(std::ostream& os, Indent indent) const;

  virtual void Initialize();
  void PreInitialize();
  virtual void InitializeIteration();
  //void BeforeThreadedGenerateData();
  
private:
  TransportImageFilterRestrictedDomain(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  TimeStepType m_TimeStep;
  TimeStepType m_TerminalTime;
  TimeStepType m_CurrentTime;
  TimeStepType m_TerminalTimeFactor;
  bool m_NeedsToHalt;

  int m_OriginId;
  int m_TargetId;
  double m_IntegrationConstant;
  int m_SolutionDomainId;
  int m_NeumannId;

  SpacingType m_Spacing;
  TransportFunctionRestrictedDomain<UpdateBufferType,TCovariantVectorImage,TLabelImage> *m_TransportFunctionRestrictedDomain;
};

} // end namspace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkTransportImageFilterRestrictedDomain.hxx"
#endif

#undef ITK_LEAN_AND_MEAN

#endif
