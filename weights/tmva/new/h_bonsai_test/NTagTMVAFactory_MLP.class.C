// Class: ReadMLP
// Automatically generated by MethodBase::MakeClass
//

/* configuration options =====================================================

#GEN -*-*-*-*-*-*-*-*-*-*-*- general info -*-*-*-*-*-*-*-*-*-*-*-

Method         : MLP::MLP
TMVA Release   : 4.2.0         [262656]
ROOT Release   : 5.34/38       [336422]
Creator        : han
Date           : Sat Apr 30 06:25:27 2022
Host           : Linux rz5950x.km.icrr.u-tokyo.ac.jp 4.18.0-305.3.1.el8_4.x86_64 #1 SMP Mon May 17 10:08:25 EDT 2021 x86_64 x86_64 x86_64 GNU/Linux
Dir            : /disk1/disk02/calib3/usr/han/mc/thermal_train
Training events: 6949538
Analysis type  : [Classification]


#OPT -*-*-*-*-*-*-*-*-*-*-*-*- options -*-*-*-*-*-*-*-*-*-*-*-*-

# Set by User:
NCycles: "300" [Number of training cycles]
HiddenLayers: "N+1,N-1" [Specification of hidden layer architecture]
NeuronType: "sigmoid" [Neuron activation function type]
EstimatorType: "CE" [MSE (Mean Square Estimator) for Gaussian Likelihood or CE(Cross-Entropy) for Bernoulli Likelihood]
V: "True" [Verbose output (short form of "VerbosityLevel" below - overrides the latter one)]
VarTransform: "N" [List of variable transformations performed before training, e.g., "D_Background,P_Signal,G,N_AllClasses" for: "Decorrelation, PCA-transformation, Gaussianisation, Normalisation, each for the given class of events ('AllClasses' denotes all events of all classes, if no class indication is given, 'All' is assumed)"]
H: "True" [Print method-specific help message]
TestRate: "5" [Test for overtraining performed at each #th epochs]
UseRegulator: "True" [Use regulator to avoid over-training]
# Default:
RandomSeed: "1" [Random seed for initial synapse weights (0 means unique seed for each run; default value '1')]
NeuronInputType: "sum" [Neuron input function type]
VerbosityLevel: "Verbose" [Verbosity level]
CreateMVAPdfs: "False" [Create PDFs for classifier outputs (signal and background)]
IgnoreNegWeightsInTraining: "False" [Events with negative weights are ignored in the training (but are included for testing and performance evaluation)]
TrainingMethod: "BP" [Train with Back-Propagation (BP), BFGS Algorithm (BFGS), or Genetic Algorithm (GA - slower and worse)]
LearningRate: "2.000000e-02" [ANN learning rate parameter]
DecayRate: "1.000000e-02" [Decay rate for learning parameter]
EpochMonitoring: "False" [Provide epoch-wise monitoring plots according to TestRate (caution: causes big ROOT output file!)]
Sampling: "1.000000e+00" [Only 'Sampling' (randomly selected) events are trained each epoch]
SamplingEpoch: "1.000000e+00" [Sampling is used for the first 'SamplingEpoch' epochs, afterwards, all events are taken for training]
SamplingImportance: "1.000000e+00" [ The sampling weights of events in epochs which successful (worse estimator than before) are multiplied with SamplingImportance, else they are divided.]
SamplingTraining: "True" [The training sample is sampled]
SamplingTesting: "False" [The testing sample is sampled]
ResetStep: "50" [How often BFGS should reset history]
Tau: "3.000000e+00" [LineSearch "size step"]
BPMode: "sequential" [Back-propagation learning mode: sequential or batch]
BatchSize: "-1" [Batch size: number of events/batch, only set if in Batch Mode, -1 for BatchSize=number_of_events]
ConvergenceImprove: "1.000000e-30" [Minimum improvement which counts as improvement (<0 means automatic convergence check is turned off)]
ConvergenceTests: "-1" [Number of steps (without improvement) required for convergence (<0 means automatic convergence check is turned off)]
UpdateLimit: "10000" [Maximum times of regulator update]
CalculateErrors: "False" [Calculates inverse Hessian matrix at the end of the training to be able to calculate the uncertainties of an MVA value]
WeightRange: "1.000000e+00" [Take the events for the estimator calculations from small deviations from the desired value to large deviations only over the weight range]
##


#VAR -*-*-*-*-*-*-*-*-*-*-*-* variables *-*-*-*-*-*-*-*-*-*-*-*-

NVar 12
NHits                         NHits                         NHits                         NHits                                                           'F'    [4,1251]
N200                          N200                          N200                          N200                                                            'F'    [6,1697]
TRMS                          TRMS                          TRMS                          TRMS                                                            'F'    [0,6.98510789871]
Beta1                         Beta1                         Beta1                         Beta1                                                           'F'    [-0.249203011394,0.999779224396]
Beta5                         Beta5                         Beta5                         Beta5                                                           'F'    [-0.232669591904,0.996693611145]
OpeningAngleMean              OpeningAngleMean              OpeningAngleMean              OpeningAngleMean                                                'F'    [1.205488801,89.6580581665]
OpeningAngleSkew              OpeningAngleSkew              OpeningAngleSkew              OpeningAngleSkew                                                'F'    [-310.384185791,502.098358154]
OpeningAngleStdev             OpeningAngleStdev             OpeningAngleStdev             OpeningAngleStdev                                               'F'    [0.155534058809,45.8755836487]
MeanDirAngleMean              MeanDirAngleMean              MeanDirAngleMean              MeanDirAngleMean                                                'F'    [0.711865603924,91.436340332]
MeanDirAngleRMS               MeanDirAngleRMS               MeanDirAngleRMS               MeanDirAngleRMS                                                 'F'    [0.227185696363,82.428024292]
DWall                         DWall                         DWall                         DWall                                                           'F'    [-100,1689.49121094]
DWallMeanDir                  DWallMeanDir                  DWallMeanDir                  DWallMeanDir                                                    'F'    [34.4869842529,5091.4375]
NSpec 1
Label                         Label                         Label                         I                                                               'F'    [0,3]


============================================================================ */

