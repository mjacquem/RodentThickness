macro( CheckExitCodeAndExitIfError )
  if(NOT ${ExitCode} EQUAL 0)
    return(${ExitCode})
  endif()
endmacro( CheckExitCodeAndExitIfError )

 

#if(WIN32)
# Creating include and lib dirs
 # file(MAKE_DIRECTORY ${TOP_BINARY_DIR}/Rscript-install/include)
 # file(MAKE_DIRECTORY ${TOP_BINARY_DIR}/Rscript-install/lib)

#elseif(APPLE)


#else()
 # Configure Step
  message("[] Configuring Rscript...")
  execute_process(COMMAND sh ${TOP_BINARY_DIR}/Rscript/configure --with-readline=no WORKING_DIRECTORY ${TOP_BINARY_DIR}/Rscript-build RESULT_VARIABLE ExitCode)
  CheckExitCodeAndExitIfError()

  # Build Step
  message("[] Building Rscript...")
  execute_process(COMMAND make WORKING_DIRECTORY ${TOP_BINARY_DIR}/Rscript-build RESULT_VARIABLE ExitCode)
  CheckExitCodeAndExitIfError()

  # Install Step
  message("[] Installing Rscript...")
  execute_process(COMMAND make install WORKING_DIRECTORY ${TOP_BINARY_DIR}/Rscript-build RESULT_VARIABLE ExitCode)
  CheckExitCodeAndExitIfError()

#endif()
