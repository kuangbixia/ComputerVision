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
  // ��ģ��ͼ���н�����ʶ������������
  m_patternDetector.buildPatternFromImage(patternImage, m_pattern);
  // ѵ����ʶ
  m_patternDetector.train(m_pattern);
}

bool ARPipeline::processFrame(const cv::Mat& inputFrame)
{
  bool patternFound = m_patternDetector.findPattern(inputFrame, m_patternInfo);

  if (patternFound)
  {
      // ������ģ�嵽����ͼ�����ת��ƽ�ƾ���
      m_patternInfo.computePose(m_pattern, m_calibration);
  }

  return patternFound;
}

// ���ģʽ�ڲ���ͼ���ϵ�λ�ã�Ҳ����Ҫ����������Ϣ��λ��
const Transformation& ARPipeline::getPatternLocation() const
{
  return m_patternInfo.pose3d;
}