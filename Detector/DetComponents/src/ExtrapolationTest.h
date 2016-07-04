#ifndef EXTRAPOLATIONTEST_H
#define EXTRAPOLATIONTEST_H

#include "GaudiAlg/GaudiAlgorithm.h"
#include "TrackingGeoSvc.h"

class ExtrapolationTest: public GaudiAlgorithm {
public:
  explicit ExtrapolationTest(const std::string&, ISvcLocator*);
  virtual ~ExtrapolationTest();
  virtual StatusCode initialize() final;
  virtual StatusCode execute() final;
  virtual StatusCode finalize() final;
private:
 SmartIF<ITrackingGeoSvc> m_geoSvc;
};

#endif /* EXTRAPOLATIONTEST_H */

