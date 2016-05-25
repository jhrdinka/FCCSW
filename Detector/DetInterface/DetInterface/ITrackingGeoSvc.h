//
//  ITrackingGeoSvc.h
//
//
//  Created by Julia Hrdinka on 31/03/15.
//
//

#ifndef ITRACKINGGEOSVC_H
#define ITRACKINGGEOSVC_H

#include "GaudiKernel/IService.h"

namespace Acts {
    class TrackingGeometry;
}


class ITrackingGeoSvc: virtual public IService {
    
public:
    //InterfaceID
    DeclareInterfaceID(ITrackingGeoSvc,1,0);
    //  static const InterfaceID& interfaceID() { return IID_ITrackingGeoSvc; }
    //receive Reconstruction geometry
    virtual std::shared_ptr<Acts::TrackingGeometry> trackingGeometry() const = 0;
    virtual ~ITrackingGeoSvc() {}
};





#endif //ITRACKINGGEOSVC_H
