#include "ExtrapolationTest.h"
#include "ACTS/Extrapolation/ExtrapolationEngine.hpp"
#include "ACTS/Extrapolation/IExtrapolationEngine.hpp"
#include "ACTS/Extrapolation/ExtrapolationEngine.hpp"
#include "ACTS/Extrapolation/RungeKuttaEngine.hpp"
#include "ACTS/Extrapolation/MaterialEffectsEngine.hpp"
#include "ACTS/Extrapolation/StaticEngine.hpp"
#include "ACTS/Extrapolation/StaticNavigationEngine.hpp"
#include "ACTS/EventData/TrackParameters.hpp"
#include "ACTS/EventData/NeutralParameters.hpp"
#include "ACTS/EventData/ParticleDefinitions.hpp"
#include "ACTS/Utilities/Definitions.hpp"
#include "ACTS/Surfaces/PerigeeSurface.hpp"

#include "ConstantFieldSvc.hpp"


DECLARE_ALGORITHM_FACTORY(ExtrapolationTest)

ExtrapolationTest::ExtrapolationTest(const std::string& aName, ISvcLocator* aSvcLoc):
  GaudiAlgorithm(aName, aSvcLoc) {}

ExtrapolationTest::~ExtrapolationTest() {}

StatusCode ExtrapolationTest::initialize() {
  m_geoSvc = service("TrackingGeometryService");
  if (! m_geoSvc) {
    error() << "Unable to locate Tracking Geometry Service" << endmsg;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}


StatusCode ExtrapolationTest::finalize() {
  StatusCode sc = GaudiAlgorithm::finalize();
  return sc;
}

StatusCode ExtrapolationTest::execute() {
    // get tracking geometry from service
    std::shared_ptr<Acts::TrackingGeometry> trkgeo = m_geoSvc->trackingGeometry();

    // random values for parameters
    double d0  = 1.;// drawGauss(m_cfg.d0Defs);
    double z0  = 1.;// drawGauss(m_cfg.z0Defs);
    double phi = 1.;// drawUniform(m_cfg.phiRange);
    double eta = 1.;// drawUniform(m_cfg.etaRange);
    double theta = 2.*atan(exp(-eta));
    double pt  = 10000.;// drawUniform(m_cfg.ptRange);
    double p   =  pt/sin(theta);
    double q   = 1.;// drawUniform({{0.,1.}}) > 0.5 ? 1. : -1.;
    Acts::Vector3D momentum(p*sin(theta)*cos(phi), p*sin(theta)*sin(phi), p*cos(theta));
    std::unique_ptr<Acts::ActsSymMatrixD<5> > cov;
    Acts::ActsVectorD<5> pars; pars << d0, z0, phi, theta, q/p;
    Acts::PerigeeSurface pSurface(Acts::Vector3D(0.,0.,0.));
    
    // hard-code settings for now
    // neutral extrapolation
    Acts::BoundParameters startParameters(std::move(cov),std::move(pars),pSurface);
    Acts::ExtrapolationCell<Acts::TrackParameters> ecc(startParameters);
    //ecc.setParticleHypothesis(m_cfg.particleType);
    ecc.addConfigurationMode(Acts::ExtrapolationMode::StopAtBoundary);
    ecc.addConfigurationMode(Acts::ExtrapolationMode::FATRAS);
    ecc.searchMode = -1;
    // now set the behavioral bits
    ecc.addConfigurationMode(Acts::ExtrapolationMode::CollectSensitive);
    ecc.addConfigurationMode(Acts::ExtrapolationMode::CollectMaterial);
    
    // force a stop in the extrapolation mode
    double pathLimit = -1;
    if (pathLimit > 0.) {
        ecc.pathLimit = pathLimit;
        ecc.addConfigurationMode(Acts::ExtrapolationMode::StopWithPathLimit);
     }
    FWE::ConstantFieldSvc::Config cffConfig;
    cffConfig.name = "ConstantMagField";
    cffConfig.field = {{ 0., 0., 20000. }};
    std::shared_ptr<Acts::IMagneticFieldSvc> magFieldSvc(new FWE::ConstantFieldSvc(cffConfig));
    
    // EXTRAPOLATOR - set up the extrapolator
    // (a) RungeKuttaPropagtator
    Acts::RungeKuttaEngine::Config rungeKuttaConfig("RungeKuttaEngine");
    rungeKuttaConfig.fieldService = magFieldSvc;
    std::shared_ptr<Acts::IPropagationEngine> rungeKuttaEngine(new Acts::RungeKuttaEngine(rungeKuttaConfig));
    // (b) MaterialEffectsEngine
    Acts::MaterialEffectsEngine::Config materialEffectsConfig("MaterialEffectsEngine");
    std::shared_ptr<Acts::IMaterialEffectsEngine> materialEffects(new Acts::MaterialEffectsEngine(materialEffectsConfig));
    // (c) StaticNavigationEngine
    Acts::StaticNavigationEngine::Config staticNavigatorConfig("StaticNavigator", Acts::Logging::INFO);
    staticNavigatorConfig.propagationEngine = rungeKuttaEngine;
    staticNavigatorConfig.trackingGeometry  = trkgeo;
    std::shared_ptr<Acts::INavigationEngine> staticNavigator(new Acts::StaticNavigationEngine(staticNavigatorConfig));
    // (d) the StaticEngine
    Acts::StaticEngine::Config staticEngineConfig("StaticEngine", Acts::Logging::INFO);
    staticEngineConfig.propagationEngine     = rungeKuttaEngine;
    staticEngineConfig.materialEffectsEngine = materialEffects;
    staticEngineConfig.navigationEngine      = staticNavigator;
    std::shared_ptr<Acts::IExtrapolationEngine> staticEngine(new Acts::StaticEngine(staticEngineConfig));
    // (e) the material engine
    Acts::ExtrapolationEngine::Config extrapolationEngineConfig("ExtrapolationEngine", Acts::Logging::INFO);
    extrapolationEngineConfig.navigationEngine = staticNavigator;
    extrapolationEngineConfig.extrapolationEngines = { staticEngine };
    extrapolationEngineConfig.propagationEngine    = rungeKuttaEngine;
    extrapolationEngineConfig.trackingGeometry     = trkgeo;
    std::shared_ptr<Acts::IExtrapolationEngine> extrapolationEngine(new Acts::ExtrapolationEngine(extrapolationEngineConfig));
    Acts::ExtrapolationCode eCode = extrapolationEngine->extrapolate(ecc);
    info() << "Extrapolation finished with code " << "\t" << "Extrapolation steps: " << ecc.navigationStep << endmsg;
    for (auto& es : ecc.extrapolationSteps) {
      const Acts::TrackParameters& pars = *es.parameters;
      info() << pars.position().z() << endmsg;
    }
    info() << "Extrapolation done!" << "\t" << "Extrapolation time: " << ecc.time << endmsg;

  return StatusCode::SUCCESS;
}
