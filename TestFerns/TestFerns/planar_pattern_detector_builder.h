#ifndef planar_pattern_detector_builder_h
#define planar_pattern_detector_builder_h

#include <vector>
using namespace std;

#include "planar_pattern_detector.h"
#include "affine_transformation_range.h"

class planar_pattern_detector_builder
{
public:
	//! Build the model, with caching.
	//! \return 0 on failure
	static planar_pattern_detector* build_with_cache(const char* image_name,
		affine_transformation_range* range,
		int maximum_number_of_points_on_model,
		int number_of_generated_images_to_find_stable_points,
		double minimum_number_of_views_rate,
		int patch_size, int yape_radius, int number_of_octaves,
		int number_of_ferns, int number_of_tests_per_fern,
		int number_of_samples_for_refinement, int number_of_samples_for_test,
		char* given_detector_data_filename = 0,
		int roi_up_left_u = -1, int roi_up_left_v = -1,
		int roi_bottom_right_u = -1, int roi_bottom_right_v = -1);

	static planar_pattern_detector* force_build(const char* image_name,
		affine_transformation_range* range,
		int maximum_number_of_points_on_model,
		int number_of_generated_images_to_find_stable_points,
		double minimum_number_of_views_rate,
		int patch_size, int yape_radius, int number_of_octaves,
		int number_of_ferns, int number_of_tests_per_fern,
		int number_of_samples_for_refinement, int number_of_samples_for_test,
		char* given_detector_data_filename = 0,
		int roi_up_left_u = -1, int roi_up_left_v = -1,
		int roi_bottom_right_u = -1, int roi_bottom_right_v = -1);

	static planar_pattern_detector* just_load(const char* given_detector_data_filename);

	//private:
	static planar_pattern_detector* learn(const char* image_name,
		affine_transformation_range* range,
		int maximum_number_of_points_on_model,
		int number_of_generated_images_to_find_stable_points,
		double minimum_number_of_views_rate,
		int patch_size, int yape_radius, int number_of_octaves,
		int number_of_ferns, int number_of_tests_per_fern,
		int number_of_samples_for_refinement, int number_of_samples_for_test,
		int roi_up_left_u = -1, int roi_up_left_v = -1,
		int roi_bottom_right_u = -1, int roi_bottom_right_v = -1);


	static void detect_most_stable_model_points(planar_pattern_detector* detector,
		int maximum_number_of_points_on_model,
		int /*yape_radius*/, int /*number_of_octaves*/,
		int number_of_generated_images,
		double minimum_number_of_views_rate);

	static pair<keypoint, int>* search_for_existing_model_point(vector< pair<keypoint, int> >* tmp_model_points,
		float cu, float cv, int scale);
};

#endif
