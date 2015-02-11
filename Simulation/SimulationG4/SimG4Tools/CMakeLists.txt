################################################################################
# Package: SimG4Tools
################################################################################
gaudi_subdir(SimG4Tools v1r0)

gaudi_depends_on_subdirs(GaudiAlg
                         GaudiKernel
			 Simulation/SimulationInterfaces)

find_package(DD4hep)

find_package(Geant4)

gaudi_install_headers(SimG4Tools)

gaudi_add_module(SimG4Tools
                  src/*.cxx
                  INCLUDE_DIRS GaudiKernel DD4hep SimulationInterfaces Geant4
                  LINK_LIBRARIES GaudiKernel DD4hep Geant4)