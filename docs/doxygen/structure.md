# Program description {#program-description}

![Structure](docs/doxygen/NTag.png)

All neutron tagging in one event is steered by the mainframe class named `EventNTagManager`, shown as the orange box in the above figure. This class holds information that can be classified into three main categories: raw TQ hits, event variables, and the TMVA variables which are grouped by a class named `NTagTMVAVariables` and eventually fed into the neural network of the MVA tool. The class `NTagEventInfo` also has a bunch of functions that manipulate the information from the input data to form the aforementioned three categories of information that we need in our search for neutron capture candidates. All manipulation of information and variables are handled by the member functions of `NTagEventInfo`.

`NTagIO`shown as the blue box in the above figure is a subclass of `NTagEventInfo`, and it deals with the I/O of SK data fies. It reads the SK data files with `skread`, and uses the member functions of its base class `NTagEventInfo` to set its member variables, i.e., the raw TQ hits, event variables, and the TMVA variables. At the end of an event, `NTagIO` will fill its trees with the above member variables, and will clear all member variables to move on to the next event and loop again. At the end of the input file (EOF), the data trees will be written to an output file.

In detail, the data flow and the search process in one event progresses as follows:

1. An SK data file with TQREAL filled can be read by `NTagIO` via `skread`, and the SK common blocks will be filled by `NTagIO::ReadFile`, which initiates the event loop. The instructions for each event is given by `NTagIO::ReadMCEvent` in case the event is from an MC, otherwise by `NTagIO::ReadDataEvent` which again splits into either of `NTagIO::ReadSHEEvent` for SHE-triggered events or `NTagIO::ReadAFTEvent` for AFT-triggered events. Each "ReadEvent" functions include a set of "Set" functions from `NTagEventInfo`, so that event variables can be read from the SK common blocks.

2. The "ReadEvent" functions mentioned above also call `NTagEventInfo::AppendRawHitInfo` to append the raw TQ hit information from the common block `sktqz` to the private member vectors of `NTagEventInfo`: `vTISKZ`, `vQISKZ`, and `vCABIZ`. The neutron capture candidates will be searched for within these raw TQ vectors.

3. If the raw TQ vectors are set, `NTagEventInfo::SearchNeutronCaptures` will search for neutron capture candidates, looking for NHits peaks within the ToF-subtracted `vTISKZ`. Each selected peak will be saved as an instance of the class `NTagCandidate`, and `NTagEventInfo::vCandidates` is a STL vector that stores all `NTagCandidate` instances from the event.

4. Via function `NTagCandidate::SetNNVariables` The properties of the found capture candidates will be passed on to the class `NTagTMVAVariables`, which holds the variables to be fed to the neural network.

5. The class `NTagTMVA` will feed variables from `NTagTMVAVariables` to ROOT::TMVA, and the TMVA reader will evaluate the weights and the input variables to find the classifier output. The classifier output is set for each `NTagCandidate` instance.

6. Finally, `NTagIO` will fill its trees with the member variables of `NTagEventInfo`, and clear all variables for the next event. If the file hits the end, the trees will be written to an output file.