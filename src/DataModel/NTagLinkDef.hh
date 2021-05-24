#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ nestedclass;
#pragma link C++ nestedtypedef;
#pragma link C++ class PMTHit+;
#pragma link C++ class Cluster<PMTHit>+;
#pragma link C++ class Particle+;
#pragma link C++ class Cluster<Particle>+;
#pragma link C++ class TrueCapture+;
#pragma link C++ class Cluster<TrueCapture>+;
#pragma link C++ class Candidate+;
#pragma link C++ class Cluster<Candidate>+;
#pragma link C++ class+protected EventParticles+;
#pragma link C++ class+protected EventTrueCaptures+;

#endif