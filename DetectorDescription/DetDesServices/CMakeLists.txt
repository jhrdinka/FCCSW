################################################################################
# Package: DetDesServices
################################################################################
gaudi_subdir(DetDesServices v1r0)

gaudi_depends_on_subdirs(GaudiKernel
			 DetectorDescription/DetDesInterfaces
			 Simulation/SimulationInterfaces
			 Simulation/SimulationFast/SimFastTools
			 Simulation/SimulationG4/SimG4Tools)

find_package(DD4hep)

gaudi_add_module(DetDesServices
		 src/*.cxx
		 INCLUDE_DIRS DetDesInterfaces SimulationInterfaces DD4hep SimFastTools SimG4Tools
		 LINK_LIBRARIES GaudiKernel DD4hep)
