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
#include "ARDrawingContext.hpp"

////////////////////////////////////////////////////////////////////
// Standard includes:
#include <Windows.h>
#include <gl/gl.h>
#include <gl/glu.h>

void ARDrawingContextDrawCallback(void* param)
{
    ARDrawingContext * ctx = static_cast<ARDrawingContext*>(param);
    if (ctx)
    {
        ctx->draw();
    }
}

ARDrawingContext::ARDrawingContext(std::string windowName, cv::Size frameSize, const CameraCalibration& c)
  : m_isTextureInitialized(false)
  , m_calibration(c)
  , m_windowName(windowName)
{
    // Create window with OpenGL support
    cv::namedWindow(windowName, cv::WINDOW_OPENGL);

    // Resize it exactly to video size
    cv::resizeWindow(windowName, frameSize.width, frameSize.height);

    // Initialize OpenGL draw callback:
    cv::setOpenGlContext(windowName);
    cv::setOpenGlDrawCallback(windowName, ARDrawingContextDrawCallback, this);
}

ARDrawingContext::~ARDrawingContext()
{
    cv::setOpenGlDrawCallback(m_windowName, 0, 0);
}

void ARDrawingContext::updateBackground(const cv::Mat& frame)
{
  frame.copyTo(m_backgroundImage);
}

void ARDrawingContext::updateWindow()
{
    cv::updateWindow(m_windowName);
}

void ARDrawingContext::draw()
{
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT); // Clear entire screen:
  drawCameraFrame();                                  // Render background
  drawAugmentedScene();                               // Draw AR
  glFlush();
}


void ARDrawingContext::drawCameraFrame()
{
  // Initialize texture for background image
  if (!m_isTextureInitialized)
  {
    glGenTextures(1, &m_backgroundTextureId);
    glBindTexture(GL_TEXTURE_2D, m_backgroundTextureId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    m_isTextureInitialized = true;
  }

  int w = m_backgroundImage.cols;
  int h = m_backgroundImage.rows;

  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glBindTexture(GL_TEXTURE_2D, m_backgroundTextureId);

  // Upload new texture data:
  if (m_backgroundImage.channels() == 3)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, m_backgroundImage.data);
  else if(m_backgroundImage.channels() == 4)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, m_backgroundImage.data);
  else if (m_backgroundImage.channels()==1)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, m_backgroundImage.data);

  const GLfloat bgTextureVertices[] = { 0, 0, w, 0, 0, h, w, h };
  const GLfloat bgTextureCoords[]   = { 1, 0, 1, 1, 0, 0, 0, 1 };
  const GLfloat proj[]              = { 0, -2.f/w, 0, 0, -2.f/h, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1 };

  glMatrixMode(GL_PROJECTION);
  glLoadMatrixf(proj);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, m_backgroundTextureId);

  // Update attribute values.
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);

  glVertexPointer(2, GL_FLOAT, 0, bgTextureVertices);
  glTexCoordPointer(2, GL_FLOAT, 0, bgTextureCoords);

  //glColor4f(1,1,1,1);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisable(GL_TEXTURE_2D);
}

void ARDrawingContext::drawAugmentedScene()
{
  // Init augmentation projection
  Matrix44 projectionMatrix;
  int w = m_backgroundImage.cols;
  int h = m_backgroundImage.rows;
  buildProjectionMatrix(m_calibration, w, h, projectionMatrix);

  glMatrixMode(GL_PROJECTION);
  glLoadMatrixf(projectionMatrix.data);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  if (isPatternPresent)
  {
    // Set the pattern transformation
    Matrix44 glMatrix = patternPose.getMat44();
    glLoadMatrixf(reinterpret_cast<const GLfloat*>(&glMatrix.data[0]));

    // Render model
    //drawCoordinateAxis();
    drawCubeModel();
  }
}

