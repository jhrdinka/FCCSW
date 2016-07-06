
#include "GenericTrackerCommon.h"

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
static DD4hep::Geometry::Ref_t createGenericTrackerEndcap (
    DD4hep::Geometry::LCDD& lcdd,
    xml_h xmlElement,
    DD4hep::Geometry::SensitiveDetector sensDet
    ) {
  // shorthands
  xml_det_t xmlDet = xmlElement;
  std::string detName = xmlDet.nameStr();
  Dimension dimensions(xmlDet.dimensions());

  // get sensitive detector type from xml
  DD4hep::XML::Dimension sdTyp = xmlElement.child("sensitive"); // retrieve the type
  if ( xmlDet.isSensitive() ) {
      sensDet.setType(sdTyp.typeStr()); // set for the whole detector
  }

  // definition of top volume
  DetElement GenericTrackerEndcapWorld(detName, xmlDet.id());

  // envelope volume with the max dimensions of tracker for visualization etc.
  DD4hep::Geometry::Tube envelopeShape(dimensions.rmin(), dimensions.rmax(),  0.5*(dimensions.z2() - dimensions.z1()));
  Volume envelopeVolume(detName, envelopeShape, lcdd.air());
  envelopeVolume.setVisAttributes(lcdd.invisible());

  // loop over 'layer' nodes in xml
  unsigned int idxLay = 0;
  for (xml_coll_t xLayerColl(xmlElement, _U(layer)); xLayerColl; ++xLayerColl, ++idxLay) {
    xml_comp_t xLayer = xLayerColl;

    xml_comp_t xModule = getNodeByName(xmlElement, "module", xLayer.attr<std::string>("module"));
    //TODO: define modules
    //TODO: add PhysVolIDs

    // create petal
    unsigned int nPhi =  16; //TODO: more sensitive value?
    double dr = xLayer.rmax() - xLayer.rmin();
    double dphi = 2 * dd4hep::pi / static_cast<double>(nPhi);
    double tn = tan(dphi); 
    Volume petalVolume(
        "GenericTrackerEndcap_petal", 
        DD4hep::Geometry::Trapezoid(
          0.5*xLayer.rmin() * tn,
          0.5*xLayer.rmax() * tn, 
          0.5*xModule.thickness(), 
          0.5*xModule.thickness(), 
          0.5*dr), 
        lcdd.material("Silicon"));
    petalVolume.setVisAttributes(lcdd, "comp0");
    petalVolume.setSensitiveDetector(sensDet);

    // handle repeat tag in xml
    double layerThickness;
    unsigned int numLayers;
    double layer_zmin;
    double layer_z;
    if (xLayer.hasAttr("repeat")) {
      // "repeat" layers  equidistant between rmin and rmax
      numLayers = xLayer.repeat();
      layerThickness = (xLayer.z2() - xLayer.z1()) / numLayers;
      layer_zmin = xLayer.z1();
    } else {
      // just one layer per xml element ( at position r )
      numLayers = 1;
      layerThickness = 10; // dummy value so layer volume will show up in display
      layer_zmin = xLayer.z1();
    }
    // create layers. TODO: layers are identical, create once and place repeatedly.
    for (unsigned int layerIndex = 0; layerIndex < numLayers; ++layerIndex, idxLay++) {
      layer_z = layerIndex * layerThickness + layer_zmin - dimensions.z1(); 
      DD4hep::Geometry::Tube layerShape(xLayer.rmin(), xLayer.rmax(), layerThickness);
      Volume layerVolume("GenericTrackerBarrel_layer" + std::to_string(layerIndex), layerShape, lcdd.air());
      layerVolume.setVisAttributes(lcdd.invisible());
      PlacedVolume placedLayerVolume = envelopeVolume.placeVolume(layerVolume, DD4hep::Geometry::Position(0,0,layer_z - 0.5 * (dimensions.z2() - dimensions.z1())));
      placedLayerVolume.addPhysVolID("layer", idxLay);
 
    double phi;
    double r = xLayer.rmin();
    for (unsigned int phiIndex = 0; phiIndex < nPhi; ++phiIndex) { 
      phi = 2*dd4hep::pi * static_cast<double>(phiIndex) / static_cast<double>(nPhi);
      // oriented along z at first
      DD4hep::Geometry::Translation3D lTranslation_ringPhiPos( 0, 0, r + 0.5 * dr);
      const double lModuleTwistAngle = 0.01 * dd4hep::pi;
      DD4hep::Geometry::RotationY lRotation_ringPhiPos(phi); 
      DD4hep::Geometry::RotationX lRotation_orientRing(0.5 * dd4hep::pi); 
      // twist petals slightly so they overlap
      DD4hep::Geometry::RotationZ lRotation_twist(lModuleTwistAngle); 
      PlacedVolume placedPetalVolume = layerVolume.placeVolume(petalVolume, lRotation_orientRing * lRotation_ringPhiPos * lTranslation_ringPhiPos * lRotation_twist );
      placedPetalVolume.addPhysVolID("rod", phiIndex);
      }
    }
  }

  Volume motherVol = lcdd.pickMotherVolume(GenericTrackerEndcapWorld);
 DD4hep::Geometry::Translation3D lTranslation_posEnvelope(0,0,- dimensions.z1() - 0.5 * (dimensions.z2() - dimensions.z1()));
  PlacedVolume placedGenericTrackerEndcap_pos = motherVol.placeVolume(envelopeVolume, DD4hep::Geometry::Position(0,0,dimensions.z1() + 0.5* (dimensions.z2() - dimensions.z1())));
 PlacedVolume placedGenericTrackerEndcap_neg = motherVol.placeVolume(envelopeVolume, lTranslation_posEnvelope * DD4hep::Geometry::RotationX(1* dd4hep::pi));
  placedGenericTrackerEndcap_pos.addPhysVolID("system", GenericTrackerEndcapWorld.id());
  GenericTrackerEndcapWorld.setPlacement(placedGenericTrackerEndcap_pos);
  //GenericTrackerEndcapWorld.setPlacement(placedGenericTrackerEndcap_neg);
  return GenericTrackerEndcapWorld;
}
} //namespace det

DECLARE_DETELEMENT(GenericTrackerEndcap, det::createGenericTrackerEndcap)
