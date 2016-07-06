#include "SimG4GeantinoDeposits.h"

// Geant4
#include "G4VModularPhysicsList.hh"
#include "MyGeantinoDeposits.h"

DECLARE_TOOL_FACTORY(SimG4GeantinoDeposits)

SimG4GeantinoDeposits::SimG4GeantinoDeposits(const std::string& aType, const std::string& aName, const IInterface* aParent) :
  AlgTool(aType, aName, aParent) {
  declareInterface<ISimG4PhysicsList>(this);
}

SimG4GeantinoDeposits::~SimG4GeantinoDeposits() {}

StatusCode SimG4GeantinoDeposits::initialize() {
  return AlgTool::initialize();
}

StatusCode SimG4GeantinoDeposits::finalize() {
  return AlgTool::finalize();
}

G4VModularPhysicsList* SimG4GeantinoDeposits::physicsList() {
   // ownership passed to SimG4Svc which will register it in G4RunManager. To be deleted in ~G4RunManager()
  return new MyPhysicsList;
}

