/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkLaplaceImageFilterRestrictedDomain.hxx,v $
  Language:  C++
  Date:      $Date: 2006-03-19 04:36:55 $
  Version:   $Revision: 1.28 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkLaplaceImageFilterRestrictedDomain_txx_
#define __itkLaplaceImageFilterRestrictedDomain_txx_

#include "itkLaplaceImageFilterRestrictedDomain.h"

namespace itk
{

/**
 * Constructor
 */
template <class TInputImage, class TLabelImage, class TOutputImage>
LaplaceImageFilterRestrictedDomain<TInputImage, TLabelImage, TOutputImage>
::LaplaceImageFilterRestrictedDomain()
{
  this->SetNumberOfRequiredInputs( 2 );
  typename LaplaceFunctionRestrictedDomain<UpdateBufferType,TLabelImage>::Pointer q
    = LaplaceFunctionRestrictedDomain<UpdateBufferType,TLabelImage>::New();
  for ( int i=0; i<ImageDimension; i++ ) m_Spacing[i] = 1;
  this->SetDifferenceFunction(q);
  this->SetNumberOfIterations(1);

  m_TimeStep =  0.5 / vcl_pow(2.0, static_cast<double>(ImageDimension));
  m_CurrentIteration = 0;
  m_TerminalNumberOfIterations = 1000;
  m_NeedsToHalt = false;
  m_LaplaceFunctionRestrictedDomain = q;

  m_OriginId = 1;
  m_SolutionDomainId = 5;
  m_NeumannId = 3;
}

template <class TInputImage, class TLabelImage, class TOutputImage>
void
LaplaceImageFilterRestrictedDomain<TInputImage, TLabelImage, TOutputImage>
::Initialize()
{
   this->PreInitialize();
   Superclass::Initialize();
}

template <class TInputImage, class TLabelImage, class TOutputImage>
void
LaplaceImageFilterRestrictedDomain<TInputImage, TLabelImage, TOutputImage>
::PreInitialize()
{
  // initialize the output

  std::cout << "Initializing the output." << std::endl;

  TOutputImage *output = this->GetOutput();

  // set the initial values all to zero
  typedef ImageRegionConstIterator< TLabelImage > LabelImageRegionConstIterator;
  typedef ImageRegionIterator< TOutputImage > OutputImageRegionIterator;
  
  OutputImageRegionIterator outputItr(  output,  output->GetRequestedRegion() );
  const TLabelImage *labelImage = this->GetLabelImage();

  LabelImageRegionConstIterator labelItr( labelImage, output->GetRequestedRegion() );

  int iTargetIds = 0;
  int iNrOfVoxels = 0;

  outputItr.GoToBegin();
  labelItr.GoToBegin();
  while( (! outputItr.IsAtEnd()) && (! labelItr.IsAtEnd()) )
    {
    
    iNrOfVoxels++;

    if ( labelItr.Get()==m_OriginId )
      {
      outputItr.Set( 0 ); 
      }
    else if ( labelItr.Get()==m_TargetId )
      {
      outputItr.Set( 1 );
      iTargetIds++;
      }
    else
      {
      outputItr.Set( 0 );
      }

    ++outputItr;
    ++labelItr;
    }

  std::cout << "set " << iTargetIds << " target ids" << std::endl;
  std::cout << "and looked at " << iNrOfVoxels << " voxels." << std::endl;

  // determine the desired time step

  LaplaceFunctionRestrictedDomain<UpdateBufferType, TLabelImage> *f = 
    dynamic_cast<LaplaceFunctionRestrictedDomain<UpdateBufferType, TLabelImage> *>
    (this->GetDifferenceFunction().GetPointer());
  if (! f)
    {
    throw ExceptionObject(__FILE__, __LINE__, "Laplace function is not set.", ITK_LOCATION);
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
  
  std::cout << "min spacing = " << minSpacing << std::endl;

  TimeStepType maximalAllowedTimeStep = minSpacing*minSpacing / vcl_pow(2.0, static_cast<double>(ImageDimension) );
  m_TimeStep = 0.5*maximalAllowedTimeStep;

  std::cout << "time step = " << m_TimeStep << std::endl;

  f->SetTimeStep(m_TimeStep);
  
  if ( this->GetUseImageSpacing() )
    f->SetSpacing( this->GetInput()->GetSpacing() );

}

/** Prepare for the iteration process. */
template <class TInputImage, class TLabelImage, class TOutputImage>
void
LaplaceImageFilterRestrictedDomain<TInputImage, TLabelImage, TOutputImage>
::InitializeIteration()
{

  LaplaceFunctionRestrictedDomain<UpdateBufferType, TLabelImage> *f = 
    dynamic_cast<LaplaceFunctionRestrictedDomain<UpdateBufferType, TLabelImage> *>
    (this->GetDifferenceFunction().GetPointer());
  if (! f)
    {
    throw ExceptionObject(__FILE__, __LINE__, "Laplace function is not set.", ITK_LOCATION);
    }

  f->InitializeIteration();
  
  if (this->GetNumberOfIterations() != 0)
    {
    this->UpdateProgress(((float)(this->m_CurrentIteration))
                         /((float)(this->m_TerminalNumberOfIterations)));
    }
  else
    {
    this->UpdateProgress(0);
    }
}

template <class TInputImage, class TLabelImage, class TOutputImage>
void
LaplaceImageFilterRestrictedDomain<TInputImage, TLabelImage, TOutputImage>
::ApplyUpdate(const TimeStepType& dt)
{

  // first lets see if we really want to make a step that large

  if ( m_CurrentIteration >= m_TerminalNumberOfIterations )
    {
    m_NeedsToHalt = true;
    }
  else
    {
    m_CurrentIteration += 1;

    // now do the actual update
    Superclass::ApplyUpdate( dt );
    }

}

template <class TInputImage, class TLabelImage, class TOutputImage>
void
LaplaceImageFilterRestrictedDomain<TInputImage, TLabelImage, TOutputImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os, indent.GetNextIndent());
  os << indent << "TimeStep: " << m_TimeStep << std::endl;
  os << indent << "TerminalNumberOfIterations: " << m_TerminalNumberOfIterations << std::endl;
  os << indent << "CurrentIteration: " << m_CurrentIteration << std::endl;
}

} // end namespace itk

#endif
