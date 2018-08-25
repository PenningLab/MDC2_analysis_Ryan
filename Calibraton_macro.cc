/**
 * # Macro to determine per pmt spe rate (hour and day)
 * To compile :
 * g++ -I /usr/common/usg/software/ROOT/6.04.00/include/ `root-config --cflags --libs` -o Calibraton_macro Calibraton_macro.cc rqlib/rqlib.so
 * To run :
 * ./Calibraton_macro -o [output filename] -list [imput filename list]
 * Usage :
 * ./Calibraton_macro -h
 *---
 *# Description of the macro
 *
 * In the following section, each part of the macro will be described. The major blocks are:
 * 1. [Headers] (#Headers)
 * 2. [Event structure] (#event_struct)
 * 3. [Cut definition] (#cuts)
 * 4. [Main executable] (#main)
 * 5. [Load chain] (#load)
 * 6. [Event loop] (#loop)
 *
 */


/**
 * ## Headers <a name="Headers"></a>
 * Include all the headers
 *
 */
// Standard C++ headers
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

// ROOT Headers
#include "TROOT.h"
#include "TSystem.h"
#include "TStopwatch.h"
#include "TFile.h"
#include "TString.h"
#include "TCanvas.h"
#include "TTree.h"
#include "TChain.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TProfile.h"
#include "TMath.h"
#include "TTimeStamp.h"
#include "TGraphErrors.h"
#include "TTree.h"
#include "TBranch.h"
#include "TF1.h"

// STD namespace
using namespace std;

// Functions prototype
void load_chain(TString txtFileList, TChain* chain);
void load_chain_by_name(TString inputName, TChain* chain, int nfiles, int position);
void eventLoop(TChain* events);

/**
 * Data type: Background or AmLi
 */
TString option = "Bkg";//"AmLi"

TFile* outfile;


/**
 * ## Lite event structure <a name="event_struct"></a>
 *
 * Description:
 *   Keep in this event structure only the required RQs for this analysis.
 *
 * ### Methods:
 * - `GetEntry(const int n)`: Get the nth event of the chain
 */

class MyEvent {
public:
    MyEvent(){}
    virtual ~MyEvent(){}
    TChain* chain;
    Int_t currentTree;
    TBranch *b_eventID, *b_triggerTimeStamp_s, *b_triggerTimeStamp_ns;
    TBranch *b_nPulsesTPCHG, *b_nPulsesTPCLG, *b_singlePEprobability_TPCHG, *b_singlePEprobability_TPCLG, *b_channelID_TPCHG, *b_channelID_TPCLG;
    TBranch *b_nPulsesSkin, *b_singlePEprobability_Skin, *b_channelID_Skin;
    TBranch *b_nPulsesODHG, *b_nPulsesODLG, *b_singlePEprobability_ODHG, *b_singlePEprobability_ODLG, *b_channelID_ODHG, *b_channelID_ODLG;
    TBranch *b_pulseStartTime_ns_TPCHG, *b_pulseStartTime_ns_TPCLG, *b_pulseStartTime_ns_Skin, *b_pulseStartTime_ns_ODHG, *b_pulseStartTime_ns_ODLG;
    TBranch *b_pulseEndTime_ns_TPCHG, *b_pulseEndTime_ns_TPCLG, *b_pulseEndTime_ns_Skin, *b_pulseEndTime_ns_ODHG, *b_pulseEndTime_ns_ODLG;
    //BP
    TBranch *b_pulseArea_phd_ODLG, *b_positiveArea_phd_ODLG, *b_negativeArea_phd_ODLG;
    TBranch *b_chPulseArea_phd_ODLG, *b_chPeakTime_ns_ODLG;
    //RW
    TBranch *b_pulseArea_phd_ODHG, *b_positiveArea_phd_ODHG, *b_negativeArea_phd_ODHG, *b_peakAmp_ODHG, *b_rmsWidth_ns_ODHG;
    TBranch *b_chPulseArea_phd_ODHG, *b_chPeakTime_ns_ODHG;

    TBranch *b_pulseArea_phd_Skin, *b_positiveArea_phd_Skin, *b_negativeArea_phd_Skin;
    TBranch *b_chPulseArea_phd_Skin, *b_chPeakTime_ns_Skin;


    //------------------------------------------------
    // Variable for extraction
    //------------------------------------------------
    unsigned int eventID;
    unsigned long triggerTimeStamp_ns, triggerTimeStamp_s;
    int nPulses_TPCHG, nPulses_TPCLG, nPulses_Skin, nPulses_ODHG, nPulses_ODLG;
    vector<float> singlePEprobability_TPCHG, singlePEprobability_TPCLG, singlePEprobability_Skin, singlePEprobability_ODHG, singlePEprobability_ODLG;
    vector<int> pulseStartTime_ns_TPCHG, pulseStartTime_ns_TPCLG, pulseStartTime_ns_Skin, pulseStartTime_ns_ODHG, pulseStartTime_ns_ODLG;
    vector<int> pulseEndTime_ns_TPCHG, pulseEndTime_ns_TPCLG, pulseEndTime_ns_Skin, pulseEndTime_ns_ODHG, pulseEndTime_ns_ODLG;
    //BP
    vector<float> pulseArea_phd_ODLG, positiveArea_phd_ODLG, negativeArea_phd_ODLG;
    //RW
    vector<float> pulseArea_phd_ODHG, positiveArea_phd_ODHG, negativeArea_phd_ODHG, peakAmp_ODHG, rmsWidth_ns_ODHG;
    vector<float> pulseArea_phd_Skin, positiveArea_phd_Skin, negativeArea_phd_Skin;

    vector<vector<float>> chPulseArea_phd_ODHG;
    vector<vector<float>> chPulseArea_phd_Skin;

    //Channel pulse parameters
    vector<vector<int>>   channelID_TPCHG, channelID_TPCLG, channelID_Skin, channelID_ODHG, channelID_ODLG, chPeakTime_ns_ODLG,chPeakTime_ns_ODHG, chPeakTime_ns_Skin;

    //bp

    vector<vector<float>> chPulseArea_phd_ODLG;

    void LoadBranches(TChain* tree);
    void GetEntry(const int n);
};

