#include "SimG4ProductionCutRegion.h"

// Geant4
#include "G4LogicalVolume.hh"
#include "G4ProductionCuts.hh"
#include "G4RegionStore.hh"
#include "G4TransportationManager.hh"

#include "GaudiKernel/SystemOfUnits.h"

DECLARE_TOOL_FACTORY(SimG4ProductionCutRegion)

SimG4ProductionCutRegion::SimG4ProductionCutRegion(const std::string& type,
                                                   const std::string& name,
                                                   const IInterface* parent)
    : GaudiTool(type, name, parent) {
  declareInterface<ISimG4RegionTool>(this);
}

SimG4ProductionCutRegion::~SimG4ProductionCutRegion() {}

StatusCode SimG4ProductionCutRegion::initialize() {
  if (GaudiTool::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  if (m_regionNames.size() == 0) {
    error() << "No region name is specified for setting thresholds or limits." << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode SimG4ProductionCutRegion::finalize() { return GaudiTool::finalize(); }

StatusCode SimG4ProductionCutRegion::create() {
  G4LogicalVolume* world =
      (*G4TransportationManager::GetTransportationManager()->GetWorldsIterator())->GetLogicalVolume();
  // count region which are found
  size_t region_counter = 0;

  for (int iter_region = 0; iter_region < world->GetNoDaughters(); ++iter_region) {
    // get region
    auto region = world->GetDaughter(iter_region)->GetLogicalVolume()->GetRegion();
    if (region &&
        std::find(std::begin(m_regionNames), std::end(m_regionNames), region->GetName()) != std::end(m_regionNames)) {
      info() << "Creating production cuts in the region " << region->GetName() << endmsg;
      region_counter++;
      // create cuts for production
      G4ProductionCuts* cuts = new G4ProductionCuts;
      cuts->SetProductionCut(m_gammaCut * CLHEP::mm, G4ProductionCuts::GetIndex("gamma"));
      cuts->SetProductionCut(m_electronCut * CLHEP::mm, G4ProductionCuts::GetIndex("e-"));
      cuts->SetProductionCut(m_positronCut * CLHEP::mm, G4ProductionCuts::GetIndex("e+"));
      // set cuts
      region->SetProductionCuts(cuts);
    }
  }
  return StatusCode::SUCCESS;
}
