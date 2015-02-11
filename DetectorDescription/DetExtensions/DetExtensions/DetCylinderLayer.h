//
//  DetLayer.h
//  
//
//  Created by Julia Hrdinka on 12/12/14.
//
//

#ifndef DET_DETLAYER_H
#define DET_DETLAYER_H

#include "DD4hep/LCDD.h"
#include "DetExtensions/IExtension.h"


namespace Det {
    
    class DetCylinderLayer : public IExtension {
        
    public:
        
        DetCylinderLayer(int modulesPhi, int modulesZ) :
        m_modulesPhi(modulesPhi),
        m_modulesZ(modulesZ)
        {}
        DetCylinderLayer(const DetCylinderLayer& layer, const DD4hep::Geometry::DetElement&)
        {
            m_modulesPhi = layer.m_modulesPhi;
            m_modulesZ   = layer.m_modulesZ;
        }
        virtual ~DetCylinderLayer()
        {}
        int modulesPhi()
        {
            return m_modulesPhi;
        }
        int modulesZ()
        {
            return m_modulesZ;
        }
        
    private:
        int m_modulesPhi;
        int m_modulesZ;
       
    };
}



#endif //DET_DETLAYER_H
