cmake_minimum_required(VERSION 2.8)
CMAKE_POLICY(VERSION 2.8)

## Remove possible existing output folder already computed to rerun all tests
message("Removing possible existing test output folder")
file(REMOVE_RECURSE ${TestingBINdirectory})

# Configure (Replace paths) and Copy needed files
configure_file( ${TestingSRCdirectory}/RodentThicknessconfigfile.bms.in ${TestingBINdirectory}/RodentThicknessconfigfile.bms )
configure_file( ${TestingSRCdirectory}/dataset.csv.in ${TestingBINdirectory}/dataset.csv)
#configure_file( ${TestingSRCdirectory}/TestGUIparameters.csv.in ${TestingBINdirectory}/TestGUIparameters.csv )

## Test 1 : Usage
add_test(NAME TestUsage COMMAND ${Executablebin}/RodentThickness --help) # Should just display the Usage : ${CMAKE_CURRENT_BINARY_DIR} = DTIAtlasBuilder-build/Testing -> need to ../
set(EXECUTABLE_OUTPUT_PATH ${TestingBINdirectory}) # So the test executables go to the right place

## Test 2 : Open the GUI for 10 sec and close it
add_executable(RodentThicknessGUITest RodentThicknessGUITest.cxx)
target_link_libraries(RodentThicknessGUITest RodentThicknessLib)
set_target_properties(RodentThicknessGUITest PROPERTIES LABELS RodentThickness)
add_test(NAME TestGUI COMMAND $<TARGET_FILE:RodentThicknessGUITest> ${TestingBINdirectory}/RodentThicknessconfigfile.bms) # The config file will be read as argv[1] (no GenerateCLP)

## Test 3 : Open the GUI for 10 sec with a param file, check the content of the QLineEdit and close it
add_executable(RodentThicknessGUITestLabels RodentThicknessGUITestLabel.cxx)
target_link_libraries(RodentThicknessGUITestLabels RodentThicknessLib)
set_target_properties(RodentThicknessGUITestLabels PROPERTIES LABELS RodentThickness)
add_test(NAME TestGUILabels COMMAND $<TARGET_FILE:RodentThicknessGUITestLabels> ${TestingBINdirectory}/dataset.csv ${TestingBINdirectory}/RodentThicknessconfigfile.bms ${Executablebin}/ImageMath A)
# The parameter file will be read as argv[1] (no GenerateCLP) and config file argv[2] # argv[3] is the folder to check to content of LineEdit



#test 4
add_test(NAME TestExecMeasureThicknessFilterhelp COMMAND ${Executablebin}/measureThicknessFilter --help)

## Test 5
add_test(NAME TestExecImageMathHelp COMMAND ${Executablebin}/ImageMath -help)

## Test 6
add_test(NAME TestExecSegPostProcessHelp COMMAND ${Executablebin}/SegPostProcessCLP --help)

#test 7
add_test(NAME TestExecGenParaMeshCLPHelp COMMAND ${Executablebin}/GenParaMeshCLP --help)

#test 8
add_test(NAME TestExecParaToSPHARMMeshCLPHelp COMMAND ${Executablebin}/ParaToSPHARMMeshCLP --help)

#test9
add_test(NAME TestExecMeasureThicknessFilter COMMAND ${Executablebin}/measureThicknessFilter --mr --sbt --workdir ${TestingBINdirectory} --ids 3 --idl 4 --idh 1 --idn 5 --ttrns 100 ${TestDataFolder}/Imagetest2.nrrd ${TestingBINdirectory}/ImageTestmeasurementoutput.nrrd)


#test 10


add_test(NAME TestExecImageMath COMMAND ${Executablebin}/ImageMath ${TestDataFolder}/Imagetest2.nrrd -extractLabel 3 -outfile ${TestingBINdirectory}/Imagetest.labelMapNCsegmentation.nrrd)


## Test 11
add_test(NAME TestExecSegPostProcess COMMAND ${Executablebin}/SegPostProcessCLP ${TestDataFolder}/Imagetest.labelMapNCsegmentation.nrrd ${TestingBINdirectory}/Imagetest.labelMapNCsegmentationSegPostProcess.nrrd)

#Test 12 
add_test(NAME TestExecGenParaMeshCLP COMMAND ${Executablebin}/GenParaMeshCLP ${TestDataFolder}/Imagetest.labelMapNCsegmentationSegPostProcess.nrrd --label 1 ${TestingBINdirectory}/test_para.vtk ${TestingBINdirectory}/test_surf.vtk --UseUnitSpacing)

#test13
add_test(NAME TestExecParaToSPHARMMeshCLP COMMAND ${Executablebin}/ParaToSPHARMMeshCLP ${TestDataFolder}/test_para.vtk ${TestDataFolder}/test_surf.vtk --subdivLevel 10 --spharmDegree 20 ${TestingBINdirectory}/test.ip.)

#test 14
add_test(NAME TestExecParaToSPHARMMeshCLP2 COMMAND ${Executablebin}/ParaToSPHARMMeshCLP ${TestDataFolder}/test_para.vtk ${TestDataFolder}/test_surf.vtk --subdivLevel 50 --spharmDegree 20 ${TestingBINdirectory}/test.subj.)