#include <vector>
#include <cmath>
#include <string>
#include <iostream>

#ifndef IClassifierReader__def
#define IClassifierReader__def

class IClassifierReader {

 public:

   // constructor
   IClassifierReader() : fStatusIsClean( true ) {}
   virtual ~IClassifierReader() {}

   // return classifier response
   virtual double GetMvaValue( const std::vector<double>& inputValues ) const = 0;

   // returns classifier status
   bool IsStatusClean() const { return fStatusIsClean; }

 protected:

   bool fStatusIsClean;
};

#endif

class ReadMLP : public IClassifierReader {

 public:

   // constructor
   ReadMLP( std::vector<std::string>& theInputVars ) 
      : IClassifierReader(),
        fClassName( "ReadMLP" ),
        fNvars( 12 ),
        fIsNormalised( false )
   {      
      // the training input variables
      const char* inputVars[] = { "NHits", "N200", "TRMS", "Beta1", "Beta5", "OpeningAngleMean", "OpeningAngleSkew", "OpeningAngleStdev", "MeanDirAngleMean", "MeanDirAngleRMS", "DWall", "DWallMeanDir" };

      // sanity checks
      if (theInputVars.size() <= 0) {
         std::cout << "Problem in class \"" << fClassName << "\": empty input vector" << std::endl;
         fStatusIsClean = false;
      }

      if (theInputVars.size() != fNvars) {
         std::cout << "Problem in class \"" << fClassName << "\": mismatch in number of input values: "
                   << theInputVars.size() << " != " << fNvars << std::endl;
         fStatusIsClean = false;
      }

      // validate input variables
      for (size_t ivar = 0; ivar < theInputVars.size(); ivar++) {
         if (theInputVars[ivar] != inputVars[ivar]) {
            std::cout << "Problem in class \"" << fClassName << "\": mismatch in input variable names" << std::endl
                      << " for variable [" << ivar << "]: " << theInputVars[ivar].c_str() << " != " << inputVars[ivar] << std::endl;
            fStatusIsClean = false;
         }
      }

      // initialize min and max vectors (for normalisation)
      fVmin[0] = -1;
      fVmax[0] = 1;
      fVmin[1] = -1;
      fVmax[1] = 0.99999988079071;
      fVmin[2] = -1;
      fVmax[2] = 1;
      fVmin[3] = -1;
      fVmax[3] = 1;
      fVmin[4] = -1;
      fVmax[4] = 1;
      fVmin[5] = -1;
      fVmax[5] = 1;
      fVmin[6] = -1;
      fVmax[6] = 0.99999988079071;
      fVmin[7] = -1;
      fVmax[7] = 1;
      fVmin[8] = -1;
      fVmax[8] = 1;
      fVmin[9] = -1;
      fVmax[9] = 1;
      fVmin[10] = -1;
      fVmax[10] = 1;
      fVmin[11] = -1;
      fVmax[11] = 0.99999988079071;

      // initialize input variable types
      fType[0] = 'F';
      fType[1] = 'F';
      fType[2] = 'F';
      fType[3] = 'F';
      fType[4] = 'F';
      fType[5] = 'F';
      fType[6] = 'F';
      fType[7] = 'F';
      fType[8] = 'F';
      fType[9] = 'F';
      fType[10] = 'F';
      fType[11] = 'F';

      // initialize constants
      Initialize();

      // initialize transformation
      InitTransform();
   }

   // destructor
   virtual ~ReadMLP() {
      Clear(); // method-specific
   }

   // the classifier response
   // "inputValues" is a vector of input values in the same order as the 
   // variables given to the constructor
   double GetMvaValue( const std::vector<double>& inputValues ) const;

 private:

   // method-specific destructor
   void Clear();

   // input variable transformation

   double fMin_1[3][12];
   double fMax_1[3][12];
   void InitTransform_1();
   void Transform_1( std::vector<double> & iv, int sigOrBgd ) const;
   void InitTransform();
   void Transform( std::vector<double> & iv, int sigOrBgd ) const;

   // common member variables
   const char* fClassName;

   const size_t fNvars;
   size_t GetNvar()           const { return fNvars; }
   char   GetType( int ivar ) const { return fType[ivar]; }

   // normalisation of input variables
   const bool fIsNormalised;
   bool IsNormalised() const { return fIsNormalised; }
   double fVmin[12];
   double fVmax[12];
   double NormVariable( double x, double xmin, double xmax ) const {
      // normalise to output range: [-1, 1]
      return 2*(x - xmin)/(xmax - xmin) - 1.0;
   }

   // type of input variable: 'F' or 'I'
   char   fType[12];

   // initialize internal variables
   void Initialize();
   double GetMvaValue__( const std::vector<double>& inputValues ) const;

   // private members (method specific)

   double ActivationFnc(double x) const;
   double OutputActivationFnc(double x) const;

   int fLayers;
   int fLayerSize[4];
   double fWeightMatrix0to1[14][13];   // weight matrix from layer 0 to 1
   double fWeightMatrix1to2[12][14];   // weight matrix from layer 1 to 2
   double fWeightMatrix2to3[1][12];   // weight matrix from layer 2 to 3

