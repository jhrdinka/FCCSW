/** @class IBFieldSvc MagneticField/MagneticField/IBFieldSvc.h
 * IBFieldSvc.h
 *
 *  Abstract interface for the magnetic field service. It provides the magnetic
 *  field value at any given point in the detector.
 *
 *
 *  @author Julia Hrdinka
 *  @date 2015-06
 */

#ifndef MAGNETICFIELD_IBFIELDSVC_H
#define MAGNETICFIELD_IBFIELDSVC_H

#include "GaudiKernel/IService.h"
#include "Math/Math.h"

class GAUDI_API IBFieldSvc : virtual public IService {
 public:
  /// InterfaceID
  DeclareInterfaceID(IBFieldSvc, 1, 0);

  /// @brief retrieve magnetic field value
  ///
  /// @param [in] pos global position
  ///
  /// @return magnetic field vector at given position
  virtual fcc::Vector3D getField(const fcc::Vector3D& position) const = 0;

  /// @brief retrieve magnetic field value
  ///
  /// @param [in]  pos  global position
  /// @param [out] deriv gradient of magnetic field vector as (3x3) matrix
  /// @return magnetic field vector
  ///
  /// @note The position @p pos is ignored and only kept as argument to provide
  ///       a consistent interface with other magnetic field services.
  /// @note currently the derivative is not calculated
  /// @todo return derivative
  virtual fcc::Vector3D getFieldGradient(const fcc::Vector3D& pos,
                                         fcc::MatrixD<3, 3>& deriv) const = 0;
  /// virtual desctructor
  virtual ~IBFieldSvc() {}
};

#endif  // MAGNETICFIELD_IBFIELDSVC_H
