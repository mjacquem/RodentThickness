/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkLaplaceFunctionRestrictedDomain.hxx,v $
  Language:  C++
  Date:      $Date: 2004-12-21 22:47:30 $
  Version:   $Revision: 1.13 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkLaplaceFunctionRestrictedDomain_txx_
#define __itkLaplaceFunctionRestrictedDomain_txx_

namespace itk {

template<class TImage, class TLabelImage>
void LaplaceFunctionRestrictedDomain<TImage, TLabelImage>
::SetLabelImage( const TLabelImage *LabelImage)
{
  this->m_LabelImage = LabelImage;
}

template<class TImage, class TLabelImage>
LaplaceFunctionRestrictedDomain<TImage, TLabelImage>
::LaplaceFunctionRestrictedDomain()
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

  m_OriginId = 4;
  m_TargetId = 5;
  m_SolutionDomainId = 11;
  m_NeumannId = 6;
}

template<class TImage, class TLabelImage>
typename LaplaceFunctionRestrictedDomain<TImage, TLabelImage>::PixelType
LaplaceFunctionRestrictedDomain<TImage, TLabelImage>
::ComputeUpdate(const NeighborhoodType &it, void * itkNotUsed(globalData),
                const FloatOffsetType& itkNotUsed(offset))
{
  unsigned int i;
  double dx[ImageDimension];

  // get the current vector at the center position
  // so that we can do proper upwinding
  
  IndexType centerIndex = it.GetIndex();
  LabelPixelType currentLabel = m_LabelImage->GetPixel( centerIndex );

  bool bDoNotUpdate = false;

  double dInner = 0;

  PixelType sumOfValues = 0;

  //if ( (currentLabel!=m_OriginId) && (currentLabel>0) )
  if ( (currentLabel==m_SolutionDomainId) )
    {

    for (i = 0; i < ImageDimension; i++)
      {

      IndexType currentIndex = centerIndex;
      currentIndex[i]-=1;
      LabelPixelType nbLabel =  m_LabelImage->GetPixel( currentIndex );

      if ( nbLabel!=m_NeumannId )
	{
	sumOfValues += (it.GetPixel(m_Center - m_Stride[i])-it.GetPixel(m_Center))/(m_Spacing[i]*m_Spacing[i]);
	}

      currentIndex = centerIndex;
      currentIndex[i]+=1;
      nbLabel =  m_LabelImage->GetPixel( currentIndex );

      if ( nbLabel!=m_NeumannId )
	{
	sumOfValues += (it.GetPixel(m_Center + m_Stride[i])-it.GetPixel(m_Center))/(m_Spacing[i]*m_Spacing[i]);
	}
      }
    }
  else 
    {
    bDoNotUpdate = true;
    }

  // and return the update
  // 
  // the equation being solved is 
  //     u_t - \Delta u = 0
  // so the update is 
  //     \Delta u 
  // with central derivatives

  if ( bDoNotUpdate )
    {
    return static_cast<PixelType>( 0 );
    }
  else
    {
    return static_cast<PixelType>( sumOfValues );
    }
}

} // end namespace itk

#endif