   double * fWeights[4];
};

inline void ReadMLP::Initialize()
{
   // build network structure
   fLayers = 4;
   fLayerSize[0] = 13; fWeights[0] = new double[13]; 
   fLayerSize[1] = 14; fWeights[1] = new double[14]; 
   fLayerSize[2] = 12; fWeights[2] = new double[12]; 
   fLayerSize[3] = 1; fWeights[3] = new double[1]; 
   // weight matrix from layer 0 to 1
   fWeightMatrix0to1[0][0] = 20.6331024166566;
   fWeightMatrix0to1[1][0] = 4.78114411736243;
   fWeightMatrix0to1[2][0] = -15.7526094255152;
   fWeightMatrix0to1[3][0] = -10.0988363675542;
   fWeightMatrix0to1[4][0] = 29.6925411751881;
   fWeightMatrix0to1[5][0] = -7.89171185585767;
   fWeightMatrix0to1[6][0] = -31.9883617277994;
   fWeightMatrix0to1[7][0] = 7.64641598995935;
   fWeightMatrix0to1[8][0] = -19.4148491835767;
   fWeightMatrix0to1[9][0] = -20.6930410305349;
   fWeightMatrix0to1[10][0] = 11.7336328997007;
   fWeightMatrix0to1[11][0] = 22.5513593036361;
   fWeightMatrix0to1[12][0] = -30.0052765851993;
   fWeightMatrix0to1[0][1] = -1.43133147814392;
   fWeightMatrix0to1[1][1] = 22.1714040839131;
   fWeightMatrix0to1[2][1] = 2.93409226113037;
   fWeightMatrix0to1[3][1] = 19.0062815742849;
   fWeightMatrix0to1[4][1] = -9.208425328661;
   fWeightMatrix0to1[5][1] = 4.14475256976353;
   fWeightMatrix0to1[6][1] = 28.5336824221064;
   fWeightMatrix0to1[7][1] = -7.41231715676441;
   fWeightMatrix0to1[8][1] = 8.49972674303439;
   fWeightMatrix0to1[9][1] = 10.4954107627228;
   fWeightMatrix0to1[10][1] = -12.243051399502;
   fWeightMatrix0to1[11][1] = -3.83031460400557;
   fWeightMatrix0to1[12][1] = 21.2197949871228;
   fWeightMatrix0to1[0][2] = -0.256705967809949;
   fWeightMatrix0to1[1][2] = -0.391589012113551;
   fWeightMatrix0to1[2][2] = 0.736193576855913;
   fWeightMatrix0to1[3][2] = -0.289231707941688;
   fWeightMatrix0to1[4][2] = 0.108124460724095;
   fWeightMatrix0to1[5][2] = 0.604479216170244;
   fWeightMatrix0to1[6][2] = 0.331716073183996;
   fWeightMatrix0to1[7][2] = -0.124703254776996;
   fWeightMatrix0to1[8][2] = 0.0724867438870276;
   fWeightMatrix0to1[9][2] = 0.00895694411480344;
   fWeightMatrix0to1[10][2] = -0.749368818318895;
   fWeightMatrix0to1[11][2] = 0.288677722169308;
   fWeightMatrix0to1[12][2] = 0.206719301654411;
   fWeightMatrix0to1[0][3] = 8.21058972308815;
   fWeightMatrix0to1[1][3] = 1.47250211161484;
   fWeightMatrix0to1[2][3] = -20.6928281102249;
   fWeightMatrix0to1[3][3] = -1.48906374014781;
   fWeightMatrix0to1[4][3] = 2.50041330313255;
   fWeightMatrix0to1[5][3] = -8.98955551380959;
   fWeightMatrix0to1[6][3] = 1.41880994425997;
   fWeightMatrix0to1[7][3] = 2.04255655954971;
   fWeightMatrix0to1[8][3] = 4.64163309943313;
   fWeightMatrix0to1[9][3] = -6.24215388824489;
   fWeightMatrix0to1[10][3] = -3.85528368222768;
   fWeightMatrix0to1[11][3] = 13.0699375006525;
   fWeightMatrix0to1[12][3] = 5.22360672162869;
   fWeightMatrix0to1[0][4] = 0.64293422958023;
   fWeightMatrix0to1[1][4] = 0.106950073782462;
   fWeightMatrix0to1[2][4] = -0.399999196092314;
   fWeightMatrix0to1[3][4] = -5.94408936310007;
   fWeightMatrix0to1[4][4] = -0.931083039983092;
   fWeightMatrix0to1[5][4] = 0.289795629581358;
   fWeightMatrix0to1[6][4] = 2.44213180152614;
   fWeightMatrix0to1[7][4] = -0.841298802211338;
   fWeightMatrix0to1[8][4] = -1.42732990681091;
   fWeightMatrix0to1[9][4] = -1.40016863178781;
   fWeightMatrix0to1[10][4] = -1.79719542747694;
   fWeightMatrix0to1[11][4] = 2.23490416755075;
   fWeightMatrix0to1[12][4] = 3.39908960971237;
   fWeightMatrix0to1[0][5] = 0.594609953307967;
   fWeightMatrix0to1[1][5] = -1.17061275172923;
   fWeightMatrix0to1[2][5] = -1.41190819487844;
   fWeightMatrix0to1[3][5] = -0.390015371509648;
   fWeightMatrix0to1[4][5] = 1.47874981337351;
   fWeightMatrix0to1[5][5] = -3.60254500848963;
   fWeightMatrix0to1[6][5] = 0.218400698796463;
   fWeightMatrix0to1[7][5] = 5.13250835057317;
   fWeightMatrix0to1[8][5] = 6.51275762300378;
   fWeightMatrix0to1[9][5] = -3.71232239202327;
   fWeightMatrix0to1[10][5] = -5.40426190044636;
   fWeightMatrix0to1[11][5] = -4.2962025779021;
   fWeightMatrix0to1[12][5] = -2.60648801003267;
   fWeightMatrix0to1[0][6] = 1.04182656690171;
   fWeightMatrix0to1[1][6] = 2.9646328766035;
   fWeightMatrix0to1[2][6] = -0.788085191501137;
   fWeightMatrix0to1[3][6] = -4.24970831492896;
   fWeightMatrix0to1[4][6] = 2.7873097317075;
   fWeightMatrix0to1[5][6] = -3.35032804013926;
   fWeightMatrix0to1[6][6] = -0.053276007512671;
   fWeightMatrix0to1[7][6] = 1.71051489868935;
   fWeightMatrix0to1[8][6] = 9.93715477440979;
   fWeightMatrix0to1[9][6] = -0.0252847378489915;
   fWeightMatrix0to1[10][6] = 2.82169344921543;
   fWeightMatrix0to1[11][6] = -8.1618702285552;
   fWeightMatrix0to1[12][6] = -5.14047561388682;
   fWeightMatrix0to1[0][7] = -1.3138296239955;
   fWeightMatrix0to1[1][7] = 0.841045885086335;
   fWeightMatrix0to1[2][7] = 0.069605419363542;
   fWeightMatrix0to1[3][7] = -5.44576901856458;
   fWeightMatrix0to1[4][7] = -0.03231810210759;
   fWeightMatrix0to1[5][7] = -3.44385815987981;
   fWeightMatrix0to1[6][7] = -0.215157470087677;
   fWeightMatrix0to1[7][7] = -3.61903867779529;
   fWeightMatrix0to1[8][7] = 2.28963519430111;
   fWeightMatrix0to1[9][7] = 4.01044752267298;
   fWeightMatrix0to1[10][7] = 2.57079613664723;
   fWeightMatrix0to1[11][7] = 3.86370933894391;
   fWeightMatrix0to1[12][7] = 1.32608460936958;
   fWeightMatrix0to1[0][8] = 9.98152373263167;
   fWeightMatrix0to1[1][8] = 4.0556069477245;
   fWeightMatrix0to1[2][8] = -27.193358273404;
   fWeightMatrix0to1[3][8] = -3.55182490948047;
   fWeightMatrix0to1[4][8] = -0.324066875066055;
   fWeightMatrix0to1[5][8] = -0.120391501531344;
   fWeightMatrix0to1[6][8] = 3.65410635840906;
   fWeightMatrix0to1[7][8] = 0.353544874138926;
   fWeightMatrix0to1[8][8] = 3.42200649825403;
   fWeightMatrix0to1[9][8] = -6.646515934991;
   fWeightMatrix0to1[10][8] = 0.00834197921637894;
   fWeightMatrix0to1[11][8] = 17.0317105519684;
   fWeightMatrix0to1[12][8] = 4.82214013318106;
   fWeightMatrix0to1[0][9] = 0.32689619263027;
   fWeightMatrix0to1[1][9] = 0.503983986346557;
   fWeightMatrix0to1[2][9] = -10.8439037278521;
   fWeightMatrix0to1[3][9] = -1.6882453013058;
   fWeightMatrix0to1[4][9] = 2.74019287769585;
   fWeightMatrix0to1[5][9] = -1.60478042484139;
   fWeightMatrix0to1[6][9] = -0.525906704062158;
   fWeightMatrix0to1[7][9] = 1.5208595771167;
   fWeightMatrix0to1[8][9] = 3.7475359677314;
   fWeightMatrix0to1[9][9] = 0.783885562235434;
   fWeightMatrix0to1[10][9] = -0.744352657150252;
   fWeightMatrix0to1[11][9] = -0.290239373243681;
   fWeightMatrix0to1[12][9] = 0.298930267535351;
   fWeightMatrix0to1[0][10] = 0.804132426920763;
   fWeightMatrix0to1[1][10] = -9.5214761034127;
   fWeightMatrix0to1[2][10] = -0.18341175393415;
   fWeightMatrix0to1[3][10] = -2.31014918373608;
   fWeightMatrix0to1[4][10] = -0.211403485834478;
   fWeightMatrix0to1[5][10] = 0.102943856129676;
   fWeightMatrix0to1[6][10] = 1.58533769274927;
   fWeightMatrix0to1[7][10] = -17.9200706005441;
   fWeightMatrix0to1[8][10] = 0.236924666710096;
   fWeightMatrix0to1[9][10] = 1.40870777266694;
   fWeightMatrix0to1[10][10] = 2.90262738350279;
   fWeightMatrix0to1[11][10] = -0.804396933453682;
   fWeightMatrix0to1[12][10] = -0.482387462695184;
   fWeightMatrix0to1[0][11] = 1.57633539274265;
   fWeightMatrix0to1[1][11] = -0.576831487323127;
   fWeightMatrix0to1[2][11] = 0.922930807600329;
   fWeightMatrix0to1[3][11] = -0.476835099321097;
   fWeightMatrix0to1[4][11] = -1.73292129517522;
   fWeightMatrix0to1[5][11] = 1.0100175066726;
   fWeightMatrix0to1[6][11] = 2.93402291980015;
   fWeightMatrix0to1[7][11] = 0.12337481918573;
   fWeightMatrix0to1[8][11] = 0.030383502679058;
   fWeightMatrix0to1[9][11] = 2.45992356531383;
   fWeightMatrix0to1[10][11] = -0.18330887766393;
   fWeightMatrix0to1[11][11] = -0.626876248150847;
   fWeightMatrix0to1[12][11] = -1.49284068551285;
   fWeightMatrix0to1[0][12] = 18.5888990082174;
   fWeightMatrix0to1[1][12] = 18.6921247558028;
   fWeightMatrix0to1[2][12] = -19.4036592291368;
   fWeightMatrix0to1[3][12] = 0.960283205951334;
   fWeightMatrix0to1[4][12] = 20.3209319174408;
   fWeightMatrix0to1[5][12] = -9.33312449191789;
   fWeightMatrix0to1[6][12] = -2.9876626940233;
   fWeightMatrix0to1[7][12] = -18.9394115583947;
   fWeightMatrix0to1[8][12] = -10.3032822010879;
   fWeightMatrix0to1[9][12] = -6.8172262942113;
   fWeightMatrix0to1[10][12] = 0.818748961937257;
   fWeightMatrix0to1[11][12] = 20.2345772365158;
   fWeightMatrix0to1[12][12] = -7.87884174711692;
   // weight matrix from layer 1 to 2
   fWeightMatrix1to2[0][0] = -1.08100127751148;
   fWeightMatrix1to2[1][0] = -0.539654779684173;
   fWeightMatrix1to2[2][0] = 9.21803508337871;
   fWeightMatrix1to2[3][0] = 1.00596742877205;
   fWeightMatrix1to2[4][0] = 3.74564627081279;
   fWeightMatrix1to2[5][0] = 4.5699706087346;
   fWeightMatrix1to2[6][0] = 1.24938559458385;
   fWeightMatrix1to2[7][0] = -5.64127846057246;
   fWeightMatrix1to2[8][0] = 0.0958500328194188;
   fWeightMatrix1to2[9][0] = 5.64877916072098;
   fWeightMatrix1to2[10][0] = 2.21308591252995;
   fWeightMatrix1to2[0][1] = -3.4224804818845;
   fWeightMatrix1to2[1][1] = 1.71292159758118;
   fWeightMatrix1to2[2][1] = 3.55010162347022;
   fWeightMatrix1to2[3][1] = -3.01043091465833;
   fWeightMatrix1to2[4][1] = -4.35261854583868;
   fWeightMatrix1to2[5][1] = 4.72070499160561;
   fWeightMatrix1to2[6][1] = -0.183924730748004;
   fWeightMatrix1to2[7][1] = 12.170670003006;
   fWeightMatrix1to2[8][1] = -1.13755194675595;
   fWeightMatrix1to2[9][1] = 4.03277283435804;
   fWeightMatrix1to2[10][1] = -4.82957894633063;
   fWeightMatrix1to2[0][2] = 4.96228453441913;
   fWeightMatrix1to2[1][2] = -2.7410670486917;
   fWeightMatrix1to2[2][2] = -6.12539207290128;
   fWeightMatrix1to2[3][2] = -1.32180655117746;
   fWeightMatrix1to2[4][2] = -1.18501165015661;
   fWeightMatrix1to2[5][2] = -7.5915206788365;
   fWeightMatrix1to2[6][2] = -2.94759178079451;
   fWeightMatrix1to2[7][2] = 3.72400306914365;
   fWeightMatrix1to2[8][2] = 6.56982682687401;
   fWeightMatrix1to2[9][2] = -1.32030037783328;
   fWeightMatrix1to2[10][2] = -1.33080863738244;
   fWeightMatrix1to2[0][3] = -2.42767897688935;
   fWeightMatrix1to2[1][3] = -0.451547793682452;
   fWeightMatrix1to2[2][3] = 0.660298588042773;
   fWeightMatrix1to2[3][3] = -1.82547141612476;
   fWeightMatrix1to2[4][3] = -1.88457686190941;
   fWeightMatrix1to2[5][3] = 3.19978136932124;
   fWeightMatrix1to2[6][3] = -1.49887942004556;
   fWeightMatrix1to2[7][3] = 1.41361185349166;
   fWeightMatrix1to2[8][3] = -2.79299273773311;
   fWeightMatrix1to2[9][3] = 0.599882026632057;
   fWeightMatrix1to2[10][3] = -0.2070793423037;
   fWeightMatrix1to2[0][4] = -7.45056951516021;
   fWeightMatrix1to2[1][4] = 3.18591626578547;
   fWeightMatrix1to2[2][4] = 5.62239827390971;
   fWeightMatrix1to2[3][4] = -0.6214288443335;
   fWeightMatrix1to2[4][4] = 5.94536540778049;
   fWeightMatrix1to2[5][4] = 3.36847240737185;
   fWeightMatrix1to2[6][4] = -0.328072689371548;
   fWeightMatrix1to2[7][4] = -7.77732705638395;
   fWeightMatrix1to2[8][4] = 0.447756977723603;
   fWeightMatrix1to2[9][4] = 2.39874706158412;
   fWeightMatrix1to2[10][4] = 2.6561033777793;
   fWeightMatrix1to2[0][5] = 1.41050990656854;
   fWeightMatrix1to2[1][5] = -3.37700467168165;
   fWeightMatrix1to2[2][5] = -6.24348149863183;
   fWeightMatrix1to2[3][5] = -1.58151362061704;
   fWeightMatrix1to2[4][5] = -2.79066485367502;
   fWeightMatrix1to2[5][5] = -5.07389029560875;
   fWeightMatrix1to2[6][5] = 0.138949422405051;
   fWeightMatrix1to2[7][5] = 9.24638564734789;
   fWeightMatrix1to2[8][5] = -0.236868363123517;
   fWeightMatrix1to2[9][5] = -5.87064947215363;
   fWeightMatrix1to2[10][5] = 4.03556553684049;
   fWeightMatrix1to2[0][6] = -4.81599632814781;
   fWeightMatrix1to2[1][6] = 3.87299774555733;
   fWeightMatrix1to2[2][6] = -2.70798915872076;
   fWeightMatrix1to2[3][6] = -10.1721828752669;
   fWeightMatrix1to2[4][6] = -1.90845686463014;
   fWeightMatrix1to2[5][6] = 0.880453982619654;
   fWeightMatrix1to2[6][6] = -1.19916056196335;
   fWeightMatrix1to2[7][6] = 1.23655407829324;
   fWeightMatrix1to2[8][6] = -6.21749706887604;
   fWeightMatrix1to2[9][6] = -3.185978350065;
   fWeightMatrix1to2[10][6] = -0.895112600259509;
   fWeightMatrix1to2[0][7] = 1.7021987080824;
   fWeightMatrix1to2[1][7] = -3.53206784274472;
   fWeightMatrix1to2[2][7] = -2.87205115458477;
   fWeightMatrix1to2[3][7] = 1.98717303571705;
   fWeightMatrix1to2[4][7] = -4.285809958075;
   fWeightMatrix1to2[5][7] = -2.62283930658323;
   fWeightMatrix1to2[6][7] = 4.44397193723421;
   fWeightMatrix1to2[7][7] = 7.61195698069489;
   fWeightMatrix1to2[8][7] = -1.75676980112037;
   fWeightMatrix1to2[9][7] = -4.49949457993172;
   fWeightMatrix1to2[10][7] = -2.5482851307618;
   fWeightMatrix1to2[0][8] = 2.87484760892517;
   fWeightMatrix1to2[1][8] = -5.13640863464103;
   fWeightMatrix1to2[2][8] = -2.41895599563884;
   fWeightMatrix1to2[3][8] = -2.15594536936114;
   fWeightMatrix1to2[4][8] = -4.7213851305907;
   fWeightMatrix1to2[5][8] = 5.33113820607053;
   fWeightMatrix1to2[6][8] = 1.87467134837369;
   fWeightMatrix1to2[7][8] = 3.20248563782272;
   fWeightMatrix1to2[8][8] = 1.18876851988525;
   fWeightMatrix1to2[9][8] = -2.20706182460304;
   fWeightMatrix1to2[10][8] = 0.0946282546703732;
   fWeightMatrix1to2[0][9] = 5.94807161603518;
   fWeightMatrix1to2[1][9] = -1.68265987063422;
   fWeightMatrix1to2[2][9] = -2.67469156002317;
   fWeightMatrix1to2[3][9] = -0.513943452466295;
   fWeightMatrix1to2[4][9] = -3.49939488368063;
   fWeightMatrix1to2[5][9] = -2.38302083767368;
   fWeightMatrix1to2[6][9] = -2.95836903550926;
   fWeightMatrix1to2[7][9] = 5.74673222422626;
   fWeightMatrix1to2[8][9] = 2.14873034680781;
   fWeightMatrix1to2[9][9] = -0.221308110121216;
   fWeightMatrix1to2[10][9] = -2.18945713079565;
   fWeightMatrix1to2[0][10] = 0.643401952676864;
   fWeightMatrix1to2[1][10] = 0.279407627405989;
   fWeightMatrix1to2[2][10] = -2.13575451695633;
   fWeightMatrix1to2[3][10] = -0.56181565353167;
   fWeightMatrix1to2[4][10] = 3.06858936794587;
   fWeightMatrix1to2[5][10] = -1.38527147354495;
   fWeightMatrix1to2[6][10] = -5.18558525925366;
   fWeightMatrix1to2[7][10] = -5.13551969390175;
   fWeightMatrix1to2[8][10] = 2.35210689698165;
   fWeightMatrix1to2[9][10] = -3.55450948115353;
   fWeightMatrix1to2[10][10] = 1.71143915342134;
   fWeightMatrix1to2[0][11] = -2.78623940032553;
   fWeightMatrix1to2[1][11] = 2.99019422056032;
   fWeightMatrix1to2[2][11] = 4.69291160591327;
   fWeightMatrix1to2[3][11] = 2.26869063153781;
   fWeightMatrix1to2[4][11] = 2.282646705821;
   fWeightMatrix1to2[5][11] = 3.58136282832996;
   fWeightMatrix1to2[6][11] = 3.95316923080031;
   fWeightMatrix1to2[7][11] = -1.22269087981562;
   fWeightMatrix1to2[8][11] = -0.330097427011746;
   fWeightMatrix1to2[9][11] = 1.75412836778005;
   fWeightMatrix1to2[10][11] = 0.823160688980844;
   fWeightMatrix1to2[0][12] = -0.296352444370149;
   fWeightMatrix1to2[1][12] = 2.75665847356897;
   fWeightMatrix1to2[2][12] = -5.97938683727197;
   fWeightMatrix1to2[3][12] = 1.37436722456821;
   fWeightMatrix1to2[4][12] = -3.18683434322629;
   fWeightMatrix1to2[5][12] = -0.15374860111664;
   fWeightMatrix1to2[6][12] = -7.32807927961669;
   fWeightMatrix1to2[7][12] = 1.75938690177288;
   fWeightMatrix1to2[8][12] = -9.4327169908144;
   fWeightMatrix1to2[9][12] = -5.69013454472101;
   fWeightMatrix1to2[10][12] = -3.87110312415805;
   fWeightMatrix1to2[0][13] = -1.91623360294674;
   fWeightMatrix1to2[1][13] = -4.48646132986983;
   fWeightMatrix1to2[2][13] = 0.890443017912795;
   fWeightMatrix1to2[3][13] = 1.77690168478091;
   fWeightMatrix1to2[4][13] = 0.298971890015203;
   fWeightMatrix1to2[5][13] = -3.76420427024392;
   fWeightMatrix1to2[6][13] = 0.618719224389459;
   fWeightMatrix1to2[7][13] = 2.28895742668408;
   fWeightMatrix1to2[8][13] = 1.8836129319738;
   fWeightMatrix1to2[9][13] = -0.698063946047512;
   fWeightMatrix1to2[10][13] = -4.78051381965383;
   // weight matrix from layer 2 to 3
   fWeightMatrix2to3[0][0] = -3.29032473005342;
   fWeightMatrix2to3[0][1] = -3.40634206879237;
   fWeightMatrix2to3[0][2] = 3.71745488835065;
   fWeightMatrix2to3[0][3] = 1.99400666684916;
   fWeightMatrix2to3[0][4] = 2.41003602966555;
   fWeightMatrix2to3[0][5] = -2.59018738633322;
   fWeightMatrix2to3[0][6] = 2.24842624150789;
   fWeightMatrix2to3[0][7] = -4.90373430767004;
   fWeightMatrix2to3[0][8] = 1.73959329623524;
   fWeightMatrix2to3[0][9] = -5.17426813588909;
   fWeightMatrix2to3[0][10] = -5.95681361833276;
   fWeightMatrix2to3[0][11] = 0.263901641432465;
}

