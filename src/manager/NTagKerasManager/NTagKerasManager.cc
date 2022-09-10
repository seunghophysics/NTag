#include <fstream>
#include <string>

#include <tensorflow/core/public/session.h>

#include "NTagGlobal.hh"
#include "Candidate.hh"
#include "NTagKerasManager.hh"

NTagKerasManager::NTagKerasManager()
: fInputTensor(tensorflow::DT_FLOAT, tensorflow::TensorShape({1,14})),
  fMsg("NTagKerasManager") 
{
    fInputData = fInputTensor.flat<float>().data();
}

void NTagKerasManager::LoadWeights(std::string weightPath)
{
    LoadModel(weightPath+"/model");
    LoadScaler(weightPath+"/scaler");
}

void NTagKerasManager::LoadModel(std::string modelPath)
{
    tensorflow::SessionOptions session_options_;
    tensorflow::RunOptions run_options_;

    auto status = tensorflow::LoadSavedModel(session_options_,
                                             run_options_,
                                             modelPath.c_str(),
                                             {"serve"},
                                             &fModel);

    if (!status.ok()) {
        std::cerr << "[NTagKerasManager] Failed to load model: " << status;
        fMsg.Print("Aborting...", pERROR);
    }

    auto model_def = fModel.GetSignatures().at("serving_default");

    fInputLayerName = model_def.inputs().begin()->second.name();
    fOutputLayerName = model_def.outputs().begin()->second.name();
}

void NTagKerasManager::LoadScaler(std::string scalerPath)
{
    std::ifstream scalerFile(scalerPath.c_str());
    std::string varName, mean, scale;

    for (auto& key: gKerasFeatures) {
        if (std::getline(scalerFile, varName, ' ') &&
            std::getline(scalerFile, mean, ' ') &&
            std::getline(scalerFile, scale)) {
            fScalerMap[varName] = std::pair<double, double>(std::stof(mean), std::stof(scale));
        }
    }
}

std::vector<float> NTagKerasManager::Transform(const Candidate& candidate)
{
    std::vector<float> scaledFeatures;

    for (auto const& key: gKerasFeatures) {
        auto scale_element = fScalerMap[key];
        double mean = scale_element.first;
        double scale = scale_element.second;
        scaledFeatures.push_back((candidate[key]-mean)/scale);
    }

    return scaledFeatures;
}

float NTagKerasManager::GetOutput(const Candidate& candidate)
{
    auto scaledFeatures = Transform(candidate);
    std::copy(scaledFeatures.begin(), scaledFeatures.end(), fInputData);

    TF_CHECK_OK(fModel.session->Run({{fInputLayerName, fInputTensor}}, 
                        {fOutputLayerName}, {}, &fOutputTensorVector));

    return fOutputTensorVector.at(0).flat<float>()(0);
}