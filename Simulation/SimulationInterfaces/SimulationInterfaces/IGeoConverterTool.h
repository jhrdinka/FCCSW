//
//  IRecoGeoConverterTool.h
//  
//
//  Created by Julia Hrdinka on 10/11/14.
//
//

#ifndef IGEOCONVERTERTOOL_H
#define IGEOCONVERTERTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "DD4hep/LCDD.h"

//class DD4hep::Geometry::LCDD;

static const InterfaceID IID_IGeoConverterTool ("IGeoConverterTool", 1 , 0);

class IGeoConverterTool : virtual public IAlgTool {

public:
    //Retrieve Interface ID
    static const InterfaceID& interfaceID() { return IID_IGeoConverterTool; }
    
    virtual StatusCode convert(DD4hep::Geometry::LCDD* m_lcdd) = 0;
    
protected:
    ~IGeoConverterTool() {}
};

#endif //IGEOCONVERTERTOOL_H
