
#include "DD4hep/DetFactoryHelper.h"

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
  xml_det_t xmlDet = xmlElement;
  std::string detName = xmlDet.nameStr();
  Dimension dimensions(xmlDet.dimensions());

  sensDet.setType("Geant4Tracker");
  DetElement GenericTrackerBarrel(detName, xmlDet.id());

  DD4hep::Geometry::Tube envelopeShape(dimensions.rmin(), dimensions.rmax(), dimensions.dz());
  Volume envelopeVolume(detName, envelopeShape, lcdd.air());
  envelopeVolume.setVisAttributes(lcdd.invisible());

  xml_comp_t xModule = xmlElement.child("module");
  Volume rodVolume(
    "GenericTrackerBarrel_rod",
    DD4hep::Geometry::Box(xModule.width(), xModule.thickness(), dimensions.dz()),
    lcdd.air());
  rodVolume.setVisAttributes(lcdd.invisible());

  Volume moduleVolume(
    "GenericTrackerBarrel_rod_module",
    DD4hep::Geometry::Box( xModule.width(), xModule.thickness(), xModule.length()),
    lcdd.air());
  moduleVolume.setVisAttributes(xModule.visStr());
  DetElement Module(GenericTrackerBarrel, "GenericTrackerBarrel_rod_module", 2);

  //@todo: add stereo offsets
  unsigned int zRepeat = dimensions.dz() / xModule.length() - 1;
  for (unsigned int zIndex = 0; zIndex < zRepeat; ++zIndex) {
      DD4hep::Geometry::Position moduleOffset(0, 0, zIndex * xModule.length());
      PlacedVolume placedModuleVolume = rodVolume.placeVolume(moduleVolume, moduleOffset);
      placedModuleVolume.addPhysVolID("module", zIndex);
  }

  unsigned int idxSubMod = 0;
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
        -0.5 * xModule.thickness() + integratedModuleComponentThickness,
        0);
      integratedModuleComponentThickness += xComp.thickness();
      PlacedVolume placedModuleComponentVolume = moduleVolume.placeVolume(moduleComponentVolume, offset);
      placedModuleComponentVolume.addPhysVolID("module_component", idxSubMod);
  }

  xml_comp_t xLayers = xmlElement.child("layer");
  unsigned int numLayers = xLayers.repeat();
  double  layerThickness = (dimensions.rmax() - dimensions.rmin()) / numLayers;
  unsigned int nPhi = 0;
  double r = 0;
  double phi = 0;
  for (unsigned int layerIndex = 0; layerIndex < numLayers; ++layerIndex) {
    r = dimensions.rmin() + layerIndex*layerThickness;
    // approximation of tklayout values
    nPhi = static_cast<unsigned int>(std::max(14., r*10. / 8.));
    for (unsigned int phiIndex = 0; phiIndex < nPhi; ++phiIndex) {
      phi = 2*M_PI * static_cast<double>(phiIndex) / static_cast<double>(nPhi);
      DD4hep::Geometry::Translation3D lTranslation(r*cos(phi), r*sin(phi), 0);
      const double lModuleTwistAngle = 0.1*M_PI;
      DD4hep::Geometry::RotationZ lRotation(phi + lModuleTwistAngle + 0.5*M_PI);
      PlacedVolume placedRodVolume = envelopeVolume.placeVolume(rodVolume, lTranslation * lRotation );
      placedRodVolume.addPhysVolID("rod", layerIndex * nPhi +  phiIndex);
    }
  }


  Volume motherVol = lcdd.pickMotherVolume(GenericTrackerBarrel);
  PlacedVolume placedGenericTrackerBarrel = motherVol.placeVolume(envelopeVolume);
  placedGenericTrackerBarrel.addPhysVolID("system", GenericTrackerBarrel.id());
  GenericTrackerBarrel.setPlacement(placedGenericTrackerBarrel);
  return GenericTrackerBarrel;
}
} //namespace det

DECLARE_DETELEMENT(GenericTrackerBarrel, det::createGenericTrackerBarrel)
