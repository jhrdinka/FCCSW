
#include "GenericTrackerCommon.h"

#include "DD4hep/DetFactoryHelper.h"

//#include "ACTS/Plugins/DD4hepPlugins/IDetExtension.hpp"
//#include "ACTS/Plugins/DD4hepPlugins/DetExtension.hpp"

using DD4hep::Geometry::Volume;
using DD4hep::Geometry::DetElement;
using DD4hep::XML::Dimension;
using DD4hep::Geometry::PlacedVolume;

namespace det {

static DD4hep::Geometry::Ref_t createGenericTrackerBarrel (
  DD4hep::Geometry::LCDD& lcdd,
  DD4hep::XML::Handle_t  xmlElement,
  DD4hep::Geometry::SensitiveDetector sensDet
  ) {
  // shorthands
  DD4hep::XML::DetElement xmlDet = static_cast<DD4hep::XML::DetElement>(xmlElement);
  Dimension dimensions(xmlDet.dimensions());
  // get sensitive detector type from xml
  DD4hep::XML::Dimension sdTyp = xmlElement.child("sensitive");
  if ( xmlDet.isSensitive() ) {
      // sensitive detector used for all sensitive parts of this detector
      sensDet.setType(sdTyp.typeStr());
  }
  // definition of top volume
  // min / max dimensions of tracker for visualization etc.
  std::string detectorName = xmlDet.nameStr();
  DetElement topDetElement(detectorName, xmlDet.id());
  DD4hep::Geometry::Tube topVolumeShape(dimensions.rmin(), dimensions.rmax(), dimensions.dz());
  Volume topVolume(detectorName, topVolumeShape, lcdd.air());
  topVolume.setVisAttributes(lcdd.invisible());

  // counts all layers - incremented in the inner loop over repeat - tags
  unsigned int idxLay = 0;
  // loop over 'layer' nodes in xml
  for (DD4hep::XML::Collection_t xLayerColl(xmlElement, _U(layers)); 
      nullptr != xLayerColl; 
      ++xLayerColl) {
    DD4hep::XML::Component xLayer = static_cast<DD4hep::XML::Component>(xLayerColl);
    
    DD4hep::XML::Component xModuleComponents = xmlElement.child("module_components");
    //TODO: add runtime check if attribute is found
    DD4hep::XML::Component xModule = getNodeByAttr(xmlElement, "module", "name", xLayer.attr<std::string>("module"));

    // optional parameters
    double stereo_offset;
    if (xLayer.hasAttr("stereo_offset")) {
      stereo_offset = xLayer.attr<double>("stereo_offset");
    } else {
      //set default
      stereo_offset = 0;
    }
    double module_twist_angle;
    if (xLayer.hasAttr("module_twist_angle")) {
      module_twist_angle = xLayer.attr<double>("module_twist_angle");
    } else {
      //set default
      module_twist_angle = 0.1 * M_PI;
    }
    double stereo_module_overlap;
    if (xLayer.hasAttr("stereo_module_overlap")) {
      stereo_module_overlap = xLayer.attr<double>("stereo_module_overlap");
    } else {
      //set default
      stereo_module_overlap = 0.9;
    }

    // get total thickness of module
    unsigned int idxSubMod = 0;
    double totalModuleComponentThickness = 0;
    for (DD4hep::XML::Collection_t xCompColl(xModuleComponents, _U(module_component)); 
         nullptr != xCompColl; 
         ++xCompColl, ++idxSubMod) {
         DD4hep::XML::Component xComp = static_cast<DD4hep::XML::Component>(xCompColl);
        totalModuleComponentThickness += xComp.thickness();
    }
    // now that thickness is known: define module components volumes
    idxSubMod = 0;
    double integratedModuleComponentThickness = 0;
    std::vector<Volume> moduleComponentVector;
    for (DD4hep::XML::Collection_t xCompColl(xModuleComponents, _U(module_component)); 
         nullptr != xCompColl; 
         ++xCompColl, ++idxSubMod) {
      DD4hep::XML::Component xComp = static_cast<DD4hep::XML::Component>(xCompColl);
      std::string moduleComponentName = "GenericTrackerBarrel_layer" + std::to_string(idxLay) + "_rod_module_component" + std::to_string(idxSubMod) + "_" + xComp.materialStr();
      Volume moduleComponentVolume(
        moduleComponentName,
        DD4hep::Geometry::Box(xModule.width(), xComp.thickness(), xModule.length()),
        lcdd.material(xComp.materialStr()));
      moduleComponentVolume.setVisAttributes(lcdd, xComp.visStr());
      if (xComp.isSensitive()) {
        moduleComponentVolume.setSensitiveDetector(sensDet);
      }
      moduleComponentVector.push_back(moduleComponentVolume);
    }

    // definition of module volume (smallest independent subdetector) 
    // define the module whose name was given in the "layer" xml Element
    Volume moduleVolume(
      "GenericTrackerBarrel_layer_rod_module",
      DD4hep::Geometry::Box( xModule.width(), xModule.thickness(), xModule.length()),
      lcdd.material("Air"));
    moduleVolume.setVisAttributes(lcdd, xModule.visStr());

    // definition of rod volume (longitudinal arrangement of modules)
    Volume rodVolume(
      "GenericTrackerBarrel_layer" + std::to_string(idxLay) + "_rod",
      DD4hep::Geometry::Box(xModule.width(), xModule.thickness() , xLayer.dz()),
      lcdd.material("Air"));
    rodVolume.setVisAttributes(lcdd.invisible());

    /// @todo: allow for more than one type of module components
    // analogous to module
    //place module substructure in module
    std::string moduleComponentName = "moduleComponent";
    idxSubMod = 0;
    for (DD4hep::XML::Collection_t xCompColl(xModuleComponents, _U(module_component)); 
         nullptr != xCompColl; 
         ++xCompColl, ++idxSubMod) {
      DD4hep::XML::Component xComp = static_cast<DD4hep::XML::Component>(xCompColl);
      DD4hep::Geometry::Position offset(
        0,
        -0.5 * totalModuleComponentThickness + integratedModuleComponentThickness,
        0);
      integratedModuleComponentThickness += xComp.thickness();
      PlacedVolume placedModuleComponentVolume = moduleVolume.placeVolume(moduleComponentVector[idxSubMod], offset);
      placedModuleComponentVolume.addPhysVolID("module_component", idxSubMod);
    }

    double layerThickness;
    unsigned int numRepeat;
    double layer_rmin;
    // handle repeat tag in xml
    if (xLayer.hasAttr("repeat")) {
      // "repeat" layers  equidistant between rmin and rmax
      numRepeat = xLayer.repeat();
      layerThickness = (xLayer.rmax() - xLayer.rmin()) / numRepeat;
      layer_rmin = xLayer.rmin();
    } else {
      // just one layer per xml element ( at position rmin )
      numRepeat = 1;
      layerThickness = 10;
      layer_rmin = xLayer.rmin();
    }
    unsigned int nPhi = 0;
    double r = 0;
    double phi = 0;
    // loop over repeated layers defined by one layer tag
    for (unsigned int repeatIndex = 0; repeatIndex < numRepeat; ++repeatIndex, ++idxLay) {
      r = layer_rmin + repeatIndex*layerThickness;
      // definition of layer volumes
      DD4hep::Geometry::Tube layerShape(r, r + layerThickness, xLayer.dz());
      std::string layerName = "GenericTrackerBarrel_layer" + std::to_string(idxLay);
      Volume layerVolume(layerName, layerShape, lcdd.material("Silicon"));
      layerVolume.setVisAttributes(lcdd.invisible());
      //layerVolume.setSensitiveDetector(sensDet);
      PlacedVolume placedLayerVolume = topVolume.placeVolume(layerVolume);
      placedLayerVolume.addPhysVolID("layer", idxLay);
      // approximation of tklayout values
      nPhi = static_cast<unsigned int>( 2 * M_PI * r / (0.9 * 2 * xModule.width()))+1;
      for (unsigned int phiIndex = 0; phiIndex < nPhi; ++phiIndex) {
        phi = 2 * M_PI * static_cast<double>(phiIndex) / static_cast<double>(nPhi);
        DD4hep::Geometry::Translation3D lTranslation(r*cos(phi), r*sin(phi), 0);
        DD4hep::Geometry::RotationZ lRotation( phi + module_twist_angle + 0.5*M_PI);
        PlacedVolume placedRodVolume = layerVolume.placeVolume(rodVolume, lTranslation * lRotation );
        placedRodVolume.addPhysVolID("rod",phiIndex);
      }
    }
    // placement of modules within rods
    unsigned int zRepeat = static_cast<int>(xLayer.dz() / (xModule.length() - stereo_module_overlap));
    // TODO: clean up stereo overlap
    for (unsigned int zIndex = 0; zIndex < zRepeat; ++zIndex) {
        stereo_offset *= -1.;
        DD4hep::Geometry::Translation3D moduleOffset(0, stereo_offset,  
          zIndex * 2 * (xModule.length() - stereo_module_overlap) - xLayer.dz() + xModule.length() - stereo_module_overlap);
        PlacedVolume placedModuleVolume = rodVolume.placeVolume(moduleVolume, moduleOffset * DD4hep::Geometry::RotationX(0));
        placedModuleVolume.addPhysVolID("module", zIndex);
    }
  }

  //TODO: add Extension to DetElement for the RecoGeometry
  //Acts::DetExtension* TopDetExt = new Acts::DetExtension(Acts::ShapeType::Cylinder);
  //topDetElement.addExtension<Acts::IDetExtension>(TopDetExt); 
  ////placements of the modules to be handed over to the tracking geometry
  //std::vector<std::shared_ptr<const Acts::Transform3D>> placements;

  Volume motherVol = lcdd.pickMotherVolume(topDetElement);
  PlacedVolume placedGenericTrackerBarrel = motherVol.placeVolume(topVolume);
  placedGenericTrackerBarrel.addPhysVolID("system", topDetElement.id());
  topDetElement.setPlacement(placedGenericTrackerBarrel);
  return topDetElement;
}
} //namespace det

DECLARE_DETELEMENT(GenericTrackerBarrel, det::createGenericTrackerBarrel)