void MyEvent::LoadBranches(TChain* tree)
{
    chain = tree;
    chain->SetMakeClass(1);
    chain->SetBranchAddress("eventHeader.eventID",                &eventID,                          &b_eventID);
    chain->SetBranchAddress("eventHeader.triggerTimeStamp_s",     &triggerTimeStamp_s,               &b_triggerTimeStamp_s);
    chain->SetBranchAddress("eventHeader.triggerTimeStamp_ns",    &triggerTimeStamp_ns,              &b_triggerTimeStamp_ns);

    chain->SetBranchAddress("pulsesTPCHG.nPulses",                &nPulses_TPCHG,                    &b_nPulsesTPCHG);
    chain->SetBranchAddress("pulsesTPCLG.nPulses",                &nPulses_TPCLG,                    &b_nPulsesTPCLG);
    chain->SetBranchAddress("pulsesTPCHG.singlePEprobability",    &singlePEprobability_TPCHG,        &b_singlePEprobability_TPCHG);
    chain->SetBranchAddress("pulsesTPCLG.singlePEprobability",    &singlePEprobability_TPCLG,        &b_singlePEprobability_TPCLG);
    chain->SetBranchAddress("pulsesTPCHG.chID",                   &channelID_TPCHG,                  &b_channelID_TPCHG);
    chain->SetBranchAddress("pulsesTPCLG.chID",                   &channelID_TPCLG,                  &b_channelID_TPCLG);

    chain->SetBranchAddress("pulsesSkin.nPulses",                 &nPulses_Skin,                     &b_nPulsesSkin);
    chain->SetBranchAddress("pulsesSkin.singlePEprobability",     &singlePEprobability_Skin,         &b_singlePEprobability_Skin);
    chain->SetBranchAddress("pulsesSkin.chID",                    &channelID_Skin,                   &b_channelID_Skin);

    chain->SetBranchAddress("pulsesODHG.nPulses",                 &nPulses_ODHG,                     &b_nPulsesODHG);
    chain->SetBranchAddress("pulsesODLG.nPulses",                 &nPulses_ODLG,                     &b_nPulsesODLG);
    chain->SetBranchAddress("pulsesODHG.singlePEprobability",     &singlePEprobability_ODHG,         &b_singlePEprobability_ODHG);
    chain->SetBranchAddress("pulsesODLG.singlePEprobability",     &singlePEprobability_ODLG,         &b_singlePEprobability_ODLG);
    chain->SetBranchAddress("pulsesODHG.chID",                    &channelID_ODHG,                   &b_channelID_ODHG);
    chain->SetBranchAddress("pulsesODLG.chID",                    &channelID_ODLG,                   &b_channelID_ODLG);

    chain->SetBranchAddress("pulsesTPCHG.pulseStartTime_ns",      &pulseStartTime_ns_TPCHG,          &b_pulseStartTime_ns_TPCHG);
    chain->SetBranchAddress("pulsesTPCLG.pulseStartTime_ns",      &pulseStartTime_ns_TPCLG,          &b_pulseStartTime_ns_TPCLG);
    chain->SetBranchAddress("pulsesSkin.pulseStartTime_ns",       &pulseStartTime_ns_Skin,           &b_pulseStartTime_ns_Skin);
    chain->SetBranchAddress("pulsesODHG.pulseStartTime_ns",       &pulseStartTime_ns_ODHG,           &b_pulseStartTime_ns_ODHG);
    chain->SetBranchAddress("pulsesODLG.pulseStartTime_ns",       &pulseStartTime_ns_ODLG,           &b_pulseStartTime_ns_ODLG);

    chain->SetBranchAddress("pulsesTPCHG.pulseEndTime_ns",        &pulseEndTime_ns_TPCHG,            &b_pulseEndTime_ns_TPCHG);
    chain->SetBranchAddress("pulsesTPCLG.pulseEndTime_ns",        &pulseEndTime_ns_TPCLG,            &b_pulseEndTime_ns_TPCLG);
    chain->SetBranchAddress("pulsesSkin.pulseEndTime_ns",         &pulseEndTime_ns_Skin,             &b_pulseEndTime_ns_Skin);
    chain->SetBranchAddress("pulsesODHG.pulseEndTime_ns",         &pulseEndTime_ns_ODHG,             &b_pulseEndTime_ns_ODHG);
    chain->SetBranchAddress("pulsesODLG.pulseEndTime_ns",         &pulseEndTime_ns_ODLG,             &b_pulseEndTime_ns_ODLG);

    //bp
    chain->SetBranchAddress("pulsesODLG.pulseArea_phd",           &pulseArea_phd_ODLG,               &b_pulseArea_phd_ODLG);
    chain->SetBranchAddress("pulsesODLG.positiveArea_phd",        &positiveArea_phd_ODLG,            &b_positiveArea_phd_ODLG);
    chain->SetBranchAddress("pulsesODLG.negativeArea_phd",        &negativeArea_phd_ODLG,            &b_negativeArea_phd_ODLG);
    chain->SetBranchAddress("pulsesODLG.chPulseArea_phd",         &chPulseArea_phd_ODLG,             &b_chPulseArea_phd_ODLG);
    chain->SetBranchAddress("pulsesODLG.chPeakTime_ns",           &chPeakTime_ns_ODLG,               &b_chPeakTime_ns_ODLG);

    //RW
    chain->SetBranchAddress("pulsesODHG.pulseArea_phd",           &pulseArea_phd_ODHG,               &b_pulseArea_phd_ODHG);
    chain->SetBranchAddress("pulsesODHG.positiveArea_phd",        &positiveArea_phd_ODHG,            &b_positiveArea_phd_ODHG);
    chain->SetBranchAddress("pulsesODHG.negativeArea_phd",        &negativeArea_phd_ODHG,            &b_negativeArea_phd_ODHG);
    chain->SetBranchAddress("pulsesODHG.peakAmp",                 &peakAmp_ODHG,                     &b_peakAmp_ODHG);
    chain->SetBranchAddress("pulsesODHG.chPulseArea_phd",         &chPulseArea_phd_ODHG,             &b_chPulseArea_phd_ODHG);
    chain->SetBranchAddress("pulsesODHG.chPeakTime_ns",           &chPeakTime_ns_ODHG,               &b_chPeakTime_ns_ODHG);
    chain->SetBranchAddress("pulsesODHG.rmsWidth_ns",             &rmsWidth_ns_ODHG,                 &b_rmsWidth_ns_ODHG);

    chain->SetBranchAddress("pulsesSkin.pulseArea_phd",           &pulseArea_phd_Skin,               &b_pulseArea_phd_Skin);
    chain->SetBranchAddress("pulsesSkin.positiveArea_phd",        &positiveArea_phd_Skin,            &b_positiveArea_phd_Skin);
    chain->SetBranchAddress("pulsesSkin.negativeArea_phd",        &negativeArea_phd_Skin,            &b_negativeArea_phd_Skin);
    chain->SetBranchAddress("pulsesSkin.chPulseArea_phd",         &chPulseArea_phd_Skin,             &b_chPulseArea_phd_Skin);
    chain->SetBranchAddress("pulsesSkin.chPeakTime_ns",           &chPeakTime_ns_Skin,               &b_chPeakTime_ns_Skin);

}


void MyEvent::GetEntry(const int n)
{
    Long64_t ientry = chain->LoadTree(n);
    if (chain->GetTreeNumber() != currentTree) {
    currentTree = chain->GetTreeNumber();
    }
    b_eventID                           ->GetEntry(ientry);
    b_triggerTimeStamp_s                ->GetEntry(ientry);
    b_triggerTimeStamp_ns               ->GetEntry(ientry);
    b_nPulsesTPCHG                      ->GetEntry(ientry);
    b_nPulsesTPCLG                      ->GetEntry(ientry);
    b_singlePEprobability_TPCHG         ->GetEntry(ientry);
    b_singlePEprobability_TPCLG         ->GetEntry(ientry);
    b_channelID_TPCHG                   ->GetEntry(ientry);
    b_channelID_TPCLG                   ->GetEntry(ientry);
    b_nPulsesSkin                       ->GetEntry(ientry);
    b_singlePEprobability_Skin          ->GetEntry(ientry);
    b_channelID_Skin                    ->GetEntry(ientry);
    b_nPulsesODHG                      ->GetEntry(ientry);
    b_nPulsesODLG                      ->GetEntry(ientry);
    b_singlePEprobability_ODHG          ->GetEntry(ientry);
    b_singlePEprobability_ODLG          ->GetEntry(ientry);
    b_channelID_ODHG                    ->GetEntry(ientry);
    b_channelID_ODLG                    ->GetEntry(ientry);
    b_pulseStartTime_ns_TPCHG           ->GetEntry(ientry);
    b_pulseStartTime_ns_TPCLG           ->GetEntry(ientry);
    b_pulseStartTime_ns_Skin            ->GetEntry(ientry);
    b_pulseStartTime_ns_ODHG            ->GetEntry(ientry);
    b_pulseStartTime_ns_ODLG            ->GetEntry(ientry);
    b_pulseEndTime_ns_TPCHG             ->GetEntry(ientry);
    b_pulseEndTime_ns_TPCLG             ->GetEntry(ientry);
    b_pulseEndTime_ns_Skin              ->GetEntry(ientry);
    b_pulseEndTime_ns_ODHG              ->GetEntry(ientry);
    b_pulseEndTime_ns_ODLG              ->GetEntry(ientry);
    ///bp
    b_pulseArea_phd_ODLG                ->GetEntry(ientry);
    b_positiveArea_phd_ODLG             ->GetEntry(ientry);
    b_negativeArea_phd_ODLG             ->GetEntry(ientry);
    b_chPulseArea_phd_ODLG              ->GetEntry(ientry);
    b_chPeakTime_ns_ODLG                ->GetEntry(ientry);

    //RW
    b_pulseArea_phd_ODHG                ->GetEntry(ientry);
    b_positiveArea_phd_ODHG             ->GetEntry(ientry);
    b_negativeArea_phd_ODHG             ->GetEntry(ientry);
    b_chPulseArea_phd_ODHG              ->GetEntry(ientry);
    b_chPeakTime_ns_ODHG                ->GetEntry(ientry);

    b_pulseArea_phd_Skin                ->GetEntry(ientry);
    b_positiveArea_phd_Skin             ->GetEntry(ientry);
    b_negativeArea_phd_Skin             ->GetEntry(ientry);
    b_chPulseArea_phd_Skin              ->GetEntry(ientry);
    b_chPeakTime_ns_Skin                ->GetEntry(ientry);
    b_peakAmp_ODHG                      ->GetEntry(ientry);
    b_rmsWidth_ns_ODHG                  ->GetEntry(ientry);
}


