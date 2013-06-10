// computes the thickness given a labelmap
// thickness measurement is based on a grid-based solution of Laplace's equation

#include <itksys/SystemTools.hxx>
#include <itkImageFileReader.h>
#include <iostream>
#include <string>

#include "itkSolveLaplacianImageFilter.h"
#include "itkLaplaceGradientImageFilter.h"
//#include "itkSolveTransportImageFilter.h"
#include "itkPropagateBCFilter.h"
#include "itkBoundaryStatisticsImageFilter.h"

#include "itkCreateBoundaryConditionImageFilter.h"

#include "itkNormalizeVectorImageFilter.h"
#include "itkNegateVectorImageFilter.h"
#include "itkTransportImageFilterRestrictedDomain.h"
#include "itkZeroImageFilter.h"
#include "itkLaplaceImageFilterRestrictedDomain.h"

#include "measureThicknessFilterCLP.h"

#include <itkCovariantVector.h>
#include <itkAddImageFilter.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkShiftScaleImageFilter.h>
#include <itkCastImageFilter.h>

#define DIMENSION 3

typedef itk::Image< short , DIMENSION > LabelMapImageType;
typedef itk::Image< double, DIMENSION > ImageType;
typedef itk::ImageFileReader< LabelMapImageType > ReaderType;
typedef itk::ImageFileReader<ImageType> ImageReaderType;
typedef itk::CovariantVector<double, DIMENSION> VectorType;
typedef itk::Image< VectorType, DIMENSION > VectorImageType;

typedef LabelMapImageType::SpacingType SpacingType;

typedef itk::CastImageFilter< LabelMapImageType, ImageType> CastImageFilterType;

typedef itk::ZeroImageFilter< ImageType, ImageType > ZeroImageFilterType;

//typedef itk::SolveLaplacianImageFilter< LabelMapImageType, ImageType
//> slIFT;
typedef itk::LaplaceImageFilterRestrictedDomain< ImageType, LabelMapImageType, ImageType > slIFT;
typedef itk::LaplaceGradientImageFilter< LabelMapImageType, ImageType > lgIFT;

//typedef itk::SolveTransportImageFilter< LabelMapImageType, ImageType > tIFT;

typedef itk::TransportImageFilterRestrictedDomain< ImageType, VectorImageType, LabelMapImageType, ImageType> TransportImageFilterRestrictedDomainType;
typedef itk::NormalizeVectorImageFilter< VectorImageType, VectorImageType > NormalizeVectorImageFilterType;

typedef itk::NegateVectorImageFilter< VectorImageType, VectorImageType > NegateVectorImageFilterType;

typedef itk::PropagateBCFilter< LabelMapImageType > pbcFT;
typedef itk::BoundaryStatisticsImageFilter< LabelMapImageType, ImageType > bstatIFT;

typedef itk::AddImageFilter<ImageType,ImageType,ImageType> AddImageFilterType;
typedef itk::ShiftScaleImageFilter<ImageType,ImageType> ShiftScaleFilterType;

typedef itk::ImageFileWriter<ImageType> WriterType;
typedef itk::ImageFileWriter<LabelMapImageType> LabelMapWriterType;

typedef itk::ImageFileReader<VectorImageType> VectorReaderType;
typedef itk::ImageFileWriter<VectorImageType> VectorWriterType;

typedef itk::CreateBoundaryConditionImageFilter<LabelMapImageType,ImageType> CreateBCImageFilterType;

//#undef _COMPLETE_
#define _COMPLETE_

