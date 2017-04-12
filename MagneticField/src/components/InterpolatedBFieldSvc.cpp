#include "InterpolatedBFieldSvc.h"
#include "ACTS/MagneticField/InterpolatedBFieldMap.hpp"

DECLARE_SERVICE_FACTORY(InterpolatedBFieldSvc)

InterpolatedBFieldSvc::InterpolatedBFieldSvc(const std::string& name,
                                             ISvcLocator* svc)
    : base_class(name, svc),
      m_actsBField(nullptr),
      m_scale(1.),
      m_fieldMapFile(""),
      m_lengthUnit(Acts::units::_mm),
      m_BFieldUnit(Acts::units::_T),
      m_nPoints(1000) {
  declareProperty("scale", m_scale, "global B-field scaling factor");
  declareProperty("fieldMap", m_fieldMapFile,
                  "path to file containing field map");
  declareProperty("lengthUnit", m_lengthUnit, "length unit used in field map");
  declareProperty("BFieldUnit", m_BFieldUnit,
                  "magnetic field unit used in field map");
  declareProperty("nPoints", m_nPoints, "number of grid points in field map");
}

InterpolatedBFieldSvc::~InterpolatedBFieldSvc() {}

StatusCode InterpolatedBFieldSvc::initialize() {
  if (Service::initialize().isFailure()) {
    error() << "Unable to initialize Service()" << endmsg;
    return StatusCode::FAILURE;
  }
  // configure the ACTS InterpolatedBField
  auto bFieldConfig = Acts::InterpolatedBFieldMap::Config();
  bFieldConfig.scale = m_scale;
  bFieldConfig.fieldMapFile = m_fieldMapFile;
  bFieldConfig.lengthUnit = m_lengthUnit;
  bFieldConfig.BFieldUnit = m_BFieldUnit;
  bFieldConfig.nPoints = m_nPoints;

  // set the interpolated magnetic field of the ACTS magnetic field service

  m_actsBField = std::make_unique<Acts::InterpolatedBFieldMap>(bFieldConfig);

  if (!m_actsBField) {
    error() << "Unable to initialize InterpolatedBFieldSvc!" << endmsg;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

fcc::Vector3D InterpolatedBFieldSvc::getField(
    const fcc::Vector3D& position) const {
  return m_actsBField->getField(position);
}

fcc::Vector3D InterpolatedBFieldSvc::getFieldGradient(
    const fcc::Vector3D& pos, fcc::MatrixD<3, 3>& deriv) const {
  return m_actsBField->getFieldGradient(pos, deriv);
}

StatusCode InterpolatedBFieldSvc::finalize() { return StatusCode::SUCCESS; }
