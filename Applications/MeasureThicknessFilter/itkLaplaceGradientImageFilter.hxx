/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkLaplaceGradientImageFilter.hxx,v $
  Language:  C++
  Date:      $Date: 2006/01/11 19:43:31 $
  Version:   $Revision: 1.21 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _itkLaplaceGradientImageFilter_txx
#define _itkLaplaceGradientImageFilter_txx
#include "itkLaplaceGradientImageFilter.h"

#include "itkConstNeighborhoodIterator.h"
#include "itkNeighborhoodInnerProduct.h"
#include "itkImageRegionIterator.h"
#include "itkDerivativeOperator.h"
#include "itkNeighborhoodAlgorithm.h"
#include "itkZeroFluxNeumannBoundaryCondition.h"
#include "itkOffset.h"
#include "itkProgressReporter.h"

#undef _DEBUG_

namespace itk
{

/*
 * Set the domain image.
 */
template <class TLabelMapImage, class TInputImage, class TOperatorValueType, class TOutputValueType>
void 
LaplaceGradientImageFilter<TLabelMapImage, TInputImage, TOperatorValueType, TOutputValueType>
::SetDomainImage(
  LabelMapImageConstPointer ptr )
{

  m_LabelMapIsSet = 1;
  m_LabelMapImage = ptr;

  //this->ProcessObject::SetNthInput( 1, const_cast< LabelMapImageType * >( ptr ) );
}


/*
 * Get the domain image.
 */

template <class TLabelMapImage, class TInputImage, class TOperatorValueType, class TOutputValueType>
typename TLabelMapImage::ConstPointer
LaplaceGradientImageFilter<TLabelMapImage, TInputImage, TOperatorValueType, TOutputValueType>
::GetDomainImage() const
{
  /*return dynamic_cast< const LabelMapImageType * >
    ( this->ProcessObject::GetInput( 1 ) );*/
  return m_LabelMapImage;
}

template <class TLabelMapImage, class TInputImage, class TOperatorValueType, class TOutputValueType>
LaplaceGradientImageFilter<TLabelMapImage, TInputImage, TOperatorValueType, TOutputValueType>
::LaplaceGradientImageFilter()
{
  this->SetNumberOfRequiredInputs(1);
  m_UseImageSpacing = true;

  this->m_DirichletLowId = 2;
  this->m_DirichletMeanId = 4;
  this->m_DirichletHighId = 3;
  this->m_NeumannId = 1;
  this->m_SolutionDomainId = 5;

  this->m_LabelMapImage = NULL;
  this->m_LabelMapIsSet = 0;

}
int roundd(double d)
{
      return static_cast<int>(d + 0.5);
}
template <class TLabelMapImage, class TInputImage, class TOperatorValueType, class TOutputValueType >
void
LaplaceGradientImageFilter< TLabelMapImage, TInputImage, TOperatorValueType, TOutputValueType >
::GenerateOutputInformation() 
{
  Superclass::GenerateOutputInformation();

  InputImageConstPointer  inputImage  = this->GetInput();
  OutputImagePointer      outputImage = this->GetOutput();

  if ( InputImageDimension!=3 ) {
    std::cerr << "Can probably only handle datasets of dimension 3" << std::endl;
  }

  outputImage->SetLargestPossibleRegion( inputImage->GetLargestPossibleRegion() );

  outputImage->SetSpacing(     inputImage->GetSpacing()    );
  outputImage->SetOrigin(      inputImage->GetOrigin()     );
  outputImage->SetDirection(   inputImage->GetDirection()  );

}

template< class TLabelMapImage, class TInputImage, class TOperatorValueType, class TOutputValueType >
void
LaplaceGradientImageFilter< TLabelMapImage, TInputImage, TOperatorValueType, TOutputValueType >
::GenerateData()
{

  InputImageConstPointer    laplaceSolution = this->GetInput();
  LabelMapImageConstPointer domainImage = this->GetDomainImage();
  
  if ( !m_LabelMapIsSet ) {
    std::cerr << "Domain image is not set. Aborting." << std::endl;
    exit(-1);
  }

  OutputImagePointer        outputImage = this->GetOutput();

  OutputImageRegionType region = outputImage->GetLargestPossibleRegion();
  
  outputImage->SetRegions( region );
  outputImage->Allocate();

  OutputPixelType a;
  OutputPixelType zeroPixelType;

  for ( unsigned int iI=0; iI<InputImageDimension; iI++ ) {
    zeroPixelType[iI] = 0;
  }

  ZeroFluxNeumannBoundaryCondition<InputImageType> nbc;

  ConstNeighborhoodIterator<InputImageType> nit;
  ImageRegionIterator<OutputImageType> it;

  const itk::Vector<double, InputImageDimension> spacing = this->GetInput()->GetSpacing();

  // TODO: have a proper boundary condition if we are at the boundary of the domain

  RadiusType radius;
  radius.Fill( 1 );

  // region should not include the boundary
  // the boundary needs to be handled independently

  typename InputImageType::RegionType regionInput = laplaceSolution->GetLargestPossibleRegion();

  typename InputImageType::SizeType size = regionInput.GetSize();
  typename InputImageType::IndexType index = regionInput.GetIndex();

#ifdef _DEBUG_
  std::cout << "size = ";
  for ( unsigned int iI=0; iI<InputImageDimension; iI++ ) {
    std::cout << size[iI] << " ";
  }
  std::cout << std::endl;

  std::cout << "index = ";
  for ( unsigned int iI=0; iI<InputImageDimension; iI++ ) {
    std::cout << index[iI] << " ";
  }
  std::cout << std::endl;
#endif

  typename InputImageType::IndexType fromIndex = index;
  typename InputImageType::IndexType toIndex;

  for ( unsigned int iI=0; iI<InputImageDimension; iI++ ) {
    toIndex[iI] = fromIndex[iI]+size[iI]-1;
  }

#ifdef _DEBUG_
  std::cout << "from index = ";
  for ( unsigned int iI=0; iI<InputImageDimension; iI++ ) {
    std::cout << fromIndex[iI] << " ";
  }
  std::cout << std::endl;

  std::cout << "to index = ";
  for ( unsigned int iI=0; iI<InputImageDimension; iI++ ) {
    std::cout << toIndex[iI] << " ";
  }
  std::cout << std::endl;
#endif  

  ConstNeighborhoodIteratorType imLS( radius, laplaceSolution, regionInput );
  ConstLabelMapNeighborhoodIteratorType imD( radius, domainImage, regionInput );

  typedef ImageRegionIterator< OutputImageType > ImageRegionIterator;
  //  ImageRegionIterator imO( outputImage, boundaryFreeRegion );
  ImageRegionIterator imO( outputImage, regionInput );

  // iterate through all these regions simultaneously

  for ( imLS.GoToBegin(), imD.GoToBegin(), imO.GoToBegin(); !( imLS.IsAtEnd() || imD.IsAtEnd() || imO.IsAtEnd() ); ++imLS, ++imD, ++imO ) {

    // go through all the dimensions

    int iCenterId = (int)(roundd(imD.GetCenterPixel()));

    typename InputImageType::IndexType currentIndex = imD.GetIndex();

    //if ( iCenterId>0 ) {
    if ( iCenterId>0 && iCenterId!=m_NeumannId ) {

      double dCenterVal = imLS.GetCenterPixel();

#ifdef _DEBUG_
      std::cout << "Computing gradient at " << currentIndex << "; ";
      std::cout << std::endl;
      std::cout.flush();
#endif

      for ( unsigned int iI=0; iI<InputImageDimension; iI++ ) {
	
	// figure out if we can compute central differences or not
      
	int iPId = (int)(roundd(imD.GetNext( iI, 1 ) ) );
	int iNId = (int)(roundd(imD.GetPrevious( iI, 1 ) ) );

	// let's see if we are at the boundary
	// and if so, disable the use of values across the boundary

	if ( (currentIndex[iI]<=fromIndex[iI]) || (iNId==m_NeumannId) ) iNId = 0;
	if ( (currentIndex[iI]>=toIndex[iI]) || (iPId==m_NeumannId) ) iPId = 0;

	if ( (currentIndex[iI]<=fromIndex[iI]) ) iNId = 0;
	if ( (currentIndex[iI]>=toIndex[iI]) ) iPId = 0;

	double dPVal = 0;
	double dNVal = 0;

	if ( iPId>0 ) dPVal = imLS.GetNext( iI, 1 );
	if ( iNId>0 ) dNVal = imLS.GetPrevious( iI, 1 );

#ifdef _DEBUG_
	std::cout << " iPId(" << iI << ") = " << iPId << "; iNId(" << iI << ") = " << iNId;
#endif

	// central differences are only possible if all the ids are larger than 0
	// otherwise one-sided differences need to be employed

	if ( iPId>0 && iNId>0 ) { // use central differences

	  a[iI] = (dPVal-dNVal)/(2.0*spacing[iI]);

	} else if ( iPId>0 ) { // forward difference
	  
	  a[iI] = (dPVal-dCenterVal)/spacing[iI];

	} else if ( iNId>0 ) { // backward difference

	  a[iI] = (dCenterVal-dNVal)/spacing[iI];

	} else { // 0,0
	  
	  a[iI] = 0.0;

	}

      }

#ifdef _DEBUG_
      std::cout << std::endl;
#endif


#ifdef _DEBUG_
      // print out gradient that was found
      
      std::cout << "gradient = ";
      for ( unsigned int iI=0; iI<InputImageDimension; iI++ ) {
	std::cout << a[iI] << " ";
      }

      std::cout << " at " << currentIndex;

      std::cout << std::endl;

#endif

      imO.Set( a );

    } else { // put in the zero vector

      imO.Set( zeroPixelType );

    }

  }

}

/**
 * Standard "PrintSelf" method
 */
  template< class TLabelMapImage, class TInputImage, class TOperatorValueType, class TOutputValueType>
void
  LaplaceGradientImageFilter< TLabelMapImage, TInputImage, TOperatorValueType, TOutputValueType >
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf( os, indent );

  os << indent << "UseImageSpacing: " 
     << (m_UseImageSpacing ? "On" : "Off") << std::endl;
}

} // end namespace itk


#endif
