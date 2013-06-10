/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkTransportImageFilterRestrictedDomain.hxx,v $
  Language:  C++
  Date:      $Date: 2006-03-19 04:36:55 $
  Version:   $Revision: 1.28 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkTransportImageFilterRestrictedDomain_txx_
#define __itkTransportImageFilterRestrictedDomain_txx_

#include "itkTransportImageFilterRestrictedDomain.h"

namespace itk
{

/**
 * Constructor
 */
template <class TInputImage, class TCovariantVectorImage, class TLabelImage, class TOutputImage>
TransportImageFilterRestrictedDomain<TInputImage, TCovariantVectorImage, TLabelImage, TOutputImage>
::TransportImageFilterRestrictedDomain()
{
  this->SetNumberOfRequiredInputs( 3 );
  typename TransportFunctionRestrictedDomain<UpdateBufferType,TCovariantVectorImage,TLabelImage>::Pointer q
    = TransportFunctionRestrictedDomain<UpdateBufferType,TCovariantVectorImage,TLabelImage>::New();
  for ( int i=0; i<ImageDimension; i++ ) m_Spacing[i] = 1;
  this->SetDifferenceFunction(q);
  this->SetNumberOfIterations(1);

  m_TimeStep = 0.25; // don't step at all until we know otherwise
  m_CurrentTime = 0;
  m_TerminalTimeFactor = 1;
  m_NeedsToHalt = false;
  m_TransportFunctionRestrictedDomain = q;

  m_OriginId = 1;
  m_IntegrationConstant = 1; // computes distance, 0: just transports.
  m_SolutionDomainId = 5;
  m_NeumannId = 3;
}

/** Prepare for the iteration process. */
template <class TInputImage, class TCovariantVectorImage, class TLabelImage, class TOutputImage>
void
TransportImageFilterRestrictedDomain<TInputImage, TCovariantVectorImage, TLabelImage, TOutputImage>
::Initialize()
{
  this->PreInitialize();
  Superclass::Initialize();
}

/** Prepare for the iteration process. */
template <class TInputImage, class TCovariantVectorImage, class TLabelImage, class TOutputImage>
void
TransportImageFilterRestrictedDomain<TInputImage, TCovariantVectorImage, TLabelImage, TOutputImage>
::PreInitialize()
{
  TransportFunctionRestrictedDomain<UpdateBufferType, TCovariantVectorImage, TLabelImage> *f = 
    dynamic_cast<TransportFunctionRestrictedDomain<UpdateBufferType, TCovariantVectorImage, TLabelImage> *>
    (this->GetDifferenceFunction().GetPointer());
  if (! f)
    {
    throw ExceptionObject(__FILE__, __LINE__, "Transport function is not set.", ITK_LOCATION);
    }
  
  // Check the timestep for stability
  double minSpacing;
  if (this->GetUseImageSpacing())
    {
    minSpacing = this->GetInput()->GetSpacing()[0];
    for (unsigned int i = 1; i < ImageDimension; i++)
      {
      if (this->GetInput()->GetSpacing()[i] < minSpacing)
        {
        minSpacing = this->GetInput()->GetSpacing()[i];
        }
      }
    
    }
  else
    {
    minSpacing = 1.0;
    }

  m_TerminalTimeFactor = m_TerminalTimeFactor*minSpacing;

  std::cout << "terminalTimeFactor = " << m_TerminalTimeFactor << std::endl;

  if ( this->GetUseImageSpacing() ) {
    f->SetSpacing( this->GetInput()->GetSpacing() );
  }
  f->AdaptiveTimeStepOn();  // adjust time step based on the velocity field

  std::cout << "Spacing : " << f->GetSpacing()[0] << std::endl;

}

/** Prepare for the iteration process. */
template <class TInputImage, class TCovariantVectorImage, class TLabelImage, class TOutputImage>
void
TransportImageFilterRestrictedDomain<TInputImage, TCovariantVectorImage, TLabelImage, TOutputImage>
::InitializeIteration()
{

  TransportFunctionRestrictedDomain<UpdateBufferType, TCovariantVectorImage, TLabelImage> *f = 
    dynamic_cast<TransportFunctionRestrictedDomain<UpdateBufferType, TCovariantVectorImage, TLabelImage> *>
    (this->GetDifferenceFunction().GetPointer());
  if (! f)
    {
    throw ExceptionObject(__FILE__, __LINE__, "Transport function is not set.", ITK_LOCATION);
    }
  
  f->InitializeIteration();

  if (this->GetNumberOfIterations() != 0)
    {
    this->UpdateProgress(((float)(this->m_CurrentTime))
                         /((float)(this->m_TerminalTime)));
    }
  else
    {
    this->UpdateProgress(0);
    }
}

template <class TInputImage, class TCovariantVectorImage, class TLabelImage, class TOutputImage>
void
TransportImageFilterRestrictedDomain<TInputImage, TCovariantVectorImage, TLabelImage, TOutputImage>
::ApplyUpdate(const TimeStepType& dt)
{

  // first lets see if we really want to make a step that large
    TimeStepType dtx = dt;
  if ( m_CurrentTime + dt > m_TerminalTime*m_TerminalTimeFactor )
    {
    dtx = m_TerminalTimeFactor*m_TerminalTime-m_CurrentTime;
    m_NeedsToHalt = true;
    }

  m_CurrentTime += dtx;

  // now do the actual update
  Superclass::ApplyUpdate( dtx );

}

template <class TInputImage, class TCovariantVectorImage, class TLabelImage, class TOutputImage>
void
TransportImageFilterRestrictedDomain<TInputImage, TCovariantVectorImage, TLabelImage, TOutputImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os, indent.GetNextIndent());
  os << indent << "TimeStep: " << m_TimeStep << std::endl;
  os << indent << "TerminalTime: " << m_TerminalTime << std::endl;
  os << indent << "TerminalTimeFactor: " << m_TerminalTimeFactor << std::endl;
  os << indent << "CurrentTime: " << m_CurrentTime << std::endl;
}

} // end namespace itk

#endif
