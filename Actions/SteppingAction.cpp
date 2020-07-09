/**
 *  @copyright Copyright 2020 The J-PET Monte Carlo Authors. All rights reserved.
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may find a copy of the License in the LICENCE file.
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  @file SteppingAction.cpp
 */

#include "../Info/PrimaryParticleInformation.h"
#include "../Info/EventMessenger.h"
#include "SteppingAction.h"

#include <G4TransportationManager.hh>
#include <G4PrimaryParticle.hh>
#include <G4RunManager.hh>
#include <G4UImanager.hh>

SteppingAction::SteppingAction(HistoManager* histo) : fHisto(histo)
{
  G4TransportationManager::GetTransportationManager()
  ->GetNavigatorForTracking()->SetPushVerbosity(0);
}

SteppingAction::~SteppingAction() {}

void SteppingAction::UserSteppingAction(const G4Step* aStep)
{
  //! Primary particles, that escape the world volume without interaction
  //! are not regeistered, if proper macro option was set -
  //! done in order to optimize simulation time
  if (EventMessenger::GetEventMessenger()->KillEventsEscapingWorld()) {
    G4StepPoint* point = aStep->GetPostStepPoint();
    if (point->GetStepStatus() == G4StepStatus::fWorldBoundary) {
      if (aStep->GetTrack()->GetParentID() == 0) {
        PrimaryParticleInformation* info = dynamic_cast<PrimaryParticleInformation*>(
          aStep->GetTrack()->GetDynamicParticle()->GetPrimaryParticle()->GetUserInformation()
        );
        if (info != nullptr) {
          G4int multiplicity = info->GetGammaMultiplicity();
          const G4int minBoundMultiplicity =
            EventMessenger::GetEventMessenger()->GetMinRegMultiplicity();
          const G4int maxBoundMultiplicity =
            EventMessenger::GetEventMessenger()->GetMaxRegMultiplicity();
          const G4int excludedMultiplicity =
            EventMessenger::GetEventMessenger()->GetExcludedMultiplicity();
          if (
            (multiplicity >= minBoundMultiplicity)
            && (multiplicity <= maxBoundMultiplicity)
            && (multiplicity != excludedMultiplicity)
          ) {
            G4RunManager::GetRunManager()->AbortEvent();
          }
        }
      }
    }
  }
  
  //! execute code for
  //! - generated by user gamma quanta
  //! - physical effects that does not occur in Sensitive Detector
  if (aStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName() == "Transportation") {
    return;
  }
  if (aStep->GetTrack()->GetParentID() != 0) {
    return;
  }
  if (aStep->GetPostStepPoint()->GetPhysicalVolume()->GetLogicalVolume()->GetSensitiveDetector() != nullptr) {
    return;
  }

  PrimaryParticleInformation* info = static_cast<PrimaryParticleInformation*>(
    aStep->GetTrack()->GetDynamicParticle()->GetPrimaryParticle()->GetUserInformation()
  );
  if (info != 0) {
    //! particle quanta interact in phantom or frame (but not SD!)
    double momentumChange = abs(aStep->GetPostStepPoint()->GetMomentum().mag2() - aStep->GetPreStepPoint()->GetMomentum().mag2());
    if (momentumChange > EventMessenger::GetEventMessenger()->GetAllowedMomentumTransfer()) {
      fHisto->AddNodeToDecayTree(info->GetGammaMultiplicity() + 10, info->GetGammaMultiplicity(), 
                                        aStep->GetTrack()->GetDynamicParticle()->GetPrimaryParticle()->GetTrackID());
      info->SetGammaMultiplicity(info->GetGammaMultiplicity() + 10);
    }
  }
}
