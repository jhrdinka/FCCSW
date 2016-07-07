#ifndef DETECTOR_GENERICTRACKERCOMMON_H
#define DETECTOR_GENERICTRACKERCOMMON_H 

#include "DD4hep/DetFactoryHelper.h"

xml_comp_t getNodeByAttr(xml_h mother, const char* nodeName, const char* attrName, std::string attrValue);
#endif
