/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkBoundaryStatisticsImageFilter.hxx,v $
  Language:  C++
  Date:      $Date: 2005/09/30 23:10:54 $
  Version:   $Revision: 1.4 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _itkBoundaryStatisticsImageFilter_txx
#define _itkBoundaryStatisticsImageFilter_txx

#include "itkBoundaryStatisticsImageFilter.h"
#include "itkImageRegionIterator.h"

#undef _DEBUG_

namespace itk
{


template <class TLabelMapImage, class TInputImage>
void 
BoundaryStatisticsImageFilter<TLabelMapImage, TInputImage>
::SetLabelMapImage(
  const TLabelMapImage * ptr )
{
  this->ProcessObject::SetNthInput( 1, const_cast< TLabelMapImage * >( ptr ) );
}

template <class TLabelMapImage, class TInputImage>
const TLabelMapImage *
BoundaryStatisticsImageFilter<TLabelMapImage, TInputImage>
::GetLabelMapImage() const
{
  return dynamic_cast< const TLabelMapImage * >
    ( this->ProcessObject::GetInput( 1 ) );
}

template <class TLabelMapImage, class TInputImage>
const std::vector<double>&
BoundaryStatisticsImageFilter<TLabelMapImage, TInputImage>
::GetStatisticsVector() const
{
  return vStatisticsValues;
}

template <class TLabelMapImage, class TInputImage>
BoundaryStatisticsImageFilter<TLabelMapImage, TInputImage>
::BoundaryStatisticsImageFilter()
{

  this->SetNumberOfRequiredInputs(2);
  this->m_SolutionDomainId = 5;
  this->m_DirichletLowId = 2;
  this->m_DirichletHighId = 3;

  this->m_Mean = 0.0;
  this->m_Std = 0.0;
  this->m_Max = 0.0;
  this->m_Min = 0.0;
  this->m_Volume = 0.0;
  this->m_NumberOfSurfaceVoxels = 0;
  this->m_NumberOfVolumeVoxels = 0;

}

template <class TLabelMapImage, class TInputImage>
void
BoundaryStatisticsImageFilter<TLabelMapImage, TInputImage>
::GenerateOutputInformation()
{
  Superclass::GenerateOutputInformation();

  InputImageConstPointer  inputImage  = this->GetInput();
  OutputImagePointer      outputImage = this->GetOutput();

  outputImage->SetLargestPossibleRegion( inputImage->GetLargestPossibleRegion() );

  outputImage->SetSpacing(     inputImage->GetSpacing()    );
  outputImage->SetOrigin(      inputImage->GetOrigin()     );
  outputImage->SetDirection(   inputImage->GetDirection()  );

}
  
template <class TLabelMapImage, class TInputImage>
void
BoundaryStatisticsImageFilter<TLabelMapImage, TInputImage>
::GenerateData()
{

  LabelMapImageConstPointer  labelMapImage  = this->GetLabelMapImage();
  OutputImagePointer      outputImage = this->GetOutput();
  InputImageConstPointer  inputImage = this->GetInput();

  this->m_Mean = 0;
  double dSquareMean = 0;
  this->m_NumberOfSurfaceVoxels = 0;
  this->m_NumberOfVolumeVoxels = 0;

  vStatisticsValues.clear();

  RegionType  region = outputImage->GetLargestPossibleRegion();

  outputImage->SetRegions( region );
  outputImage->Allocate();

  // set the output to zero

  typedef ImageRegionIterator< TInputImage > ImageRegionIterator;
  ImageRegionIterator outputItr( outputImage, region );

  for ( outputItr.GoToBegin(); !outputItr.IsAtEnd(); ++outputItr ) {
    outputItr.Set( 0 );
  }
  
  // loop over the labelmap and search for solution domain voxels
  // that are adjacent to boundary values
  // put the used values in the output image (so this can easily be used in slicer to compute statistics)

  RadiusType radius;
  radius.Fill( 1 );

  ConstLabelMapNeighborhoodIteratorType inN( radius, labelMapImage, labelMapImage->GetLargestPossibleRegion() );

  for ( inN.GoToBegin(); !inN.IsAtEnd(); ++inN ) {

    LabelMapPixelType cPixel = inN.GetCenterPixel();

    if ( cPixel==m_SolutionDomainId ) {

      this->m_NumberOfVolumeVoxels++;

      // check if there is a 2d neighbor that is a BC id

      IndexType currentIndex = inN.GetIndex();
      PixelType currentVal = inputImage->GetPixel( currentIndex );

      int iFoundDirichletLow = 0;
      int iFoundDirichletHigh = 0;

      for ( int iI=0; iI<InputImageDimension; iI++ ) {

	IndexType indexM = currentIndex;
	indexM[ iI ]--;

	LabelMapPixelType cPixelM = inN.GetPrevious( iI, 1 );

	if ( cPixelM==m_DirichletLowId ) iFoundDirichletLow++;
	if ( cPixelM==m_DirichletHighId ) iFoundDirichletHigh++;

	IndexType indexP = currentIndex;
	indexP[ iI ]++;
	
	LabelMapPixelType cPixelP = inN.GetNext( iI, 1 );

	if ( cPixelP==m_DirichletLowId ) iFoundDirichletLow++;
	if ( cPixelP==m_DirichletHighId ) iFoundDirichletHigh++;

      }

      if ( iFoundDirichletLow && iFoundDirichletHigh ) {

	if ( this->m_NumberOfSurfaceVoxels==0 ) {
	  this->m_Min = currentVal;
	  this->m_Max = currentVal;
	} else {
	  if (this->m_Min>currentVal) this->m_Min = currentVal;
	  if (this->m_Max<currentVal) this->m_Max = currentVal;
	}

	outputImage->SetPixel( currentIndex, currentVal);
	this->m_Mean+= 2*currentVal;
	dSquareMean+= 2*pow(currentVal,2);
	this->m_NumberOfSurfaceVoxels+=2;	

	vStatisticsValues.push_back( currentVal );
	vStatisticsValues.push_back( currentVal );

      } else if ( iFoundDirichletLow || iFoundDirichletHigh ) {

	if ( this->m_NumberOfSurfaceVoxels==0 ) {
	  this->m_Min = currentVal;
	  this->m_Max = currentVal;
	} else {
	  if (this->m_Min>currentVal) this->m_Min = currentVal;
	  if (this->m_Max<currentVal) this->m_Max = currentVal;
	}

	outputImage->SetPixel( currentIndex, currentVal);
	this->m_Mean+= currentVal;
	dSquareMean+= pow(currentVal,2);
	this->m_NumberOfSurfaceVoxels++;	

	vStatisticsValues.push_back( currentVal );

      }

    }

  }

  this->m_Mean/=this->m_NumberOfSurfaceVoxels;
  dSquareMean/=this->m_NumberOfSurfaceVoxels;
  
  double dVariance = dSquareMean-pow(this->m_Mean,2);
  this->m_Std = sqrt(dVariance);
  
  const itk::Vector<double, InputImageDimension> spacing = this->GetInput()->GetSpacing();

  this->m_Volume = this->m_NumberOfVolumeVoxels;

  for ( int iI=0; iI<InputImageDimension; iI++ ) {
    this->m_Volume*=spacing[iI];
  }

}

 

/**
 *
 */
template <class TLabelMapImage, class TInputImage>
void 
BoundaryStatisticsImageFilter<TLabelMapImage, TInputImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
}

 

  
} // end namespace itk

#endif
