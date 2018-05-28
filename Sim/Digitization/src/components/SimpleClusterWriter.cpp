#include "SimpleClusterWriter.h"
#include "ACTS/Digitization/DigitizationModule.hpp"
#include "ACTS/Digitization/PlanarModuleCluster.hpp"
#include "ACTS/Digitization/Segmentation.hpp"
#include "ACTS/Layers/Layer.hpp"
#include "ACTS/Utilities/GeometryID.hpp"
#include "Digitization/FCCDigitizationCell.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include <cstring>
#include <string>

DECLARE_ALGORITHM_FACTORY(SimpleClusterWriter)

SimpleClusterWriter::SimpleClusterWriter(const std::string& name, ISvcLocator* svcLoc) : GaudiAlgorithm(name, svcLoc) {
  declareProperty("planarClusters", m_planarClusterHandle,
                  "Handle to sim::FCCPlanarCluster needed to possibly write out additional information for studies");
}

StatusCode SimpleClusterWriter::initialize() {
  if (GaudiAlgorithm::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }

  // Setup ROOT I/O
  m_outputFile = TFile::Open(m_filename.value().c_str(), "RECREATE");
  if (!m_outputFile) {
    error() << "Could not open " << m_filename << endmsg;
    return StatusCode::FAILURE;
  }
  m_outputFile->cd();
  m_outputTree = new TTree(m_treename.value().c_str(), "TTree from TrackSimpleClusterWriter");
  if (!m_outputTree) {
    error() << "Could not create TTree " << m_treename << endmsg;
    return StatusCode::FAILURE;
  }
  m_outputTree->SetDirectory(0);
  // Set the branches
  // m_outputTree->Branch("event_nr", &m_eventNr);
  m_outputTree->Branch("volumeID", &m_volumeID);
  m_outputTree->Branch("layerID", &m_layerID);
  m_outputTree->Branch("surfaceID", &m_surfaceID);
  m_outputTree->Branch("g_x", &m_x);
  m_outputTree->Branch("g_y", &m_y);
  m_outputTree->Branch("g_z", &m_z);

  m_outputTree->Branch("nChannels", &m_nChannels);
  m_outputTree->Branch("nChannelsOn", &m_nChannelsOn);

  m_outputTree->Branch("nTracksPerCluster", &m_nTracksPerCluster);
  m_outputTree->Branch("EnergyPerCluster", &m_energyPerCluster);

  m_outputTree->Branch("size_x", &m_size_x);
  m_outputTree->Branch("size_y", &m_size_y);

  m_outputTree->Branch("l_x", &m_lx);
  m_outputTree->Branch("l_y", &m_ly);

  m_outputTree->Branch("detElementID", &m_detElementID);

  return StatusCode::SUCCESS;
}

StatusCode SimpleClusterWriter::execute() {

  auto clusters = m_planarClusterHandle.get();

  for (auto& cluster : (*clusters)) {
    const auto& cells = cluster.digitizationCells();

    for (auto cellA = std::begin(cells); cellA != (std::end(cells) - 1); cellA++) {
      auto indexA = std::distance(cells.begin(), cellA);
      for (auto cellB = (cellA + 1); cellB != std::end(cells); cellB++) {
        auto indexB = std::distance(cells.begin(), cellB);
        // the channels
        auto c0A = cellA->channel0;
        auto c1A = cellA->channel1;
        auto c0B = cellB->channel0;
        auto c1B = cellB->channel1;

        // the conditions
        bool isSameChannel0 = (c0A == c0B);
        bool isSameChannel1 = (c1A == c1B);
        // same cell
        if (isSameChannel0 && isSameChannel1) {
          std::cout << "ERROR same cell!" << std::endl;
        }  // if
      }    // cellsB
    }      // cellsA

    // set event number
    m_eventNr = Gaudi::Hive::currentContext().evt();
    // the cluster surface
    auto& clusterSurface = cluster.referenceSurface();
    // set geometry IDs
    m_volumeID = clusterSurface.geoID().value(Acts::GeometryID::volume_mask);
    m_layerID = clusterSurface.geoID().value(Acts::GeometryID::layer_mask);
    m_surfaceID = clusterSurface.geoID().value(Acts::GeometryID::sensitive_mask);
    m_detElementID = clusterSurface.associatedDetectorElement()->identify();
    // local cluster information: position, @todo coveraiance
    auto parameters = cluster.parameters();
    Acts::Vector2D local(parameters[Acts::ParDef::eLOC_0], parameters[Acts::ParDef::eLOC_1]);
    /// prepare for calculating the
    Acts::Vector3D pos(0, 0, 0);
    Acts::Vector3D mom(1, 1, 1);
    // transform local into global position information
    clusterSurface.localToGlobal(local, mom, pos);
    // set position
    m_x = pos.x();
    m_y = pos.y();
    m_z = pos.z();
    // local position
    m_lx = local.x();
    m_ly = local.y();
    m_nTracksPerCluster = cluster.nTracks;
    m_energyPerCluster = cluster.energy;
    //  std::cout << "write: " << pos.x() << std::endl;

    // get the corresponding binUtility of the layer
    auto surfaces = clusterSurface.associatedLayer()->surfaceArray()->arrayObjects();

    std::vector<int> cell_IDx;  ///< cell ID in lx
    std::vector<int> cell_IDy;  ///< cell ID in ly

    auto detectorElement = clusterSurface.associatedDetectorElement();
    if (detectorElement && detectorElement->digitizationModule()) {
      auto digitationModule = detectorElement->digitizationModule();
      const Acts::Segmentation& segmentation = digitationModule->segmentation();
      // write out the total number of channels of this surface
      m_nChannels = segmentation.binUtility().bins();
      m_nChannelsOn = cells.size();
      for (auto& cell : cells) {
        // cell identification
        cell_IDx.push_back(cell.channel0);
        cell_IDy.push_back(cell.channel1);
        //  m_cell_data.push_back(cell.data);
      }
    }
    // fill the cluster size
    m_size_x = (*std::max_element(cell_IDx.begin(), cell_IDx.end())) -
        (*std::min_element(cell_IDx.begin(), cell_IDx.end())) + 1;
    m_size_y = (*std::max_element(cell_IDy.begin(), cell_IDy.end())) -
        (*std::min_element(cell_IDy.begin(), cell_IDy.end())) + 1;

    // fill the tree
    m_outputTree->Fill();
  }
  return StatusCode::SUCCESS;
}

StatusCode SimpleClusterWriter::finalize() {
  if (GaudiAlgorithm::finalize().isFailure()) {
    return StatusCode::FAILURE;
  }

  // write out all clusters
  m_outputFile->cd();
  m_outputTree->Write();
  m_outputFile->Close();

  return StatusCode::SUCCESS;
}
