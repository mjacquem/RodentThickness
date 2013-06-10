/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkSolveLaplacianImageFilter.hxx,v $
  Language:  C++
  Date:      $Date: 2005/09/30 23:10:54 $
  Version:   $Revision: 1.4 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _itkSolveLaplacianImageFilter_txx
#define _itkSolveLaplacianImageFilter_txx

#include "itkSolveLaplacianImageFilter.h"
#include "itkImageRegionIterator.h"

#include <fstream>

#undef _DEBUG_

namespace itk
{

template <class TInputImage, class TOutputImage>
SolveLaplacianImageFilter<TInputImage, TOutputImage>
::SolveLaplacianImageFilter()
{
  this->m_DirichletLowId = 2;
  this->m_DirichletMeanId = 4;
  this->m_DirichletHighId = 3;
  this->m_NeumannId = 1;
  this->m_SolutionDomainId = 5;

  this->m_DirichletLowVal = 0;
  this->m_DirichletMeanVal = 0.5;
  this->m_DirichletHighVal = 1.0;
  this->m_NeumannVal = 0.0;
  this->m_SolutionDomainVal = -1.0;

  this->m_KernelWidth = 3.0;
}

template <class TInputImage, class TOutputImage>
double 
SolveLaplacianImageFilter<TInputImage,TOutputImage>
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


template <class TInputImage, class TOutputImage>
void
SolveLaplacianImageFilter<TInputImage, TOutputImage>
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
  
template <class TInputImage, class TOutputImage>
void
SolveLaplacianImageFilter<TInputImage, TOutputImage>
::GenerateData()
{

  InputImageConstPointer  inputImage  = this->GetInput();
  OutputImagePointer      outputImage = this->GetOutput();

  OutputRegionType  region = outputImage->GetLargestPossibleRegion();

  outputImage->SetRegions( region );
  outputImage->Allocate();


  std::map< int, double > idToTemp;
  idToTemp[ m_NeumannId ] = m_NeumannVal;
  idToTemp[ m_DirichletLowId ] = m_DirichletLowVal;
  idToTemp[ m_DirichletHighId ] = m_DirichletHighVal;
  idToTemp[ m_DirichletMeanId ] = m_DirichletMeanVal;
  idToTemp[ m_SolutionDomainId ] = m_SolutionDomainVal;


  RadiusType radius;
  radius.Fill( 1 );
  ConstInputImageNeighborhoodIteratorType in( radius, inputImage, inputImage->GetLargestPossibleRegion() );

  // first get the number of unknowns and create the index to index map

  std::map< IndexType, int, globalIndxLtFcn > globalIndxToLocalIndx;
  std::map< int, IndexType > localIndxToGlobalIndx;
  std::map< IndexType, int, globalIndxLtFcn > globalIndxToNeumannIndx;
  std::map< IndexType, double*, globalIndxLtFcn > globalIndxToNeumannNormal;

  int iUnknowns = 0;
  int iDirichletUnknowns = 0;
  int iNeumannUnknowns = 0;

  for ( in.GoToBegin(); !in.IsAtEnd(); ++in ) {
    
    if ( in.GetCenterPixel() == m_SolutionDomainId ) {
      
      globalIndxToLocalIndx[ in.GetIndex() ] = iUnknowns;
      localIndxToGlobalIndx[ iUnknowns ] = in.GetIndex();
      iUnknowns++;
      iDirichletUnknowns++;

    }

  }

#ifdef _DEBUG_
  std::cout << "Number of unknowns = " << iUnknowns << std::endl;
#endif

  itk::fem::LinearSystemWrapperVNL eqs;
  eqs.SetSystemOrder( iUnknowns );
  eqs.SetNumberOfSolutions( 1 );
  eqs.SetNumberOfVectors( 1 );
  eqs.SetNumberOfMatrices( 1 );

  // now build the linear system

  eqs.InitializeMatrix( 0 );
  eqs.InitializeVector( 0 );
  eqs.InitializeSolution( 0 );

  // TODO, introduce support for unequal spacing

  const itk::Vector<double, InputImageDimension> spacing = this->GetInput()->GetSpacing();

  itk::Vector<double, InputImageDimension> spacingFactors;
  for ( int iI=0; iI<InputImageDimension; iI++ ) {
    spacingFactors[iI] = 1;
    for ( int iJ=0; iJ<iI; iJ++ ) {
      spacingFactors[iI] *= spacing[iJ];
    }
    for ( int iJ=iI+1; iJ<InputImageDimension; iJ++ ) {
      spacingFactors[iI] *= spacing[iJ];
    }
  }

  double dDiagonalSpacingConstant = 0;
  for ( int iI=0; iI<InputImageDimension; iI++ ) {
    dDiagonalSpacingConstant += spacingFactors[iI];
  }

  /*double dDXS = 1.0;
  double dDYS = 1.0;
  double dDZS = 1.0;*/

  // template<unsigned int VImageDimension = 2> virtual const SpacingType& itk::ImageBase< VImageDimension >::GetSpacing()

  for ( in.GoToBegin(); !in.IsAtEnd(); ++in )
    {
     
      if ( in.GetCenterPixel() == m_SolutionDomainId ) {

	int iIC = globalIndxToLocalIndx[ in.GetIndex() ];

	// let's do the bookkeeping (fill in the entries of the matrix and the RHS vector
	// corresponding to the relation at id iCurrentId), constitutes the iCurrentId's row
	// of the matrix
	// need to make sure (for the solver) that the diagonal element is positive

	// fill in the diagonal value first

	//eqs.SetMatrixValue( iIC, iIC, 2*(dDYS*dDZS+dDXS*dDZS+dDXS*dDYS), 0 );
	eqs.SetMatrixValue( iIC, iIC, 2*(dDiagonalSpacingConstant), 0 );
	
	// now distribute the remaining values on the matrix and the rhs vector

	double dRHS = 0;


	/*  this->SetPixel(this->GetCenterNeighborhoodIndex()
	    + (i * this->GetStride(axis)), v); }*/
  
	for ( int iI=0; iI<InputImageDimension; iI++ ) {

	  // go positive and negative in both directions and see if the value
	  // is known or needs to be solved for

	  IndexType indexM = in.GetIndex();
	  indexM[ iI ]--;

	  IndexType indexP = in.GetIndex();
	  indexP[ iI ]++;

#ifdef _DEBUG_
	  std::cout << "indexM = " << indexM << std::endl;
	  std::cout << "indexP = " << indexP << std::endl;
#endif

	  if ( globalIndxToLocalIndx.count( indexM )>0 ) {
	    int iCIndex = globalIndxToLocalIndx[ indexM ];
	    //eqs.SetMatrixValue( iIC, iCIndex, -dDYS*dDZS, 0 );
	    eqs.SetMatrixValue( iIC, iCIndex, -spacingFactors[iI], 0 );

	  } else if ( in.GetPrevious( iI, 1 ) == m_NeumannId ) {
#ifdef _DEBUG_
	    std::cout << "Found Neumann id." << std::endl;
#endif
	    double dCurrentMatrixValue = eqs.GetMatrixValue( iIC, iIC, 0 );
	    eqs.SetMatrixValue( iIC, iIC, dCurrentMatrixValue-spacingFactors[iI], 0 );

	  } else {
	    double dVal = idToTemp[ (int)round( in.GetPrevious( iI, 1 ) ) ];
	    //dRHS += dDYS*dDZS*dVal;
	    dRHS += spacingFactors[iI]*dVal;
	  }

	  if ( globalIndxToLocalIndx.count( indexP )>0 ) {
	    int iCIndex = globalIndxToLocalIndx[ indexP ];
	    //eqs.SetMatrixValue( iIC, iCIndex, -dDYS*dDZS, 0 );
	    eqs.SetMatrixValue( iIC, iCIndex, -spacingFactors[iI], 0 );

	  } else if ( in.GetNext( iI, 1 ) == m_NeumannId ) {
#ifdef _DEBUG_
	    std::cout << "Found Neumann id." << std::endl;
#endif
	    double dCurrentMatrixValue = eqs.GetMatrixValue( iIC, iIC, 0 );
	    eqs.SetMatrixValue( iIC, iIC, dCurrentMatrixValue-spacingFactors[iI], 0 );


	  } else {
	    double dVal = idToTemp[ (int)round( in.GetNext( iI,1 ) ) ];
	    //dRHS += dDYS*dDZS*dVal;
	    dRHS += spacingFactors[iI]*dVal;
	  }


	}

	eqs.SetVectorValue( iIC, dRHS, 0 );


      }

#ifdef _DEBUG_
      if ( in.GetCenterPixel() != 0 ) {
	std::cout << "found " << in.GetCenterPixel() << " at " << in.GetIndex() << std::endl;
      }
#endif

    }

  std::cout << "done." << std::endl;

  try {

#ifdef _DEBUG_
    std::cout << "is matrix initialized = " << eqs.IsMatrixInitialized( 0 ) << std::endl;
    std::cout.flush();
#endif    

    eqs.Solve();

  }

  catch(  itk::ExceptionObject & exp )
    {
      std::cerr << exp << std::endl;
    }
  
  // first copy the input to the output, translated from id to value

  typedef ImageRegionIterator< TOutputImage > OutputImageRegionIteratorType;
  typedef ImageRegionConstIterator< TInputImage > InputImageRegionConstIteratorType;

  InputImageRegionConstIteratorType   inputItr(  inputImage,  region );
  OutputImageRegionIteratorType       outputItr( outputImage, region );

  inputItr.GoToBegin();
  outputItr.GoToBegin();

  while( ! outputItr.IsAtEnd() )
    {

      double dTemp = idToTemp[ (int)round(inputItr.Get()) ];
      outputItr.Set( dTemp );
      ++inputItr;
      ++outputItr;
    }

  // now write the newly computed values to the output

  for ( unsigned int iI=0; iI<iUnknowns; iI++ ) {

#ifdef _DEBUG_
    std::cout << "sol(" << iI << " ) = " << eqs.GetSolutionValue( iI, 0 ) << std::endl;
    std::cout << "ltg = " << localIndxToGlobalIndx[ iI ] << std::endl;
#endif

    outputImage->SetPixel( localIndxToGlobalIndx[ iI ], eqs.GetSolutionValue( iI, 0 ) );

  }

}

 

/**
 *
 */
template <class TInputImage, class TOutputImage>
void 
SolveLaplacianImageFilter<TInputImage, TOutputImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
}

 

  
} // end namespace itk

#endif
