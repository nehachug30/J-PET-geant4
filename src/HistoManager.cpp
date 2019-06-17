#include "HistoManager.h"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "PrimaryParticleInformation.h"

#include <vector>



HistoManager::HistoManager()
: fMakeControlHisto(true)
{
    fEventPack = new JPetGeantEventPack();
    fGeantInfo =  fEventPack->GetEventInformation();
}

HistoManager::~HistoManager()
{
}

void HistoManager::Book()
{
    G4String fileName = "mcGeant.root";
    fRootFile = new TFile(fileName,"RECREATE");  
    if (! fRootFile) { 
        G4cout << " HistoManager::Book :"                    
            << " problem creating the ROOT TFile "  
            << G4endl;   
        return; 
    }

    Int_t bufsize=32000;
    Int_t splitlevel=2;


    fTree = new TTree("T", "Tree keeps output from Geant simulation",splitlevel);
    fTree->SetAutoSave(1000000000); // autosave when 1 Gbyte written
    fBranchEventPack = fTree->Branch("eventPack", &fEventPack, bufsize, splitlevel);

      if ( MakeControlHisto()){
            BookHistograms(); 
      }
}

void HistoManager::BookHistograms()
{

    /// histograms - registered hits parameters
    fHisto[0] = new TH1F("gen_gamma_multiplicity","Generated gammas multiplicity",10,0,10);
    fHisto[0]->GetXaxis()->SetTitle("Gamma quanta multiplicity: 1=prompt; 2=2g; 3=3g");
    fHisto[0]->GetYaxis()->SetTitle("Entries");

    fHisto[1] = new TH1F("gen_hit_time","Gen hit time",100, 0.0, 15000.0);
    fHisto[1]->GetXaxis()->SetTitle("Hit-times in scintillators [ps]");
    fHisto[1]->GetYaxis()->SetTitle("Entries");

    fHisto[2] = new TH1F("gen_hit_eneDepos","Gen hit ene deposition",750, 0.0, 1500.0);
    fHisto[2]->GetXaxis()->SetTitle("Deposited energy in scintillators [keV]");
    fHisto[2]->GetYaxis()->SetTitle("Entries");

    fHisto[3] = new TH1F("gen_hits_z_pos", "gen hits Z position",100, -60.0, 60.0);
    fHisto[3]->GetXaxis()->SetTitle("Hit-position along z [cm]");
    fHisto[3]->GetYaxis()->SetTitle("Entries");

    fHisto2D[0] = new TH2F("gen_hits_xy_pos","GEN hits XY pos",
      121, -60.5, 60.5,121, -60.5, 60.5);
    fHisto2D[0]->GetXaxis()->SetTitle("Hit-position X [cm]");
    fHisto2D[0]->GetYaxis()->SetTitle("Hit-position Y [cm]");

    /// histograms - source position
    fHisto[4] = new TH1F("gen_lifetime","Gen lifetime",100, 0.0, 1500.0);
    fHisto[4]->GetXaxis()->SetTitle("Lifetime (2/3g) [ps]");
    fHisto[4]->GetYaxis()->SetTitle("Entries");

    fHisto[5] = new TH1F("gen_prompt_lifetime","Gen prompt lifetime",100, 0.0, 1500.0);
    fHisto[5]->GetXaxis()->SetTitle("Lifetime prompt gamma [ps]");
    fHisto[5]->GetYaxis()->SetTitle("Entries");


    fHisto2D[1] = new TH2F("gen_XY","GEN XY coordinates of annihilation point",
    121, -21.5, 21.5,121, -21.5, 21.5);
    fHisto2D[1]->GetXaxis()->SetTitle("Annihilation point (2/3g) X [cm]");
    fHisto2D[1]->GetYaxis()->SetTitle("Annihilation point (2/3g) Y [cm]");

    fHisto2D[2] = new TH2F("gen_XZ","GEN XZ coordinates of annihilation point",
          121, -21.5, 21.5,121, -60.5, 60.5);
    fHisto2D[2]->GetXaxis()->SetTitle("Annihilation point (2/3g) X [cm]");
    fHisto2D[2]->GetYaxis()->SetTitle("Annihilation point (2/3g) Z [cm]");

    fHisto2D[3] = new TH2F("gen_YZ","GEN YZ coordinates of  annihilation point",
          121, -21.5, 21.5,121, -60.5, 60.5);
    fHisto2D[3]->GetXaxis()->SetTitle("Annihilation point (2/3g) Y [cm]");
    fHisto2D[3]->GetYaxis()->SetTitle("Annihilation point (2/3g) Z [cm]");


    fHisto2D[4] = new TH2F("gen_prompt_XY","GEN prompt XY coordinates of annihilation point",
    121, -21.5, 21.5,121, -21.5, 21.5);
    fHisto2D[4]->GetXaxis()->SetTitle("Prompt emission point X [cm]");
    fHisto2D[4]->GetYaxis()->SetTitle("Prompt emission point Y [cm]");


    fHisto2D[5] = new TH2F("gen_prompt_XZ","GEN prompt XZ coordinates of annihilation point",
          121, -21.5, 21.5,121, -60.5, 60.5);
    fHisto2D[5]->GetXaxis()->SetTitle("Prompt emission point X [cm]");
    fHisto2D[5]->GetYaxis()->SetTitle("Prompt emission point Z [cm]");


    fHisto2D[6] = new TH2F("gen_prompt_YZ","GEN prompt YZ coordinates of  annihilation point",
            121, -21.5, 21.5,121, -60.5, 60.5);
    fHisto2D[6]->GetXaxis()->SetTitle("Prompt emission point Y [cm]");
    fHisto2D[6]->GetYaxis()->SetTitle("Prompt emission point Z [cm]");

    fHisto2D[7] = new TH2F("gen_3g_angles","GEN angles 3g",
            190, -5., 185. ,190 , -5., 185. );
    fHisto2D[7]->GetXaxis()->SetTitle("#Theta_{12} [degree]");
    fHisto2D[7]->GetYaxis()->SetTitle("#Theta_{23} [degree]");

    fHisto2D[8] = new TH2F("gen_energy","GEN E_1 vs E_2",
            100, 0., 550. , 100 , 0., 550. );
    fHisto2D[8]->GetXaxis()->SetTitle("E_{1} [keV]");
    fHisto2D[8]->GetYaxis()->SetTitle("E_{2} [keV]");

    fHisto[6] = new TH1F("gen_g_ene","Gen energy", 200, 0.0, 1500.0);
    fHisto[6]->GetXaxis()->SetTitle("E_1 gen [keV]");
    fHisto[6]->GetYaxis()->SetTitle("Entries");




}

