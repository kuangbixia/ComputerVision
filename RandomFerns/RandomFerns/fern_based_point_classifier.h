#ifndef fern_based_point_classifier_h
#define fern_based_point_classifier_h

#include <fstream>
using namespace std;

#include "keypoint.h"
#include "ferns.h"
#include "affine_image_generator06.h"

class fern_based_point_classifier
{
public:
    fern_based_point_classifier(char* filename);
    fern_based_point_classifier(ifstream& f);
    bool correctly_read;

    fern_based_point_classifier(int number_of_classes,
        int number_of_ferns, int number_of_tests_per_fern,
        int dx_min, int dx_max, int dy_min, int dy_max, int ds_min, int ds_max);

    ~fern_based_point_classifier();

    bool save(char* filename);
    bool save(ofstream& f);

    //! Call this function BEFORE CALLING the train function.
    void reset_leaves_distributions(int prior_number = 1);

    //! You can call this function with different images.
    //! The KEYPOINT CLASSES must be given by the class_index field of the keypoint class.
    void train(keypoint* keypoints, int number_of_keypoints,
        int number_of_octaves, int yape_radius,
        int number_of_generated_images,
        affine_image_generator06* image_generator);

    //! YOU MUST CALL finalize_training() AFTER CALLING train().
    //! IT COMPUTES THE POSTERIOR PROBAS FROM THE NUMBER OF SAMPLES:
    void finalize_training(void);

    float test(keypoint* keypoints, int number_of_keypoints,
        int number_of_octaves, int yape_radius,
        int number_of_generated_images,
        affine_image_generator06* image_generator,
        bool verbose = false);

    int recognize(fine_gaussian_pyramid* pyramid, int u, int v, int level);
    void recognize(fine_gaussian_pyramid* pyramid, keypoint* keypoints, int number_of_keypoints);
    void recognize(fine_gaussian_pyramid* pyramid, keypoint* keypoints, int number_of_keypoints, float* distributions);
    void recognize(fine_gaussian_pyramid* pyramid, keypoint* K);
    float* recognize_with_distribution(fine_gaussian_pyramid* pyramid, keypoint* K);
    void recognize(fine_gaussian_pyramid* pyramid, keypoint* K, float* distribution);

    int recognize_verbose(fine_gaussian_pyramid* pyramid, int u, int v, int level);

    //! Used for graph generations:
    void set_number_of_ferns_to_use(int number_of_ferns_to_use);
    int  get_number_of_ferns_to_use(void);

    //private:
    void load(ifstream& f);

    ferns* Ferns;

    int number_of_classes;
    short* leaves_counters;
    float* leaves_distributions;
    int step1, step2;
    int* number_of_samples_for_class;
    int prior_number;
    int number_of_ferns_to_use;

    float* preallocated_distribution_for_a_keypoint;
};

#endif
