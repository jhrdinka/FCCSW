import os
from Gaudi.Configuration import *

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=['Detector/DetFCChhTrackerSimple/compact/FCCTracker.xml'], OutputLevel = DEBUG)

from Configurables import TrackingGeoSvc
trkgeoservice = TrackingGeoSvc("TrackingGeometryService")#, GeometryService=geoservice) 

from Configurables import SimG4Svc
geantservice = SimG4Svc("SimG4Svc",
                        detector='SimG4DD4hepDetector',
                        physicslist="SimG4FtfpBert",
                        actions="SimG4FullSimActions")

export_fname = "TestBox.gdml"
# check if file exists and delete it:
if os.path.isfile(export_fname):
    os.remove(export_fname)


from Configurables import GeoToGdmlDumpSvc
geodumpservice = GeoToGdmlDumpSvc("GeoDump", gdml=export_fname)

from Configurables import TrkGeoToGdmlDumpSvc
trkgeodumpservice = TrkGeoToGdmlDumpSvc("TrkGeoDump", gdml="TrkGeoDump.gdml")

from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [],
                EvtSel = 'NONE',
                EvtMax   = 1,
                # order is important, as GeoSvc is needed by SimG4Svc
                ExtSvc = [geoservice, geantservice, trkgeoservice, geodumpservice, trkgeodumpservice],
                #ExtSvc = [trkgeodumpservice],
                OutputLevel=DEBUG
 )
