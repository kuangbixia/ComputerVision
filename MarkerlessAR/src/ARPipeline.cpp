/*****************************************************************************
*   Markerless AR desktop application.
******************************************************************************
*   by Khvedchenia Ievgen, 5th Dec 2012
*   http://computer-vision-talks.com
******************************************************************************
*   Ch3 of the book "Mastering OpenCV with Practical Computer Vision Projects"
*   Copyright Packt Publishing 2012.
*   http://www.packtpub.com/cool-projects-with-opencv/book
*****************************************************************************/

////////////////////////////////////////////////////////////////////
// File includes:
#include "ARPipeline.hpp"

ARPipeline::ARPipeline(const cv::Mat& patternImage, const CameraCalibration& calibration)
  : m_calibration(calibration)
{
   // m_patternDetector= PatternDetector(new cv::SurfFeatureDetector(1000),new cv::SurfDescriptorExtractor(),new cv::FlannBasedMatcher(),false);
    m_patternDetector= PatternDetector(
        new cv::SurfFeatureDetector(1000),
        new cv::FREAK(false, false), 
        new cv::BFMatcher(cv::NORM_HAMMING, true),
        false);
    //m_patternDetector= PatternDetector(new cv::ORB(1000),new cv::SurfDescriptorExtractor(), new cv::FlannBasedMatcher(),false);
  // 从模板图像中建立标识的特征描述子
  m_patternDetector.buildPatternFromImage(patternImage, m_pattern);
  // 训练标识
  m_patternDetector.train(m_pattern);
}

bool ARPipeline::processFrame(const cv::Mat& inputFrame)
{
  bool patternFound = m_patternDetector.findPattern(inputFrame, m_patternInfo);

  if (patternFound)
  {
      // 计算由模板到测试图像的旋转、平移矩阵
      m_patternInfo.computePose(m_pattern, m_calibration);
  }

  return patternFound;
}

// 获得模式在测试图像上的位置，也就是要生成虚拟信息的位置
const Transformation& ARPipeline::getPatternLocation() const
{
  return m_patternInfo.pose3d;
}