int main( int argc, const char ** argv ) {

  PARSE_ARGS;

  itksys::SystemTools::ChangeDirectory(workingDirectory.c_str());
  
  // labelmap as an input

  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( inputVolume.c_str() );

  try {
    reader->Update();
  }

  catch(  itk::ExceptionObject & exp )
    {
      std::cerr << exp << std::endl;
      return 1;
    }

  // first create a field of zeros as the initial conditions

  CastImageFilterType::Pointer readerCast = CastImageFilterType::New();
  readerCast->SetInput( reader->GetOutput() );

  ZeroImageFilterType::Pointer zeros = ZeroImageFilterType::New();
  zeros->SetInput( readerCast->GetOutput() );
  zeros->Update();

  pbcFT::Pointer propagateBCFilter = pbcFT::New();
  propagateBCFilter->SetDirichletLowId( dirichletLowId );
  propagateBCFilter->SetDirichletHighId( dirichletHighId );
  propagateBCFilter->SetNeumannId( neumannId );
  propagateBCFilter->SetSolutionDomainId( solutionDomainId );
  propagateBCFilter->SetInput( reader->GetOutput() );
  propagateBCFilter->Update();

    /// test
    if (true) {
        CreateBCImageFilterType::Pointer bcForward = CreateBCImageFilterType::New();
        bcForward->SetInput( propagateBCFilter->GetOutput() );
        bcForward->SetOriginLabel( dirichletLowId );
        bcForward->SetOriginValue( -10 );
        bcForward->Update();
        WriterType::Pointer writer = WriterType::New();
        writer->SetInput(bcForward->GetOutput());
        writer->SetFileName("/tmp/bcForward.nii.gz");
        writer->Write();
    }

    //test
#ifdef _COMPLETE_

  std::string correctedBC = "correctedBC.nhdr";

  if ( correctedBC!="" ) {

    LabelMapWriterType::Pointer writerBCP = LabelMapWriterType::New();
    writerBCP->SetInput( propagateBCFilter->GetOutput() );
    writerBCP->SetFileName( correctedBC.c_str() );
    writerBCP->UseCompressionOn();

    try	
      {
	writerBCP->Update();
      }
    catch (itk::ExceptionObject & e)
      {
	std::cerr << "exception in BCP file writer " << std::endl;
	std::cerr << e << std::endl;
	return 1;
      }

  }
#endif

  ImageType::Pointer laplaceOutput = NULL;
  if (cachedLaplacianOutput == "" || !itksys::SystemTools::FileExists(cachedLaplacianOutput.c_str(), true)) {
    std::cout << "Computing the Laplace solution ...";
    
    slIFT::Pointer laplace = slIFT::New();
    laplace->SetInput( zeros->GetOutput() );
    laplace->SetLabelImage( propagateBCFilter->GetOutput() );

    laplace->SetNeumannId( neumannId );
    laplace->SetSolutionDomainId( solutionDomainId );
    laplace->SetOriginId( dirichletLowId );
    laplace->SetTargetId( dirichletHighId );
    laplace->UseImageSpacingOn();
    laplace->SetTerminalNumberOfIterations( 2000 );

    laplace->Update();

    /*slIFT::Pointer laplace = slIFT::New();
    laplace->SetDirichletLowId( dirichletLowId );
    laplace->SetDirichletHighId( dirichletHighId );
    laplace->SetNeumannId( neumannId );
    laplace->SetSolutionDomainId( solutionDomainId );

    laplace->SetInput( propagateBCFilter->GetOutput() );
    laplace->Update();*/

    std::cout << " done." << std::endl;

#ifdef _COMPLETE_

    std::string laplaceSolution = "laplaceSolution.nhdr";

    if ( laplaceSolution!="" ) {

      // scale Laplace result

      ShiftScaleFilterType::Pointer shiftScaleLaplaceFilter = ShiftScaleFilterType::New();
      shiftScaleLaplaceFilter->SetInput( laplace->GetOutput() );
      shiftScaleLaplaceFilter->SetShift( 1.0 );
      shiftScaleLaplaceFilter->SetScale( 1000 );

      // write out the Laplace result

      WriterType::Pointer writerLaplace = WriterType::New();
      writerLaplace->SetInput( shiftScaleLaplaceFilter->GetOutput() );
      writerLaplace->SetFileName( laplaceSolution.c_str() );
      writerLaplace->UseCompressionOn();

      try	
        {
    writerLaplace->Update();
        }
      catch (itk::ExceptionObject & e)
        {
    std::cerr << "exception in Laplace file writer " << std::endl;
    std::cerr << e << std::endl;
    return 1;
        }
    }

    if (cachedLaplacianOutput != "") {
      std::cout << "Writing Laplacian cache ...";
      WriterType::Pointer cacheLaplaceWriter = WriterType::New();
      cacheLaplaceWriter->SetFileName(cachedLaplacianOutput.c_str());
      cacheLaplaceWriter->SetInput(laplace->GetOutput());
      cacheLaplaceWriter->UseCompressionOn();
      cacheLaplaceWriter->Write();
      std::cout << " done " << std::endl;
    }

    laplaceOutput = laplace->GetOutput();
  } else {
    std::cout << "Reading cached Laplacian output ... " << cachedLaplacianOutput;

    ImageReaderType::Pointer readerLaplace = ImageReaderType::New();
    readerLaplace->SetFileName(cachedLaplacianOutput.c_str());
    readerLaplace->Update();
    std::cout << " done " << std::endl;

    laplaceOutput = readerLaplace->GetOutput();
  }
#endif

  // now put that in a filter that computes the transport equation

  std::cout << "Computing the Laplace gradient ...";

  lgIFT::Pointer laplaceGradient = lgIFT::New();
  laplaceGradient->SetInput( laplaceOutput );
  laplaceGradient->SetDomainImage( propagateBCFilter->GetOutput() );
  laplaceGradient->SetDirichletLowId( dirichletLowId );
  laplaceGradient->SetDirichletHighId( dirichletHighId );
  laplaceGradient->SetNeumannId( neumannId );
  laplaceGradient->SetSolutionDomainId( solutionDomainId );
  laplaceGradient->Update();
  
  std::cout << " done." << std::endl;

  // compute the normalized gradient in both directions

  std::cout << "Computing the vector field normalizations ...";

  NormalizeVectorImageFilterType::Pointer normalizedGradient = NormalizeVectorImageFilterType::New();
  normalizedGradient->SetInput( laplaceGradient->GetOutput() );
  normalizedGradient->Update();

 // now the negated version of it

  NegateVectorImageFilterType::Pointer negatedNormalizedGradient = NegateVectorImageFilterType::New();
  negatedNormalizedGradient->SetInput( normalizedGradient->GetOutput() );
  negatedNormalizedGradient->Update();


  std::cout << " done." << std::endl;

  std::cout << "Writing the gradient vector field ...";
  VectorWriterType::Pointer gradientWriter = VectorWriterType::New();
  gradientWriter->SetFileName("laplacianGradient.nrrd");
  gradientWriter->SetInput(normalizedGradient->GetOutput());
  gradientWriter->Write();
  std::cout << " done." << std::endl;

 
  // now solve the transport equation in one direction

  // create the boundary conditions so that we start with a negative
  // value

  // we need to find the spacing of the image first to create the
  // correct discrete boundary condition

  SpacingType st = reader->GetOutput()->GetSpacing();
  // check that there are equal spacings

  double cS = st[0];

  double boundaryOffset;

  if ( subtractBoundaryThickness ) {
    for ( int iI=1; iI<DIMENSION; iI++ ) {
      if ( st[iI]!=cS ) 
	{
	std::cerr << "Spacing is not isotropic: " << st << std::endl;
	std::cerr << "Not sure how to adapt the boundary conditions. Abort." << std::endl;
	return EXIT_FAILURE;
	}
      }
    boundaryOffset = cS/2;
    }
  else
    {
    boundaryOffset = 0;
    }

  CreateBCImageFilterType::Pointer bcForward = CreateBCImageFilterType::New();
  bcForward->SetInput( propagateBCFilter->GetOutput() );
  bcForward->SetOriginLabel( dirichletLowId );
  bcForward->SetOriginValue( -boundaryOffset );
	bcForward->Update();
	
	if (true) {
		cout << "Writing bcForward.nii" << endl;
    WriterType::Pointer writerTF = WriterType::New();
    writerTF->SetInput( bcForward->GetOutput() );
    writerTF->SetFileName( "bcForward.nii" );
    writerTF->UseCompressionOn();
		writerTF->Write();
	}

  std::cout << "Computing the forward transport equation ... ttrns = " << transportTerminalTime << std::endl;

  TransportImageFilterRestrictedDomainType::Pointer transport = TransportImageFilterRestrictedDomainType::New();
  //transport->SetInput( zeros->GetOutput() ); // use the Laplace
  //output here, but anything goes, could just pass a array of zeros
  transport->SetInput( bcForward->GetOutput() ); 
  transport->UseImageSpacingOn();
  transport->SetFlowField( normalizedGradient->GetOutput() );
  transport->SetLabelImage( propagateBCFilter->GetOutput() );
  transport->SetOriginId( dirichletLowId );
  transport->SetTargetId( dirichletHighId );
  transport->SetSolutionDomainId( solutionDomainId );
  transport->SetNeumannId( neumannId );
  transport->SetIntegrationConstant( 1.0 );
  transport->SetTerminalTime( transportTerminalTime );
  transport->SetTimeStep( 0.25 );
  transport->Update();

  std::cout << " done." << std::endl;

#ifdef _COMPLETE_

  std::string forwardTransportEquation = "forwardTransportEquation.nhdr";

  if ( forwardTransportEquation!="" ) {

    ShiftScaleFilterType::Pointer shiftScaleFilterTF = ShiftScaleFilterType::New();
    shiftScaleFilterTF->SetInput( transport->GetOutput() );
    shiftScaleFilterTF->SetShift( 0.0 );
    shiftScaleFilterTF->SetScale( 1000 );
    
    WriterType::Pointer writerTF = WriterType::New();
    writerTF->SetInput( shiftScaleFilterTF->GetOutput() );
    writerTF->SetFileName( forwardTransportEquation.c_str() );
    writerTF->UseCompressionOn();

    try	
      {
	writerTF->Update();
      }
    catch (itk::ExceptionObject & e)
      {
	std::cerr << "exception in transport forward file writer " << std::endl;
	std::cerr << e << std::endl;
	return 1;
      }
  }
#endif

  // solve it in reverse

  CreateBCImageFilterType::Pointer bcReverse = CreateBCImageFilterType::New();
  bcReverse->SetInput( propagateBCFilter->GetOutput() );
  bcReverse->SetOriginLabel( dirichletHighId );
  bcReverse->SetOriginValue( -boundaryOffset );

  std::cout << "Computing backward transport equation ... ttrns = " << transportTerminalTime << std::endl;

  TransportImageFilterRestrictedDomainType::Pointer transportReverse = TransportImageFilterRestrictedDomainType::New();

//  transportReverse->SetInput( zeros->GetOutput() );
  transportReverse->SetInput( bcReverse->GetOutput() );
  transportReverse->UseImageSpacingOn();
  transportReverse->SetFlowField( negatedNormalizedGradient->GetOutput() );
  transportReverse->SetLabelImage( propagateBCFilter->GetOutput() );
  transportReverse->SetOriginId( dirichletHighId );
  transportReverse->SetTargetId( dirichletLowId );
  transportReverse->SetNeumannId( neumannId );
  transportReverse->SetSolutionDomainId( solutionDomainId );
  transportReverse->SetIntegrationConstant( 1.0 );
  transportReverse->SetTerminalTime( transportTerminalTime );
  transportReverse->SetTimeStep( 0.25 );
  transportReverse->Update();

  std::cout << " done." << std::endl;

#ifdef _COMPLETE_

  std::string backwardTransportEquation = "backwardTransportEquation.nhdr";

  if ( backwardTransportEquation!="" ) {
  
    ShiftScaleFilterType::Pointer shiftScaleFilterTR = ShiftScaleFilterType::New();
    shiftScaleFilterTR->SetInput( transportReverse->GetOutput() );
    shiftScaleFilterTR->SetShift( 0.0 );
    shiftScaleFilterTR->SetScale( 1000 );
    
    WriterType::Pointer writerTR = WriterType::New();
    writerTR->SetInput( shiftScaleFilterTR->GetOutput() );
    writerTR->SetFileName( backwardTransportEquation.c_str() );
    writerTR->UseCompressionOn();

    try	
      {
	writerTR->Update();
      }
    catch (itk::ExceptionObject & e)
      {
	std::cerr << "exception in transport reverse writer " << std::endl;
	std::cerr << e << std::endl;
	return 1;
      }
  }    
#endif

  // now add them to get the end result

  AddImageFilterType::Pointer addImageFilter = AddImageFilterType::New();
  addImageFilter->SetInput1( transport->GetOutput() );
  addImageFilter->SetInput2( transportReverse->GetOutput() );

  ShiftScaleFilterType::Pointer shiftScaleFilter = ShiftScaleFilterType::New();
  shiftScaleFilter->SetInput( addImageFilter->GetOutput() );
  shiftScaleFilter->SetShift( 0.0 );
  shiftScaleFilter->SetScale( 1000 );
  
  // this is the result
  // let's write it out
  
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( shiftScaleFilter->GetOutput() );
  writer->SetFileName( outputVolume.c_str() );
  writer->UseCompressionOn();

  try	
    {
      writer->Update();
    }
  catch (itk::ExceptionObject & e)
    {
      std::cerr << "exception in thickness file writer " << std::endl;
      std::cerr << e << std::endl;
      return 1;
    }

  std::cout << "Computing the voxel statistics ...";
  
  bstatIFT::Pointer bstatFilter = bstatIFT::New();
  bstatFilter->SetInput( addImageFilter->GetOutput() );
  bstatFilter->SetLabelMapImage( propagateBCFilter->GetOutput() );
  bstatFilter->SetSolutionDomainId( solutionDomainId );
  bstatFilter->SetDirichletLowId( dirichletLowId );
  bstatFilter->SetDirichletHighId( dirichletHighId );
  bstatFilter->Update();

  std::cout << " done." << std::endl;

  if ( machineReadableOutput ) {

    std::cout << std::endl;
    std::cout << "mean = " << bstatFilter->GetMean() << std::endl;
    std::cout << "std = " << bstatFilter->GetStd() << std::endl;
    std::cout << "min = " << bstatFilter->GetMin() << std::endl;
    std::cout << "max = " << bstatFilter->GetMax() << std::endl;
    std::cout << "volume = " << bstatFilter->GetVolume() << std::endl;
    std::cout << "number of volume voxels = " << bstatFilter->GetNumberOfVolumeVoxels() << std::endl;
    std::cout << "number of surface voxels = " << bstatFilter->GetNumberOfSurfaceVoxels() << std::endl;
    std::cout << std::endl;
    std::cout << "Note: voxels may be counted multiple times for the statistics, depending on their surrounding boundary conditions." << std::endl;
    
  } else {

    std::cout << bstatFilter->GetMean() << " ";
    std::cout << bstatFilter->GetStd() << " ";
    std::cout << bstatFilter->GetMin() << " ";
    std::cout << bstatFilter->GetMax() << " ";
    std::cout << bstatFilter->GetVolume() << " ";
    std::cout << bstatFilter->GetNumberOfVolumeVoxels() << " ";
    std::cout << bstatFilter->GetNumberOfSurfaceVoxels() << std::endl;

  }

  if ( thicknessListOutput!="" ) {

    const std::vector<double> vStatisticsValues = bstatFilter->GetStatisticsVector();

    // write it out

    std::ofstream outputStream( thicknessListOutput.c_str() );
    
    std::vector<double>::const_iterator iterVec;
    for ( iterVec=vStatisticsValues.begin(); iterVec!=vStatisticsValues.end(); iterVec++ ) {
      outputStream << *iterVec << std::endl;
    }

    outputStream.close();

  }

#ifdef _COMPLETE_

  std::string statisticVoxels = "statisticVoxels.nhdr";

  if ( statisticVoxels!="" ) {

    ShiftScaleFilterType::Pointer bstatShiftScaleFilter = ShiftScaleFilterType::New();
    bstatShiftScaleFilter->SetInput( bstatFilter->GetOutput() );
    bstatShiftScaleFilter->SetShift( 0.0 );
    bstatShiftScaleFilter->SetScale( 1000 );
    
    // this is the result
    // let's write it out
    
    WriterType::Pointer bstatWriter = WriterType::New();
    bstatWriter->SetInput( bstatShiftScaleFilter->GetOutput() );
    bstatWriter->SetFileName( statisticVoxels.c_str() );
    bstatWriter->UseCompressionOn();

    try	
      {
	bstatWriter->Update();
      }
    catch (itk::ExceptionObject & e)
      {
	std::cerr << "exception in bstat file writer " << std::endl;
	std::cerr << e << std::endl;
	return 1;
      }
  }
#endif

  return 0;

}
