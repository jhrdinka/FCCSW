from Gaudi.Configuration import *
## create DD4hep geometry
from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=['file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
                                         'file:Detector/DetFCChhTrackerTkLayout/compact/Tracker.xml'])
## create tracking geometry
from Configurables import TrackingGeoSvc
trkgeoservice = TrackingGeoSvc("TrackingGeometryService")

## generate constant magnetic field
from Configurables import ConstantBFieldSvc
bFieldSvc = ConstantBFieldSvc("ConstBFieldSvc")
bFieldSvc.bFieldValuesXYZ = [0.,0.,0.]

## configure the extrapolation tool
from Configurables import ExtrapolationTool
extrapolationTool = ExtrapolationTool("ExtrapolationTool")
extrapolationTool.trackingGeometrySvc   = trkgeoservice
extrapolationTool.magneticFieldSvc      = bFieldSvc
extrapolationTool.collectSensitive      = TRUE
extrapolationTool.collectPassive        = TRUE
extrapolationTool.collectBoundary       = TRUE
extrapolationTool.collectMaterial       = TRUE
extrapolationTool.sensitiveCurvilinear  = FALSE
extrapolationTool.searchMode            = 1
extrapolationTool.pathLimit             = -1.

## configure the extrapolation test
from Configurables import ExtrapolationTest
extrapolationTest = ExtrapolationTest("ExtrapolationTest")
extrapolationTest.extrapolationTool = extrapolationTool
extrapolationTest.nEvents   = 1.
extrapolationTest.etaMin    = 0.
extrapolationTest.etaMax    = 0.
extrapolationTest.phiMin    = 0.
extrapolationTest.phiMax    = 0.

## configure run
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [extrapolationTest],
               EvtSel = 'NONE',
               EvtMax   = 1,
               ExtSvc = [geoservice, trkgeoservice, bFieldSvc],
               OutputLevel=VERBOSE
               )



