#ifndef EXAMPLES_EXTRAPOLATIONTEST_H
#define EXAMPLES_EXTRAPOLATIONTEST_H

#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/ToolHandle.h"
#include "Math/Math.h"
#include "RecInterface/IExtrapolationTool.h"

/** @class ExtrapolationTest
 *
 *  An algorithm testing the ExtrapolationTool in Reconstruction/RecTools.
 *  It generates a random momentum in a given range and calls the extrapolation
 *   of the ExtrapolationTool which does the extrapolation through the Tracking
 *   geometry.
 *
 *
 *  @author Julia Hrdinka
 *  @date   2017-02
 */

class ExtrapolationTest : public GaudiAlgorithm {
  friend class AlgFactory<ExtrapolationTest>;

public:
  /// Constructor.
  ExtrapolationTest(const std::string& name, ISvcLocator* svcLoc);
  /// Initialize.
  virtual StatusCode initialize();
  /// Execute.
  virtual StatusCode execute();
  /// Finalize.
  virtual StatusCode finalize();

private:
  /// the extrapolation tool
  ToolHandle<IExtrapolationTool> m_extrapolationTool;
  /// the number of events to be executed
  size_t m_nEvents;
  /// the random numbers generator
  Rndm::Numbers m_flatGenerator;
  /// minimum in eta for random creation of momentum
  double m_etaMin;
  /// maximum in eta for random creation of momentum
  double m_etaMax;
  /// minimum in theta for random creation of momentum
  double m_phiMin;
  /// maximum in theta for random creation of momentum
  double m_phiMax;
};

#endif /* EXAMPLES_EXTRAPOLATIONTEST_H */