void HistoManager::FillHistoGenInfo(const G4Event* anEvent)
{
    for( int i=0; i<anEvent->GetNumberOfPrimaryVertex(); i++)
    {
        VtxInformation* info =  dynamic_cast<VtxInformation*>( anEvent->GetPrimaryVertex(i)->GetUserInformation());    
        if( info != 0 )
        {
            AddGenInfo(info);
        }

        for (int j=0; j<anEvent->GetPrimaryVertex(i)->GetNumberOfParticle(); j++)
        {
          G4PrimaryParticle* particle = anEvent->GetPrimaryVertex(i)->GetPrimary(j);
          if(particle != nullptr )
          {
            AddGenInfoParticles(particle);
          }
        }
    }


    double theta_12 = (180./TMath::Pi())*(fGeantInfo->GetMomentumGamma(1)).Angle(fGeantInfo->GetMomentumGamma(2));
    double theta_23 = (180./TMath::Pi())*(fGeantInfo->GetMomentumGamma(2)).Angle(fGeantInfo->GetMomentumGamma(3));

    fHisto2D[7]->Fill(theta_12,theta_23);
    fHisto2D[8]->Fill(fGeantInfo->GetMomentumGamma(1).Mag(),fGeantInfo->GetMomentumGamma(2).Mag());
    fHisto[6]->Fill(fGeantInfo->GetMomentumGamma(1).Mag());



}

void HistoManager::AddGenInfoParticles(G4PrimaryParticle* particle)
{
  PrimaryParticleInformation* infoParticle = static_cast<PrimaryParticleInformation*> (particle->GetUserInformation());

  if (infoParticle == nullptr) return;

  G4int index = infoParticle->GetIndex();

  fGeantInfo->SetMomentumGamma( index, particle->GetPx()/keV, particle->GetPy()/keV, particle->GetPz()/keV);

}


