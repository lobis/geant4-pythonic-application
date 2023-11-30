
#ifndef GEANT4_APPLICATION_RUNACTION_H
#define GEANT4_APPLICATION_RUNACTION_H

#include <G4RunManager.hh>
#include <G4UserRunAction.hh>

#include "geant4/DataModel.h"

namespace geant4_app {

class RunAction : public G4UserRunAction {
public:
    RunAction();

    void BeginOfRunAction(const G4Run*) override;
    void EndOfRunAction(const G4Run*) override;

    /// Only one instance of RunAction is created for each thread.
    static data::Builder& GetBuilder();

private:
    data::Builder builder = data::MakeBuilder();
    static data::Builder* builderMainPtr;
    std::mutex mutex;
};

}// namespace geant4_app
#endif// GEANT4_APPLICATION_RUNACTION_H
