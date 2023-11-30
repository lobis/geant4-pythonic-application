
#include "geant4/RunAction.h"
#include <iostream>

using namespace std;

RunAction::RunAction() : G4UserRunAction() {}

void RunAction::BeginOfRunAction(const G4Run*) {
    if (IsMaster()) {
        cout << "RunAction::BeginOfRunAction" << endl;
        builderMainPtr = &builder;
    } else {
        cout << "RunAction::BeginOfRunAction (worker)" << endl;
    }
}

void RunAction::EndOfRunAction(const G4Run*) {
    if (IsMaster()) {
        cout << "RunAction::EndOfRunAction" << endl;
    } else {
        cout << "RunAction::EndOfRunAction (worker)" << endl;
        string error;
        if (!builder.is_valid(error)) {
            throw std::runtime_error("Builder is not valid: " + error);
        }

        // move the data into the main thread builder (lock)
        std::lock_guard<std::mutex> lock(mutex);
        auto& builderMain = *builderMainPtr;
    }
}

geant4::data::Builder& RunAction::GetBuilder() {
    auto runAction = dynamic_cast<RunAction*>(const_cast<G4UserRunAction*>(G4RunManager::GetRunManager()->GetUserRunAction()));
    return runAction->builder;
}

geant4::data::Builder* RunAction::builderMainPtr = nullptr;
