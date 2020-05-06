#ifndef affine_transformation_range_h
#define affine_transformation_range_h

#include <fstream>
using namespace std;

class affine_transformation_range
{
public:
    affine_transformation_range(void);
    ~affine_transformation_range(void);

    void load(ifstream& f);
    void load_in_degrees(ifstream& f);
    void save(ofstream& f);

    //! Global rotation. Default = [0 : 2Pi]
    void set_range_variation_for_theta(float min_theta, float max_theta);
    //! Skew. Default = [0 : Pi]
    void set_range_variation_for_phi(float min_phi, float max_phi);
    //! Default method. Between 0.5 : 1.5 for both
    void independent_scaling(float min_lambda1, float max_lambda1, float min_lambda2, float max_lambda2);
    //! Constrained scaling. Adds a constraint on the range of the product of l1 and l2.
    void constrained_scaling(float min_lambda1, float max_lambda1,
        float min_lambda2, float max_lambda2,
        float min_l1_l2, float max_l1_l2);

    void generate_random_parameters(float& theta, float& phi, float& lambda1, float& lambda2);

    //  private:
    float min_theta, max_theta;
    float min_phi, max_phi;
    int scaling_method;
    float min_lambda1, max_lambda1;
    float min_lambda2, max_lambda2;
    float min_l1_l2, max_l1_l2;     // for scaling method = 1 only (constrained scaling)
};

#endif
