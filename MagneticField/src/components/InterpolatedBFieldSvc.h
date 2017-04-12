#ifndef MAGNETICFIELD_INTERPOLATEDBFIELDSVC_H
#define MAGNETICFIELD_INTERPOLATEDBFIELDSVC_H

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Service.h"
#include "MagneticField/IBFieldSvc.h"

/** @class InterpolatedBFieldSvc InterpolatedBFieldSvc.h
 * MagneticField/InterpolatedBFieldSvc.h
 *
 *	This service is the implementation of the IBFieldSvc for magnetic fiel
 *  read in from a map. It provides the magnetic field value at any given point
 *  in the detector using tri-linear interpolation. This service is a wrapper
 *  which internally uses the Acts::InterpolatedBField in
 *  ACTS/MagneticField/InterpolatedBField.
 *
 *  @note currently the calculation of the derivatives is not implemented on the
 *  ACTS::ConstantBField
 *
 *  @author Julia Hrdinka
 *  @date 2017-03
 */

namespace Acts {
class InterpolatedBFieldMap;
}

class InterpolatedBFieldSvc : public extends1<Service, IBFieldSvc> {
 public:
  /// Default constructor
  InterpolatedBFieldSvc(const std::string& name, ISvcLocator* svc);

  /// Destructor
  virtual ~InterpolatedBFieldSvc();
  /// Initialize function
  virtual StatusCode initialize();
  /// Finalize function
  virtual StatusCode finalize();
  /// @copydoc IBFieldSvc::getField(const fcc::Vector3D)
  virtual fcc::Vector3D getField(const fcc::Vector3D& position) const final;
  /// @copydoc IBFieldSvc::getField(const fcc::Vector3D&,fcc::Vector3D)
  virtual fcc::Vector3D getFieldGradient(const fcc::Vector3D& pos,
                                         fcc::MatrixD<3, 3>& deriv) const final;

 private:
  /// unique pointer to the internally used Acts::InterpolatedBField
  std::unique_ptr<Acts::InterpolatedBFieldMap> m_actsBField;
  /// @brief global B-field scaling factor
  ///
  /// @note Negative values for @p scale are accepted and will invert the
  ///       direction of the magnetic field.
  double m_scale;
  /// @brief path to file containing field map
  ///
  /// @note For the description of the format of such a file, please refer to
  /// the documentation for InterpolatedBFieldMap::InterpolatedBFieldMap.
  std::string m_fieldMapFile;
  /// length unit used in field map
  double m_lengthUnit;
  /// magnetic field unit used in field map
  double m_BFieldUnit;
  /// @brief number of grid points in field map
  ///
  /// @note This information is only used as a hint for the required size of
  ///       the internal vectors. A correct value is not needed, but will help
  ///       to speed up the field map initialization process.
  size_t m_nPoints;
};

#endif  // MAGNETICFIELD_INTERPOLATEDBFIELDSVC_H
