#ifndef SIMG4COMPONENTS_G4GEANTINODEPOSITS_H
#define SIMG4COMPONENTS_G4GEANTINODEPOSITS_H
// Gaudi
#include "GaudiKernel/AlgTool.h"

// FCCSW
#include "SimG4Interface/ISimG4PhysicsList.h"


class SimG4GeantinoDeposits: public AlgTool, virtual public ISimG4PhysicsList {
public:
  explicit SimG4GeantinoDeposits(const std::string& aType , const std::string& aName,
                  const IInterface* aParent);
  virtual ~SimG4GeantinoDeposits();
  /**  Initialize.
   *   @return status code
   */
  virtual StatusCode initialize();
  /**  Finalize.
   *   @return status code
   */
  virtual StatusCode finalize();
  /** Get the physics list.
   *  @return pointer to G4VModularPhysicsList (ownership is transferred to the caller)
   */
  virtual G4VModularPhysicsList* physicsList();

};

#endif /* SIMG4COMPONENTS_G4GEANTINODEPOSITS_H */
