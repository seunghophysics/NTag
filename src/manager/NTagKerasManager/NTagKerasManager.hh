#ifndef NTAGKERASMANAGER_HH
#define NTAGKERASMANAGER_HH

#include <tensorflow/cc/saved_model/loader.h>

#include "Printer.hh"

class NTagKerasManager
{
    public:
        NTagKerasManager();
        ~NTagKerasManager() {}

        void LoadWeights(std::string weightPath);
        void LoadModel(std::string modelPath);
        void LoadScaler(std::string scalerPath);
        std::vector<float> Transform(const Candidate& candidate);

        float GetOutput(const Candidate& candidate);

    private:
        // model
        tensorflow::SavedModelBundle fModel;

        // scaler
        std::map<std::string, std::pair<float, float>> fScalerMap;

        // input, output tensors
        std::string fInputLayerName;
        std::string fOutputLayerName;
        tensorflow::Tensor fInputTensor;
        float* fInputData;
        std::vector<tensorflow::Tensor> fOutputTensorVector;

        Printer fMsg;
};

#endif