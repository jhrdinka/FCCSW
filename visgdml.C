#include "TGeoManager.h"

void visgdml () {
  TGeoManager* gman = new TGeoManager();
  gman->Import("TestBox.gdml");
  gman->SetVisLevel(5);
  TGeoVolume* top = gman->GetTopVolume();
  top->Draw("ogl");
  gman->SetVisOption(0);
}
