#include "ExtrapolationTest.h"

DECLARE_COMPONENT(ExtrapolationTest)

ExtrapolationTest::ExtrapolationTest(const std::string& name, ISvcLocator* svcLoc)
    : GaudiAlgorithm(name, svcLoc)

{
  declareProperty("extrapolationTool", m_extrapolationTool);
  declareProperty("nEvents", m_nEvents, "The number of Events ");
  declareProperty("etaMin", m_etaMin = -5., "Minimum in eta of direction");
  declareProperty("etaMax", m_etaMax = 5., "Maximum in eta of direction");
  declareProperty("phiMin", m_phiMin = -M_PI, "Minimum in phi of direction");
  declareProperty("phiMax", m_phiMax = M_PI, "Maximum in phi of direction");
}

StatusCode ExtrapolationTest::initialize() {
  if (GaudiAlgorithm::initialize().isFailure()) return StatusCode::FAILURE;
  IRndmGenSvc* randSvc = svc<IRndmGenSvc>("RndmGenSvc", true);
  if (m_flatGenerator.initialize(randSvc, Rndm::Flat(0., 1.)).isFailure()) return StatusCode::FAILURE;
  if (!m_extrapolationTool.retrieve()) {
    error() << "Extrapolation tool cannot be retieved" << endmsg;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode ExtrapolationTest::execute() {
  // generate the random momentum within the given ranges
  double eta = m_etaMin + m_flatGenerator() * fabs(m_etaMax - m_etaMin);
  double phi = m_phiMin + m_flatGenerator() * fabs(m_phiMax - m_phiMin);
  double theta = 2. * atan(exp(-eta));
  fcc::Vector3D momentum(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
  // extrapolate the trackthrough the tracking geometry
  m_extrapolationTool->extrapolate(fcc::Vector3D(0., 0., 0.), momentum, 0.);

  return StatusCode::SUCCESS;
}

StatusCode ExtrapolationTest::finalize() {
  if (GaudiAlgorithm::finalize().isFailure()) return StatusCode::FAILURE;

  return StatusCode::SUCCESS;
}