void ARDrawingContext::buildProjectionMatrix(const CameraCalibration& calibration, int screen_width, int screen_height, Matrix44& projectionMatrix)
{
  float nearPlane = 0.01f;  // Near clipping distance
  float farPlane  = 100.0f;  // Far clipping distance

  // Camera parameters
  float f_x = calibration.fx(); // Focal length in x axis
  float f_y = calibration.fy(); // Focal length in y axis (usually the same?)
  float c_x = calibration.cx(); // Camera primary point x
  float c_y = calibration.cy(); // Camera primary point y

  projectionMatrix.data[0] = -2.0f * f_x / screen_width;
  projectionMatrix.data[1] = 0.0f;
  projectionMatrix.data[2] = 0.0f;
  projectionMatrix.data[3] = 0.0f;

  projectionMatrix.data[4] = 0.0f;
  projectionMatrix.data[5] = 2.0f * f_y / screen_height;
  projectionMatrix.data[6] = 0.0f;
  projectionMatrix.data[7] = 0.0f;

  projectionMatrix.data[8] = 2.0f * c_x / screen_width - 1.0f;
  projectionMatrix.data[9] = 2.0f * c_y / screen_height - 1.0f;    
  projectionMatrix.data[10] = -( farPlane + nearPlane) / ( farPlane - nearPlane );
  projectionMatrix.data[11] = -1.0f;

  projectionMatrix.data[12] = 0.0f;
  projectionMatrix.data[13] = 0.0f;
  projectionMatrix.data[14] = -2.0f * farPlane * nearPlane / ( farPlane - nearPlane );        
  projectionMatrix.data[15] = 0.0f;
}


void ARDrawingContext::drawCoordinateAxis()
{
  static float lineX[] = {0,0,0,1,0,0};
  static float lineY[] = {0,0,0,0,1,0};
  static float lineZ[] = {0,0,0,0,0,1};

  glLineWidth(2);

  glBegin(GL_LINES);

  glColor3f(1.0f, 0.0f, 0.0f);
  glVertex3fv(lineX);
  glVertex3fv(lineX + 3);

  glColor3f(0.0f, 1.0f, 0.0f);
  glVertex3fv(lineY);
  glVertex3fv(lineY + 3);

  glColor3f(0.0f, 0.0f, 1.0f);
  glVertex3fv(lineZ);
  glVertex3fv(lineZ + 3);

  glEnd();
}

void ARDrawingContext::drawCubeModel()
{
    cv::Mat TextureImage = cv::imread("TangrenjieTanan.jpg");
    if (TextureImage.empty()) {
        return;
    }
    int w = TextureImage.cols;
    int h = TextureImage.rows;

    unsigned int texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 8);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    if (TextureImage.channels() == 3)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, TextureImage.data);
    else if (TextureImage.channels() == 4)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, TextureImage.data);
    else if (TextureImage.channels() == 1)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, TextureImage.data);

  
  const GLfloat vertices[] = {
    // 背面
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    // 上面
    -1.0f,  1.0f, -1.0f,
    1.0f,  1.0f, -1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,
    // 右面
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    // 左面
    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,
    // 前面
    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,
    // 下面
    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
    1.0f, -1.0f, 1.0f,
    1.0f, -1.0f, 1.0f,
    1.0f, -1.0f,  -1.0f,
    -1.0f, -1.0f,  -1.0f
  };
  const GLfloat texCoords[] = {
      // 背面
      1.0f, 1.0f,
      1.0f, 0.0f,
      0.0f, 0.0f,
      0.0f, 0.0f,
      0.0f, 1.0f,
      1.0f, 1.0f,
      // 上面
      0.0f, 1.0f,
      1.0f, 1.0f,
      1.0f, 0.0f,
      1.0f, 0.0f,
      0.0f, 0.0f,
      0.0f, 1.0f,
      // 右面
      1.0f, 0.0f,
      0.0f, 0.0f,
      0.0f, 1.0f,
      0.0f, 1.0f,
      1.0f, 1.0f,
      1.0f, 0.0f,
      // 左面
      1.0f, 0.0f,
      0.0f, 0.0f,
      0.0f, 1.0f,
      0.0f, 1.0f,
      1.0f, 1.0f,
      1.0f, 0.0f,
      // 前面
      0.0f, 0.0f,
      0.0f, 1.0f,
      1.0f, 1.0f,
      1.0f, 1.0f,
      1.0f, 0.0f,
      0.0f, 0.0f,
      // 下面
      0.0f, 0.0f,
      0.0f, 1.0f,
      1.0f, 1.0f,
      1.0f, 1.0f,
      1.0f, 0.0f,
      0.0f, 0.0f
  };

  glScalef(0.35, 0.35, 0.35);
  glRotatef(-45.0, 0.0, 1.0, 0.0);
  glTranslatef(-2, 0, 2);

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glVertexPointer(3, GL_FLOAT, 0, vertices);
  glTexCoordPointer(2, GL_FLOAT, 0, texCoords);

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  //glEnable(GL_DEPTH_TEST);

  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texture_id);
  glDrawArrays(GL_TRIANGLES, 0, 36);

  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisable(GL_TEXTURE_2D);


}
