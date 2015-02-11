//
//  IDetConstructionSvc.h
//  
//
//  Created by Julia Hrdinka on 09/10/14.
//
//

#ifndef _IDetDesSvc_h
#define _IDetDesSvc_h


#include "GaudiKernel/IService.h"
#include "DD4hep/VolumeManager.h"
class TGeoManager;


//muss ich GAUDI-API dazuschreiben?

class GAUDI_API IDetDesSvc: virtual public IService {

public:
    /// InterfaceID
    DeclareInterfaceID(IDetDesSvc,1,0);
    
    virtual StatusCode buildDetector () = 0;
    
    virtual StatusCode destroyDetector () = 0;
    
    virtual ~IDetDesSvc() {}
    
    virtual TGeoManager& GetTGeo() = 0;
    
    virtual DD4hep::Geometry::Volume getWorldVolume() = 0;
    
    virtual DD4hep::Geometry::DetElement getDetWorld() = 0;
    
};



#endif
