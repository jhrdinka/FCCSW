################################################################################
# Package: MagneticField
################################################################################

gaudi_subdir(MagneticField v1r0)

gaudi_depends_on_subdirs(GaudiKernel
 						 Math)

find_package(ACTS COMPONENTS Core)
#workaround for the lone -lGeom flag exported by ACTS
link_directories(${ROOT_INCLUDE_DIR}/../lib)

gaudi_install_headers(MagneticField)

gaudi_add_module(MagneticField
                 src/components/*.cpp
                 INCLUDE_DIRS GaudiKernel ACTS Math
                 LINK_LIBRARIES GaudiKernel ACTS::ACTSCore)
