/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkTransportFunctionRestrictedDomain.h,v $
  Language:  C++
  Date:      $Date: 2008-01-24 19:25:07 $
  Version:   $Revision: 1.18 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkTransportFunctionRestrictedDomain_h_
#define __itkTransportFunctionRestrictedDomain_h_

#include "itkFiniteDifferenceFunction.h"
#include "itkImageRegionConstIterator.h"

namespace itk {

/**
 * \class TransportFunctionRestrictedDomain
 *
 * This class implements a transport equation

*/

template <class TImage, class TCovariantVectorImage, class TLabelImage>
class ITK_EXPORT TransportFunctionRestrictedDomain :
    public FiniteDifferenceFunction<TImage>
{
public:
  /** Standard class typedefs. */
  typedef TransportFunctionRestrictedDomain Self;
  typedef FiniteDifferenceFunction<TImage> Superclass;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self> ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods) */
  itkTypeMacro( TransportFunctionRestrictedDomain,
                FiniteDifferenceFunction );
  
  /** Inherit some parameters from the superclass type. */
  typedef typename Superclass::ImageType        ImageType;
  typedef typename Superclass::PixelType        PixelType;
  typedef                      PixelType  ScalarValueType;
  typedef typename TLabelImage::PixelType LabelPixelType;
  typedef typename Superclass::TimeStepType     TimeStepType;
  typedef typename Superclass::RadiusType       RadiusType;
  typedef typename Superclass::NeighborhoodType NeighborhoodType;
  typedef typename Superclass::FloatOffsetType  FloatOffsetType;
  typedef typename NeighborhoodType::IndexType  IndexType;
  typedef typename TCovariantVectorImage::PixelType VectorPixelType;
  typedef typename TImage::SpacingType SpacingType;

  typedef typename itk::ImageRegionConstIterator< TCovariantVectorImage > ConstVectorFieldIteratorType;

  /** Inherit some parameters from the superclass type. */
  itkStaticConstMacro(ImageDimension, unsigned int,Superclass::ImageDimension);

  /** A global data type for this class of equations.  Used to store
   * values that are needed in calculating the time step and other intermediate
   * products such as derivatives that may be used by virtual functions called
   * from ComputeUpdate.  Caching these values here allows the ComputeUpdate
   * function to be const and thread safe.*/

  struct GlobalDataStruct
  {
    TimeStepType m_MaxTimeStep;
    TimeStepType m_fac;
  };

  void SetIntegrationConstant( const ScalarValueType &v )
  {
    m_IntegrationConstant = v;
  }

  const ScalarValueType &GetIntegrationConstant()
  {
    return m_IntegrationConstant;
  }

  void SetSolutionDomainId( const int &i )
  {
    m_SolutionDomainId = i;
  }

  const int &GetSolutionDomainId()
  {
    return m_SolutionDomainId;
  }

  void SetNeumannId( const int &i )
  {
    m_NeumannId = i;
  }

  const int &GetNeumannId()
  {
    return m_NeumannId;
  }

  void SetTargetId( const int &i )
  {
    m_TargetId = i;
  }

  const int &GetTargetId()
  {
    return m_TargetId;
  }

  void SetOriginId( const int &i )
  {
    m_OriginId = i;
  }

  const int &GetOriginId()
  {
    return m_OriginId;
  }

  void SetTimeStepFraction(const TimeStepType &t)
  { m_TimeStepFraction = t; }

  const TimeStepType &GetTimeStepFraction() const
  { return m_TimeStepFraction; }

  void SetAdaptiveTimeStep(const bool &t)
  { m_AdaptiveTimeStep = t; }

  const TimeStepType &GetAdaptiveTimeStep() const
  { return m_AdaptiveTimeStep; }

  void AdaptiveTimeStepOn()
  { m_AdaptiveTimeStep = true; }

  void AdaptiveTimeStepOff()
  { m_AdaptiveTimeStep = false; }

  virtual void *GetGlobalDataPointer() const
  {
    GlobalDataStruct *ans = new GlobalDataStruct();
    
    ans->m_MaxTimeStep   = itk::NumericTraits<TimeStepType>::max();
    ans->m_fac = m_TimeStepFraction;
    return ans; 
  }

/** When the finite difference solver filter has finished using a global
   * data pointer, it passes it to this method, which frees the memory.
   * The solver cannot free the memory because it does not know the type
   * to which the pointer points. */
  virtual void ReleaseGlobalDataPointer(void *GlobalData) const
    { delete (GlobalDataStruct *) GlobalData; }


  /** Compute incremental update. */
  virtual PixelType ComputeUpdate(const NeighborhoodType &neighborhood,
                                  void *globalData,
                                  const FloatOffsetType& offset = FloatOffsetType(0.0)
    );

  /** Returns the time step supplied by the user.  We don't need to use the
   * global data supplied since we are returning a fixed value.  */
  virtual TimeStepType ComputeGlobalTimeStep(void *GlobalData) const
  {
    TimeStepType dt;

    GlobalDataStruct *d = (GlobalDataStruct *)GlobalData;

    if ( m_AdaptiveTimeStep )
      {
      dt = d->m_MaxTimeStep;
      } 
    else
      {
      dt = this->GetTimeStep();
      }


    //std::cout << "time step = " << dt << std::endl;


    d->m_MaxTimeStep = itk::NumericTraits<TimeStepType>::max();

    return dt;

  }

  void SetTimeStep(const TimeStepType &t)
  { m_TimeStep = t; }

  const TimeStepType &GetTimeStep() const
  { return m_TimeStep; }

  void SetSpacing( const SpacingType st )
  {
    this->m_Spacing = st;
  }

  SpacingType & GetSpacing()
  {
    return this->m_Spacing;
  }

  /** This method is called prior to each iteration of the solver. */
  virtual void InitializeIteration()
  {
    Superclass::InitializeIteration();
  }

  /** Set/Get the covariant vector image that defines the flow direction */
  virtual void SetFlowField( const TCovariantVectorImage *FlowField); 
  const TCovariantVectorImage* GetFlowField(void) { return m_FlowField; }
  
  virtual void SetLabelImage( const TLabelImage *LabelImage); 
  const TLabelImage* GetLabelImage(void) { return m_LabelImage; }


protected:
  TransportFunctionRestrictedDomain();
  ~TransportFunctionRestrictedDomain() {}
  void PrintSelf(std::ostream& os, Indent indent) const
  {
    Superclass::PrintSelf(os,indent);
    os << indent << "TimeStep: " << m_TimeStep << std::endl;
    os << indent << "TimeStepFraction: " << m_TimeStepFraction << std::endl;
    os << indent << "AdaptiveTimeStep: " << m_AdaptiveTimeStep << std::endl;
  }
  
private:
  TransportFunctionRestrictedDomain(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  /** */
  unsigned long m_Center;
  unsigned long m_Stride[ImageDimension];

  const TCovariantVectorImage *m_FlowField;
  const TLabelImage *m_LabelImage;
  SpacingType m_Spacing;

  TimeStepType m_TimeStep;
  TimeStepType m_TimeStepFraction;

  bool m_AdaptiveTimeStep;

  int m_OriginId;
  int m_TargetId;
  double m_IntegrationConstant;
  int m_SolutionDomainId;
  int m_NeumannId;
};


  
}// end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkTransportFunctionRestrictedDomain.hxx"
#endif

#endif
