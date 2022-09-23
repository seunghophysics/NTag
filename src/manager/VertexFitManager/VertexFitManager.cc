#include <math.h>

#include "VertexFitManager.hh"

float VertexFitManager::GetGoodness(const PMTHitCluster& hitCluster, const TVector3& vertex, const float& t0)
{
    if (hitCluster.IsEmpty()) {
        std::cerr << "WARNING: Empty hit cluster is passed to VertexFitManager::GetGoodness, returning 0...\n";
        return 0;
    }

    auto cluster = hitCluster;
    cluster.SetVertex(vertex);

    float numerator = 0;
    float denominator = 0;
    for (auto const& hit: cluster) {
        float w_hit = exp(-0.5 * pow(((hit.t() - t0) / 60.), 2));       // hit weight
        numerator += w_hit * exp(-0.5 * pow(((hit.t() - t0) / 5.), 2)); // numerator: sum of weight * effective likelihood
        denominator += w_hit;                                           // denominator: sum of weights
    }

    float goodness = numerator/denominator;

    if (isnan(goodness)) {
        std::cerr << "WARNING: Vertex fit goodness is NaN! returning 0...\n";
        return 0;
    }
    else {
        return goodness;
    }
}
