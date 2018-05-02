#ifndef SIMG4FULL_SIMG4PRODUCTIONCUTREGION_H
#define SIMG4FULL_SIMG4PRODUCTIONCUTREGION_H

#include <cfloat>

// Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/ToolHandle.h"

// FCCSW
#include "SimG4Interface/ISimG4ParticleSmearTool.h"
#include "SimG4Interface/ISimG4RegionTool.h"

// Geant
#include "G4UserLimits.hh"
class G4Region;

/** @class SimG4ProductionCutRegion SimG4Full/src/components/SimG4ProductionCutRegion.h SimG4ProductionCutRegion.h
 *
 *  Tool for creating production cuts in given regions.
 *
 *  @author Julia Hrdinka
*/

class SimG4ProductionCutRegion : public GaudiTool, virtual public ISimG4RegionTool {
public:
  explicit SimG4ProductionCutRegion(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~SimG4ProductionCutRegion();
  /**  Initialize.
   *   @return status code
   */
  virtual StatusCode initialize() final;
  /**  Finalize.
   *   @return status code
   */
  virtual StatusCode finalize() final;
  /**  Create regions and fast simulation models
   *   @return status code
   */
  virtual StatusCode create() final;

private:
  /// Names of the regions where production thresholds should be applied
  Gaudi::Property<std::vector<std::string>> m_regionNames{this, "regionNames", {}, "Names of the regions."};
  /// range to be traversed below which no secondary will be generated for gammas (default is Geant4 default)
  Gaudi::Property<double> m_gammaCut{this, "gammaCut", 0.7,
                                     "Production cut for gammas in mm, default is Geant4 default of 0.7mm."};
  /// range to be traversed below which no secondary will be generated for electrons (default is Geant4 default)
  Gaudi::Property<double> m_electronCut{this, "electronCut", 0.7,
                                        "Production cut for electrons in mm, default is Geant4 default of 0.7mm."};
  /// range to be traversed below which no secondary will be generated for positron (default is Geant4 default)
  Gaudi::Property<double> m_positronCut{this, "positronCut", 0.7,
                                        "Production cut for positrons in mm, default is Geant4 default of 0.7mm."};
};

#endif /* SIMG4FULL_SIMG4PRODUCTIONCUTREGION_H */