inline double ReadMLP::GetMvaValue__( const std::vector<double>& inputValues ) const
{
   if (inputValues.size() != (unsigned int)fLayerSize[0]-1) {
      std::cout << "Input vector needs to be of size " << fLayerSize[0]-1 << std::endl;
      return 0;
   }

   for (int l=0; l<fLayers; l++)
      for (int i=0; i<fLayerSize[l]; i++) fWeights[l][i]=0;

   for (int l=0; l<fLayers-1; l++)
      fWeights[l][fLayerSize[l]-1]=1;

   for (int i=0; i<fLayerSize[0]-1; i++)
      fWeights[0][i]=inputValues[i];

   // layer 0 to 1
   for (int o=0; o<fLayerSize[1]-1; o++) {
      for (int i=0; i<fLayerSize[0]; i++) {
         double inputVal = fWeightMatrix0to1[o][i] * fWeights[0][i];
         fWeights[1][o] += inputVal;
      }
      fWeights[1][o] = ActivationFnc(fWeights[1][o]);
   }
   // layer 1 to 2
   for (int o=0; o<fLayerSize[2]-1; o++) {
      for (int i=0; i<fLayerSize[1]; i++) {
         double inputVal = fWeightMatrix1to2[o][i] * fWeights[1][i];
         fWeights[2][o] += inputVal;
      }
      fWeights[2][o] = ActivationFnc(fWeights[2][o]);
   }
   // layer 2 to 3
   for (int o=0; o<fLayerSize[3]; o++) {
      for (int i=0; i<fLayerSize[2]; i++) {
         double inputVal = fWeightMatrix2to3[o][i] * fWeights[2][i];
         fWeights[3][o] += inputVal;
      }
      fWeights[3][o] = OutputActivationFnc(fWeights[3][o]);
   }

   return fWeights[3][0];
}

