/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkPropagateBCFilter.hxx,v $
  Language:  C++
  Date:      $Date: 2005/08/08 19:22:20 $
  Version:   $Revision: 1.8 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _itkPropagateBCFilter_txx
#define _itkPropagateBCFilter_txx

#include "itkPropagateBCFilter.h"

#include <itkNeighborhood.h>

#include <map>
#include <vector>
#include <set>

#undef _DEBUG_

namespace itk
{


/**
 * Constructor
 */
  template <typename TInputImage>
PropagateBCFilter<TInputImage>
::PropagateBCFilter()
{

  this->m_DirichletLowId = 2;
  this->m_DirichletMeanId = 4;
  this->m_DirichletHighId = 3;
  this->m_NeumannId = 1;
  this->m_SolutionDomainId = 5;
  this->m_BoundaryConditionToDetermineId = 100;
  this->m_NumberOfIterations = 250;
  this->m_KernelWidth = 3.0;

}

template <class TInputImage>
void
PropagateBCFilter<TInputImage>
::GenerateOutputInformation()
{

  Superclass::GenerateOutputInformation();

  ImageConstPointer inputImage = this->GetInput();
  ImagePointer      outputImage = this->GetOutput();

  outputImage->SetLargestPossibleRegion( inputImage->GetLargestPossibleRegion() );

  outputImage->SetSpacing(     inputImage->GetSpacing()    );
  outputImage->SetOrigin(      inputImage->GetOrigin()     );
  outputImage->SetDirection(   inputImage->GetDirection()  );

}

template <typename TInputImage>
double 
PropagateBCFilter<TInputImage>
::kernelValue( double dR ) {

  double dQ = fabs( dR )/m_KernelWidth;

  double dKernelValue;

  if (dQ<=1) {
    dKernelValue = pow(2-dQ,3)-4*pow(1-dQ,3);
  } else if (dQ<=2) {
    dKernelValue = pow(2-dQ,3);
  } else {
    dKernelValue = 0.0;
  }

  // don't normalize for now

  //dKernelValue /= 4*PI;

  return dKernelValue;

}

/**
 * Compute filter for Gaussian kernel
 */
  template <typename TInputImage>
void
  PropagateBCFilter<TInputImage>
::GenerateData(void)
{

  itkDebugMacro(<< "PropagateBCFilter generating data ");

  ImageConstPointer inputImage = this->GetInput();
  ImagePointer outputImage = this->GetOutput();

  RegionType region = outputImage->GetLargestPossibleRegion();

  outputImage->SetRegions( region );
  outputImage->Allocate();

  DoubleImagePointer bcImage = DoubleImageType::New();
  bcImage->SetRegions( region );
  bcImage->SetLargestPossibleRegion( inputImage->GetLargestPossibleRegion() );
  bcImage->SetSpacing( inputImage->GetSpacing() );
  bcImage->SetOrigin( inputImage->GetOrigin() );
  bcImage->SetDirection( inputImage->GetDirection() );
  bcImage->Allocate();

  // first translate the input to the bcImage

  ImageRegionConstIteratorType inputItr( inputImage, region );
  //ImageRegionIteratorType outputItr( outputImage, region );
  DoubleImageRegionIteratorType bcItr( bcImage, region );

  inputItr.GoToBegin();
  bcItr.GoToBegin();

  while ( ! bcItr.IsAtEnd() ) {

    // translate the values (Neumann: 0, Dirichlet low: 1, Dirichlet high: 2, ignore others

    PixelType iVal = inputItr.Get();

    if ( iVal==m_NeumannId ) {
      bcItr.Set( 0 );
    } else if ( iVal==m_DirichletLowId ) {
      bcItr.Set( 1 );
    } else if ( iVal==m_DirichletHighId ) {
      bcItr.Set( 2 );
    } else {
      bcItr.Set( 0 );
    }

    ++inputItr;
    ++bcItr;

  }

  // find all the voxels that are not BC voxels
  // but are 2d adjacent to a m_SolutionDomainId voxel
  // these are the indices that need to be solved for


  // all the boundary values in an 2^d neighborhood of a solution domain
  // voxel needs to be known


  // go through all the voxels and put the candidate voxels in the vector vDI,
  // i.e., the voxels that have a known BC neighbor (3^d-1), and a (2d) neighbor
  // that belongs to the solution domain

  const itk::Vector<double, ImageDimension> spacing = this->GetInput()->GetSpacing();

  RadiusType radius;
  radius.Fill( 1 );


  std::vector<IndexType> bcSet;

  ImageConstNeighborhoodIteratorType in( radius, inputImage, inputImage->GetLargestPossibleRegion() );

  for ( in.GoToBegin(); !in.IsAtEnd(); ++in ) {

    if ( in.GetCenterPixel() == 0 ) {

      // let's see if this is a candidate voxel or not

      // iterate over the 2d neighborhood to see if there is a solutiondomain voxel

      int iFoundSolutionDomainVoxel = 0;
      int iFoundBCVoxel = 0;

      for ( int iI=0; iI<ImageDimension; iI++ ) {

	PixelType pPrev = in.GetPrevious( iI, 1 );
	PixelType pNext = in.GetNext( iI, 1 );

	if ( pPrev==m_SolutionDomainId || pNext==m_SolutionDomainId ) {

	  iFoundSolutionDomainVoxel = 1;
	  break;
	}

      }  // for ( int iI=...

      if ( iFoundSolutionDomainVoxel ) {

	// add it to the list of bc to be computed
	// and set it in the bcImage

	IndexType currentIndex = in.GetIndex();

	bcSet.push_back( currentIndex );
	bcImage->SetPixel( currentIndex, 0.5 );

      }


    } // if ( in.GetCenter

  } // for ( in.GoToBegin()

  // now let's find the indices for every element in bcSet that the averaging needs to be computed over
  // (this is performed within a 3^d-1 connected setting

  std::vector< std::vector<IndexType>* > averagingIndices;
  std::vector< std::vector<double>* > averagingWeights;
  typename std::vector< IndexType >::iterator bcIter;

  //RadiusType radius;
  //radius.Fill( 1 );

  SizeType size;
  for ( int iI=0; iI<ImageDimension; iI++ ) {
    size[iI] = 3;
  }

  RegionType desiredRegion;
  desiredRegion.SetSize( size );

  for ( bcIter=bcSet.begin(); bcIter!=bcSet.end(); bcIter++ ) {

    std::vector<IndexType>* iterationIndices = new std::vector<IndexType>;
    std::vector<double>* weights = new std::vector<double>;
    // push back the current index
    IndexType currentIndex = *bcIter;
    //iterationIndices->push_back( currentIndex ); // (is already included in search loop below)
    // now push back all the 3^d-1 neighbors that should be used

#ifdef _DEBUG_    
    std::cout << currentIndex << " : ";
#endif

    IndexType start;
    for ( int iI=0; iI<ImageDimension; iI++ ) {
      start[iI] = currentIndex[iI]-1;
    }

    desiredRegion.SetIndex( start );

    ImageRegionConstIteratorType imItr( inputImage, desiredRegion );
    DoubleImageRegionConstIteratorType bcItr( bcImage, desiredRegion );

    // go through the region

    int iNumberSearched = 0;
    int iNumberFound = 0;
    double dSum = 0;
    double dSumOfWeights = 0;

    for ( imItr.GoToBegin(), bcItr.GoToBegin(); !(imItr.IsAtEnd() || bcItr.IsAtEnd() ); ++imItr, ++bcItr ) {
      
      iNumberSearched++;

      // check if it is Neumann, or Dirichlet in imItr or if a value is set in bcItr

      PixelType cPixel = imItr.Get();
      double currentBCImageVal = bcItr.Get();

      // let's not take into account Neumann voxels, because they may skew the result, only allow newly
      // created low or high Dirichlet values
      //if ( cPixel==m_DirichletLowId || cPixel==m_DirichletHighId || cPixel==m_NeumannId || (currentBCImageVal>0) ) {
      if ( cPixel==m_DirichletLowId || cPixel==m_DirichletHighId || (currentBCImageVal>0) ) {

	iNumberFound++;

	IndexType candidateIndex = imItr.GetIndex();
	iterationIndices->push_back( candidateIndex );
#ifdef _DEBUG_
	std::cout << imItr.GetIndex() << " ";
#endif

	dSum+= currentBCImageVal;

	// compute radius (with respect to the currentIndex )

	double dR = 0;

	for ( int iK=0; iK<ImageDimension; iK++ ) {
	  dR += pow((candidateIndex[iK]-currentIndex[iK])*spacing[iK],2);
	}
	dR = sqrt(dR);
       
	double dKernelValue = kernelValue( dR );
	weights->push_back( dKernelValue );

	dSumOfWeights += dKernelValue;

      }

    }
    
#ifdef _DEBUG_
    std::cout << "; number searched = " << iNumberSearched << "; number found = " << iNumberFound << "; dSum = " << dSum << "; dSumOfWeights = " << dSumOfWeights << std::endl;
#endif

    // go through the weights and normalize them

    for ( int iK=0; iK<iNumberFound; iK++ ) {

      (*weights)[iK]/= dSumOfWeights;

    }

    averagingIndices.push_back( iterationIndices );
    averagingWeights.push_back( weights );

  }

  // now do the iterations

  std::vector< double > vInc;
  int iNumberOfBCToDetermine = averagingIndices.size();
  vInc.resize( iNumberOfBCToDetermine );

  for ( int iI=0; iI<m_NumberOfIterations; iI++ ) {

#ifdef _DEBUG_
    std::cout << "iteration " << iI << "\r";
#endif

    for ( int iJ=0; iJ<iNumberOfBCToDetermine; iJ++ ) {

      // compute the average

      double dCurrentAverage = 0;

      std::vector< IndexType >* pvAveValues = averagingIndices[iJ];
      std::vector< double >* pvAveWeights = averagingWeights[iJ];
      int iAveragingVoxels = pvAveValues->size();

      for ( int iK=0; iK<iAveragingVoxels; iK++ ) {

	IndexType currentAveragingIndex = (*pvAveValues)[iK];

	double dCurrentVal = bcImage->GetPixel( currentAveragingIndex );
	double dCurrentAveragingWeight = (*pvAveWeights)[iK];

	dCurrentAverage += dCurrentVal*dCurrentAveragingWeight;

      }

      vInc[iJ] = dCurrentAverage;

    }

    // now write these values back into the bcImage

    for ( int iJ=0; iJ<iNumberOfBCToDetermine; iJ++ ) {

      bcImage->SetPixel( bcSet[iJ], vInc[iJ] );

    }

  }

#ifdef _DEBUG_
  std::cout << std::endl;
#endif

  // clear the averaging indices memory

  for ( int iI=0; iI<iNumberOfBCToDetermine; iI++ ) {
    delete averagingIndices[iI];
    delete averagingWeights[iI];
  }

  // now write everything to the output

  // first copy the input to the output

  ImageRegionIteratorType outputItr( outputImage, region );

  inputItr.GoToBegin();
  outputItr.GoToBegin();

  while ( ! outputItr.IsAtEnd() ) {

    outputItr.Set( inputItr.Get() );
    ++inputItr;
    ++outputItr;

  }

  // now put in the additionally computed boundary conditions

  int iNewNeumannBC = 0;
  int iNewDirichletLowBC = 0;
  int iNewDirichletHighBC = 0;

  for ( int iJ=0; iJ<iNumberOfBCToDetermine; iJ++ ) {

    IndexType currentIndex = bcSet[iJ];
    double dCurrentRoundedValue = roundd(bcImage->GetPixel( currentIndex ));

    //std::cout << bcImage->GetPixel( currentIndex ) << std::endl;

    PixelType iNewId = 0;

    if ( dCurrentRoundedValue==0 ) {
      iNewId = m_NeumannId;
      iNewNeumannBC++;
    } else if ( dCurrentRoundedValue==1) {
      iNewId = m_DirichletLowId;
      iNewDirichletLowBC++;
    } else if ( dCurrentRoundedValue==2) {
      iNewId = m_DirichletHighId;
      iNewDirichletHighBC++;
    } else {
      std::cerr << "Boundary condition interpolation produced an illegal value = " << dCurrentRoundedValue << std::endl;
    }

    outputImage->SetPixel( currentIndex, iNewId );

  }

  //#ifdef _DEBUG_
  std::cout << "Introduced " << iNumberOfBCToDetermine << " new boundary conditions." << std::endl;
  std::cout << "Dirichlet low = " << iNewDirichletLowBC << std::endl;
  std::cout << "Dirichlet high = " << iNewDirichletHighBC << std::endl;
  std::cout << "Neumann = " << iNewNeumannBC << std::endl;
  //#endif

}


template <typename TInputImage>
void
PropagateBCFilter<TInputImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);

}


} // end namespace itk

#endif