/**
 * # Some use full functions
 *
 * ### Description:
 *  - `Loggify(TAxis* axis)` : Convert the linear bining of the axis into a log-scale bining
 *  - `LoggifyX(TH1* h)`     : Convert in log-scale the X axis of the TH1
 *  - `LoggifyXY(TH2* h)`    : Convert in log-scale the X and Y axis of the TH2
 *  - `LoggifyX(TH2* h)`     : Convert in log-scale the X axis of the TH2
 *  - `LoggifyY(TH2* h)`     : Convert in log-scale the Y axis of the TH2
 */
void Loggify(TAxis* axis) {
    int bins = axis->GetNbins();

    Axis_t from = TMath::Log10(axis->GetXmin());
    Axis_t to = TMath::Log10(axis->GetXmax());
    Axis_t width = (to - from) / bins;
    Axis_t *new_bins = new Axis_t[bins + 1];

    for (int i = 0; i <= bins; i++) new_bins[i] = TMath::Power(10, from + i * width);
    axis->Set(bins, new_bins);
    delete[] new_bins;
}

void LoggifyX(TH1* h)  { Loggify(h->GetXaxis()); }
void LoggifyXY(TH2* h) { Loggify(h->GetXaxis()); Loggify(h->GetYaxis()); }
void LoggifyX(TH2* h)  { Loggify(h->GetXaxis()); }
void LoggifyY(TH2* h)  { Loggify(h->GetYaxis()); }
// Command line actions


// Cuts
//bool CXAreaPhdCut        (MyEvent* evt){return evt->pulsesODLG.chPulseArea_phd > 3; }
/**
 * ## Main executable <a name="main"></a>
 *
 * Main executable
 *
 * ## Arguments:
 * - `txtFileList`: Name of the file that contains the list of file to process (default: "RQfile.list")
 */