double ReadMLP::ActivationFnc(double x) const {
   // sigmoid
   return 1.0/(1.0+exp(-x));
}
double ReadMLP::OutputActivationFnc(double x) const {
   // sigmoid
   return 1.0/(1.0+exp(-x));
}
   
// Clean up
inline void ReadMLP::Clear() 
{
   // clean up the arrays
   for (int lIdx = 0; lIdx < 4; lIdx++) {
      delete[] fWeights[lIdx];
   }
}
   inline double ReadMLP::GetMvaValue( const std::vector<double>& inputValues ) const
   {
      // classifier response value
      double retval = 0;

      // classifier response, sanity check first
      if (!IsStatusClean()) {
         std::cout << "Problem in class \"" << fClassName << "\": cannot return classifier response"
                   << " because status is dirty" << std::endl;
         retval = 0;
      }
      else {
         if (IsNormalised()) {
            // normalise variables
            std::vector<double> iV;
            iV.reserve(inputValues.size());
            int ivar = 0;
            for (std::vector<double>::const_iterator varIt = inputValues.begin();
                 varIt != inputValues.end(); varIt++, ivar++) {
               iV.push_back(NormVariable( *varIt, fVmin[ivar], fVmax[ivar] ));
            }
            Transform( iV, -1 );
            retval = GetMvaValue__( iV );
         }
         else {
            std::vector<double> iV;
            int ivar = 0;
            for (std::vector<double>::const_iterator varIt = inputValues.begin();
                 varIt != inputValues.end(); varIt++, ivar++) {
               iV.push_back(*varIt);
            }
            Transform( iV, -1 );
            retval = GetMvaValue__( iV );
         }
      }

      return retval;
   }

