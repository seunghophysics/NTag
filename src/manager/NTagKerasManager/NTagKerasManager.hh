#ifndef NTAGTMVAMANAGER_HH
#define NTAGTMVAMANAGER_HH

#include <tensorflow/cc/saved_model/loader.h>

class NTagKerasManager
{
    public:
        NTagKerasManager();
        ~NTagKerasManager() {}

        void LoadModel(const char* modelPath);
        void LoadScaler(const char* scalerPath);
        std::vector<double> Transform(const Candidate& candidate);

        float GetOutput(const Candidate& candidate);

    private:
        // model
        tensorflow::SavedModelBundle fModel;

        // scaler
        std::map<std::string, std::pair<double, double>> fScalerMap;

        // input, output tensors
        std::string fInputLayerName;
        std::string fOutputLayerName;
        tensorflow::Tensor fInputTensor;
        double* fInputData;
        std::vector<tensorflow::Tensor> fOuptutTensorVector;
};

#endif