
# Find external tools

# Git protocol
option(USE_GIT_PROTOCOL "If behind a firewall turn this off to use http instead." ON)
set(git_protocol "git")
if(NOT USE_GIT_PROTOCOL)
  set(git_protocol "http")
else(NOT USE_GIT_PROTOCOL)
  set(git_protocol "git")
endif()

#===== Macro set paths ===============================================
macro( SetPathsRecompile )
  foreach( tool ${Tools} )
 # set(InstallPath ${CMAKE_INSTALL_PREFIX}) # Non cache variable so its value can change and be updated
    set(TOOL${tool} ${CMAKE_INSTALL_PREFIX}/${tool} CACHE STRING "Path to the ${tool} executable")
    get_filename_component(${tool}Path ${TOOL${tool}} REALPATH ABSOLUTE) # Set the real path in the config file
# set(${tool}Path ${TOOL${tool}}) # ${proj}Path = variable changed in the RodentThickness config file (non cache)
    mark_as_advanced(CLEAR TOOL${tool}) # Show the option in the gui
    if(DEFINED TOOL${tool}Sys)
      mark_as_advanced(FORCE TOOL${tool}Sys) # Hide the unuseful option in the gui
    endif()
  endforeach()
endmacro( SetPathsRecompile )

macro( SetPathsSystem )
  foreach( tool ${Tools} )
    get_filename_component(${tool}Path ${TOOL${tool}}Sys REALPATH ABSOLUTE) # Set the real path in the config file
# set(${tool}Path ${TOOL${tool}Sys})
    mark_as_advanced(CLEAR TOOL${tool}Sys) # Show the option in the gui
    if(DEFINED TOOL${tool})
      mark_as_advanced(FORCE TOOL${tool}) # Hide the option in the gui
    endif()
  endforeach()
endmacro( SetPathsSystem )

#===== Macro search tools ===============================================
macro( FindToolsMacro Proj )
  set( AllToolsFound ON )
  foreach( tool ${Tools} )
    find_program( TOOL${tool}Sys ${tool}) # search TOOL${tool}Sys in the PATH
    if(${TOOL${tool}Sys} STREQUAL "TOOL${tool}Sys-NOTFOUND") # If program not found, give a warning message and set AllToolsFound variable to OFF
      message( WARNING "${tool} not found. It will not be recompiled, so either set it to ON, or get ${Proj} manually." )
      set( AllToolsFound OFF )
    endif() # Found on system
  endforeach()
endmacro()


#===== Macro add tool ===============================================
 # if SourceCodeArgs or CMAKE_ExtraARGS passed to the macro as arguments, only the first word is used (each element of the list is taken as ONE argument) => use as "global variables"
macro( AddToolMacro Proj )

  # Update and test tools
  if(COMPILE_EXTERNAL_${Proj}) # If need to recompile, just set the paths here
    SetPathsRecompile() # Uses the list "Tools"
  else(COMPILE_EXTERNAL_${Proj}) # If no need to recompile, search the tools on the system and need to recompile if some tool not found

    # search the tools on the system and warning if not found

   
    FindToolsMacro( ${Proj} )
    
    # If some program not found, reset all tools to the recompiled path and recompile the whole package
    if(NOT AllToolsFound) # AllToolsFound set or reset in FindToolsMacro()
# set( COMPILE_EXTERNAL_${Proj} ON CACHE BOOL "" FORCE)
      SetPathsRecompile() # Uses the list "Tools"
    else()
      SetPathsSystem() # Uses the list "Tools"
    endif()

  endif(COMPILE_EXTERNAL_${Proj})

# After the main if() because we could need to recompile after not having found all tools on system
  if(COMPILE_EXTERNAL_${Proj})
    # Add project
    ExternalProject_Add(${Proj}
      ${SourceCodeArgs} # No difference between args passed separated with ';', spaces or return to line
      BINARY_DIR CorticalRodentThickness-build/${Proj}-build
      SOURCE_DIR CorticalRodentThickness-build/${Proj} # creates the folder if it doesn't exist
      CMAKE_GENERATOR ${gen}
      CMAKE_ARGS
        ${COMMON_BUILD_OPTIONS_FOR_EXTERNALPACKAGES}
       -DCMAKE_LIBRARY_OUTPUT_DIRECTORY:PATH=${CMAKE_CURRENT_BINARY_DIR}/CorticalRodentThickness-build/${Proj}-build/bin
       -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY:PATH=${CMAKE_CURRENT_BINARY_DIR}/CorticalRodentThickness-build/${Proj}-build/bin
       -DCMAKE_RUNTIME_OUTPUT_DIRECTORY:PATH=${CMAKE_CURRENT_BINARY_DIR}/CorticalRodentThickness-build/${Proj}-build/bin
       -DCMAKE_BUNDLE_OUTPUT_DIRECTORY:PATH=${CMAKE_CURRENT_BINARY_DIR}/CorticalRodentThickness-build/${Proj}-build/bin
       -DCMAKE_INSTALL_PREFIX:PATH=CorticalRodentThickness-build/${Proj}-install
       ${CMAKE_ExtraARGS}
      INSTALL_COMMAND "" # So the install step of the external project is not done
    )

    list(APPEND RodentThicknessExternalToolsDependencies ${Proj})

  endif(COMPILE_EXTERNAL_${Proj})
endmacro( AddToolMacro )








