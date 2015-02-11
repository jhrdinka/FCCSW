
from Gaudi.Configuration import *
from Configurables import ApplicationMgr, TestAlgorithm, DD4HepDetDesSvc, Geant4GeoConverterTool

geant4  = Geant4GeoConverterTool("Geant4GeoConverterTool")

detservice = DD4HepDetDesSvc("DD4HepDetDesSvc", OutputLevel = VERBOSE, G4ConverterTool = geant4)

test = TestAlgorithm("TestAlgorithm")

ApplicationMgr(EvtSel='NONE',
               EvtMax=1,
               ExtSvc = [detservice],
               TopAlg=[test])
