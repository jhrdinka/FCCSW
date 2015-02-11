//
//  DD4HepDetDesSvc.h
//  
//
//  Created by Julia Hrdinka on 13/10/14.
//
//

#ifndef _DD4HepDetDesSvc_h
#define _DD4HepDetDesSvc_h

#include "DD4hep/LCDD.h"
#include "GaudiKernel/Service.h"
#include "DetDesInterfaces/IDetDesSvc.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/ToolHandle.h"
#include "SimulationInterfaces/IGeoConverterTool.h"
#include "SimFastTools/RecoGeoConverterTool.h"
#include "SimG4Tools/Geant4GeoConverterTool.h"

#include <string>

//forward declaration
//class IGeoConverterTool;
class TGeoManager;
//class VolumeManager;
class Handle;

class DetElement;

typedef DD4hep::Geometry::VolumeManager VolumeManager;


class DD4HepDetDesSvc: public extends2<Service, IDetDesSvc, IIncidentListener> {

 public:
    //standard constructor
    DD4HepDetDesSvc (const std::string& name, ISvcLocator* svc);
    
    //destructor
    virtual ~DD4HepDetDesSvc ();
    
    virtual StatusCode initialize ();
    virtual StatusCode finalize ();
    
    void handle(const Incident&);
    
    virtual StatusCode buildDetector ();
    virtual StatusCode destroyDetector ();
    
    virtual TGeoManager& GetTGeo();
    
    virtual DD4hep::Geometry::Volume getWorldVolume();
    
    virtual DD4hep::Geometry::DetElement getDetWorld();
      
protected:
    //Detector Description Object    
    ToolHandle <IGeoConverterTool> m_g4Converter;
    
    
    DD4hep::Geometry::LCDD* m_lcdd;
    std::string   m_xmlFileName;
    MsgStream m_log;

};


#endif
