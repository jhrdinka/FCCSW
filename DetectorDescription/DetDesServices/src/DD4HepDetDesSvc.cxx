//
//  DD4HepDetDesSvc.cxx
//  
//
//  Created by Julia Hrdinka on 13/10/14.
//
//

#include "DetDesServices/DD4HepDetDesSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/DataObjectHandle.h"


//#include "DDG4/Geant4Kernel.h"

using namespace Gaudi;

DECLARE_COMPONENT(DD4HepDetDesSvc)

DD4HepDetDesSvc::DD4HepDetDesSvc(const std::string& name, ISvcLocator* svc)
: base_class(name, svc),
  m_lcdd(0),
  m_xmlFileName("file:DetectorDescription/Detectors/compact/TestTracker3.xml"),
  m_log(msgSvc(), name)
{
  declareProperty("G4ConverterTool",m_g4Converter);
    declarePublicTool(m_g4Converter, "Geant4GeoConverterTool/Geant4GeoConverterTool");
}

DD4HepDetDesSvc::~DD4HepDetDesSvc() {
    
    if (destroyDetector().isFailure())
        m_log << MSG::WARNING << "Could not call destroyDetector() successfully." << endmsg;
    else
        m_log << MSG::INFO << "destroyDetector() returned SUCCESSFUL." << endmsg;
}

StatusCode
DD4HepDetDesSvc::initialize () {

    // we retrieve the the static instance of the DD4HEP::Geometry
    m_lcdd = &(DD4hep::Geometry::LCDD::getInstance());
    m_lcdd->addExtension<IDetDesSvc>(this);
    
    if (buildDetector().isFailure())
        m_log << MSG::WARNING << "Could not call buildDetector() successfully." << endmsg;
    else
        m_log << MSG::INFO << "buildDetector() returned SUCCESSFUL." << endmsg;
    
    IIncidentSvc* incidentsvc;
    StatusCode sc = service("IncidentSvc", incidentsvc);
    Incident detinc ("DD4HepDetDesSvc","DD4HepDetectorBuild");
    if (sc.isSuccess() ) {
        incidentsvc->fireIncident(detinc);
    }
    
    if (m_g4Converter.retrieve().isFailure())
       m_log << MSG::WARNING << "Retrieving G4Converter failed." << endmsg;
        
    return StatusCode::SUCCESS;
}

StatusCode
DD4HepDetDesSvc::finalize () {
    
    return StatusCode::SUCCESS;
}


void DD4HepDetDesSvc::handle(const Incident& inc) {
    m_log << MSG::INFO << "Handling incident type'" << inc.type() << "'" << endmsg;
    m_log << MSG::INFO << "Incident source '" << inc.source() << "'" << endmsg;
}

StatusCode
DD4HepDetDesSvc::buildDetector () {
   m_log << MSG::INFO << "buildDetector() called." << endmsg;
    
    //load geometry
   m_log << MSG::INFO << "loading geometry from file:  '" << m_xmlFileName << "'" << endmsg;
   char* arg = (char*) m_xmlFileName.c_str();
   m_lcdd->apply("DD4hepXMLLoader", 1, &arg);
   
    m_g4Converter->convert(m_lcdd);
    m_log << MSG::INFO << "AFTER CONVERT" << endmsg;
    
    
  return StatusCode::SUCCESS;
}



StatusCode
DD4HepDetDesSvc::destroyDetector () {

    m_lcdd->destroyInstance();
    return StatusCode::SUCCESS;
}

TGeoManager& DD4HepDetDesSvc::GetTGeo(){
    return (m_lcdd->manager());
}

DD4hep::Geometry::Volume DD4HepDetDesSvc::getWorldVolume()
{
    return (m_lcdd->worldVolume());
}

DD4hep::Geometry::DetElement DD4HepDetDesSvc::getDetWorld()
{
    return (m_lcdd->world());
}


