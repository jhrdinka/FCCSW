################################################################################
# Package: TestAlgorithms
################################################################################
gaudi_subdir(TestAlgorithms v1r0)

#this declaration will not be needed in the future

gaudi_depends_on_subdirs(GaudiAlg
			 GaudiKernel
             		 DetectorDescription/DetDesInterfaces
			 RecoGeometry)
			# DetectorDescription/DetDesServices
			# DetectorDescription/Detectors)
			 #DD4hep/v00-09/share/sources/DDCore)

find_package(ROOT COMPONENTS MathCore GenVector Geom)

find_package(DD4hep)


gaudi_add_module(TestAlgorithms
		 src/*.cxx
		 INCLUDE_DIRS Algebra ROOT  DetDesInterfaces DD4hep RecoGeometry GaudiKernel
                 LINK_LIBRARIES GaudiAlgLib DataObjects ROOT GaudiKernel DD4hep RecoGeometry)

	