TH2D* Create2DHist(string h2name,string Xname,string Yname, double Xbins, double Xbin_min, double Xbin_max,double Ybins, double Ybin_min, double Ybin_max){
    TH2D* h = new TH2D(h2name.c_str(),("#font[132]{" + h2name + "}").c_str(),Xbins,Xbin_min,Xbin_max,Ybins,Ybin_min,Ybin_max);
    h->SetXTitle(("#font[132]{" + Xname + "}").c_str());
    h->SetYTitle(("#font[132]{" + Yname + "}").c_str());
    h->GetXaxis()->SetLabelFont(132);
    h->GetYaxis()->SetLabelFont(132);
    return h;

}
TH1D* Create1DHist(string h1name,string Xname,string Yname, double Xbins, double Xbin_min, double Xbin_max,int color = 4){
    TH1D* h = new TH1D(h1name.c_str(),("#font[132]{" + h1name + "}").c_str(),Xbins,Xbin_min,Xbin_max);
    h->SetXTitle(("#font[132]{" + Xname + "}").c_str());
    h->SetYTitle(("#font[132]{" + Yname + "}").c_str());
    h->GetXaxis()->SetLabelFont(132);
    h->GetYaxis()->SetLabelFont(132);
    h->SetMarkerColor(color);
    h->SetLineColor(color);
    return h;

}
TH1D* CreateSumw2Hist(string hname,string Xname,string Yname, double Xbins, double Xbin_min, double Xbin_max,int color = 4){
    TH1D* h = new TH1D(hname.c_str(),("#font[132]{" + hname + "}").c_str(),Xbins,Xbin_min,Xbin_max);
    h->SetXTitle(("#font[132]{" + Xname + "}").c_str());
    h->SetYTitle(("#font[132]{" + Yname + "}").c_str());
    h->GetXaxis()->SetLabelFont(132);
    h->GetYaxis()->SetLabelFont(132);
    h->SetMarkerColor(color);
    h->SetLineColor(color);
    h->Sumw2();
    return h;

}
static void show_usage(string name){
    cout<<" Usage : ./Calibration_macro [-co] "<<name<<" Options:\n"
    <<" -list : load the list of filename \n"
    <<" -type : currently one can choose : AmLi, Na22, background \n"
    <<" -debug : debug mode \n"
    <<" -o : out file name, if it is not assigned the default file name is [sourceName_position_numberfiles_calibration.root] \n"
    <<" -p : position of the source (currently 700 0r 1336)\n"
    <<" -h or --help : Show the usage\n"
    <<" Enjoy ! -Ryan Wang"<<endl;
}
int main(int argc, char* argv[])
{

    //------------------------------------------------
    // Prevent canvases from being drawn.
    //------------------------------------------------
    gROOT->SetBatch(kTRUE);

    //------------------------------------------------
    // Create and start a timer
    //------------------------------------------------
    TStopwatch* clock = new TStopwatch();
    clock->Start();
    // Read in the aruments from command line3
    // Read in the info from command line

    //TString outname  = "neutron_rates.root";
    //TString txtFileList = "RQfile.list.calib";

    TString outname;
    TString txtFileList;
    TString inputName;
    bool useList = false;
    bool debug = false;
    bool custom_outname = false;
    int position;
    int number_of_files = 0;

    if (argc<2){
        show_usage(argv[0]);
        return 1;
    }
    for (int i=1;i<argc;++i){
        string arg = argv[i];
        if ((arg=="-h") || (arg=="--help")){
          show_usage(argv[0]);
        return 0;
        }
        else if (arg=="-o"){
            outname = argv[i+1];
            custom_outname = true;
        }
        else if (arg=="-type"){
            inputName = argv[i+1];
        }
        else if (arg=="-n"){
            number_of_files = atoi(argv[i+1]);
        }
        else if (arg=="-debug"){
            debug = true;
        }
        else if (arg=="-p"){
            position = atoi(argv[i+1]);
        }
        else if (arg=="-list"){
            txtFileList = argv[i+1] ;
            useList = true;
        }
    }
    cout<<" Outname is : "<<outname<<" Input list is : "<<txtFileList<<endl;
    //getchar();
    //------------------------------------------------
    // Create the output file. If the file already
    // exist: overwrite on it ("recreate" option)
    //------------------------------------------------

    char temp_out[50];
    if (custom_outname){
        outfile = new TFile(outname, "recreate");
    }
    else{
        sprintf(temp_out,"%s_%u_%ufiles_calibration.root",inputName.Data(),position,number_of_files);
        outfile = new TFile(temp_out, "recreate");
    }

    cout << "Writing output to: "<<outfile->GetName()<<endl;

    //------------------------------------------------
    // Creation of the TChain and set the TTree Events
    // as the tree to extract from the file that we
    // are going to load
    //------------------------------------------------
    TChain* chain = new TChain("Events", "Events");
    if (useList){
        load_chain(txtFileList, chain);
    }
    else {
        load_chain_by_name(inputName, chain, number_of_files, position);
    }


    //------------------------------------------------
    // Create the event structure and load the
    // branches in it from the chain
    //------------------------------------------------
    MyEvent* evt = new MyEvent();
    evt->LoadBranches(chain);

    //------------------------------------------------
    // Preparation of the output
    //------------------------------------------------
    //  outfile = new TFile(outname, "recreate");

    // Create output tree (following Tomasz's example)
    TTree* ODSPECalibration = new TTree("ODSPECalibration", "ODSPECalibration");

    size_t TPCPmtCount  = 494; //0-252 and 300-540
    size_t SkinPmtCount = 93;  //600-692
    size_t ODPmtCount   = 120; //801-920


    // Definition of the histogram properties, this is not the definition of the cuts !!
    int Nbins_tpc   = 541;      float min_tpc   = 0;       float max_tpc = 540;
    int Nbins_skin  = 93;       float min_skin  = 600;     float max_skin = 692;
    int Nbins_od    = 120;      float min_od   = 801;      float max_od  = 920;
    int Nbins_hour  = 24;       float min_hour  = 0;       float max_hour= 24;
    int Nbins_day   = 4;        float min_day   = 0;       float max_day = 3 ;

    std::vector<std::vector<unsigned long long>> livetime;
    int firstPulseStart = 0;
    int lastPulseEnd = 0;
    int duration = 0;
    livetime.resize(Nbins_day);
    for (int i = 0; i < Nbins_day; ++i) {
    livetime[i].resize(Nbins_hour);
    for (int j = 0; j < Nbins_hour; ++j) {
      livetime[i][j] = 0;
    }
    }

    float hour_s = 60*60;    //min x sec
    float day_s  = 24*60*60; //hour x min x sec

    //------------------------------------------------
    // Definition of histograms
    //------------------------------------------------
    TH1D* h_triggerTimeStamp_ns = new TH1D("triggerTimeStamp_ns", "triggerTimeStamp_ns", 100000, 0, 2.5e+08);
    TH1D* h_triggerTimeStamp_s = new TH1D("triggerTimeStamp_s", "triggerTimeStamp_s", 10000, 0, 10000);
    TH1D* h_triggerTimeStamp_s_ODLG = new TH1D("triggerTimeStamp_s_ODLG", "triggerTimeStamp_s_ODLG", 10000, 0, 10000);
    TH1F* h_pulse_duration_ms_ODLG = new TH1F("pulse_duration_ms_ODLG", "pulse_duration_ms_ODLG", 300, 0, 10);
    TH1F* h_pulseArea_phd_ODLG = new TH1F("pulseArea_phd_ODLG", "pulseArea_phd_ODLG", 30000, 0, 3000);
    TH1F* h_positiveArea_phd_ODLG = new TH1F("positiveArea_phd_ODLG", "positiveArea_phd_ODLG", 3000, 0, 3000);
    TH1F* h_negativeArea_phd_ODLG = new TH1F("negativeArea_phd_ODLG", "negativeArea_phd_ODLG", 300, -300, 10);
    TH1F* h_nPulses_ODLG = new TH1F("nPulses_ODLG", "nPulses_ODLG", 100, 0, 100);
    TH1F* h_chPulseArea_phd_ODLG = new TH1F("chPulseArea_phd_ODLG", "chPulseArea_phd_ODLG", 20, 0, 20);
    TH1F* h_chPeakTime_ns_ODLG = new TH1F("chPeakTime_ns_ODLG", "chPeakTime_ns_ODLG", 700, 0, 700);
    TH1F* h_channelID_ODLG = new TH1F("channelID_ODLG", "channelID_ODLG", 1000, 0, 1000);
    TH1F* h_summed_pulseArea_phd_ODLG = new TH1F("Summed_pulseArea_phd_ODLG", "Summed_pulseArea_phd_ODLG", 3000, 0, 3000);
    h_summed_pulseArea_phd_ODLG->Sumw2();

    TH1F* h_pulseArea_phd_ODHG = new TH1F("pulseArea_phd_ODHG", "pulseArea_phd_ODHG", 3000, 0, 3000);
    TH1F* h_pulseArea_phd_ODHG_cut = new TH1F("pulseArea_phd_ODHG_cut", "pulseArea_phd_ODHG_cut", 3000, 0, 3000);
    TH1F* h_pulseArea_phd_ODHG_1us = new TH1F("pulseArea_phd_ODHG_1us", "pulseArea_phd_ODHG_1us", 3000, 0, 3000);
    TH1F* h_pulseArea_phd_ODHG_2us = new TH1F("pulseArea_phd_ODHG_2us", "pulseArea_phd_ODHG_2us", 3000, 0, 3000);
    TH1F* h_pulseArea_phd_ODHG_3us = new TH1F("pulseArea_phd_ODHG_3us", "pulseArea_phd_ODHG_3us", 3000, 0, 3000);
    TH1F* h_startTime_ODHG = new TH1F("startTime_ODHG", "startTime_ODHG", 8000, 0, 8000);
    TH1F* h_pulseArea_phd_ODHG_5mus = new TH1F("pulseArea_phd_ODHG_10us", "pulseArea_phd_ODHG_10us", 3000, 0, 3000);
    TH1F* h_pulseArea_phd_ODHG_310us = new TH1F("pulseArea_phd_ODHG_310us", "pulseArea_phd_ODHG_310us", 3000, 0, 3000);
    TH1F* h_positiveArea_phd_ODHG = new TH1F("positiveArea_phd_ODHG", "positiveArea_phd_ODHG", 3000, 0, 3000);
    TH1F* h_negativeArea_phd_ODHG = new TH1F("negativeArea_phd_ODHG", "negativeArea_phd_ODHG", 300, -100, 10);
    TH1F* h_rmsWidth_ns = new TH1F("rmsWidth_ns", "rmsWidth_ns", 300, 0, 3000);

    TH1F* h_pulseArea_phd_Skin = new TH1F("pulseArea_phd_Skin", "pulseArea_phd_Skin", 3000, 0, 3000);
    TH1F* h_positiveArea_phd_Skin = new TH1F("positiveArea_phd_Skin", "positiveArea_phd_Skin", 3000, 0, 3000);
    TH1F* h_negativeArea_phd_Skin = new TH1F("negativeArea_phd_Skin", "negativeArea_phd_Skin", 300, -100, 10);


    TH2F* h_pulseStartTime_ns_pulseArea_phd_ODHG = new TH2F("pulseStartTime_ns_pulseArea_phd_ODHG","pulseStartTime_ns_pulseArea_phd_ODHG",50000,0,500000,3000,0,3000);
    TH2F* h_pulseStartTime_ns_pulseArea_phd_ODLG = new TH2F("pulseStartTime_ns_pulseArea_phd_ODLG","pulseStartTime_ns_pulseArea_phd_ODLG",50000,0,500000,3000,0,3000);
    TH2F* h_pulseStartTime_ns_pulseArea_phd_Skin = new TH2F("pulseStartTime_ns_pulseArea_phd_Skin","pulseStartTime_ns_pulseArea_phd_Skin",50000,0,500000,3000,0,3000);
    TH2F* h_pulse_amp_pulse_area_ODHG = new TH2F("pulse_amp_pulse_area_ODHG","pulse_amp_pulse_area_ODHG",1000,0,0.1,3000,0,3000);
    TH2F* h_rmswdith_pulse_area_ODHG = new TH2F("rmswdith_pulse_area_ODHG","rmswdith_pulse_area_ODHG",3000,0,3000,2000,0,1000);
    TH2F* h_pulseStartTime_ns_pulseArea_phd_ODHG_5us = new TH2F("pulseStartTime_ns_pulseArea_phd_ODHG_5us","pulseStartTime_ns_pulseArea_phd_ODHG_5us",50000,5000,500000,3000,0,3000);

    //TH1D* h_pulseArea_phd_ODHG_3us_5us = Create1DHist("pulseArea_phd_ODHG_3us_5us"," Energy (phd) "," Counts (/phd) ", 3000, 0, 3000);
    TH2D* h_pulseRatio_time_ODHG = Create2DHist("h_pulseRatio_time_ODHG ", " pulse time "," pulse Height/pulseArea",5000, 0, 500,1000, 0, 0.1);
    TH1D* h_channelPeakTime_20us = Create1DHist("channelPeakTime_20us"," Pulse Time (ns) "," Counts (/ns) ", 2000, 0, 20000);
    TH1D* h_pulseArea_phd_ODHG_int_1us = Create1DHist("pulseArea_phd_ODHG_int_1us"," pulse area (phd)"," Counts (/phd)",3000,0,3000);
    TH1D* h_pulseArea_phd_ODHG_int_2us = Create1DHist("pulseArea_phd_ODHG_int_2us"," pulse area (phd)"," Counts (/phd)",3000,0,3000);
    TH1D* h_pulseArea_phd_ODHG_int_3us = Create1DHist("pulseArea_phd_ODHG_int_3us"," pulse area (phd)"," Counts (/phd)",3000,0,3000);
    TH1D* h_pulseArea_phd_ODHG_int_30us = Create1DHist("pulseArea_phd_ODHG_int_30us"," pulse area (phd)"," Counts (/phd)",3000,0,3000);
    TH1D* h_pulseArea_phd_ODHG_int_130us = Create1DHist("pulseArea_phd_ODHG_int_130us"," pulse area (phd)"," Counts (/phd)",3000,0,3000);
    TH1D* h_pulseArea_phd_ODHG_int_230us = Create1DHist("pulseArea_phd_ODHG_int_230us"," pulse area (phd)"," Counts (/phd)",3000,0,3000);
    TH1D* h_pulseArea_phd_ODHG_int_330us = Create1DHist("pulseArea_phd_ODHG_int_330us"," pulse area (phd)"," Counts (/phd)",3000,0,3000);
    TH1D* h_pulseArea_phd_ODHG_int_500us = Create1DHist("pulseArea_phd_ODHG_int_500us"," pulse area (phd)"," Counts (/phd)",3000,0,3000);

    TH1D* h_pulseArea_phd_ODHG_int_1000us = Create1DHist("pulseArea_phd_ODHG_int_1000us"," pulse area (phd)"," Counts (/phd)",3000,0,3000);
    TH1D* h_pulseArea_phd_ODHG_int_1500us = Create1DHist("pulseArea_phd_ODHG_int_1500us"," pulse area (phd)"," Counts (/phd)",3000,0,3000);
    TH1D* h_pulseArea_phd_ODHG_int_2000us = Create1DHist("pulseArea_phd_ODHG_int_2000us"," pulse area (phd)"," Counts (/phd)",3000,0,3000);
    TH1D* h_pulseArea_phd_ODHG_int_2500us = Create1DHist("pulseArea_phd_ODHG_int_2500us"," pulse area (phd)"," Counts (/phd)",3000,0,3000);
    TH1D* h_pulseArea_phd_ODHG_int_3000us = Create1DHist("pulseArea_phd_ODHG_int_3000us"," pulse area (phd)"," Counts (/phd)",3000,0,3000);
    TH1D* h_pulseArea_phd_ODHG_int_3700us = Create1DHist("pulseArea_phd_ODHG_int_3700us"," pulse area (phd)"," Counts (/phd)",3000,0,3000);

    TH1D* h_pulseArea_phd_ODLG_int_1us = Create1DHist("pulseArea_phd_ODLG_int_1us"," pulse area (phd)"," Counts (/phd)",3000,0,3000);
    TH1D* h_pulseArea_phd_ODLG_int_2us = Create1DHist("pulseArea_phd_ODLG_int_2us"," pulse area (phd)"," Counts (/phd)",3000,0,3000);
    TH1D* h_pulseArea_phd_ODLG_int_3us = Create1DHist("pulseArea_phd_ODLG_int_3us"," pulse area (phd)"," Counts (/phd)",3000,0,3000);
    TH1D* h_pulseArea_phd_ODLG_int_30us = Create1DHist("pulseArea_phd_ODLG_int_30us"," pulse area (phd)"," Counts (/phd)",3000,0,3000);

    TH1F* h_pulseArea_phd_ODLG_1us = new TH1F("pulseArea_phd_ODLG_1us", "pulseArea_phd_ODLG_1us", 3000, 0, 3000);
    TH1F* h_pulseArea_phd_ODLG_2us = new TH1F("pulseArea_phd_ODLG_2us", "pulseArea_phd_ODLG_2us", 3000, 0, 3000);
    TH1F* h_pulseArea_phd_ODLG_3us = new TH1F("pulseArea_phd_ODLG_3us", "pulseArea_phd_ODLG_3us", 3000, 0, 3000);
    TH1F* h_pulseArea_phd_ODLG_310us = new TH1F("pulseArea_phd_ODLG_310us", "pulseArea_phd_ODLG_310us", 3000, 0, 3000);


    //vector of histos
    std::vector <TH1D*> hvec_chPulseArea_phd_OD;
    for (int i=0; i<120; ++i){
    TString name="chPulseArea_phd_ODL_"; name+=i;
    TH1D* h = new TH1D(name,name,40,0,40);
    hvec_chPulseArea_phd_OD.push_back(h);
    }


    //------------------------------------------------
    // Main event loop
    //------------------------------------------------

    // Count the total number of event in the TChain
    const Int_t nevents = evt->chain->GetEntries();

    //check time
    evt->GetEntry(0);
    double first_ns=evt->triggerTimeStamp_ns;
    double first_s=evt->triggerTimeStamp_s;
    evt->GetEntry(nevents-1);
    double last_ns=evt->triggerTimeStamp_ns;
    double last_s=evt->triggerTimeStamp_s;
    cout<<"duration of run: "<<(last_s-first_s)<< "sec "<<endl;
    // Loop over all the events
    //  for (Int_t n=0; n<nevents; ++n) {
    float min_time_ns=-99;
    float max_time_ns=-99;
    float min_time_s=-99;
    float max_time_s=-99;

    min_time_ns=evt->triggerTimeStamp_ns;
    max_time_ns=evt->triggerTimeStamp_ns;
    min_time_s=evt->triggerTimeStamp_s;
    max_time_s=evt->triggerTimeStamp_s;
    int processed_events=0;
    double sum_pule_time=0;
    float_t total_pulse_NegareHG=0,total_pulse_PosareHG=0;
    float_t total_pulse_areaHG = 0, total_pulse_areaHG_cut =0;
    float_t pulseArea_phd_ODHG_5mus_temp=0;
    float_t total_pulse_NegareLG=0,total_pulse_PosareLG=0;
    float_t total_pulse_areaLG = 0;
    float_t Int_window_ODHG_1us=0,Int_window_ODHG_2us=0,Int_window_ODHG_3us=0,Int_window_ODHG_30us=0,Int_window_ODHG_130us=0,Int_window_ODHG_230us=0,Int_window_ODHG_330us=0,Int_window_ODHG_500us=0;
    float_t pulseArea_phd_ODHG_3us=0,pulseArea_phd_ODHG_2us=0,pulseArea_phd_ODHG_1us=0,pulseArea_phd_ODHG_310us=0;
    float_t Int_window_ODHG_1000us=0,Int_window_ODHG_1500us=0,Int_window_ODHG_2000us=0,Int_window_ODHG_2500us=0,Int_window_ODHG_3000us=0,Int_window_ODHG_3700us=0;
    float_t pulseArea_phd_ODLG_3us=0,pulseArea_phd_ODLG_2us=0,pulseArea_phd_ODLG_1us=0,pulseArea_phd_ODLG_310us=0;
    float_t Int_window_ODLG_1us=0,Int_window_ODLG_2us=0,Int_window_ODLG_3us=0,Int_window_ODLG_30us=0;
    //  for (Int_t n=0; n<1000; ++n) {
    for (Int_t n=0; n<nevents; ++n) {
    if (n%1000 == 0) cout << "Processing "<< n << "/"<<nevents<<endl;
    processed_events++;

    //------------------------------------------------
    // Load the nth event in the RQEvent class,
    // aka set all the RQs in evt
    //------------------------------------------------
    evt->GetEntry(n);
    //cout<<" This is event : "<<n<<endl;
    //------------------------------------------------
    // create booleans for the cuts
    //------------------------------------------------

    //------------------------------------------------
    // fill histograms with and without cuts application
    //------------------------------------------------
    // TPC High gain
    unsigned long firstDay = 1491004800;
    h_triggerTimeStamp_ns->Fill(evt->triggerTimeStamp_ns);
    h_triggerTimeStamp_s->Fill(evt->triggerTimeStamp_s);
    if(evt->triggerTimeStamp_ns<min_time_ns)  min_time_ns=evt->triggerTimeStamp_ns;
    if(evt->triggerTimeStamp_ns>max_time_ns)  max_time_ns=evt->triggerTimeStamp_ns;

    if(evt->triggerTimeStamp_ns<min_time_s)  min_time_s=evt->triggerTimeStamp_s;
    if(evt->triggerTimeStamp_ns>max_time_s)  max_time_s=evt->triggerTimeStamp_s;

    int day  = trunc((evt->triggerTimeStamp_s - firstDay) / 86400);      //need to get day from trigger time stamp
    int hour = trunc(((evt->triggerTimeStamp_s - firstDay) % 86400)/3600); //need to get hour from trigger time stamp


    if (evt->nPulses_ODLG != 0) {
        total_pulse_areaLG =0;
        total_pulse_PosareLG = 0;
        total_pulse_NegareLG = 0;
        pulseArea_phd_ODLG_3us=0,pulseArea_phd_ODLG_2us=0,pulseArea_phd_ODLG_1us=0,pulseArea_phd_ODLG_310us=0;
        Int_window_ODLG_1us=0,Int_window_ODLG_2us=0,Int_window_ODLG_3us=0,Int_window_ODLG_30us=0;
        if(evt->pulseStartTime_ns_ODLG[0] < firstPulseStart) firstPulseStart = evt->pulseStartTime_ns_ODLG[0];
        if(evt->pulseEndTime_ns_ODLG[(evt->nPulses_ODLG)-1] > lastPulseEnd) lastPulseEnd = evt->pulseEndTime_ns_ODLG[(evt->nPulses_ODLG)-1];
        h_triggerTimeStamp_s_ODLG->Fill(evt->triggerTimeStamp_s);
        for (int ip=0;ip<evt->pulseArea_phd_ODLG.size();ip++){
            total_pulse_areaLG+=evt->pulseArea_phd_ODLG[ip];
            total_pulse_PosareLG+=evt->positiveArea_phd_ODLG[ip];
            total_pulse_NegareLG+=evt->negativeArea_phd_ODLG[ip];
            h_pulseStartTime_ns_pulseArea_phd_ODLG->Fill(evt->pulseStartTime_ns_ODLG[ip],evt->pulseArea_phd_ODLG[ip]);


            if (evt->pulseStartTime_ns_ODLG[ip]<10000 && evt->pulseStartTime_ns_ODLG[ip]>3000){
                pulseArea_phd_ODLG_310us+=evt->pulseArea_phd_ODLG[ip];
            }
            if (evt->pulseStartTime_ns_ODLG[ip]<630 && evt->pulseStartTime_ns_ODLG[ip]>0){
                pulseArea_phd_ODLG_1us+=evt->pulseArea_phd_ODLG[ip];
            }
            if (evt->pulseStartTime_ns_ODLG[ip]>630 && evt->pulseStartTime_ns_ODLG[ip]<1000){
                pulseArea_phd_ODLG_2us+=evt->pulseArea_phd_ODLG[ip];
            }
            if (evt->pulseStartTime_ns_ODLG[ip]<3000 && evt->pulseStartTime_ns_ODLG[ip]>2200){
                pulseArea_phd_ODLG_3us+=evt->pulseArea_phd_ODLG[ip];
            }

            if (evt->pulseStartTime_ns_ODLG[ip]>0 && evt->pulseStartTime_ns_ODLG[ip]<600)
                Int_window_ODLG_1us+=evt->pulseArea_phd_ODLG[ip];
            if (evt->pulseStartTime_ns_ODLG[ip]>0 && evt->pulseStartTime_ns_ODLG[ip]<2000)
                Int_window_ODLG_2us+=evt->pulseArea_phd_ODLG[ip];

            if (evt->pulseStartTime_ns_ODLG[ip]>0 && evt->pulseStartTime_ns_ODLG[ip]<3000)
                Int_window_ODLG_3us+=evt->pulseArea_phd_ODLG[ip];
            if (evt->pulseStartTime_ns_ODLG[ip]>0 && evt->pulseStartTime_ns_ODLG[ip]<30000)
                Int_window_ODLG_30us+=evt->pulseArea_phd_ODLG[ip];
        }

        h_pulseArea_phd_ODLG_3us->Fill(pulseArea_phd_ODLG_3us);
        h_pulseArea_phd_ODLG_2us->Fill(pulseArea_phd_ODLG_2us);
        h_pulseArea_phd_ODLG_1us->Fill(pulseArea_phd_ODLG_1us);
        h_pulseArea_phd_ODLG_310us->Fill(pulseArea_phd_ODLG_310us);

        h_pulseArea_phd_ODLG_int_1us->Fill(Int_window_ODLG_1us);
        h_pulseArea_phd_ODLG_int_2us->Fill(Int_window_ODLG_2us);
        h_pulseArea_phd_ODLG_int_3us->Fill(Int_window_ODLG_3us);
        h_pulseArea_phd_ODLG_int_30us->Fill(Int_window_ODLG_30us);

        h_pulseArea_phd_ODLG->Fill(total_pulse_areaLG);
        h_positiveArea_phd_ODLG->Fill(total_pulse_PosareLG);
        h_negativeArea_phd_ODLG->Fill(total_pulse_NegareLG);
    }
    duration = lastPulseEnd - firstPulseStart;
    //cout<<" Size of pulse area : "<<evt->pulseArea_phd_ODHG.size()<<" size of pulse amp : "<<evt->peakAmp_ODHG.size()<<endl;
    // ODHG
    if (evt->nPulses_ODHG != 0) {
        //if(evt->pulseStartTime_ns_ODHG[0] < firstPulseStart) firstPulseStart = evt->pulseStartTime_ns_ODLG[0];
        //if(evt->pulseEndTime_ns_ODLG[(evt->nPulses_ODLG)-1] > lastPulseEnd) lastPulseEnd = evt->pulseEndTime_ns_ODLG[(evt->nPulses_ODLG)-1];
        //h_triggerTimeStamp_s_ODHG->Fill(evt->triggerTimeStamp_s);
        total_pulse_areaHG =0;
        total_pulse_areaHG_cut =0;
        pulseArea_phd_ODHG_5mus_temp =0;
        total_pulse_PosareHG = 0;
        total_pulse_NegareHG = 0;
        pulseArea_phd_ODHG_3us=0,pulseArea_phd_ODHG_2us=0,pulseArea_phd_ODHG_1us=0,pulseArea_phd_ODHG_310us=0;
        Int_window_ODHG_1us=0,Int_window_ODHG_2us=0,Int_window_ODHG_3us=0,Int_window_ODHG_30us=0,Int_window_ODHG_130us=0,Int_window_ODHG_230us=0,Int_window_ODHG_330us=0,Int_window_ODHG_500us=0;
        Int_window_ODHG_1000us=0,Int_window_ODHG_1500us=0,Int_window_ODHG_2000us=0,Int_window_ODHG_2500us=0,Int_window_ODHG_3000us=0,Int_window_ODHG_3700us=0;
        for (int ip=0;ip<evt->pulseArea_phd_ODHG.size();ip++){
            total_pulse_areaHG+=evt->pulseArea_phd_ODHG[ip];
            total_pulse_PosareHG+=evt->positiveArea_phd_ODHG[ip];
            total_pulse_NegareHG+=evt->negativeArea_phd_ODHG[ip];
            h_startTime_ODHG->Fill(evt->pulseStartTime_ns_ODHG[ip]);
            h_pulseStartTime_ns_pulseArea_phd_ODHG->Fill(evt->pulseStartTime_ns_ODHG[ip],evt->pulseArea_phd_ODHG[ip]);
            h_pulse_amp_pulse_area_ODHG->Fill(evt->peakAmp_ODHG[ip]/evt->pulseArea_phd_ODHG[ip],evt->pulseArea_phd_ODHG[ip]);
            h_rmswdith_pulse_area_ODHG->Fill(evt->pulseArea_phd_ODHG[ip],(evt->pulseEndTime_ns_ODHG[ip]-evt->pulseStartTime_ns_ODHG[ip]));
            if (debug)
                cout<<" this is event : "<<n<<" looping pulse : "<<ip<<" RMS width is : "<<evt->rmsWidth_ns_ODHG[ip]<<endl;
            if (evt->pulseArea_phd_ODHG[ip]>3)
                h_pulseRatio_time_ODHG->Fill(evt->pulseStartTime_ns_ODHG[ip]/1000,evt->peakAmp_ODHG[ip]/evt->pulseArea_phd_ODHG[ip]);


            if (evt->pulseStartTime_ns_ODHG[ip]>10000){
                h_pulseStartTime_ns_pulseArea_phd_ODHG_5us->Fill(evt->pulseStartTime_ns_ODHG[ip],evt->pulseArea_phd_ODHG[ip]);
                pulseArea_phd_ODHG_5mus_temp+=evt->pulseArea_phd_ODHG[ip];
            }
            if (evt->pulseStartTime_ns_ODHG[ip]<10000 && evt->pulseStartTime_ns_ODHG[ip]>3000){
                pulseArea_phd_ODHG_310us+=evt->pulseArea_phd_ODHG[ip];
            }
            if (evt->pulseStartTime_ns_ODHG[ip]<1500 && evt->pulseStartTime_ns_ODHG[ip]>0){
                pulseArea_phd_ODHG_1us+=evt->pulseArea_phd_ODHG[ip];
            }
            if (evt->pulseStartTime_ns_ODHG[ip]>1500 && evt->pulseStartTime_ns_ODHG[ip]<2200){
                pulseArea_phd_ODHG_2us+=evt->pulseArea_phd_ODHG[ip];
            }
            if (evt->pulseStartTime_ns_ODHG[ip]<3000 && evt->pulseStartTime_ns_ODHG[ip]>2200){
                pulseArea_phd_ODHG_3us+=evt->pulseArea_phd_ODHG[ip];
            }
            if ((evt->peakAmp_ODHG[ip]/evt->pulseArea_phd_ODHG[ip]<0.015) && (evt->peakAmp_ODHG[ip]/evt->pulseArea_phd_ODHG[ip]>0.005)&& (evt->pulseArea_phd_ODHG[ip]>8)){
                total_pulse_areaHG_cut+=evt->pulseArea_phd_ODHG[ip];
            }

            if (evt->pulseStartTime_ns_ODHG[ip]>0 && evt->pulseStartTime_ns_ODHG[ip]<600)
                Int_window_ODHG_1us+=evt->pulseArea_phd_ODHG[ip];
            if (evt->pulseStartTime_ns_ODHG[ip]>0 && evt->pulseStartTime_ns_ODHG[ip]<2000)
                Int_window_ODHG_2us+=evt->pulseArea_phd_ODHG[ip];

            if (evt->pulseStartTime_ns_ODHG[ip]>0 && evt->pulseStartTime_ns_ODHG[ip]<3000)
                Int_window_ODHG_3us+=evt->pulseArea_phd_ODHG[ip];
            if (evt->pulseStartTime_ns_ODHG[ip]>0 && evt->pulseStartTime_ns_ODHG[ip]<30000)
                Int_window_ODHG_30us+=evt->pulseArea_phd_ODHG[ip];

            if (evt->pulseStartTime_ns_ODHG[ip]>0 && evt->pulseStartTime_ns_ODHG[ip]<130000)
                Int_window_ODHG_130us+=evt->pulseArea_phd_ODHG[ip];
            if (evt->pulseStartTime_ns_ODHG[ip]>0 && evt->pulseStartTime_ns_ODHG[ip]<230000)
                Int_window_ODHG_230us+=evt->pulseArea_phd_ODHG[ip];

            if (evt->pulseStartTime_ns_ODHG[ip]>0 && evt->pulseStartTime_ns_ODHG[ip]<330000)
                Int_window_ODHG_330us+=evt->pulseArea_phd_ODHG[ip];
            if (evt->pulseStartTime_ns_ODHG[ip]>0 && evt->pulseStartTime_ns_ODHG[ip]<500000)
                Int_window_ODHG_500us+=evt->pulseArea_phd_ODHG[ip];

            if (evt->pulseStartTime_ns_ODHG[ip]>0 && evt->pulseStartTime_ns_ODHG[ip]<1000000)
                Int_window_ODHG_1000us+=evt->pulseArea_phd_ODHG[ip];

            if (evt->pulseStartTime_ns_ODHG[ip]>0 && evt->pulseStartTime_ns_ODHG[ip]<1500000)
                Int_window_ODHG_1500us+=evt->pulseArea_phd_ODHG[ip];
            if (evt->pulseStartTime_ns_ODHG[ip]>0 && evt->pulseStartTime_ns_ODHG[ip]<2000000)
                Int_window_ODHG_2000us+=evt->pulseArea_phd_ODHG[ip];

            if (evt->pulseStartTime_ns_ODHG[ip]>0 && evt->pulseStartTime_ns_ODHG[ip]<2500000)
                Int_window_ODHG_2500us+=evt->pulseArea_phd_ODHG[ip];
            if (evt->pulseStartTime_ns_ODHG[ip]>0 && evt->pulseStartTime_ns_ODHG[ip]<3000000)
                Int_window_ODHG_3000us+=evt->pulseArea_phd_ODHG[ip];
            if (evt->pulseStartTime_ns_ODHG[ip]>0 && evt->pulseStartTime_ns_ODHG[ip]<3700000)
                Int_window_ODHG_3700us+=evt->pulseArea_phd_ODHG[ip];
            // Read pulses from each channel
            if ((evt->peakAmp_ODHG[ip]/evt->pulseArea_phd_ODHG[ip]<0.015) && (evt->peakAmp_ODHG[ip]/evt->pulseArea_phd_ODHG[ip]>0.005)&& (evt->pulseArea_phd_ODHG[ip]>8)){
                for (unsigned int ii=0; ii<evt->chPeakTime_ns_ODHG[ip].size(); ii++) {
                    if (evt->chPeakTime_ns_ODHG[ip][ii] < 20000)
                        h_channelPeakTime_20us->Fill(evt->chPeakTime_ns_ODHG[ip][ii]);
                    }
            }
        }
        h_pulseArea_phd_ODHG_3us->Fill(pulseArea_phd_ODHG_3us);
        h_pulseArea_phd_ODHG_2us->Fill(pulseArea_phd_ODHG_2us);
        h_pulseArea_phd_ODHG_1us->Fill(pulseArea_phd_ODHG_1us);
        h_pulseArea_phd_ODHG_int_1us->Fill(Int_window_ODHG_1us);
        h_pulseArea_phd_ODHG_int_2us->Fill(Int_window_ODHG_2us);
        h_pulseArea_phd_ODHG_int_3us->Fill(Int_window_ODHG_3us);
        h_pulseArea_phd_ODHG_int_30us->Fill(Int_window_ODHG_30us);

        h_pulseArea_phd_ODHG_int_130us->Fill(Int_window_ODHG_130us);
        h_pulseArea_phd_ODHG_int_230us->Fill(Int_window_ODHG_230us);
        h_pulseArea_phd_ODHG_int_330us->Fill(Int_window_ODHG_330us);
        h_pulseArea_phd_ODHG_int_500us->Fill(Int_window_ODHG_500us);

        h_pulseArea_phd_ODHG_int_1000us->Fill(Int_window_ODHG_1000us);
        h_pulseArea_phd_ODHG_int_1500us->Fill(Int_window_ODHG_1500us);
        h_pulseArea_phd_ODHG_int_2000us->Fill(Int_window_ODHG_2000us);
        h_pulseArea_phd_ODHG_int_2500us->Fill(Int_window_ODHG_2500us);
        h_pulseArea_phd_ODHG_int_3000us->Fill(Int_window_ODHG_3000us);
        h_pulseArea_phd_ODHG_int_3700us->Fill(Int_window_ODHG_3700us);

        h_pulseArea_phd_ODHG_5mus->Fill(pulseArea_phd_ODHG_5mus_temp);
        h_pulseArea_phd_ODHG_310us->Fill(pulseArea_phd_ODHG_310us);
        h_pulseArea_phd_ODHG->Fill(total_pulse_areaHG);
        h_pulseArea_phd_ODHG_cut->Fill(total_pulse_areaHG_cut);
        h_positiveArea_phd_ODHG->Fill(total_pulse_PosareHG);
        h_negativeArea_phd_ODHG->Fill(total_pulse_NegareHG);

    }
    // Skin
    if (evt->nPulses_Skin != 0) {
        //if(evt->pulseStartTime_ns_Skin[0] < firstPulseStart) firstPulseStart = evt->pulseStartTime_ns_ODLG[0];
        //if(evt->pulseEndTime_ns_ODLG[(evt->nPulses_ODLG)-1] > lastPulseEnd) lastPulseEnd = evt->pulseEndTime_ns_ODLG[(evt->nPulses_ODLG)-1];
        //h_triggerTimeStamp_s_Skin->Fill(evt->triggerTimeStamp_s);
        for (int ip=0;ip<evt->pulseArea_phd_Skin.size();ip++){
        h_pulseStartTime_ns_pulseArea_phd_Skin->Fill(evt->pulseStartTime_ns_Skin[ip],evt->pulseArea_phd_Skin[ip]);
        }

        h_pulseArea_phd_Skin->Fill(evt->pulseArea_phd_Skin[0]);
        h_positiveArea_phd_Skin->Fill(evt->positiveArea_phd_Skin[0]);
        h_negativeArea_phd_Skin->Fill(evt->negativeArea_phd_Skin[0]);
    }
    //check some OD pulses
    if (evt->nPulses_ODLG != 0) {
        if(evt->pulseStartTime_ns_ODLG[0] < firstPulseStart) firstPulseStart = evt->pulseStartTime_ns_ODLG[0];
        if(evt->pulseEndTime_ns_ODLG[(evt->nPulses_ODLG)-1] > lastPulseEnd) lastPulseEnd = evt->pulseEndTime_ns_ODLG[(evt->nPulses_ODLG)-1];
    }
    duration = lastPulseEnd - firstPulseStart;
    sum_pule_time+=duration;
    h_pulse_duration_ms_ODLG->Fill(duration/1E6); //fill in ms
    //cout<<" Size of pulse area HG is : "<<evt->pulseArea_phd_ODHG.size()<<endl;

    // OD Low gain
    //    for (int i = 0; i < evt->nPulses_ODLG; ++i) {
    //    cout<<"------------"<<evt->nPulses_ODLG<<"------------"<<endl;
    h_nPulses_ODLG->Fill(evt->nPulses_ODLG);
    //cout<<"number of pulses " <<evt->nPulses_ODLG<< " pulsearea 0"<<evt->pulseArea_phd_ODLG[0]<<endl;
    float all_PulseArea_phd=0;
    for(int i=0; i<evt->nPulses_ODLG; ++i){
            //cout << " area: " << evt->pulseArea_phd_ODLG[i] << "  size: " <<  evt->chPulseArea_phd_ODLG[i].size() << endl;
        float all_chPulseArea_phd=0;
        for (unsigned int ii=0; ii<evt->chPulseArea_phd_ODLG[i].size(); ii++) {
            //cout <<"\t"<<i<<"     "<< ii << "   " << evt->channelID_ODLG[i][ii]-1000 << "   " << evt->chPulseArea_phd_ODLG[i][ii] <<"  ns " << evt->chPeakTime_ns_ODLG[i][ii]<< endl;
            all_chPulseArea_phd+=evt->chPulseArea_phd_ODLG[i][ii];
            //if (evt->chPulseArea_phd_ODLG[i][ii] > 3)
            all_PulseArea_phd+=evt->chPulseArea_phd_ODLG[i][ii];
            h_chPulseArea_phd_ODLG->Fill(evt->chPulseArea_phd_ODLG[i][ii]);
        	h_chPeakTime_ns_ODLG->Fill(evt->chPeakTime_ns_ODLG[i][ii]);
        	h_channelID_ODLG->Fill(evt->channelID_ODLG[i][ii]-1000);
        	int pmtid =(evt->channelID_ODLG[i][0])-1801;
        	hvec_chPulseArea_phd_OD.at(pmtid)->Fill(evt->chPulseArea_phd_ODLG[i][ii]);
        }

            //cout<<"\t"<<"all_chPulseArea_phd "<<all_chPulseArea_phd<<endl;
    }
    h_summed_pulseArea_phd_ODLG->Fill(all_PulseArea_phd);
    //if (all_PulseArea_phd >400)
        //cout<<" Summed pulse area is : "<<all_PulseArea_phd<<" RQ HG pulse area is : "<<evt->pulseArea_phd_ODHG[0]<<" RQ LG pulse area is : "<<evt->pulseArea_phd_ODLG[0]<<endl;
    //getchar();
    //    cout<<"all_PulseArea_phd "<<all_PulseArea_phd<<endl;
    // if(evt->nPulses_ODLG != 0)
    // cout<<"number of pulses" <<evt->nPulses_ODLG<< " pulsearea 0"<<evt->pulseArea_phd_ODLG[0]<<endl;
    // for (int i = 0; i < evt->nPulses_ODLG; ++i) {
    //   //      if ( evt->singlePEprobability_ODLG[i] != 0) {
    //   if ( 1 ) {
    // 	  pmtID = (evt->channelID_ODLG[i][0])-1000;
    // 	  cout<<evt->singlePEprobability_ODLG[i]<< " " <<pmtID<<" "<<evt->chPulseArea_phd_ODLG[i][0]<<" "<<evt->chPulseArea_phd_ODLG[i][1]<<" "<<evt->chPulseArea_phd_ODLG[i][2]<<endl;
    //   }
    // }

    }//int nevents

    //--------------------------------------------------
    // Convert to rate
    //--------------------------------------------------
    int count = 0;
    unsigned long long time = 0;
    unsigned long long time_day = 0;
    double rate = 0;
    //------------------------------------------------
    // end event loop
    //------------------------------------------------


    //write and close output file
    outfile->Write();
    outfile->Close();

    std::cout<<"times_ns: "<<min_time_ns<<" "<<max_time_ns<<" "<<processed_events<<endl;
    std::cout<<"summed time of all pulses "<<sum_pule_time/1E9<<" sec"<<endl;
    std::cout<<"Overall time "<<last_s-first_s <<" sec, rate: "<<processed_events/(last_s-first_s)<<"evts/sec"<<endl;
    cout << "Done!"<<" "<<clock->RealTime()<<" s."<<endl;

    delete chain;
    delete clock;

    }

    /**
    * ## Load chain <a name="load"></a>
    *
    * Open a set of ROOT files specified in txt file list
    * ### Arguments:
    * - `txtFileList`: Name of the file that contains the list of file to process
    * - `chain`: Pointer to the TChain
    *
    */
