/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkTransportFunctionRestrictedDomain.hxx,v $
  Language:  C++
  Date:      $Date: 2004-12-21 22:47:30 $
  Version:   $Revision: 1.13 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkTransportFunctionRestrictedDomain_txx_
#define __itkTransportFunctionRestrictedDomain_txx_

using namespace std;

namespace itk {

template<class TImage, class TCovariantVectorImage, class TLabelImage>
void TransportFunctionRestrictedDomain<TImage, TCovariantVectorImage, TLabelImage>
::SetFlowField( const TCovariantVectorImage *FlowField)
{
  this->m_FlowField = FlowField;
}

template<class TImage, class TCovariantVectorImage, class TLabelImage>
void TransportFunctionRestrictedDomain<TImage, TCovariantVectorImage, TLabelImage>
::SetLabelImage( const TLabelImage *LabelImage)
{
  this->m_LabelImage = LabelImage;
}

template<class TImage, class TCovariantVectorImage, class TLabelImage>
TransportFunctionRestrictedDomain<TImage, TCovariantVectorImage, TLabelImage>
::TransportFunctionRestrictedDomain()
{
  unsigned int i;
  RadiusType r;

  for (i = 0; i < ImageDimension; ++i)
    {
    r[i] = 1;
    }
  this->SetRadius(r);

  // Dummy neighborhood used to set up the slices.
  Neighborhood<PixelType, ImageDimension> it;
  it.SetRadius(r);
  // Slice the neighborhood
  m_Center =  it.Size() / 2;

  for (i = 0; i< ImageDimension; ++i)
    {
    m_Stride[i] = it.GetStride(i);
    }

  m_TimeStep = 0; // set it to 0 initially
  m_TimeStepFraction = 0.25;
  m_AdaptiveTimeStep = false;

  m_OriginId = 4;
  m_TargetId = 5;
  m_IntegrationConstant = 1; // computes distance, 0: just transports.
  m_SolutionDomainId = 11;
  m_NeumannId = 6;

}

template<class TImage, class TCovariantVectorImage, class TLabelImage>
typename TransportFunctionRestrictedDomain<TImage, TCovariantVectorImage, TLabelImage>::PixelType
TransportFunctionRestrictedDomain<TImage, TCovariantVectorImage, TLabelImage>
::ComputeUpdate(const NeighborhoodType &it, void * globalData,
                const FloatOffsetType& itkNotUsed(offset))
{
  unsigned int i;
  double dx[ImageDimension];

  // Global data structure
  GlobalDataStruct *gd = (GlobalDataStruct *)globalData;

  // get the current vector at the center position
  // so that we can do proper upwinding
  
  IndexType centerIndex = it.GetIndex();
  VectorPixelType currentVector = m_FlowField->GetPixel( centerIndex );

  LabelPixelType currentLabel = m_LabelImage->GetPixel( centerIndex );

  bool bDoNotUpdate = false;

	double dMag = currentVector[0]*currentVector[0] + currentVector[1]*currentVector[1] + currentVector[2]*currentVector[2];
  double dInner = 0;
	int iNrOfNeumannDomainValues = 0;
	int iNrOfTargetValues = 0;
	int iNrOfSolutionDomainValues = 0;


  //if ( (currentLabel!=m_OriginId) && (currentLabel>0) )
  if ( (currentLabel==m_SolutionDomainId) || (currentLabel==m_TargetId) ) {
     // Calculate the partial derivatives for each dimension using
     // upwinding

    //if ( currentLabel==m_SolutionDomainId ) iNrOfSolutionDomainValues++;

    for (i = 0; i < ImageDimension; i++) {
      if ( currentVector[i]>0 ) {
				IndexType currentIndex = centerIndex;
				currentIndex[i]-=1; LabelPixelType nbLabel =  m_LabelImage->GetPixel( currentIndex );

				if ( nbLabel==m_NeumannId ) {
					dx[i] = 0;
					iNrOfNeumannDomainValues ++;
				} else {
					//  m_Spacing[i] = 0.047;
					if ( nbLabel==m_SolutionDomainId ) iNrOfSolutionDomainValues++;
					if ( nbLabel==m_TargetId) iNrOfTargetValues++;
					// use a backward difference
					int a = int(it.GetPixel(m_Center));
					int b = int(it.GetPixel(m_Center-m_Stride[i]));

					dx[i] = (it.GetPixel(m_Center) - it.GetPixel(m_Center - m_Stride[i]))/m_Spacing[i];
					//std::cout << "spacing: " << m_Spacing[i] << std::endl;
					//std::cout << "dxvalue: " << a << "," << b << std::endl;
				}
			} else {
				IndexType currentIndex = centerIndex;
				currentIndex[i]+=1;
				LabelPixelType nbLabel =  m_LabelImage->GetPixel( currentIndex );

				if ( nbLabel==m_NeumannId ) {
					dx[i] = 0;
					iNrOfNeumannDomainValues ++;
				} else {
					if ( nbLabel==m_SolutionDomainId ) iNrOfSolutionDomainValues++;
					if ( nbLabel==m_TargetId) iNrOfTargetValues++;
					dx[i] = (it.GetPixel(m_Center + m_Stride[i]) - it.GetPixel(m_Center))/m_Spacing[i];
				}
			}
		}

		if ( iNrOfSolutionDomainValues==0  || (iNrOfSolutionDomainValues == 1 && (iNrOfNeumannDomainValues == 2 || currentLabel == m_TargetId))) {
				bDoNotUpdate = true;
		} else {
				//std::cout << "# of solution domain values : " << iNrOfSolutionDomainValues << std::endl;
		} 

    // now we can take the inner product with the velocity vector

    for ( i = 0; i < ImageDimension; i++) {
      dInner += dx[i]*currentVector[i];
		}  
	} else {
    bDoNotUpdate = true;
	}

  // compute the maximal possible time step allowed by the current
  // velocity

  for ( i = 0; i < ImageDimension; i++ ) {
    gd->m_MaxTimeStep = vnl_math_min(gd->m_MaxTimeStep,gd->m_fac/fabs(currentVector[i])*m_Spacing[i]);
	}

  // and return the update
  // 
  // the equation being solved is 
  //     u_t + v \nabla u = integrationConstant
  // so the update is 
  //     -v \nabla u + integrationConstant
  // with upwinding derivatives

  if ( bDoNotUpdate ) {
      return static_cast<PixelType>( 0 );
	} else {
			double dOut = -dInner + m_IntegrationConstant;
      return static_cast<PixelType>( dOut );
	}
}

} // end namespace itk

#endif
