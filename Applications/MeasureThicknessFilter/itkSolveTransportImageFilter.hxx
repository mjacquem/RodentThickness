/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkSolveTransportImageFilter.hxx,v $
  Language:  C++
  Date:      $Date: 2005/09/30 23:10:54 $
  Version:   $Revision: 1.4 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _itkSolveTransportImageFilter_txx
#define _itkSolveTransportImageFilter_txx

#include "itkSolveTransportImageFilter.h"
#include "itkImageRegionIterator.h"

#undef _DEBUG_

namespace itk
{


template <class TLabelMapImage, class TInputImage, class TVectorFieldValueType>
void 
SolveTransportImageFilter<TLabelMapImage, TInputImage, TVectorFieldValueType>
::SetVectorField(
  const VectorFieldType * ptr )
{
  this->ProcessObject::SetNthInput( 1, const_cast< VectorFieldType * >( ptr ) );
}

template <class TLabelMapImage, class TInputImage, class TVectorFieldValueType>
const typename SolveTransportImageFilter<TLabelMapImage, TInputImage, TVectorFieldValueType>::VectorFieldType *
SolveTransportImageFilter<TLabelMapImage, TInputImage, TVectorFieldValueType>
::GetVectorField() const
{
  return dynamic_cast< const VectorFieldType * >
    ( this->ProcessObject::GetInput( 1 ) );
}

template <class TLabelMapImage, class TInputImage, class TVectorFieldValueType>
void 
SolveTransportImageFilter<TLabelMapImage, TInputImage, TVectorFieldValueType>
::SetInitialValueField(
  const TInputImage * ptr )
{
  this->ProcessObject::SetNthInput( 2, const_cast< TInputImage * >( ptr ) );
}

template <class TLabelMapImage, class TInputImage, class TVectorFieldValueType>
const TInputImage *
SolveTransportImageFilter<TLabelMapImage, TInputImage, TVectorFieldValueType>
::GetInitialValueField() const
{
  return dynamic_cast< const TInputImage * >
    ( this->ProcessObject::GetInput( 2 ) );
}


template <class TLabelMapImage, class TInputImage, class TVectorFieldValueType>
SolveTransportImageFilter<TLabelMapImage, TInputImage, TVectorFieldValueType>
::SolveTransportImageFilter()
{

  this->SetNumberOfRequiredInputs(2);
  this->m_OriginId = 1;
  this->m_ReverseVectorField = false;
  this->m_IntegrationConstant = 1.0; // computes distance, 0: just transports.
  this->m_SolutionDomainId = 5;
  this->m_InitialValue = 0.0;
  this->m_OutputSolutionOnBoundary = false;
  this->m_ComputeDistanceToInterface = true;

}

template <class TLabelMapImage, class TInputImage, class TVectorFieldValueType>
void
SolveTransportImageFilter<TLabelMapImage, TInputImage, TVectorFieldValueType>
::GenerateOutputInformation()
{
  Superclass::GenerateOutputInformation();

  LabelMapImageConstPointer  labelMapImage  = this->GetInput();
  OutputImagePointer      outputImage = this->GetOutput();

  if ( InputImageDimension!=3 ) {
    std::cerr << "Can probably only handle datasets of dimension 3" << std::endl;
  }

  outputImage->SetLargestPossibleRegion( labelMapImage->GetLargestPossibleRegion() );

  outputImage->SetSpacing(     labelMapImage->GetSpacing()    );
  outputImage->SetOrigin(      labelMapImage->GetOrigin()     );
  outputImage->SetDirection(   labelMapImage->GetDirection()  );

}
  
template <class TLabelMapImage, class TInputImage, class TVectorFieldValueType>
void
SolveTransportImageFilter<TLabelMapImage, TInputImage, TVectorFieldValueType>
::normalizeVector( VectorType &vec ) {

  // first compute the norm

  double dNorm = 0;
  for ( int iI=0; iI<InputImageDimension; iI++ ) {
    dNorm += vec[iI]*vec[iI];
  }

  dNorm = sqrt(dNorm);

  if ( dNorm>0 ) {

    for ( int iI=0; iI<InputImageDimension; iI++ ) {
      vec[iI]/=dNorm;
    }

  }

}


template <class TLabelMapImage, class TInputImage, class TVectorFieldValueType>
int
SolveTransportImageFilter<TLabelMapImage, TInputImage, TVectorFieldValueType>
::flowsOut( IndexType index ) {
  
  // check if this is a voxel which tries to flow outside the solution domain,
  // i.e., its dependencies should be strictly dependent on voxels with id neq 0.

  LabelMapImageConstPointer  labelMapImage  = this->GetInput();
  VectorFieldConstPointer vectorField = this->GetVectorField();

  // get gradient vector

  VectorType v = vectorField->GetPixel( index );

  int iFoundValueNeq0 = 0;

  for ( int iI=0; iI<InputImageDimension; iI++ ) {

    IndexType indexC = index;
    double dV = v[iI];
    if ( m_ReverseVectorField ) dV*=-1;

    // now check if the vector points to a proper id neq 0

    if ( dV>0 ) {
      indexC[ iI ]--;
    }

    if ( dV<0 ) {
      indexC[ iI ]++;
    }

    if ( dV!=0 ) {

      iFoundValueNeq0 = 1;

      if ( labelMapImage->GetPixel( indexC )==0 ){

#ifdef _DEBUG_
	std::cout << "Does not flow out!" << index << std::endl;
#endif

	return 0;
      }

    }

  }

  if ( iFoundValueNeq0 ) {
    return 1;
  } else {
    return 0;
  }

}

template <class TLabelMapImage, class TInputImage, class TVectorFieldValueType>
void
SolveTransportImageFilter<TLabelMapImage, TInputImage, TVectorFieldValueType>
::GenerateData()
{

  LabelMapImageConstPointer  labelMapImage  = this->GetInput();
  VectorFieldConstPointer vectorField = this->GetVectorField();
  OutputImagePointer      outputImage = this->GetOutput();
  InputImageConstPointer  initialValues = this->GetInitialValueField();

  int iInitialValueField = 0;

  if ( initialValues ) {
    iInitialValueField = 1;
  } else {
#ifdef _DEBUG_
    std::cout << "No initial value field given, using initial value constant = " << m_InitialValue << std::endl;
#endif
  }
  
  RegionType  region = outputImage->GetLargestPossibleRegion();

  outputImage->SetRegions( region );
  outputImage->Allocate();


  ConstLabelMapIteratorType in( labelMapImage, labelMapImage->GetLargestPossibleRegion() );

  // first get the number of unknowns and create the index to index map

  std::map< IndexType, int, globalIndxLtFcn > globalIndxToLocalIndx;
  std::map< int, IndexType > localIndxToGlobalIndx;

  int iUnknowns = 0;

  for ( in.GoToBegin(); !in.IsAtEnd(); ++in ) {
    
    int iId = (int)round(in.Get());

    if ( iId > 0 && (iId!=m_OriginId)  ) {
      
      if ( iId==m_SolutionDomainId || (iId!=m_SolutionDomainId && flowsOut( in.GetIndex() ) ) ) {

	// if it is a boundary voxel and the gradient points into the domain, the value is declared
	// as known and considered an origin voxel

	IndexType inT = in.GetIndex();

#ifdef _DEBUG_
	std::cout << " in.Get() = " << in.Get() << " ; " << inT << "; " << globalIndxToLocalIndx[ inT ] << "; iUnknowns = " << iUnknowns << std::endl;
#endif

	globalIndxToLocalIndx[ inT ] = iUnknowns;
	localIndxToGlobalIndx[ iUnknowns ] = inT;
	iUnknowns++;

#ifdef _DEBUG_
	std::cout << "count = " << globalIndxToLocalIndx.count( inT ) << " " << inT << ";  = " << globalIndxToLocalIndx[ inT ] << std::endl;
#endif

      }

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

  const itk::Vector<double, InputImageDimension> spacing = this->GetInput()->GetSpacing();

  // use neighborhood iterator for the domain image
  // use image iterator for vector image

  RadiusType radius;
  radius.Fill( 1 );

  ConstLabelMapNeighborhoodIteratorType inN( radius, labelMapImage, labelMapImage->GetLargestPossibleRegion() );
  ConstVectorFieldIteratorType iV( vectorField, vectorField->GetLargestPossibleRegion() );

  //std::set<SMatrixCoors,matrixCoorsLtFcn> usedMatrixCoors;

  int iNumberOfEntries = 0;

  for ( inN.GoToBegin(), iV.GoToBegin(); !inN.IsAtEnd() && !iV.IsAtEnd(); ++inN, ++iV )
    {
     
#ifdef _DEBUG_
      if ( inN.GetCenterPixel()>0 ) {
	std::cout << "inN.GetCenterPixel() == " << inN.GetCenterPixel() << "; " << inN.GetIndex() << std::endl;
      }
#endif
      // look at the unknowns

      if ( globalIndxToLocalIndx.count( inN.GetIndex() )>0 ) { // this is an unknown that needs to be put in the equation system

	int iIC = globalIndxToLocalIndx[ inN.GetIndex() ];

	// get the current velocity vector

	VectorType v = iV.Get();

	// vector needs to be normalized to one

	normalizeVector( v );

#ifdef _DEBUG_
	std::cout << "vector = " << v[0] << " " << v[1] << " " << v[2] << "; at " << inN.GetIndex() << std::endl;
#endif

	double dDiagonalValue = 0;
	double dRHS = m_IntegrationConstant;

	for ( int iI=0; iI<InputImageDimension; iI++ ) {

	  IndexType indexM = inN.GetIndex();
	  indexM[ iI ]--;

	  IndexType indexP = inN.GetIndex();
	  indexP[ iI ]++;

	  double dV = v[iI];

#ifdef _DEBUG_
	  std::cout << "dV = " << dV << "  spacing = " << spacing[iI] << std::endl;
#endif

	  if ( m_ReverseVectorField ) dV*= -1;

	  if ( dV>0 ) { // backward difference

#ifdef _DEBUG_
	    std::cout << "+= " << dV/spacing[iI] << std::endl;
#endif
	    dDiagonalValue += dV/spacing[iI];

	    if ( globalIndxToLocalIndx.count( indexM )>0 ) {
	      int iCIndex = globalIndxToLocalIndx[ indexM ];
	      eqs.SetMatrixValue( iIC, iCIndex, -dV/spacing[iI], 0 );

	    } else {
	      // this should ideally point to a point that has the OriginId
	      // if not, set to a default value, but issue warning

	      int iCId = (int)round( inN.GetPrevious( iI, 1 ) );
	      double dVal;

	      if ( iCId!=0 ) {
		//dVal = initialValuesN.GetPrevious( iI, 1 );
		if ( iInitialValueField ) {
		  dVal = initialValues->GetPixel( indexM );
		} else {
		  dVal = m_InitialValue;
		}
	      } else {
		dVal = 0.0;
		std::cerr << "Tried to reference a point that did not have an origin ID, id = " << iCId << std::endl;
	      }

	      if ( m_ComputeDistanceToInterface ) { // correct it in the direction the distance is computed

		dVal-= spacing[iI]/2.0;

	      }

	      dRHS += dVal*dV/spacing[iI];

	    }

	  } 

	  if ( dV<0 ) { // dV<0 forward difference
	
#ifdef _DEBUG_    
	    std::cout << "-= " << dV/spacing[iI] << std::endl;
#endif
	    dDiagonalValue -= dV/spacing[iI];

	    if ( globalIndxToLocalIndx.count( indexP )>0 ) {
	      int iCIndex = globalIndxToLocalIndx[ indexP ];
	      eqs.SetMatrixValue( iIC, iCIndex, dV/spacing[iI], 0 );

	    } else {

	      // this should ideally point to a point that has the OriginId
	      // if not, set to a default value, but issue warning

	      int iCId = (int)round( inN.GetNext( iI, 1 ) );
	      double dVal;

	      if ( iCId!=0 ) {
		//dVal = initialValuesN.GetPrevious( iI, 1 );
		if ( iInitialValueField ) {
		  dVal = initialValues->GetPixel( indexP );
		} else {
		  dVal = m_InitialValue;
		}
	      } else {
		dVal = 0.0;
		std::cerr << "Tried to reference a point that did not have an origin ID, id = " << iCId << std::endl;
	      }

	      if ( m_ComputeDistanceToInterface ) { // correct it in the direction the distance is computed

		dVal-= spacing[iI]/2.0;

	      }

	      dRHS -= dVal*dV/spacing[iI];

	    }

	  }

	}

#ifdef _DEBUG_
	std::cout << "dDiagonalValue = " << dDiagonalValue << std::endl;
	std::cout << "dRHS = " << dRHS << std::endl;
#endif

	eqs.SetMatrixValue( iIC, iIC, dDiagonalValue, 0 );
	eqs.SetVectorValue( iIC, dRHS, 0 );

      }

    }


#ifdef _DEBUG_
  // print out the matrix

  for ( int iI=0; iI<iUnknowns; iI++ ) {
    for ( int iJ=0; iJ<iUnknowns; iJ++ ) {
      std::cout << eqs.GetMatrixValue(iI,iJ,0) << " ";
    }
    std::cout << std::endl;
  }

  // print out the right hand side

  for ( int iI=0; iI<iUnknowns; iI++ ) {
    std::cout << eqs.GetVectorValue( iI, 0 ) << std::endl;
  }

  std::cout << std::endl;
#endif


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
  
  // first copy the initial conditions input to the output

  typedef ImageRegionIterator< TInputImage > ImageRegionIterator;
  typedef ImageRegionConstIterator< TInputImage > ImageRegionConstIterator;

  if ( iInitialValueField ) {

    ImageRegionConstIterator   initialValuesItr(  initialValues,  region );
    ImageRegionIterator        outputItr( outputImage, region );

    initialValuesItr.GoToBegin();
    outputItr.GoToBegin();

    while( ! outputItr.IsAtEnd() )
      {
	outputItr.Set( initialValuesItr.Get() );
	++initialValuesItr;
	++outputItr;
      }
  } else { // set everything to 0

    ImageRegionIterator        outputItr( outputImage, region );

    outputItr.GoToBegin();

    while( ! outputItr.IsAtEnd() )
      {
	outputItr.Set( 0.0 );
	++outputItr;
      }

  }

  // now write the newly computed values to the output

  for ( unsigned int iI=0; iI<iUnknowns; iI++ ) {

#ifdef _DEBUG_
    std::cout << "sol( " << iI << " ) = " << eqs.GetSolutionValue( iI, 0 ) << ";   ";
    std::cout << "ltg = " << localIndxToGlobalIndx[ iI ] << std::endl;
#endif

    IndexType currentIndex = localIndxToGlobalIndx[ iI ];
    LabelMapPixelType lPixel = labelMapImage->GetPixel( currentIndex );
    
    //std::cout << lPixel << "=?" << m_SolutionDomainId << std::endl;

    if ( m_OutputSolutionOnBoundary || lPixel==m_SolutionDomainId ) {

			double eOut = eqs.GetSolutionValue(iI, 0);
			if (eOut > 1000) {
				std::cout << "Error : " << eOut << std::endl;
			}
      outputImage->SetPixel( currentIndex, eqs.GetSolutionValue( iI, 0 ) );

    }

  }

}

 

/**
 *
 */
template <class TLabelMapImage, class TInputImage, class TVectorFieldValueType>
void 
SolveTransportImageFilter<TLabelMapImage, TInputImage, TVectorFieldValueType>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
}

 

  
} // end namespace itk

#endif