void HistoManager::AddGenInfo(VtxInformation* info)
{
    bool is3g = info->GetThreeGammaGen();
    bool is2g = info->GetTwoGammaGen();
    bool isprompt = info->GetPromptGammaGen();

    // Geant system of units 
    //    millimeter              (mm)
    //    nanosecond              (ns)
    //    Mega electron Volt      (MeV)
    // Framework system of units 
    //    time: ps
    //    distance: cm
    //    energy: keV
    if ( is2g || is3g )
    {
        fGeantInfo->SetThreeGammaGen(is3g);
        fGeantInfo->SetTwoGammaGen(is2g);
        fGeantInfo->SetVtxPosition(info->GetVtxPositionX()/cm,info->GetVtxPositionY()/cm,info->GetVtxPositionZ()/cm);
        fGeantInfo->SetLifetime(info->GetLifetime()/ps);
        fGeantInfo->SetRunNr(info->GetRunNr());

            if ( MakeControlHisto()){
                  if(is2g) fHisto[0]->Fill(2);
                  if(is3g) fHisto[0]->Fill(3);
            
                  fHisto[4]->Fill(info->GetLifetime()/ps);
                  fHisto2D[1]->Fill(info->GetVtxPositionX()/cm,info->GetVtxPositionY()/cm);
                  fHisto2D[2]->Fill(info->GetVtxPositionX()/cm,info->GetVtxPositionZ()/cm);
                  fHisto2D[3]->Fill(info->GetVtxPositionY()/cm,info->GetVtxPositionZ()/cm);
            }


    }

    if (isprompt)
    {
        fGeantInfo->SetPromptGammaGen(isprompt);
        fGeantInfo->SetPromptLifetime(info->GetLifetime()/ps);
        fGeantInfo->SetVtxPromptPosition(info->GetVtxPositionX()/cm,info->GetVtxPositionY()/cm,info->GetVtxPositionZ()/cm);
        fGeantInfo->SetRunNr(info->GetRunNr());

            if ( MakeControlHisto()){
                  fHisto[0]->Fill(1);
                  fHisto[5]->Fill(info->GetLifetime()/ps);
                  fHisto2D[4]->Fill(info->GetVtxPositionX()/cm,info->GetVtxPositionY()/cm);
                  fHisto2D[5]->Fill(info->GetVtxPositionX()/cm,info->GetVtxPositionZ()/cm);
                  fHisto2D[6]->Fill(info->GetVtxPositionY()/cm,info->GetVtxPositionZ()/cm);
            }

    }

      
}



void HistoManager::AddNewHit(DetectorHit* hit)
{

  JPetGeantScinHits* geantHit =  fEventPack->ConstructNextHit();

   geantHit->Fill(
           fEventPack->GetEventNumber(),    //int evID, 
           hit->GetScinID(),    //int scinID, 
           hit->GetTrackID(),    //int trkID, 
           hit->GetTrackPDG(),    //int trkPDG, 
           hit->GetNumInteractions(),    //int nInter,
           hit->GetEdep()/keV,    //float ene, 
           hit->GetTime()/ps    //float time, 
               );

         // ugly way but there is no easy way to cast g4vector into root vector
    geantHit->SetHitPosition(
             hit->GetPosition().getX()/cm,
             hit->GetPosition().getY()/cm,
             hit->GetPosition().getZ()/cm);

    geantHit->SetPolarizationIn(
             hit->GetPolarizationIn().getX(),
             hit->GetPolarizationIn().getY(),
             hit->GetPolarizationIn().getZ());

    geantHit->SetPolarizationOut(
             hit->GetPolarizationOut().getX(),
             hit->GetPolarizationOut().getY(),
             hit->GetPolarizationOut().getZ());

    geantHit->SetMomentumIn(
             hit->GetMomentumIn().getX()/keV,
             hit->GetMomentumIn().getY()/keV,
             hit->GetMomentumIn().getZ()/keV);

    geantHit->SetMomentumOut(
             hit->GetMomentumOut().getX()/keV,
             hit->GetMomentumOut().getY()/keV,
             hit->GetMomentumOut().getZ()/keV);

    geantHit->SetGenGammaMultiplicity(hit->GetGenGammaMultiplicity());
    geantHit->SetGenGammaIndex(hit->GetGenGammaIndex());


      if ( MakeControlHisto()){
            fHisto[1]->Fill(hit->GetTime()/ps);
            fHisto[2]->Fill(hit->GetEdep()/keV);
            fHisto[3]->Fill(hit->GetPosition().getZ()/cm);
            fHisto2D[0]->Fill(hit->GetPosition().getX()/cm,
            hit->GetPosition().getY()/cm);
      }
}




void HistoManager::Save()
{
    if (! fRootFile) return;
     //fRootFile->Write(); 
     fTree->Write();

      if ( MakeControlHisto()){
            for(int i=0; i<MaxHisto; i++) fHisto[i]->Write();
            for(int i=0; i<MaxHisto2D; i++) fHisto2D[i]->Write();
      }
     fRootFile->Close(); 

    G4cout << "\n----> Histograms and ntuples are saved\n" << G4endl;
}


