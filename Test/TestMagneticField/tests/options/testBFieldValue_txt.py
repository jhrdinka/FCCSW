# This example prints our the magnetic field

from Gaudi.Configuration import *

# Set up the magnetic field service
from Configurables import InterpolatedBFieldSvc
bFieldSvc = InterpolatedBFieldSvc("BFieldSvc", fieldMap = '/eos/experiment/fcc/hh/simulation/MagneticField/FCChhBField_rz.txt', lengthScalor=1000., bFieldScalor= 1., cylinderCoordinates = TRUE, firstOctant=TRUE, OutputLevel = VERBOSE)

# Set up the magnetic field writer
from Configurables import BFieldValueCheck
bFieldValueCheck = BFieldValueCheck("BFieldValueCheck", bFieldSvc=bFieldSvc, fieldMap = '/eos/experiment/fcc/hh/simulation/MagneticField/FCChhBField_rz.root')

# set up the event loop
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [],
               EvtSel = 'NONE',
               EvtMax   = 1,
               # order is important, as GeoSvc is needed by SimG4Svc
               ExtSvc = [bFieldSvc,bFieldValueCheck],
               OutputLevel=VERBOSE
               )