void load_chain(TString txtFileList, TChain* chain){

    cout << "Loading file names from "<<txtFileList << " into "<<chain->GetName()<<endl;

    //------------------------------------------------
    // Open the txtFileList
    //------------------------------------------------
    ifstream fileList(txtFileList);
    string file;

    //------------------------------------------------
    // Check if the txtFileList exists, exit if not
    //------------------------------------------------
    if (fileList.is_open()) {
    //------------------------------------------------
    // Loop over the txtFileList
    //------------------------------------------------
        while ( getline(fileList, file) ) {
          //------------------------------------------------
          // Add the current file in the TChain
          //------------------------------------------------
          chain->AddFile(file.c_str());
        }
        //------------------------------------------------
        // Close the txtFileList
        //------------------------------------------------
        fileList.close();
    }else{
        cout<<"The file "<< txtFileList <<" doesn't exist. Exiting !!"<<endl;
        exit(-1);
    }
}
void load_chain_by_name(TString inputName, TChain* chain, int nfiles, int position){

    cout << "Loading file names from "<<inputName << " into "<<chain->GetName()<<endl;

    if (inputName.Contains("AmLi")){
        if (position==1336){
            for (int i=0; i<nfiles;i++){
                char name[150];
                sprintf(name,"/projecta/projectdirs/lz/data/MDC2/calibration/LZAP-3.10.0-PHYSICS-3.10.0/AmLi_%u/lz_AmLi_1336000010_%04u_lzap.root",position,i);
                chain->AddFile(name);
            }
        }
        if (position==700){
            for (int i=0; i<nfiles;i++){
                char name[150];
                sprintf(name,"/projecta/projectdirs/lz/data/MDC2/calibration/LZAP-3.10.0-PHYSICS-3.10.0/AmLi_%u/lz_AmLi_700000010_%04u_lzap.root",position,i);
                chain->AddFile(name);
            }
        }
    }
    else if (inputName.Contains("Na")){
        if (position==1336){
            for (int i=0; i<nfiles;i++){
                char name[150];
                sprintf(name,"/projecta/projectdirs/lz/data/MDC2/calibration/LZAP-3.10.0-PHYSICS-3.10.0/Na22_%u/lz_Na22_1336000010_%04u_lzap.root",position,i);
                chain->AddFile(name);
            }
        }
        if (position==700){
            for (int i=0; i<nfiles;i++){
                char name[150];
                sprintf(name,"/projecta/projectdirs/lz/data/MDC2/calibration/LZAP-3.10.0-PHYSICS-3.10.0/Na22_%u/lz_Na22_700000010_%04u_lzap.root",position,i);
                chain->AddFile(name);
            }
        }
    }
    else if (inputName.Contains("background")){
        for (int i=0; i<nfiles;i++){
            char name[150];
            sprintf(name,"/projecta/projectdirs/lz/data/MDC2/background/LZAP-3.10.0-PHYSICS-3.10.0/20170927/lz_20170927%02u_lzap.root",i);
            chain->AddFile(name);
        }
    }

}
