//
//  Geant4GeoConverterTool.h
//  
//
//  Created by Julia Hrdinka on 10/11/14.
//
//

#ifndef GEANT4GEOCONVERTERTOOL_H
#define GEANT4GEOCONVERTERTOOL_H

#include "GaudiKernel/AlgTool.h"
#include "SimulationInterfaces/IGeoConverterTool.h"
#include "DD4hep/LCDD.h"

class Geant4GeoConverterTool : public AlgTool, virtual public IGeoConverterTool {
public:
    Geant4GeoConverterTool(const std::string& type, const std::string& name, const IInterface* parent);
    virtual ~Geant4GeoConverterTool (){}
    
    //  virtual StatusCode initialize();
    //  virtual StatusCode finalize ();
   
    virtual StatusCode convert(DD4hep::Geometry::LCDD* m_lcdd);
};

#endif // GEANT4GEOCONVERTERTOOL_H
