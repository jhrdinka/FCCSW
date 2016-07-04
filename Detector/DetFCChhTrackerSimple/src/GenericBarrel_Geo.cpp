
#include "DD4hep/DetFactoryHelper.h"

#include "ACTS/Plugins/DD4hepPlugins/IDetExtension.hpp"
#include "ACTS/Plugins/DD4hepPlugins/DetExtension.hpp"

#include <memory>
#include <string>

using DD4hep::Geometry::Volume;
using DD4hep::Geometry::DetElement;
using DD4hep::XML::Dimension;
using DD4hep::Geometry::PlacedVolume;

namespace det {

static DD4hep::Geometry::Ref_t createGenericTrackerBarrel (
  DD4hep::Geometry::LCDD& lcdd,
  xml_h xmlElement,
  DD4hep::Geometry::SensitiveDetector sensDet
  ) {
  // shorthands
  xml_det_t xmlDet = xmlElement;
  std::string detName = xmlDet.nameStr();
  Dimension dimensions(xmlDet.dimensions());

  // definition of top volume
  DetElement GenericTrackerBarrelWorld(detName, xmlDet.id());

  // get sensitive detector type from xml
  DD4hep::XML::Dimension sdTyp = xmlElement.child("sensitive"); // retrieve the type
  if ( xmlDet.isSensitive() ) {
      sensDet.setType(sdTyp.typeStr());                             // set for the whole detector
  }


  // envelope volume with the max dimensions of tracker for visualization etc.
  DD4hep::Geometry::Tube envelopeShape(dimensions.rmin(), dimensions.rmax(), dimensions.dz());
  Volume envelopeVolume(detName, envelopeShape, lcdd.air());
  envelopeVolume.setVisAttributes(lcdd.invisible());

  // definition of module volume (smallest independent subdetector) 
  xml_comp_t xModule = xmlElement.child("module");
  Volume moduleVolume(
    "GenericTrackerBarrel_rod_module",
    DD4hep::Geometry::Box( xModule.width(), xModule.thickness(), xModule.length()),
    lcdd.air());
  moduleVolume.setVisAttributes(lcdd, xModule.visStr());

  // definition of rod volume (longitudinal arrangement of modules)
  Volume rodVolume(
    "GenericTrackerBarrel_rod",
    DD4hep::Geometry::Box(xModule.width(), xModule.thickness(), dimensions.dz()),
    lcdd.air());
  rodVolume.setVisAttributes(lcdd, "comp3");
  DetElement Rod(GenericTrackerBarrelWorld, "GenericTrackerBarrel_rod", 3);
  
  // substructure of module (add the moment just 'module components', i.e. sheets of different materials)
  unsigned int idxSubMod = 0;
  // get total thickness of module
  double totalModuleComponentThickness = 0;
  for (xml_coll_t xCompColl(xModule, _U(module_component)); xCompColl; ++xCompColl, ++idxSubMod) {
      xml_comp_t xComp = xCompColl;
      totalModuleComponentThickness += xComp.thickness();
  }
  // second pass: actual placements, now knowing the thickness of the module
  double integratedModuleComponentThickness = 0;
  for (xml_coll_t xCompColl(xModule, _U(module_component)); xCompColl; ++xCompColl, ++idxSubMod) {
      xml_comp_t xComp = xCompColl;
      std::string subModuleName = "GenericTrackerBarrel_rod_module_component_" + xComp.materialStr();
      Volume moduleComponentVolume(
        subModuleName,
        DD4hep::Geometry::Box(xModule.width(), xModule.thickness(), dimensions.dz()),
        lcdd.material(xComp.materialStr()));
      moduleComponentVolume.setVisAttributes(lcdd, xComp.visStr());
      if (xComp.isSensitive()) {
        moduleComponentVolume.setSensitiveDetector(sensDet);
      }
      DD4hep::Geometry::Position offset(
        0,
        -0.5 * totalModuleComponentThickness + integratedModuleComponentThickness,
        0);
      integratedModuleComponentThickness += xComp.thickness();
      PlacedVolume placedModuleComponentVolume = moduleVolume.placeVolume(moduleComponentVolume, offset);
      placedModuleComponentVolume.addPhysVolID("module_component", idxSubMod);
  }

  // placement of modules within rods
  double overlap = 0.9;
  double stereo_offset = .2;
  unsigned int zRepeat = static_cast<int>(dimensions.dz() / (xModule.length() - overlap)) + 1;
  for (unsigned int zIndex = 0; zIndex < zRepeat; ++zIndex) {
      stereo_offset *= -1.;
      DD4hep::Geometry::Position moduleOffset(0, stereo_offset,  zIndex * 2 * (xModule.length() - overlap) - dimensions.dz() + xModule.length() - overlap);
      PlacedVolume placedModuleVolume = rodVolume.placeVolume(moduleVolume, moduleOffset);
      placedModuleVolume.addPhysVolID("module", zIndex);
  }

  // placement of rods within layers/barrel
  xml_comp_t xLayers = xmlElement.child("layer");
  unsigned int numLayers = xLayers.repeat();
  double  layerThickness = (dimensions.rmax() - dimensions.rmin()) / numLayers;
  unsigned int nPhi = 0;
  double r = 0;
  double phi = 0;
  for (unsigned int layerIndex = 0; layerIndex < numLayers; ++layerIndex) {
    DetElement Layer(GenericTrackerBarrelWorld, "GenericTrackerBarrel_layer" + std::to_string(layerIndex), layerIndex);
    r = dimensions.rmin() + layerIndex*layerThickness;
    // approximation of tklayout values
    nPhi = static_cast<unsigned int>(std::max(14., r* 7. / 8.));
    for (unsigned int phiIndex = 0; phiIndex < nPhi; ++phiIndex) {
      phi = 2*M_PI * static_cast<double>(phiIndex) / static_cast<double>(nPhi);
      DD4hep::Geometry::Translation3D lTranslation(r*cos(phi), r*sin(phi), 0);
      const double lModuleTwistAngle = 0.05*M_PI;
      DD4hep::Geometry::RotationZ lRotation(phi + lModuleTwistAngle + 0.5*M_PI);
      PlacedVolume placedRodVolume = envelopeVolume.placeVolume(rodVolume, lTranslation * lRotation );
      placedRodVolume.addPhysVolID("rod", layerIndex * nPhi +  phiIndex);
    }
  }

  Volume motherVol = lcdd.pickMotherVolume(GenericTrackerBarrelWorld);

  //add Extension to DetElement for the RecoGeometry
  //Acts::DetExtension* WorldDetExt = new Acts::DetExtension(Acts::ShapeType::Cylinder);
  //GenericTrackerBarrelWorld.addExtension<Acts::IDetExtension>(WorldDetExt); 
  ////placements of the modules to be handed over to the tracking geometry
  //std::vector<std::shared_ptr<const Acts::Transform3D>> placements;



  PlacedVolume placedGenericTrackerBarrel = motherVol.placeVolume(envelopeVolume);
  placedGenericTrackerBarrel.addPhysVolID("system", GenericTrackerBarrelWorld.id());
  GenericTrackerBarrelWorld.setPlacement(placedGenericTrackerBarrel);
  return GenericTrackerBarrelWorld;
}
} //namespace det

DECLARE_DETELEMENT(GenericTrackerBarrel, det::createGenericTrackerBarrel)
