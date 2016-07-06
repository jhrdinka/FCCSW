
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
      sensDet.setType(sdTyp.typeStr()); // set for the whole detector
  }

  // envelope volume with the max dimensions of tracker for visualization etc.
  DD4hep::Geometry::Tube envelopeShape(dimensions.rmin(), dimensions.rmax(), dimensions.dz());
  Volume envelopeVolume(detName, envelopeShape, lcdd.air());
  envelopeVolume.setVisAttributes(lcdd.invisible());


  unsigned int idxLay = 0;
    xml_comp_t xModuleComponents = xmlElement.child("module_components");
    xml_comp_t xtmpModule = xmlElement.child("module");
    unsigned int idxSubMod = 0;
    // get total thickness of module
    double totalModuleComponentThickness = 0;
    for (xml_coll_t xCompColl(xModuleComponents, _U(module_component)); xCompColl; ++xCompColl, ++idxSubMod) {
        xml_comp_t xComp = xCompColl;
        totalModuleComponentThickness += xComp.thickness();
    }
    // second pass: define module components and place in module
    idxSubMod = 0;
    double integratedModuleComponentThickness = 0;
    std::vector<Volume> moduleComponentVector;
    for (xml_coll_t xCompColl(xModuleComponents, _U(module_component)); xCompColl; ++xCompColl, ++idxSubMod) {
        xml_comp_t xComp = xCompColl;
        std::string moduleComponentName = "GenericTrackerBarrel_layer" + std::to_string(idxLay) + "_rod_module_component" + std::to_string(idxSubMod) + "_" + xComp.materialStr();
          Volume moduleComponentVolume(
          moduleComponentName,
          DD4hep::Geometry::Box(0.5 * xtmpModule.width(), 0.5 * xComp.thickness(), 0.5 * xtmpModule.length()),
          lcdd.material(xComp.materialStr()));
        moduleComponentVolume.setVisAttributes(lcdd, xComp.visStr());
        if (xComp.isSensitive()) {
          moduleComponentVolume.setSensitiveDetector(sensDet);
        }
        moduleComponentVector.push_back(moduleComponentVolume);

      }

    //xml_comp_t xLayer = xLayerColl;
    // definition of module volume (smallest independent subdetector) 
    // defined first because its dimensions are used both in its substructure (module_components)
    // and superstructure (rods)
    xml_comp_t xModule = getNodeByName(xmlElement, "module", "GenericTrackerModule");
    // define the module whose name was given in the "layer" xml Element
    Volume moduleVolume(
      "GenericTrackerBarrel_layer_rod_module",
      DD4hep::Geometry::Box( 0.5 * xModule.width(), 0.5 * xModule.thickness(), 0.5 * xModule.length()),
      lcdd.material("Carbon")); //TODO: change back material
    moduleVolume.setVisAttributes(lcdd, xModule.visStr());
    //moduleVolume.setSensitiveDetector(sensDet);

  int layer_detEl_counter = 0;
  int component_detEl_counter = 0;
  int rod_detEl_counter = 0;
  int module_detEl_counter = 0;
  double stereo_offset = 0;
  // loop over 'layer' nodes in xml
  for (xml_coll_t xLayerColl(xmlElement, _U(layer)); xLayerColl; ++xLayerColl, idxLay++) {
    xml_comp_t xLayer = xLayerColl;
    

    //moduleVolume.setSensitiveDetector(sensDet);

    /// @todo: handle analogously to module
    std::string moduleComponentName = "moduleComponent";
    idxSubMod = 0;
    for (xml_coll_t xCompColl(xModuleComponents, _U(module_component)); xCompColl; ++xCompColl, ++idxSubMod) {
        xml_comp_t xComp = xCompColl;
        DD4hep::Geometry::Position offset(
          0,
          -0.5 * totalModuleComponentThickness + integratedModuleComponentThickness,
          0);
        integratedModuleComponentThickness += xComp.thickness();
        PlacedVolume placedModuleComponentVolume = moduleVolume.placeVolume(moduleComponentVector[idxSubMod], offset);
        placedModuleComponentVolume.addPhysVolID("module_component", idxSubMod);
        //DetElement moduleComponentDetElement(GenericTrackerBarrelWorld, moduleComponentName, component_detEl_counter);
        //component_detEl_counter++;
        //moduleComponentDetElement.setPlacement(placedModuleComponentVolume);
    }
    // definition of rod volume (longitudinal arrangement of modules)
    Volume rodVolume(
      "GenericTrackerBarrel_layer" + std::to_string(idxLay) + "_rod",
      DD4hep::Geometry::Box(0.5 * xModule.width(), 0.5 * xModule.thickness() , xLayer.dz()),
      lcdd.material("Silicon")); //TODO: change back material
    //rodVolume.setVisAttributes(lcdd, "rme");
    rodVolume.setVisAttributes(lcdd.invisible());
    //rodVolume.setSensitiveDetector(sensDet);
    // handle repeat tag in xml
    double layerThickness;
    unsigned int numLayers;
    double layer_rmin;
    if (xLayer.hasAttr("repeat")) {
      // "repeat" layers  equidistant between rmin and rmax
      numLayers = xLayer.repeat();
      layerThickness = (xLayer.rmax() - xLayer.rmin()) / numLayers;
      layer_rmin = xLayer.rmin();
    } else {
      // just one layer per xml element ( at position r )
      numLayers = 1;
      layerThickness = 10;
      layer_rmin = xLayer.r();
    }
      unsigned int nPhi = 0;
      double r = 0;
      double phi = 0;
      for (unsigned int layerIndex = 0; layerIndex < numLayers; ++layerIndex, ++idxLay) {
        std::cout<<idxLay<<std::endl;
        r = layer_rmin + layerIndex*layerThickness;
        // definition of layer volumes
        DD4hep::Geometry::Tube layerShape(r, r + layerThickness, xLayer.dz());
        std::string layerName = "GenericTrackerBarrel_layer" + std::to_string(idxLay);
        Volume layerVolume(layerName, layerShape, lcdd.material("Silicon"));
        layerVolume.setVisAttributes(lcdd.invisible());
        //DetElement layerDetElement(GenericTrackerBarrelWorld, layerName, layer_detEl_counter)
        layer_detEl_counter++;
        //layerVolume.setSensitiveDetector(sensDet);
        PlacedVolume placedLayerVolume = envelopeVolume.placeVolume(layerVolume);
        placedLayerVolume.addPhysVolID("layer", idxLay);
        // approximation of tklayout values
        nPhi = static_cast<unsigned int>( 2 * M_PI * r / (0.9 * xModule.width()))+1;
        for (unsigned int phiIndex = 0; phiIndex < nPhi; ++phiIndex) {
          phi = 2 * M_PI * static_cast<double>(phiIndex) / static_cast<double>(nPhi);
          DD4hep::Geometry::Translation3D lTranslation(r*cos(phi), r*sin(phi), 0);
          /// @todo: move moduletwistangle to xml (maybe with default if it is not set? 
          /// does not seem like a crucial / often changed parameter)
          const double lModuleTwistAngle = 0.1*M_PI;
          DD4hep::Geometry::RotationZ lRotation( phi + lModuleTwistAngle + 0.5*M_PI);
          PlacedVolume placedRodVolume = layerVolume.placeVolume(rodVolume, lTranslation * lRotation );
          placedRodVolume.addPhysVolID("rod",phiIndex);
        }
      }
      // placement of modules within rods
      /// @todo move to xml (maybe with defaults?)
      double overlap = 0.9;
      unsigned int zRepeat = static_cast<int>(xLayer.dz() / (0.5 * xModule.length() - overlap));
      for (unsigned int zIndex = 0; zIndex < zRepeat; ++zIndex) {
          stereo_offset *= -1.;
          DD4hep::Geometry::Translation3D moduleOffset(0, stereo_offset,  
            zIndex * 2 * (0.5*xModule.length() - overlap) - xLayer.dz() + 0.5*xModule.length() - overlap);
          PlacedVolume placedModuleVolume = rodVolume.placeVolume(moduleVolume, moduleOffset * DD4hep::Geometry::RotationX(0));
          placedModuleVolume.addPhysVolID("module", zIndex);
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
