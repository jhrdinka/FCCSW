#ifndef MAGNETICFIELD_CONSTANTBFIELDSVC_H
#define MAGNETICFIELD_CONSTANTBFIELDSVC_H

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Service.h"
#include "MagneticField/IBFieldSvc.h"

/** @class ConstantBFieldSvc ConstantBFieldSvc.h
 * MagneticField/ConstantBFieldSvc.h
 *
 *	This service is the implementation of the IBFieldSvc for a constant
 *  magnetic field. It provides the magnetic field value at any given point in
 *  the detector. This service is a wrapper which internally uses the
 *  Acts::ConstantBField in ACTS/MagneticField/ConstantBField.
 *
 *  @note currently the calculation of the derivatives is not implemented on the
 *  ACTS::ConstantBField
 *
 *  @author Julia Hrdinka
 *  @date 2017-03
 */

namespace Acts {
class ConstantBField;
}

class ConstantBFieldSvc : public extends1<Service, IBFieldSvc> {
 public:
  /// Default constructor
  ConstantBFieldSvc(const std::string& name, ISvcLocator* svc);

  /// Destructor
  virtual ~ConstantBFieldSvc();
  /// Initialize function
  virtual StatusCode initialize();
  /// Finalize function
  virtual StatusCode finalize();
  /// @copydoc IBFieldSvc::getField(const fcc::Vector3D&)
  virtual fcc::Vector3D getField(const fcc::Vector3D& position) const final;
  /// @copydoc IBFieldSvc::getField(const fcc::Vector3D&,fcc::Vector3D)
  virtual fcc::Vector3D getFieldGradient(const fcc::Vector3D& pos,
                                         fcc::MatrixD<3, 3>& deriv) const final;

 private:
  /// unique pointer to the internally used Acts::ConstantBField
  std::unique_ptr<Acts::ConstantBField> m_actsBField;
  /// the values of the magnetic field in cartesian coordinates xyz
  std::vector<float> m_bFieldValuesXYZ;
};

#endif  // MAGNETICFIELD_CONSTANTBFIELDSVC_H
