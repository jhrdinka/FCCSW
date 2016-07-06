
#include "G4Geantino.hh"
#include "G4VUserPhysicsList.hh"
#include "G4VModularPhysicsList.hh"
#include "G4VProcess.hh"
#include "G4VContinuousProcess.hh"
#include "G4ProcessManager.hh"
#include "G4ProcessType.hh"


class MyPhysicsProcess: public G4VContinuousProcess {
  public:

  MyPhysicsProcess(const G4String& processName ="MyProcess",
               G4ProcessType type = fUserDefined)
             : G4VContinuousProcess(processName, type)
             {
                       G4cout << GetProcessName() << " is created " << G4endl;
             }


  G4VParticleChange* AlongStepDoIt(const G4Track& aTrack,
           const G4Step&  aStep) {
   aParticleChange.Initialize(aTrack);
   G4double kinEnergyStart = aTrack.GetKineticEnergy();  
  G4double edepo = kinEnergyStart * 0.01;
  aParticleChange.ClearDebugFlag();
  aParticleChange.ProposeLocalEnergyDeposit( edepo );
  aParticleChange.SetNumberOfSecondaries(0);

  aParticleChange.ProposeEnergy( kinEnergyStart - edepo );

  return &aParticleChange;

    
    };
G4double GetContinuousStepLimit(const G4Track& aTrack,
            G4double ,
            G4double currentMinimumStep,
                                    G4double&) {return currentMinimumStep;};
};

class MyPhysicsList : public G4VModularPhysicsList {

 void ConstructProcess()
   {
     G4cout<<"Construct Process"<<G4endl;
  theParticleIterator->reset();

  while( (*theParticleIterator)() ){

    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    G4String particleName = particle->GetParticleName();
    //G4cout<<particleName<<G4endl;


      MyPhysicsProcess* mpro   = new MyPhysicsProcess();

      pmanager->AddContinuousProcess(mpro);
}
/*   G4PhysicsListHelper* ph = G4PhysicsListHelper::GetPhysicsListHelper();
 // Add standard EM Processes
   theParticleIterator->reset();
   while( (*theParticleIterator)() ){
     G4ParticleDefinition* particle = theParticleIterator->value();
     G4String particleName = particle->GetParticleName();
 
 
       ph->RegisterProcess(new MyPhysicsProcess(), particle); 
    } */
         // Define transportation process
             AddTransportation();
               }
  void ConstructParticle() {
    G4Geantino::GeantinoDefinition();
  }

  void SetCuts()
  {
      //   the G4VUserPhysicsList::SetCutsWithDefault() method sets 
        //   the default cut value for all particle types 
          SetCutsWithDefault();   
  }

};

