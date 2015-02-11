#include "TestAlgorithms/TestAlgorithm.h"


DECLARE_COMPONENT(TestAlgorithm)

TestAlgorithm::TestAlgorithm(const std::string& name, ISvcLocator* svcLoc):
  GaudiAlgorithm(name, svcLoc), m_detsvc(nullptr)
{}

StatusCode TestAlgorithm::initialize() {
    
    if (GaudiAlgorithm::initialize().isFailure()){
        return StatusCode::FAILURE;
    }
    
    if (service("DD4HepDetDesSvc", m_detsvc, true).isFailure()) {
        error() << "Couldn't get DD4HepDetDesSvc" << endmsg;
        return StatusCode::FAILURE;
    }
    

    return StatusCode::SUCCESS;
}


StatusCode TestAlgorithm::execute() {
    
    
  return StatusCode::SUCCESS;
}
    

StatusCode TestAlgorithm::finalize() {
    
    if (GaudiAlgorithm::finalize().isFailure())
        return StatusCode::FAILURE;
    
    return StatusCode::SUCCESS;
    
}
