#include "viewport.h"

#include "CMU462.h"

namespace CMU462 {

void ViewportImp::set_viewbox( float centerX, float centerY, float vspan ) {

  // Task 5 (part 2): 
  // Set svg coordinate to normalized device coordinate transformation. Your input
  // arguments are defined in SVG canvas coordinates.
  this->centerX = centerX;
  this->centerY = centerY;
  this->vspan = vspan; 

  if (vspan <= 0.0f) {
    set_svg_2_norm(Matrix3x3::identity());
    return;
  }

  // Map the current view box in SVG coordinates to normalized [0,1]^2 space.
  // centerX/centerY is the center of the box, and vspan is half of its size.
  Matrix3x3 m = Matrix3x3::identity();
  float inv_span2 = 1.0f / (2.0f * vspan);
  m(0,0) = inv_span2;
  m(1,1) = inv_span2;
  m(0,2) = 0.5f - centerX * inv_span2;
  m(1,2) = 0.5f - centerY * inv_span2;
  set_svg_2_norm(m);

}

void ViewportImp::update_viewbox( float dx, float dy, float scale ) { 
  
  this->centerX -= dx;
  this->centerY -= dy;
  this->vspan *= scale;
  set_viewbox( centerX, centerY, vspan );
}

} // namespace CMU462
