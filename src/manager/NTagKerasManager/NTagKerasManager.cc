#include <fstream>

#include <tensorflow/core/public/session.h>

#include "NTagKerasManager.hh"

NTagKerasManager::NTagKerasManager()
: fInputTensor(tensorflow::DT_DOUBLE, tensorflow::TensorShape({1,14})) 
{
    fInputData = fInputTensor.flat<double>().data();
}

void NTagKerasManager::LoadModel(const char* modelPath)
{
    tensorflow::SessionOptions session_options_;
    tensorflow::RunOptions run_options_;

    auto status = tensorflow::LoadSavedModel(session_options_,
                                             run_options_,
                                             modelPath,
                                             {"serve"},
                                             &fModel);

    if (!status.ok()) {
        std::cerr << "Failed to load model: " << status;
        return -1;
    }

    auto sig_map = fModel.GetSignatures().at("serving_default");

    fInputLayerName = model_def.inputs().begin()->second.name();
    fOutputLayerName = model_def.outputs().begin()->second.name();
}

void NTagKerasManager::LoadScaler(const char* scalerPath)
{
    std::ifstream scalerFile(scalerPath);

    std::string varName;
    double mean, scale;

    if (std::getline(scalerFile, varName, ',') &&
        std::getline(scalerFile, mean, ' ') &&
        std::getline(scalerFile, scale)) {
        fScalerMap[varName] = std::pair<double, double>(mean, scale);
    }
}

std::vector<double> NTagKerasManager::Transform(const Candidate& candidate)
{
    std::vector<double> scaledFeatures;

    for (auto const& pair: candidate.GetFeatureMap()) {
        auto scale_element = fScalerMap[pair.first];
        double mean = scale_element.first;
        double scale = scale_element.second;
        scaledFeatures.push_back((pair.second-mean)/scale);
    }

    return scaledFeatures;
}

float NTagKerasManager::GetOutput(const Candidate& candidate)
{
    auto scaledFeatures = Transform(candidate);
    std::copy(scaledFeatures.begin(), scaledFeatures.end(), fInputData);

    fModel.session->Run({{fInputLayerName, fInputTensor}}, 
                        {fOutputLayerName}, {}, &fOuptutTensorVector);

    return fOuptutTensorVector.at(0).flat<float>()(0);
}