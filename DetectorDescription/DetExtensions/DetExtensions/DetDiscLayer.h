//
//  DetDiscLayer.h
//  
//
//  Created by Julia Hrdinka on 07/01/15.
//
//

#ifndef DET_DETDISCLAYER
#define DET_DETDISCLAYER

#include "DD4hep/LCDD.h"
#include "DetExtensions/IExtension.h"

namespace Det {
    
    class DetDiscLayer : public IExtension {
    
    public:
        
        DetDiscLayer(int modulesR, int modulesPhi) :
        m_modulesR(modulesR),
        m_modulesPhi(modulesPhi)
        {}
        DetDiscLayer(const DetDiscLayer& layer, const DD4hep::Geometry::DetElement&)
        {
            m_modulesR   = layer.m_modulesR;
            m_modulesPhi = layer.m_modulesPhi;
        }
        virtual ~DetDiscLayer()
        {}
        int modulesR()
        {
            return m_modulesR;
        }
        int modulesPhi()
        {
            return m_modulesPhi;
        }
        
    private:
        int m_modulesR;
        int m_modulesPhi;
    };
}

#endif //DET_DETDISCLAYER