//_______________________________________________________________________
inline void ReadMLP::InitTransform_1()
{
   // Normalization transformation, initialisation
   fMin_1[0][0] = 5;
   fMax_1[0][0] = 41;
   fMin_1[1][0] = 4;
   fMax_1[1][0] = 1251;
   fMin_1[2][0] = 4;
   fMax_1[2][0] = 1251;
   fMin_1[0][1] = 9;
   fMax_1[0][1] = 68;
   fMin_1[1][1] = 6;
   fMax_1[1][1] = 1697;
   fMin_1[2][1] = 6;
   fMax_1[2][1] = 1697;
   fMin_1[0][2] = 0.0302576832473;
   fMax_1[0][2] = 5.89165401459;
   fMin_1[1][2] = 0;
   fMax_1[1][2] = 6.98510789871;
   fMin_1[2][2] = 0;
   fMax_1[2][2] = 6.98510789871;
   fMin_1[0][3] = -0.22215230763;
   fMax_1[0][3] = 0.966714978218;
   fMin_1[1][3] = -0.249203011394;
   fMax_1[1][3] = 0.999779224396;
   fMin_1[2][3] = -0.249203011394;
   fMax_1[2][3] = 0.999779224396;
   fMin_1[0][4] = -0.197003424168;
   fMax_1[0][4] = 0.708240568638;
   fMin_1[1][4] = -0.232669591904;
   fMax_1[1][4] = 0.996693611145;
   fMin_1[2][4] = -0.232669591904;
   fMax_1[2][4] = 0.996693611145;
   fMin_1[0][5] = 14.4037160873;
   fMax_1[0][5] = 89.6580581665;
   fMin_1[1][5] = 1.205488801;
   fMax_1[1][5] = 88.392906189;
   fMin_1[2][5] = 1.205488801;
   fMax_1[2][5] = 89.6580581665;
   fMin_1[0][6] = -226.828231812;
   fMax_1[0][6] = 235.626159668;
   fMin_1[1][6] = -310.384185791;
   fMax_1[1][6] = 502.098358154;
   fMin_1[2][6] = -310.384185791;
   fMax_1[2][6] = 502.098358154;
   fMin_1[0][7] = 0.340838849545;
   fMax_1[0][7] = 37.9926528931;
   fMin_1[1][7] = 0.155534058809;
   fMax_1[1][7] = 45.8755836487;
   fMin_1[2][7] = 0.155534058809;
   fMax_1[2][7] = 45.8755836487;
   fMin_1[0][8] = 9.03515720367;
   fMax_1[0][8] = 88.8447265625;
   fMin_1[1][8] = 0.711865603924;
   fMax_1[1][8] = 91.436340332;
   fMin_1[2][8] = 0.711865603924;
   fMax_1[2][8] = 91.436340332;
   fMin_1[0][9] = 0.84177583456;
   fMax_1[0][9] = 71.8967437744;
   fMin_1[1][9] = 0.227185696363;
   fMax_1[1][9] = 82.428024292;
   fMin_1[2][9] = 0.227185696363;
   fMax_1[2][9] = 82.428024292;
   fMin_1[0][10] = -100;
   fMax_1[0][10] = 1682.86828613;
   fMin_1[1][10] = -100;
   fMax_1[1][10] = 1689.49121094;
   fMin_1[2][10] = -100;
   fMax_1[2][10] = 1689.49121094;
   fMin_1[0][11] = 72.2292175293;
   fMax_1[0][11] = 5030.35546875;
   fMin_1[1][11] = 34.4869842529;
   fMax_1[1][11] = 5091.4375;
   fMin_1[2][11] = 34.4869842529;
   fMax_1[2][11] = 5091.4375;
}

