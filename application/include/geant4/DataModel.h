#ifndef GEANT4PYTHONICAPPLICATION_DATAMODEL_H
#define GEANT4PYTHONICAPPLICATION_DATAMODEL_H

#include "awkward/LayoutBuilder.h"

#include <G4SystemOfUnits.hh>
#include <G4UnitsTable.hh>

#include <pybind11/pybind11.h>

namespace py = pybind11;

class G4Event;
class G4Track;
class G4Step;

namespace geant4::data {

enum Field : std::size_t { eventId,
                           //
                           trackId,
                           trackParentId,
                           trackInitialEnergy,
                           trackInitialTime,
                           //
                           hitsEnergy,
                           hitsTime,
                           hitsProcess,
};

using UserDefinedMap = std::map<std::size_t, std::string>;

template<std::size_t field_name, class BUILDER>
using RecordField = awkward::LayoutBuilder::Field<field_name, BUILDER>;

template<class... BUILDERS>
using RecordBuilder = awkward::LayoutBuilder::Record<UserDefinedMap, BUILDERS...>;

template<class PRIMITIVE, class BUILDER>
using ListOffsetBuilder = awkward::LayoutBuilder::ListOffset<PRIMITIVE, BUILDER>;

template<class PRIMITIVE>
using NumpyBuilder = awkward::LayoutBuilder::Numpy<PRIMITIVE>;

typedef unsigned int id;
using Builder = RecordBuilder<RecordField<static_cast<std::size_t>(Field::eventId), NumpyBuilder<id>>,
                              RecordField<static_cast<std::size_t>(Field::trackId), ListOffsetBuilder<id, NumpyBuilder<id>>>,
                              RecordField<static_cast<std::size_t>(Field::trackParentId), ListOffsetBuilder<id, NumpyBuilder<id>>>,
                              RecordField<static_cast<std::size_t>(Field::trackInitialEnergy), ListOffsetBuilder<id, NumpyBuilder<float>>>,
                              RecordField<static_cast<std::size_t>(Field::trackInitialTime), ListOffsetBuilder<id, NumpyBuilder<float>>>,
                              RecordField<static_cast<std::size_t>(Field::hitsEnergy), ListOffsetBuilder<id, ListOffsetBuilder<id, NumpyBuilder<float>>>>,
                              RecordField<static_cast<std::size_t>(Field::hitsTime), ListOffsetBuilder<id, ListOffsetBuilder<id, NumpyBuilder<float>>>>
                              // RecordField<static_cast<std::size_t>(Field::hitsProcess), ListOffsetBuilder<id, ListOffsetBuilder<id, NumpyBuilder<std::string>>>>
                              >;

inline Builder MakeBuilder() {
    return {
            {
                    {Field::eventId, "event_id"},
                    {Field::trackId, "track.id"},
                    {Field::trackParentId, "track.parent_id"},
                    {Field::trackInitialEnergy, "track.energy"},
                    {Field::trackInitialTime, "track.time"},
                    {Field::hitsEnergy, "track.hits.energy"},
                    {Field::hitsTime, "track.hits.time"},
                    // {Field::hitsProcess, "track.hits.process"}},
            }};
}

void InsertEventBegin(const G4Event* event, Builder& builder);
void InsertEventEnd(const G4Event* event, Builder& builder);
void InsertTrackBegin(const G4Track* track, Builder& builder);
void InsertTrackEnd(const G4Track* track, Builder& builder);
void InsertStep(const G4Step* step, Builder& builder);

py::object SnapshotBuilder(Builder& builder);

namespace units {
static constexpr auto energy = CLHEP::keV;
static constexpr auto time = CLHEP::us;
static constexpr auto length = CLHEP::mm;
}// namespace units

}// namespace geant4::data


#endif// GEANT4PYTHONICAPPLICATION_DATAMODEL_H
