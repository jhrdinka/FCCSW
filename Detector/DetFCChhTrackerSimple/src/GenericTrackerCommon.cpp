#include "GenericTrackerCommon.h"

/// Given a XML element with several daughters with the same name, e.g.
/// <detector> <layer name="1" /> <layer name="2"> </detector>
/// this method returns the first daughter of type nodeName whose attribute has a given value
/// e.g. returns <layer name="2"/> when called with (detector, "layer", "name", "1")
/// @todo: runtime checks that attribute is present
xml_comp_t getNodeByAttr(xml_h mother, const char* nodeName, const char* attrName, std::string attrValue) {
  for (xml_coll_t xCompColl(mother, nodeName); xCompColl; ++xCompColl) {
      xml_comp_t xComp = xCompColl;
      if (xComp.attr<std::string>(attrName) == attrValue) {
        return xComp;
      }
  }
  return xml_comp_t(nullptr);
}