#test 15
add_test(NAME TestExecMeshPointsIntensitySamplingHelp COMMAND ${Executablebin}/MeshPointsIntensitySampling --help)


#test 16
add_test(NAME TestExecBinaryToDistanceMap COMMAND ${Executablebin}/BinaryToDistanceMap --help)


#test 17
add_test(NAME TestExecMeshPointsIntensitySampling COMMAND ${Executablebin}/MeshPointsIntensitySampling --workDir ${TestingBINdirectory} --inputAsPhysicalCoord --distanceVector test.distanceVector.nrrd -i nn -a ${TestingBINdirectory}/testimage.txt -m ${TestingBINdirectory}/testimage.sampling.vtk ${TestDataFolder}/ImageTestmeasurementoutput.nrrd ${TestDataFolder}/2.correspondence.vtk)

#test 18

add_test(NAME TestPythonVersion COMMAND ${pathexecpython} ${TestDataFolder}/pythontestversion.py)


#test 19
add_test(NAME TestExecShapeWorks COMMAND ${pathexecpython} ${TestDataFolder}/shapeworks.py -h)



#test 20
add_test(NAME TestExecvtkPointAttribute COMMAND ${pathexecpython} ${TestDataFolder}/vtkPointAttributes.py -h)


#test 21
add_test(NAME TestExecconcatToColumns COMMAND ${pathexecpython} ${TestDataFolder}/concatToColumns.py -h)

set(BUILD_SHARED_LIBS OFF) 

#if(ITK_VERSION_MAJOR EQUAL 3)
  add_executable(RodentThicknessTest TestITKv3.cxx) # This executable will run pipeline and then compare output with reference
  target_link_libraries(RodentThicknessTest RodentThicknessLib)
set_target_properties(RodentThicknessTest PROPERTIES LABELS RodentThickness)
if(BUILD_SHARED_LIBS)
    set_target_properties(RodentThicknessTest PROPERTIES COMPILE_FLAGS "-DBUILD_SHARED_LIBS") # definition of the variable for c++ preprocessor
  endif(BUILD_SHARED_LIBS)
  set(NoGUITestCommand $<TARGET_FILE:RodentThicknessTest>) # Slicer_LAUNCH_COMMAND will launch the Slicer environment # TARGET_FILE so path not given directly = no need to be known
  set(NameOfMain ModuleEntryPoint)
#elseif(ITK_VERSION_MAJOR EQUAL 4) # BRAINS/BRAINSCommonLib/BuildScripts/CMakeBRAINS3BuildMacros.cmake : macro MakeTestDriverFromSEMTool
#  set(CMAKE_TESTDRIVER_BEFORE_TESTMAIN "#include \"itkTestDriverBeforeTest.inc\"")
#    set(CMAKE_TESTDRIVER_AFTER_TESTMAIN "#include \"itkTestDriverAfterTest.inc\"")
#  create_test_sourcelist(RodentThickness RodentThicknessTestDriver.cxx TestITKv4.cxx # Creates DTIAtlasBuilderTestDriver.cxx (= test source) into bin/Testing
 #   EXTRA_INCLUDE itkTestDriverIncludeRequiredIOFactories.h
 #   FUNCTION ProcessArgumentsAndRegisterRequiredFactories
#    )
#  add_executable(RodentThicknessTestDriver RodentThicknessTestDriver.cxx TestITKv4.cxx)
 # target_link_libraries(RodentThicknessTestDriver RodentThicknessLib ${ITKTestKernel_LIBRARIES}) # ITKTestKernel_LIBRARIES only defined in ITKv4

#  set(NoGUITestCommand $<TARGET_FILE:RodentThicknessTestDriver>) # Slicer_LAUNCH_COMMAND will launch the Slicer environment # TARGET_FILE so path not given directly = no need to be known
#  set(NameOfMain RodentThicknessTestITKv4)
#endif()

set(CompareParametersImageMathFA
  --compareIntensityTolerance 200 # pixel OK if in range [value-200;value+200] => FA between 0 and 10000 (ImageMath)
  --compareRadiusTolerance 1 # pixel OK if one of neighbors in the given radius is in the tolerance range
  --compareNumberOfPixelsTolerance 50 # Set the number of pixels that are allowed to fail
  )

set(RTParameters
  --nogui # no need if ITKv4 -> already in DTIAtlasBuilderTest.cxx
  --dataset ${TestingBINdirectory}/dataset.csv
  --configfile ${TestingBINdirectory}/RodentThicknessconfigfile.bms
  --PathBms ${TestDataFolder}/batchmakeScript
  --WorkDir ${TestingBINdirectory}
  --CStat 
)

add_test( NAME TestnoGUIAffineOutput COMMAND ${NoGUITestCommand}
  --compare # --compare is an cmd line argument from the itk test files
   ${TestDataFolder}/2.labelMapNCsegmentationSegPostProcess.nrrd
   ${TestingBINdirectory}/RodentThicknessResults/Output/MeasurementandSPHARM/2.labelMapNCsegmentationSegPostProcess.nrrd# Affine Output
   ${CompareParametersImageMathFA}
  ${NameOfMain} # After this line, give the parameters to use in DTIAtlasBuilder
    ${RTParameters}
  )



