#include "TrackingGeoSvc.h"

//ACTS
#include "ACTS/Detector/TrackingGeometry.hpp"
#include "ACTS/Tools/CylinderVolumeHelper.hpp"
#include "ACTS/Tools/CylinderVolumeBuilder.hpp"
#include "ACTS/Plugins/DD4hepPlugins/DD4hepCylinderGeometryBuilder.hpp"

using namespace Gaudi;

DECLARE_SERVICE_FACTORY(TrackingGeoSvc)

TrackingGeoSvc::TrackingGeoSvc(const std::string& name, ISvcLocator* svc) :
base_class(name, svc),
m_geoSvc("",name),
m_log(msgSvc(), name),
m_trackingGeo(nullptr)
{
    declareProperty("GeometryService", m_geoSvc);
}

TrackingGeoSvc::~TrackingGeoSvc()
{}

StatusCode TrackingGeoSvc::initialize()
{
    m_log << MSG::INFO << "TrackingGeoSvc initialize()" << endmsg;
    StatusCode sc = Service::initialize();
    if(m_geoSvc.retrieve()==StatusCode::FAILURE) {
        m_log << MSG::ERROR << "Could not retrieve DD4hep geometry" << endmsg;
        return StatusCode::FAILURE;
    }
    // configure the cylinder volume helper
    Acts::CylinderVolumeHelper::Config cvhConfig;
    auto cylinderVolumeHelper = std::make_shared<Acts::CylinderVolumeHelper>(cvhConfig);
    // configure the volume builder
    Acts::CylinderVolumeBuilder::Config pvbConfig;
    pvbConfig.trackingVolumeHelper              = cylinderVolumeHelper;
    auto cylinderVolumeBuilder = std::make_shared<Acts::CylinderVolumeBuilder>(pvbConfig);
    // configure geometry builder with the surface array creator
    Acts::DD4hepCylinderGeometryBuilder::Config cgConfig;
    cgConfig.detWorld = m_geoSvc->getDD4HepGeo();
    cgConfig.volumeHelper = cylinderVolumeHelper;
    cgConfig.volumeBuilder = cylinderVolumeBuilder;
    auto geometryBuilder = std::make_shared<Acts::DD4hepCylinderGeometryBuilder>(cgConfig);
    // set the tracking geometry
    m_trackingGeo = (std::move(geometryBuilder->trackingGeometry()));
    
    m_log << MSG::INFO << "TrackingGeoSvc initialize end" << endmsg;
    return StatusCode::SUCCESS;
}

StatusCode TrackingGeoSvc::finalize() {
    return StatusCode::SUCCESS;
}

/** Query the interfaces. */
/*StatusCode TrackingGeoSvc::queryInterface(const InterfaceID& riid, void** ppvInterface)
 {
 if ( Acts::IID_IDD4hepGeometrySvc == riid )
 *ppvInterface = (ITrackingGeoSvc*)this;
 else  {
 // Interface is not directly available: try out a base class
 return Service::queryInterface(riid, ppvInterface);
 }
 addRef();
 return StatusCode::SUCCESS;
 }*/
