/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkLaplaceFunctionRestrictedDomain.h,v $
  Language:  C++
  Date:      $Date: 2008-01-24 19:25:07 $
  Version:   $Revision: 1.18 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkLaplaceFunctionRestrictedDomain_h_
#define __itkLaplaceFunctionRestrictedDomain_h_

#include "itkFiniteDifferenceFunction.h"
#include "itkImageRegionConstIterator.h"

namespace itk {

/**
 * \class LaplaceFunctionRestrictedDomain
 *
 * This class implements a transport equation

*/

template <class TImage, class TLabelImage>
class ITK_EXPORT LaplaceFunctionRestrictedDomain :
    public FiniteDifferenceFunction<TImage>
{
public:
  /** Standard class typedefs. */
  typedef LaplaceFunctionRestrictedDomain Self;
  typedef FiniteDifferenceFunction<TImage> Superclass;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self> ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods) */
  itkTypeMacro( LaplaceFunctionRestrictedDomain,
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
  typedef typename TImage::SpacingType SpacingType;

  /** Inherit some parameters from the superclass type. */
  itkStaticConstMacro(ImageDimension, unsigned int,Superclass::ImageDimension);

  /** A global data type for this class of equations.  Used to store
   * values that are needed in calculating the time step and other intermediate
   * products such as derivatives that may be used by virtual functions called
   * from ComputeUpdate.  Caching these values here allows the ComputeUpdate
   * function to be const and thread safe.*/

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


  virtual void *GetGlobalDataPointer() const
  {
    return 0;
  }

/** When the finite difference solver filter has finished using a global
   * data pointer, it passes it to this method, which frees the memory.
   * The solver cannot free the memory because it does not know the type
   * to which the pointer points. */
  virtual void ReleaseGlobalDataPointer(void *itkNotUsed(GlobalData)) const
    { }


  /** Compute incremental update. */
  virtual PixelType ComputeUpdate(const NeighborhoodType &neighborhood,
                                  void *itkNotUsed(globalData),
                                  const FloatOffsetType& offset = FloatOffsetType(0.0)
    );

  /** Returns the time step supplied by the user.  We don't need to use the
   * global data supplied since we are returning a fixed value.  */
  virtual TimeStepType ComputeGlobalTimeStep(void *itkNotUsed(GlobalData)) const
  {
    return this->GetTimeStep();
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

  virtual void SetLabelImage( const TLabelImage *LabelImage); 
  const TLabelImage* GetLabelImage(void) { return m_LabelImage; }


protected:
  LaplaceFunctionRestrictedDomain();
  ~LaplaceFunctionRestrictedDomain() {}
  void PrintSelf(std::ostream& os, Indent indent) const
  {
    Superclass::PrintSelf(os,indent);
    os << indent << "TimeStep: " << m_TimeStep << std::endl;
  }
  
private:
  LaplaceFunctionRestrictedDomain(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  /** */
  unsigned long m_Center;
  unsigned long m_Stride[ImageDimension];

  const TLabelImage *m_LabelImage;
  SpacingType m_Spacing;

  TimeStepType m_TimeStep;

  int m_OriginId;
  int m_TargetId;
  int m_SolutionDomainId;
  int m_NeumannId;
};


  
}// end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkLaplaceFunctionRestrictedDomain.hxx"
#endif

#endif
