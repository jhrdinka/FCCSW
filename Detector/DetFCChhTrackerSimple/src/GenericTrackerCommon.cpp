#include "GenericTrackerCommon.h"
xml_comp_t getNodeByName(xml_h mother, const char* nodeName, std::string attrName) {
  for (xml_coll_t xCompColl(mother, nodeName); xCompColl; ++xCompColl) {
      xml_comp_t xComp = xCompColl;
      if (xComp.attr<std::string>("name") == attrName) {
        return xComp;
      }
  }
  return xml_comp_t(nullptr);
}
