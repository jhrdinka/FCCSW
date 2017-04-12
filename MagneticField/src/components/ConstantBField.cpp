#include "ACTS/MagneticField/ConstantBField.hpp"
#include "ACTS/Utilities/Units.hpp"
#include "ConstantBFieldSvc.h"

DECLARE_SERVICE_FACTORY(ConstantBFieldSvc)

ConstantBFieldSvc::ConstantBFieldSvc(const std::string& name, ISvcLocator* svc)
    : base_class(name, svc), m_actsBField(nullptr), m_bFieldValuesXYZ() {
  declareProperty("bFieldValuesXYZ", m_bFieldValuesXYZ,
                  "Magnetic field Values in cartesian coordinates xyz.");
}

ConstantBFieldSvc::~ConstantBFieldSvc() {}

StatusCode ConstantBFieldSvc::initialize() {
  if (Service::initialize().isFailure()) {
    error() << "Unable to initialize Service()" << endmsg;
    return StatusCode::FAILURE;
  }
  // set the constant magnetic field of the ACTS magnetic field service
  if (m_bFieldValuesXYZ.size() == 3) {
    m_actsBField = std::make_unique<Acts::ConstantBField>(
        m_bFieldValuesXYZ[0] * Acts::units::_T,
        m_bFieldValuesXYZ[1] * Acts::units::_T,
        m_bFieldValuesXYZ[2] * Acts::units::_T);
  } else {
    error()
        << "Unable to initialize ConstantBFieldSvc: input for BFieldValues "
           "has wrong size - needs to be 3 for the cartesian coordinates xyz!"
        << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

fcc::Vector3D ConstantBFieldSvc::getField(const fcc::Vector3D& position) const {
  return m_actsBField->getField(position);
}

fcc::Vector3D ConstantBFieldSvc::getFieldGradient(
    const fcc::Vector3D& pos, fcc::MatrixD<3, 3>& deriv) const {
  return m_actsBField->getFieldGradient(pos, deriv);
}

StatusCode ConstantBFieldSvc::finalize() { return StatusCode::SUCCESS; }
