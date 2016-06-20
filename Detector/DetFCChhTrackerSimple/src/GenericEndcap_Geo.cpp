#include "DD4hep/DetFactoryHelper.h"

using DD4hep::Geometry::Volume;
using DD4hep::Geometry::DetElement;
using DD4hep::XML::Dimension;
using DD4hep::Geometry::PlacedVolume;

namespace det {
static DD4hep::Geometry::Ref_t createGenericTrackerEndcap (
    DD4hep::Geometry::LCDD& lcdd,
    xml_h xmlElement,
    DD4hep::Geometry::SensitiveDetector sensDet
    ) {
  // shorthands
  xml_det_t xmlDet = xmlElement;
  std::string detName = xmlDet.nameStr();
  Dimension dimensions(xmlDet.dimensions());
  // create DD4hep DetElements
  sensDet.setType("Geant4Tracker");
  DetElement GenericTrackerEndcap(detName, xmlDet.id());
  DetElement layer("petal", 0);
  DetElement petal("petal", 0);
  DetElement module("module", 0);

  // create envelope
  DD4hep::Geometry::Tube envelopeShape(dimensions.rmin(), dimensions.rmax(), dimensions.z() + dimensions.dz());
  Volume envelopeVolume(detName, envelopeShape, lcdd.air());
  DD4hep::Geometry::Position zOffset(0, 0, dimensions.z());
  DD4hep::Geometry::Position neg_zOffset(0, 0, -1 * dimensions.z());
  envelopeVolume.setVisAttributes(lcdd.invisible());

  xml_comp_t xModule = xmlElement.child("module");
  unsigned int nPhi = 2 * dd4hep::pi * dimensions.rmin() / xModule.width() ;
  double stereo_offset = xModule.thickness() * 3;
  double dphi = 2 * dd4hep::pi / static_cast<double>(nPhi);
  double tn = tan(dphi); 
  // create petal
  double dr = dimensions.rmax() -  dimensions.rmin();
  Volume petalVolume(
      "GenericTrackerEndcap_petal", 
      DD4hep::Geometry::Trapezoid(dimensions.rmin() * tn,  dimensions.rmax() * tn, xModule.thickness(), xModule.thickness(), dr), 
      lcdd.air());
  //petalVolume.setVisAttributes(lcdd.invisible());

  // place Volumes in r 
  unsigned int rRepeat = (dr  / xModule.length());
  double dr2 = rRepeat * xModule.length();
  for (unsigned int rIndex = 0; rIndex < rRepeat; ++rIndex) {
    double r1 = dimensions.rmin() + rIndex * dr2 / static_cast<double>(rRepeat);
    double r2 = r1 + dr2 / static_cast<double>(rRepeat);
    double x1 = r1 * tn;
    double x2 = r2 * tn;
    Volume moduleVolume(
      "GenericTrackerEndcap_petal_module", 
      DD4hep::Geometry::Trapezoid( x1, x2, xModule.thickness(), xModule.thickness(), xModule.length()),
      lcdd.air());

    moduleVolume.setVisAttributes(lcdd.invisible());
    //moduleVolume.setVisAttributes(xModule.visStr());
    stereo_offset = stereo_offset * -1;
  DD4hep::Geometry::Position moduleOffset(0., stereo_offset, rIndex * 2 * xModule.length() - dr + xModule.length());
  PlacedVolume placedModuleVolume = petalVolume.placeVolume(moduleVolume,  moduleOffset);
  module.setPlacement(placedModuleVolume);
  }

  // create layers
  xml_comp_t xLayers = xmlElement.child("layer");
  unsigned int numLayers = xLayers.repeat();
  double  layerThickness = dimensions.dz() / numLayers;
  double z = 0;
  double r = dimensions.rmin() + dr;
  double phi = 0;
  for (unsigned int layerIndex = 0; layerIndex < numLayers; ++layerIndex) {
    z = dimensions.z() + layerIndex*layerThickness;
    // approximation of tklayout values
    for (unsigned int phiIndex = 0; phiIndex < nPhi; ++phiIndex) { 
      phi = 2*dd4hep::pi * static_cast<double>(phiIndex) / static_cast<double>(nPhi);
      DD4hep::Geometry::Translation3D lTranslation(r*cos(phi), r*sin(phi), z);
      const double lModuleTwistAngle = 0.05 * dd4hep::pi;
      DD4hep::Geometry::RotationZ lRotation(phi + 0.5 * dd4hep::pi); 
      // twist petals slightly so they overlap
      DD4hep::Geometry::RotationY lRotation_twist(lModuleTwistAngle); 
      // position petal so that the narrow end is 
      DD4hep::Geometry::RotationX lRotation_fixTrapezoid(-1.5 * dd4hep::pi); 
      PlacedVolume placedPetalVolume = envelopeVolume.placeVolume(petalVolume, lTranslation * lRotation * lRotation_twist * lRotation_fixTrapezoid );
      petal.setPlacement(placedPetalVolume);
    }
  }

  Volume motherVol = lcdd.pickMotherVolume(GenericTrackerEndcap);
  PlacedVolume placedGenericTrackerEndcap_pos = motherVol.placeVolume(envelopeVolume, zOffset);
  PlacedVolume placedGenericTrackerEndcap_neg = motherVol.placeVolume(envelopeVolume, DD4hep::Geometry::RotationX(dd4hep::pi) * DD4hep::Geometry::Translation3D(zOffset));
  placedGenericTrackerEndcap_pos.addPhysVolID("system", GenericTrackerEndcap.id());
  GenericTrackerEndcap.setPlacement(placedGenericTrackerEndcap_pos);
  GenericTrackerEndcap.setPlacement(placedGenericTrackerEndcap_neg);
  return GenericTrackerEndcap;
}
} //namespace det

DECLARE_DETELEMENT(GenericTrackerEndcap, det::createGenericTrackerEndcap)
