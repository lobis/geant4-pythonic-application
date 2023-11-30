
#include "geant4/Application.h"
#include "geant4/ActionInitialization.h"
#include "geant4/DetectorConstruction.h"
#include "geant4/PhysicsList.h"
#include "geant4/RunAction.h"

#include <G4RunManagerFactory.hh>

#include <random>

using namespace std;
using namespace geant4;
namespace py = pybind11;

Application* Application::pInstance = nullptr;


Application::Application() {
    // if the instance already exists, return it, otherwise create one
    if (pInstance != nullptr) {
        throw runtime_error("Application can only be created once");
        return;
    }
    pInstance = this;
}

Application::~Application() {
    // Recreating the Application will likely cause a segfault due to how Geant4 works, we should not allow it
    // It's preferable to throw an exception that we can handle in Python
    pInstance = this;// nullptr;
}

void Application::SetupRandomEngine() {
    CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine);
    if (randomSeed == 0) {
        randomSeed = std::random_device()();
    }
    CLHEP::HepRandom::setTheSeed(randomSeed);
}

void Application::SetupDetector(string gdml, const set<string>& sensitiveVolumes) {
    if (runManager == nullptr) {
        throw runtime_error("RunManager needs to be set up first");
    }

    if (runManager->GetUserDetectorConstruction() != nullptr) {
        throw runtime_error("Detector is already set up");
    }

    // sensitive volumes should be a set of strings
    set<string> sensitiveVolumesSet;
    for (const auto& volumeName: sensitiveVolumes) {
        sensitiveVolumesSet.insert(volumeName);
    }

    runManager->SetUserInitialization(new DetectorConstruction(std::move(gdml), sensitiveVolumesSet));
}

void Application::SetupPhysics() {
    if (runManager == nullptr) {
        throw runtime_error("RunManager needs to be set up first");
    }

    if (runManager->GetUserPhysicsList() != nullptr) {
        throw runtime_error("Physics is already set up");
    }

    runManager->SetUserInitialization(new PhysicsList());
}

void Application::SetupAction() {
    if (runManager == nullptr) {
        throw runtime_error("RunManager needs to be set up first");
    }

    // check detector and physics are set up
    if (runManager->GetUserDetectorConstruction() == nullptr) {
        throw runtime_error("Detector needs to be set up first");
    }

    if (runManager->GetUserPhysicsList() == nullptr) {
        throw runtime_error("Physics needs to be set up first");
    }

    if (runManager->GetUserActionInitialization() != nullptr) {
        throw runtime_error("Action is already set up");
    }

    runManager->SetUserInitialization(new ActionInitialization());
}

void Application::SetupManager(unsigned short nThreads) {
    if (runManager != nullptr) {
        throw runtime_error("RunManager is already set up");
    }
    const auto runManagerType = nThreads > 0 ? G4RunManagerType::MTOnly : G4RunManagerType::SerialOnly;
    runManager = unique_ptr<G4RunManager>(G4RunManagerFactory::CreateRunManager(runManagerType));
    if (nThreads > 0) {
        runManager->SetNumberOfThreads((G4int) nThreads);
    }
}

void Application::Initialize() {
    if (!IsSetup()) {
        throw runtime_error("Application needs to be set up first");
    }

    SetupRandomEngine();

    runManager->Initialize();
    isInitialized = true;
}

py::object Application::Run(int nEvents) {
    if (!IsInitialized()) {
        throw runtime_error("Application needs to be initialized first");
    }
    runManager->BeamOn(nEvents);

    auto& builder = RunAction::GetBuilder();
    return geant4::data::SnapshotBuilder(builder);
}

bool Application::IsSetup() const {
    return runManager != nullptr && runManager->GetUserDetectorConstruction() != nullptr &&
           runManager->GetUserPhysicsList() != nullptr && runManager->GetUserActionInitialization() != nullptr;
}

bool Application::IsInitialized() const {
    return runManager != nullptr && isInitialized;
}

void Application::SetRandomSeed(long seed) {
    randomSeed = seed;
}
