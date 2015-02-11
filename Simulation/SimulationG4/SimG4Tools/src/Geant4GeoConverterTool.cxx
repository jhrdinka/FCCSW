//
//  Geant4GeoConverterTool.cxx
//  
//
//  Created by Julia Hrdinka on 10/11/14.
//
//

#include "SimG4Tools/Geant4GeoConverterTool.h"
//#include "DDG4/Geant4Config.h"
#include "DDG4/Geant4Kernel.h"
#include "DDG4/Geant4DetectorConstruction.h"

#include "G4RunManager.hh"

#include "DD4hep/Printout.h"

DECLARE_COMPONENT(Geant4GeoConverterTool)

// Standard Constructor
Geant4GeoConverterTool::Geant4GeoConverterTool(const std::string& type,
                                   const std::string& name,
                                   const IInterface* parent)
: AlgTool( type, name, parent ) {
    
    // Declare additional interface
    declareInterface<IGeoConverterTool>(this);
    
    //Declare Properties of the specific tool
    //=> for JobOptions
    
}

StatusCode Geant4GeoConverterTool::convert(DD4hep::Geometry::LCDD* lcdd){
   
    G4VUserDetectorConstruction* detector = new DD4hep::Simulation::Geant4DetectorConstruction(*lcdd);
    G4RunManager * runManager = new G4RunManager;
    runManager->SetUserInitialization(detector); //constructs detector (calls Construct in Geant4DetectorConstruction)
 
    return StatusCode::SUCCESS;
}