//_______________________________________________________________________
inline void ReadMLP::Transform_1( std::vector<double>& iv, int cls) const
{
   // Normalization transformation
   if (cls < 0 || cls > 2) {
   if (2 > 1 ) cls = 2;
      else cls = 2;
   }
   const int nVar = 12;

   // get indices of used variables

   // define the indices of the variables which are transformed by this transformation
   static std::vector<int> indicesGet;
   static std::vector<int> indicesPut;

   if ( indicesGet.empty() ) { 
      indicesGet.reserve(fNvars);
      indicesGet.push_back( 0);
      indicesGet.push_back( 1);
      indicesGet.push_back( 2);
      indicesGet.push_back( 3);
      indicesGet.push_back( 4);
      indicesGet.push_back( 5);
      indicesGet.push_back( 6);
      indicesGet.push_back( 7);
      indicesGet.push_back( 8);
      indicesGet.push_back( 9);
      indicesGet.push_back( 10);
      indicesGet.push_back( 11);
   } 
   if ( indicesPut.empty() ) { 
      indicesPut.reserve(fNvars);
      indicesPut.push_back( 0);
      indicesPut.push_back( 1);
      indicesPut.push_back( 2);
      indicesPut.push_back( 3);
      indicesPut.push_back( 4);
      indicesPut.push_back( 5);
      indicesPut.push_back( 6);
      indicesPut.push_back( 7);
      indicesPut.push_back( 8);
      indicesPut.push_back( 9);
      indicesPut.push_back( 10);
      indicesPut.push_back( 11);
   } 

   static std::vector<double> dv;
   dv.resize(nVar);
   for (int ivar=0; ivar<nVar; ivar++) dv[ivar] = iv[indicesGet.at(ivar)];
   for (int ivar=0;ivar<12;ivar++) {
      double offset = fMin_1[cls][ivar];
      double scale  = 1.0/(fMax_1[cls][ivar]-fMin_1[cls][ivar]);
      iv[indicesPut.at(ivar)] = (dv[ivar]-offset)*scale * 2 - 1;
   }
}

//_______________________________________________________________________
inline void ReadMLP::InitTransform()
{
   InitTransform_1();
}

//_______________________________________________________________________
inline void ReadMLP::Transform( std::vector<double>& iv, int sigOrBgd ) const
{
   Transform_1( iv, sigOrBgd );
}
