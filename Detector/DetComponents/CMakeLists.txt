################################################################################
# Package: DetComponents
################################################################################
gaudi_subdir(DetComponents v1r0)

gaudi_depends_on_subdirs(GaudiKernel
			 Detector/DetInterface)

find_package(ROOT COMPONENTS MathCore GenVector Geom)

find_package(DD4hep COMPONENTS DDG4)

#find_package(Geant4)

find_package(ACTS)
find_package(Eigen)
add_definitions(-DACTS_PARAMETER_DEFINITIONS_PLUGIN=\"ACTS/Utilities/detail/DefaultParameterDefinitions.hpp\")

gaudi_install_headers(DetComponents)

gaudi_add_module(DetComponents
		 src/*.cpp
		 INCLUDE_DIRS GaudiKernel ROOT DD4hep DetInterfaces Eigen ACTS #Geant4
		 LINK_LIBRARIES GaudiKernel ROOT DD4hep ${DD4hep_DDG4_LIBRARY} ACTS::ACTSCore ACTS::ACTSDD4hepPlugin )#Geant4)
