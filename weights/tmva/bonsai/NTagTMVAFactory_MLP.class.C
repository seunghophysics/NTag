// Class: ReadMLP
// Automatically generated by MethodBase::MakeClass
//

/* configuration options =====================================================

#GEN -*-*-*-*-*-*-*-*-*-*-*- general info -*-*-*-*-*-*-*-*-*-*-*-

Method         : MLP::MLP
TMVA Release   : 4.2.0         [262656]
ROOT Release   : 5.28/00       [334848]
Creator        : han
Date           : Tue Dec  7 18:52:04 2021
Host           : Linux sukla01 3.10.0-327.36.3.el7.x86_64 #1 SMP Thu Oct 20 04:56:07 EDT 2016 x86_64 x86_64 x86_64 GNU/Linux
Dir            : /disk02/calib3/usr/han/mc/random
Training events: 3919388
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
NHits                         NHits                         NHits                         NHits                                                           'F'    [4,929]
N200                          N200                          N200                          N200                                                            'F'    [7,1420]
TRMS                          TRMS                          TRMS                          TRMS                                                            'F'    [1.13287651539,90465.7734375]
Beta1                         Beta1                         Beta1                         Beta1                                                           'F'    [-0.331758022308,0.999759197235]
Beta5                         Beta5                         Beta5                         Beta5                                                           'F'    [-0.30250954628,0.996392130852]
OpeningAngleMean              OpeningAngleMean              OpeningAngleMean              OpeningAngleMean                                                'F'    [1.37802994251,88.5620346069]
OpeningAngleSkew              OpeningAngleSkew              OpeningAngleSkew              OpeningAngleSkew                                                'F'    [-297.246673584,345.846557617]
OpeningAngleStdev             OpeningAngleStdev             OpeningAngleStdev             OpeningAngleStdev                                               'F'    [0.00358418771066,44.4997673035]
MeanDirAngleMean              MeanDirAngleMean              MeanDirAngleMean              MeanDirAngleMean                                                'F'    [0.75348341465,93.5002670288]
MeanDirAngleRMS               MeanDirAngleRMS               MeanDirAngleRMS               MeanDirAngleRMS                                                 'F'    [0.189099594951,86.6220550537]
DWall                         DWall                         DWall                         DWall                                                           'F'    [-100,1687.50146484]
DWallMeanDir                  DWallMeanDir                  DWallMeanDir                  DWallMeanDir                                                    'F'    [33.1192550659,5091.44970703]
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
      fVmax[2] = 0.99999988079071;
      fVmin[3] = -1;
      fVmax[3] = 0.99999988079071;
      fVmin[4] = -1;
      fVmax[4] = 0.99999988079071;
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
   fWeightMatrix0to1[0][0] = 5.23722673570172;
   fWeightMatrix0to1[1][0] = 74.2770539597431;
   fWeightMatrix0to1[2][0] = -15.3482717746976;
   fWeightMatrix0to1[3][0] = 0.356855226111042;
   fWeightMatrix0to1[4][0] = 37.3936453636112;
   fWeightMatrix0to1[5][0] = -25.9620659091613;
   fWeightMatrix0to1[6][0] = -56.6941079388265;
   fWeightMatrix0to1[7][0] = 37.5109843049213;
   fWeightMatrix0to1[8][0] = -3.37639165870469;
   fWeightMatrix0to1[9][0] = 3.25447580955502;
   fWeightMatrix0to1[10][0] = 3.74403134454182;
   fWeightMatrix0to1[11][0] = -16.1090928071658;
   fWeightMatrix0to1[12][0] = -37.2489844367786;
   fWeightMatrix0to1[0][1] = -2.15809946833881;
   fWeightMatrix0to1[1][1] = -3.91472335125889;
   fWeightMatrix0to1[2][1] = 3.93306522622207;
   fWeightMatrix0to1[3][1] = -0.65567137481554;
   fWeightMatrix0to1[4][1] = 0.878988864418446;
   fWeightMatrix0to1[5][1] = 5.95813435223259;
   fWeightMatrix0to1[6][1] = 17.5885335917107;
   fWeightMatrix0to1[7][1] = -16.6537129084289;
   fWeightMatrix0to1[8][1] = 2.96416257723236;
   fWeightMatrix0to1[9][1] = -5.38192717739428;
   fWeightMatrix0to1[10][1] = -1.76128869208958;
   fWeightMatrix0to1[11][1] = 9.39824596006543;
   fWeightMatrix0to1[12][1] = -8.11960995187564;
   fWeightMatrix0to1[0][2] = 0.997787076950026;
   fWeightMatrix0to1[1][2] = -35.1674349362673;
   fWeightMatrix0to1[2][2] = 9.44207592407869;
   fWeightMatrix0to1[3][2] = 5.74659117376486;
   fWeightMatrix0to1[4][2] = -17.811738313269;
   fWeightMatrix0to1[5][2] = 13.6619575385717;
   fWeightMatrix0to1[6][2] = 18.752932908914;
   fWeightMatrix0to1[7][2] = -9.86140819086314;
   fWeightMatrix0to1[8][2] = 7.33117181822117;
   fWeightMatrix0to1[9][2] = -0.476502741797574;
   fWeightMatrix0to1[10][2] = -2.01481179628108;
   fWeightMatrix0to1[11][2] = 5.96500928138177;
   fWeightMatrix0to1[12][2] = 19.2735007561137;
   fWeightMatrix0to1[0][3] = -2.85712066010733;
   fWeightMatrix0to1[1][3] = -1.37040791112919;
   fWeightMatrix0to1[2][3] = -16.5859389125629;
   fWeightMatrix0to1[3][3] = 2.29586832331962;
   fWeightMatrix0to1[4][3] = -3.87175435559411;
   fWeightMatrix0to1[5][3] = -5.04888976932816;
   fWeightMatrix0to1[6][3] = -2.44718201700672;
   fWeightMatrix0to1[7][3] = -1.8885744046816;
   fWeightMatrix0to1[8][3] = -7.64868086439569;
   fWeightMatrix0to1[9][3] = -6.29154765695205;
   fWeightMatrix0to1[10][3] = -1.56625949004711;
   fWeightMatrix0to1[11][3] = 5.38208938854425;
   fWeightMatrix0to1[12][3] = -4.46572471531735;
   fWeightMatrix0to1[0][4] = -6.17872157980017;
   fWeightMatrix0to1[1][4] = -0.54503153772215;
   fWeightMatrix0to1[2][4] = -4.68599528543629;
   fWeightMatrix0to1[3][4] = -0.434042541817627;
   fWeightMatrix0to1[4][4] = -0.971645223132146;
   fWeightMatrix0to1[5][4] = -6.26656104347825;
   fWeightMatrix0to1[6][4] = 1.74842192059371;
   fWeightMatrix0to1[7][4] = -1.24619324828201;
   fWeightMatrix0to1[8][4] = -4.26785731171176;
   fWeightMatrix0to1[9][4] = 0.571926825828984;
   fWeightMatrix0to1[10][4] = 0.881982663068312;
   fWeightMatrix0to1[11][4] = 4.08609970120186;
   fWeightMatrix0to1[12][4] = 3.96067369211074;
   fWeightMatrix0to1[0][5] = -5.50568526720167;
   fWeightMatrix0to1[1][5] = -3.23552018346317;
   fWeightMatrix0to1[2][5] = 1.15086371125976;
   fWeightMatrix0to1[3][5] = 6.17887800745049;
   fWeightMatrix0to1[4][5] = -4.56116162971304;
   fWeightMatrix0to1[5][5] = 4.2071245298052;
   fWeightMatrix0to1[6][5] = -5.08256978762418;
   fWeightMatrix0to1[7][5] = -3.98426643371554;
   fWeightMatrix0to1[8][5] = 3.54437636816563;
   fWeightMatrix0to1[9][5] = 3.81164324246647;
   fWeightMatrix0to1[10][5] = 7.86581856946423;
   fWeightMatrix0to1[11][5] = -1.22029202519385;
   fWeightMatrix0to1[12][5] = -11.2575764579086;
   fWeightMatrix0to1[0][6] = 1.75585291415319;
   fWeightMatrix0to1[1][6] = 0.122553632181111;
   fWeightMatrix0to1[2][6] = 1.09613275549365;
   fWeightMatrix0to1[3][6] = 5.92313233424463;
   fWeightMatrix0to1[4][6] = 0.301908455457953;
   fWeightMatrix0to1[5][6] = 7.56449642048399;
   fWeightMatrix0to1[6][6] = -5.99661017749322;
   fWeightMatrix0to1[7][6] = -2.42659658866023;
   fWeightMatrix0to1[8][6] = -0.23853002189855;
   fWeightMatrix0to1[9][6] = 1.1481468926592;
   fWeightMatrix0to1[10][6] = 2.63321800876083;
   fWeightMatrix0to1[11][6] = -12.7422827973892;
   fWeightMatrix0to1[12][6] = 1.39160099378374;
   fWeightMatrix0to1[0][7] = 1.95231050004369;
   fWeightMatrix0to1[1][7] = -0.293362857171305;
   fWeightMatrix0to1[2][7] = -0.295884296531626;
   fWeightMatrix0to1[3][7] = -2.98313566029521;
   fWeightMatrix0to1[4][7] = 3.13901493157554;
   fWeightMatrix0to1[5][7] = -1.31581134844085;
   fWeightMatrix0to1[6][7] = 2.12965081121461;
   fWeightMatrix0to1[7][7] = 0.927302649265841;
   fWeightMatrix0to1[8][7] = -4.56146916982688;
   fWeightMatrix0to1[9][7] = -2.95746240697555;
   fWeightMatrix0to1[10][7] = 0.106761787794503;
   fWeightMatrix0to1[11][7] = -3.98112601296419;
   fWeightMatrix0to1[12][7] = 1.62654067955327;
   fWeightMatrix0to1[0][8] = -1.94989621410016;
   fWeightMatrix0to1[1][8] = 1.06098134307222;
   fWeightMatrix0to1[2][8] = -21.5224949288506;
   fWeightMatrix0to1[3][8] = -0.261494598618498;
   fWeightMatrix0to1[4][8] = -4.70684397980766;
   fWeightMatrix0to1[5][8] = -3.9753802836125;
   fWeightMatrix0to1[6][8] = -3.29298238492279;
   fWeightMatrix0to1[7][8] = -2.13347692483757;
   fWeightMatrix0to1[8][8] = 0.818322977339421;
   fWeightMatrix0to1[9][8] = -10.0177821456727;
   fWeightMatrix0to1[10][8] = -3.51207224784502;
   fWeightMatrix0to1[11][8] = 5.24991001836703;
   fWeightMatrix0to1[12][8] = 0.283882832705283;
   fWeightMatrix0to1[0][9] = -0.557389264973541;
   fWeightMatrix0to1[1][9] = -0.672829500200221;
   fWeightMatrix0to1[2][9] = -6.51718620030109;
   fWeightMatrix0to1[3][9] = 3.18228410581903;
   fWeightMatrix0to1[4][9] = -0.168539371272514;
   fWeightMatrix0to1[5][9] = 0.501525336287484;
   fWeightMatrix0to1[6][9] = 0.0187942066454898;
   fWeightMatrix0to1[7][9] = 0.643435555850324;
   fWeightMatrix0to1[8][9] = -0.7222134059895;
   fWeightMatrix0to1[9][9] = 0.0690924507663358;
   fWeightMatrix0to1[10][9] = 0.981351051992217;
   fWeightMatrix0to1[11][9] = -1.18116222520114;
   fWeightMatrix0to1[12][9] = -0.661556242226176;
   fWeightMatrix0to1[0][10] = 0.69000403180775;
   fWeightMatrix0to1[1][10] = 0.0405403828679519;
   fWeightMatrix0to1[2][10] = -0.121242392057838;
   fWeightMatrix0to1[3][10] = -15.1856836339789;
   fWeightMatrix0to1[4][10] = 1.59323787701988;
   fWeightMatrix0to1[5][10] = -0.17881295363799;
   fWeightMatrix0to1[6][10] = 0.908741656400593;
   fWeightMatrix0to1[7][10] = -0.481256724818794;
   fWeightMatrix0to1[8][10] = 0.826526232446413;
   fWeightMatrix0to1[9][10] = 3.49597388289644;
   fWeightMatrix0to1[10][10] = 4.37655392187503;
   fWeightMatrix0to1[11][10] = -0.193465742116773;
   fWeightMatrix0to1[12][10] = 0.149967138771672;
   fWeightMatrix0to1[0][11] = -0.449524964576569;
   fWeightMatrix0to1[1][11] = 0.817477209683906;
   fWeightMatrix0to1[2][11] = 0.70713434511691;
   fWeightMatrix0to1[3][11] = 0.364616314074588;
   fWeightMatrix0to1[4][11] = -0.309301281538316;
   fWeightMatrix0to1[5][11] = 1.34317803341461;
   fWeightMatrix0to1[6][11] = 0.47840635925215;
   fWeightMatrix0to1[7][11] = -3.03137720273475;
   fWeightMatrix0to1[8][11] = 0.351050056571847;
   fWeightMatrix0to1[9][11] = 2.78421513043895;
   fWeightMatrix0to1[10][11] = -1.01945913969279;
   fWeightMatrix0to1[11][11] = 0.911495342870275;
   fWeightMatrix0to1[12][11] = -0.599241527723559;
   fWeightMatrix0to1[0][12] = 1.43351952726583;
   fWeightMatrix0to1[1][12] = 33.5984649612006;
   fWeightMatrix0to1[2][12] = -10.3421796081801;
   fWeightMatrix0to1[3][12] = -7.14635155704681;
   fWeightMatrix0to1[4][12] = 19.6689178043378;
   fWeightMatrix0to1[5][12] = -10.7937355918867;
   fWeightMatrix0to1[6][12] = -18.0619367878574;
   fWeightMatrix0to1[7][12] = 9.91752957680092;
   fWeightMatrix0to1[8][12] = -5.9431035753808;
   fWeightMatrix0to1[9][12] = 0.111511135233725;
   fWeightMatrix0to1[10][12] = 0.179090783775409;
   fWeightMatrix0to1[11][12] = -3.44474565993072;
   fWeightMatrix0to1[12][12] = -20.9120856792825;
   // weight matrix from layer 1 to 2
   fWeightMatrix1to2[0][0] = 3.58828546053523;
   fWeightMatrix1to2[1][0] = -0.429670242125124;
   fWeightMatrix1to2[2][0] = 2.80255392244321;
   fWeightMatrix1to2[3][0] = 1.60809831334021;
   fWeightMatrix1to2[4][0] = -1.6106358500154;
   fWeightMatrix1to2[5][0] = 0.453457605833424;
   fWeightMatrix1to2[6][0] = -5.12923797718052;
   fWeightMatrix1to2[7][0] = -0.453540432176687;
   fWeightMatrix1to2[8][0] = 2.00091623541411;
   fWeightMatrix1to2[9][0] = 0.751279780782636;
   fWeightMatrix1to2[10][0] = 1.71372226022844;
   fWeightMatrix1to2[0][1] = -9.39056535734082;
   fWeightMatrix1to2[1][1] = -2.83944361766804;
   fWeightMatrix1to2[2][1] = 4.38252620443261;
   fWeightMatrix1to2[3][1] = -4.04331375885636;
   fWeightMatrix1to2[4][1] = 5.47138872797881;
   fWeightMatrix1to2[5][1] = 2.80519840931249;
   fWeightMatrix1to2[6][1] = 3.99518676847857;
   fWeightMatrix1to2[7][1] = 3.30437052189022;
   fWeightMatrix1to2[8][1] = 1.39516739376469;
   fWeightMatrix1to2[9][1] = 1.06607135962721;
   fWeightMatrix1to2[10][1] = -2.43175564991377;
   fWeightMatrix1to2[0][2] = 5.63707436048939;
   fWeightMatrix1to2[1][2] = -1.67869755200993;
   fWeightMatrix1to2[2][2] = -7.18842629028352;
   fWeightMatrix1to2[3][2] = -0.382671260800278;
   fWeightMatrix1to2[4][2] = -0.807151045014917;
   fWeightMatrix1to2[5][2] = -1.0529352958314;
   fWeightMatrix1to2[6][2] = -0.296461124628046;
   fWeightMatrix1to2[7][2] = -4.91083982200615;
   fWeightMatrix1to2[8][2] = -4.28227399203782;
   fWeightMatrix1to2[9][2] = -2.6221770806238;
   fWeightMatrix1to2[10][2] = 5.0842227477103;
   fWeightMatrix1to2[0][3] = -0.717500991191055;
   fWeightMatrix1to2[1][3] = -0.728461906224216;
   fWeightMatrix1to2[2][3] = 2.48767395983888;
   fWeightMatrix1to2[3][3] = 0.309979900440002;
   fWeightMatrix1to2[4][3] = -1.84991035918084;
   fWeightMatrix1to2[5][3] = 1.22632976261356;
   fWeightMatrix1to2[6][3] = -1.23234061303234;
   fWeightMatrix1to2[7][3] = 2.57509954931386;
   fWeightMatrix1to2[8][3] = -8.76414028158447;
   fWeightMatrix1to2[9][3] = 0.480568777725497;
   fWeightMatrix1to2[10][3] = -15.125023967658;
   fWeightMatrix1to2[0][4] = -6.71508407159657;
   fWeightMatrix1to2[1][4] = 4.01442333613126;
   fWeightMatrix1to2[2][4] = 2.39225236297001;
   fWeightMatrix1to2[3][4] = -3.51362051073974;
   fWeightMatrix1to2[4][4] = 4.97200666312106;
   fWeightMatrix1to2[5][4] = -5.82303181089223;
   fWeightMatrix1to2[6][4] = -0.426351942436971;
   fWeightMatrix1to2[7][4] = 1.17844623730832;
   fWeightMatrix1to2[8][4] = -0.346602615778729;
   fWeightMatrix1to2[9][4] = -3.76709277877884;
   fWeightMatrix1to2[10][4] = 4.6007013269238;
   fWeightMatrix1to2[0][5] = 3.08243590163312;
   fWeightMatrix1to2[1][5] = -3.79307922523159;
   fWeightMatrix1to2[2][5] = -4.32014643279496;
   fWeightMatrix1to2[3][5] = -0.769382576405099;
   fWeightMatrix1to2[4][5] = -0.57103580385008;
   fWeightMatrix1to2[5][5] = -1.78989418890815;
   fWeightMatrix1to2[6][5] = -0.826596692204017;
   fWeightMatrix1to2[7][5] = -2.99930840602444;
   fWeightMatrix1to2[8][5] = -4.0984620058139;
   fWeightMatrix1to2[9][5] = -0.0498062003582826;
   fWeightMatrix1to2[10][5] = 0.372289028833207;
   fWeightMatrix1to2[0][6] = -1.10637030214345;
   fWeightMatrix1to2[1][6] = 1.47108228560772;
   fWeightMatrix1to2[2][6] = -5.44352547184569;
   fWeightMatrix1to2[3][6] = -8.57095547029908;
   fWeightMatrix1to2[4][6] = -3.40341205233885;
   fWeightMatrix1to2[5][6] = -3.12258871447721;
   fWeightMatrix1to2[6][6] = -1.8110728514458;
   fWeightMatrix1to2[7][6] = -2.33577100186126;
   fWeightMatrix1to2[8][6] = -5.42768056169845;
   fWeightMatrix1to2[9][6] = -2.86455875979157;
   fWeightMatrix1to2[10][6] = 0.640762003354879;
   fWeightMatrix1to2[0][7] = -4.10916263567571;
   fWeightMatrix1to2[1][7] = 0.126932541612639;
   fWeightMatrix1to2[2][7] = 4.68136844664216;
   fWeightMatrix1to2[3][7] = -2.46443330849629;
   fWeightMatrix1to2[4][7] = 1.05048719745066;
   fWeightMatrix1to2[5][7] = 2.44893079292497;
   fWeightMatrix1to2[6][7] = -0.056569186199983;
   fWeightMatrix1to2[7][7] = 2.50238044698836;
   fWeightMatrix1to2[8][7] = 3.15266884630885;
   fWeightMatrix1to2[9][7] = -5.54161735571336;
   fWeightMatrix1to2[10][7] = -1.00177189484064;
   fWeightMatrix1to2[0][8] = 4.12770578777212;
   fWeightMatrix1to2[1][8] = -1.31428053094509;
   fWeightMatrix1to2[2][8] = -4.61953771652306;
   fWeightMatrix1to2[3][8] = -1.03846557474217;
   fWeightMatrix1to2[4][8] = -1.72058601211895;
   fWeightMatrix1to2[5][8] = 2.82568051971451;
   fWeightMatrix1to2[6][8] = -2.39430366576677;
   fWeightMatrix1to2[7][8] = -1.77832306398803;
   fWeightMatrix1to2[8][8] = -3.79065049053025;
   fWeightMatrix1to2[9][8] = -1.09441157198438;
   fWeightMatrix1to2[10][8] = 2.418526990917;
   fWeightMatrix1to2[0][9] = 2.58036134661941;
   fWeightMatrix1to2[1][9] = -5.63047978185243;
   fWeightMatrix1to2[2][9] = 2.7737805745648;
   fWeightMatrix1to2[3][9] = -0.90530745439994;
   fWeightMatrix1to2[4][9] = -1.47835492002073;
   fWeightMatrix1to2[5][9] = -0.254115532648532;
   fWeightMatrix1to2[6][9] = -0.302176260214651;
   fWeightMatrix1to2[7][9] = -0.915451506945898;
   fWeightMatrix1to2[8][9] = 1.79073534436294;
   fWeightMatrix1to2[9][9] = 1.85965920769929;
   fWeightMatrix1to2[10][9] = 2.00611379341746;
   fWeightMatrix1to2[0][10] = 2.22558760893331;
   fWeightMatrix1to2[1][10] = -0.319620407736588;
   fWeightMatrix1to2[2][10] = -1.63888244421582;
   fWeightMatrix1to2[3][10] = -1.83421308288956;
   fWeightMatrix1to2[4][10] = 1.26446318401666;
   fWeightMatrix1to2[5][10] = -7.32680559290827;
   fWeightMatrix1to2[6][10] = 0.0932472933392029;
   fWeightMatrix1to2[7][10] = -2.90506455287999;
   fWeightMatrix1to2[8][10] = 1.64535971627241;
   fWeightMatrix1to2[9][10] = -1.48177121060173;
   fWeightMatrix1to2[10][10] = 7.50335109329451;
   fWeightMatrix1to2[0][11] = 0.0379638648665722;
   fWeightMatrix1to2[1][11] = 3.56603090046198;
   fWeightMatrix1to2[2][11] = -4.11757273351421;
   fWeightMatrix1to2[3][11] = -0.51325886597618;
   fWeightMatrix1to2[4][11] = -0.730604404384732;
   fWeightMatrix1to2[5][11] = -4.86006921143192;
   fWeightMatrix1to2[6][11] = -0.159651921196619;
   fWeightMatrix1to2[7][11] = -3.63658913060136;
   fWeightMatrix1to2[8][11] = -7.16535072148406;
   fWeightMatrix1to2[9][11] = 0.77503559746845;
   fWeightMatrix1to2[10][11] = 0.423094964534857;
   fWeightMatrix1to2[0][12] = 3.22837698946908;
   fWeightMatrix1to2[1][12] = 0.456870044843695;
   fWeightMatrix1to2[2][12] = -3.47741547586181;
   fWeightMatrix1to2[3][12] = 1.17582387813221;
   fWeightMatrix1to2[4][12] = -2.84541575665446;
   fWeightMatrix1to2[5][12] = 0.182191831877514;
   fWeightMatrix1to2[6][12] = -9.39749059736894;
   fWeightMatrix1to2[7][12] = -3.99990236090377;
   fWeightMatrix1to2[8][12] = 0.909959071603939;
   fWeightMatrix1to2[9][12] = -0.910506146339855;
   fWeightMatrix1to2[10][12] = 2.34074469409369;
   fWeightMatrix1to2[0][13] = -2.3050334708765;
   fWeightMatrix1to2[1][13] = -0.512575816850749;
   fWeightMatrix1to2[2][13] = 0.0971553391910853;
   fWeightMatrix1to2[3][13] = 2.59051055453994;
   fWeightMatrix1to2[4][13] = -2.25609912821565;
   fWeightMatrix1to2[5][13] = -1.32554530901861;
   fWeightMatrix1to2[6][13] = -0.716320849693213;
   fWeightMatrix1to2[7][13] = 1.25680287999673;
   fWeightMatrix1to2[8][13] = -2.90222483938116;
   fWeightMatrix1to2[9][13] = 0.475541324691168;
   fWeightMatrix1to2[10][13] = 13.6771482664481;
   // weight matrix from layer 2 to 3
   fWeightMatrix2to3[0][0] = -2.12574023085435;
   fWeightMatrix2to3[0][1] = -3.43872732678348;
   fWeightMatrix2to3[0][2] = 2.94283545421467;
   fWeightMatrix2to3[0][3] = 4.83815471155825;
   fWeightMatrix2to3[0][4] = 6.39374751683566;
   fWeightMatrix2to3[0][5] = -7.61702636070866;
   fWeightMatrix2to3[0][6] = 7.70656808321849;
   fWeightMatrix2to3[0][7] = -4.29316630513461;
   fWeightMatrix2to3[0][8] = 6.40820813564978;
   fWeightMatrix2to3[0][9] = -1.84623683234733;
   fWeightMatrix2to3[0][10] = -9.57942348949713;
   fWeightMatrix2to3[0][11] = 5.72935070359266;
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
   fMin_1[0][0] = 4;
   fMax_1[0][0] = 98;
   fMin_1[1][0] = 4;
   fMax_1[1][0] = 929;
   fMin_1[2][0] = 4;
   fMax_1[2][0] = 929;
   fMin_1[0][1] = 7;
   fMax_1[0][1] = 141;
   fMin_1[1][1] = 7;
   fMax_1[1][1] = 1420;
   fMin_1[2][1] = 7;
   fMax_1[2][1] = 1420;
   fMin_1[0][2] = 1.13287651539;
   fMax_1[0][2] = 90465.7734375;
   fMin_1[1][2] = 1.63016009331;
   fMax_1[1][2] = 445.063537598;
   fMin_1[2][2] = 1.13287651539;
   fMax_1[2][2] = 90465.7734375;
   fMin_1[0][3] = -0.278158336878;
   fMax_1[0][3] = 0.992744386196;
   fMin_1[1][3] = -0.331758022308;
   fMax_1[1][3] = 0.999759197235;
   fMin_1[2][3] = -0.331758022308;
   fMax_1[2][3] = 0.999759197235;
   fMin_1[0][4] = -0.199322745204;
   fMax_1[0][4] = 0.896410405636;
   fMin_1[1][4] = -0.30250954628;
   fMax_1[1][4] = 0.996392130852;
   fMin_1[2][4] = -0.30250954628;
   fMax_1[2][4] = 0.996392130852;
   fMin_1[0][5] = 8.76419734955;
   fMax_1[0][5] = 87.4012527466;
   fMin_1[1][5] = 1.37802994251;
   fMax_1[1][5] = 88.5620346069;
   fMin_1[2][5] = 1.37802994251;
   fMax_1[2][5] = 88.5620346069;
   fMin_1[0][6] = -218.365188599;
   fMax_1[0][6] = 282.412017822;
   fMin_1[1][6] = -297.246673584;
   fMax_1[1][6] = 345.846557617;
   fMin_1[2][6] = -297.246673584;
   fMax_1[2][6] = 345.846557617;
   fMin_1[0][7] = 0.331546127796;
   fMax_1[0][7] = 36.0273170471;
   fMin_1[1][7] = 0.00358418771066;
   fMax_1[1][7] = 44.4997673035;
   fMin_1[2][7] = 0.00358418771066;
   fMax_1[2][7] = 44.4997673035;
   fMin_1[0][8] = 4.10792016983;
   fMax_1[0][8] = 88.5570907593;
   fMin_1[1][8] = 0.75348341465;
   fMax_1[1][8] = 93.5002670288;
   fMin_1[2][8] = 0.75348341465;
   fMax_1[2][8] = 93.5002670288;
   fMin_1[0][9] = 2.11999750137;
   fMax_1[0][9] = 77.0618896484;
   fMin_1[1][9] = 0.189099594951;
   fMax_1[1][9] = 86.6220550537;
   fMin_1[2][9] = 0.189099594951;
   fMax_1[2][9] = 86.6220550537;
   fMin_1[0][10] = -100;
   fMax_1[0][10] = 1678.73999023;
   fMin_1[1][10] = -100;
   fMax_1[1][10] = 1687.50146484;
   fMin_1[2][10] = -100;
   fMax_1[2][10] = 1687.50146484;
   fMin_1[0][11] = 121.676742554;
   fMax_1[0][11] = 5016.7890625;
   fMin_1[1][11] = 33.1192550659;
   fMax_1[1][11] = 5091.44970703;
   fMin_1[2][11] = 33.1192550659;
   fMax_1[2][11] = 5091.44970703;
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
