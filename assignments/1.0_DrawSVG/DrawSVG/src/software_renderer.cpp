#include "software_renderer.h"

#include <cmath>
#include <vector>
#include <iostream>
#include <algorithm>

#include "triangulation.h"

using namespace std;

namespace CMU462 {


// Implements SoftwareRenderer //

void SoftwareRendererImp::draw_svg( SVG& svg ) {

  // set top level transformation
  transformation = svg_2_screen;

  // clear supersample storage for this frame
  if (sample_buffer.empty()) {
    resize_sample_buffer();
  } else {
    fill(sample_buffer.begin(), sample_buffer.end(), Color::White);
  }

  // draw all elements
  for ( size_t i = 0; i < svg.elements.size(); ++i ) {
    draw_element(svg.elements[i]);
  }

  // draw canvas outline
  Vector2D a = transform(Vector2D(    0    ,     0    )); a.x--; a.y--;
  Vector2D b = transform(Vector2D(svg.width,     0    )); b.x++; b.y--;
  Vector2D c = transform(Vector2D(    0    ,svg.height)); c.x--; c.y++;
  Vector2D d = transform(Vector2D(svg.width,svg.height)); d.x++; d.y++;

  rasterize_line(a.x, a.y, b.x, b.y, Color::Black);
  rasterize_line(a.x, a.y, c.x, c.y, Color::Black);
  rasterize_line(d.x, d.y, b.x, b.y, Color::Black);
  rasterize_line(d.x, d.y, c.x, c.y, Color::Black);

  // resolve and send to render target
  resolve();

}

void SoftwareRendererImp::set_sample_rate( size_t sample_rate ) {

  // Task 4: 
  // You may want to modify this for supersampling support
  this->sample_rate = max<size_t>(1, sample_rate);
  resize_sample_buffer();

}

void SoftwareRendererImp::set_render_target( unsigned char* render_target,
                                             size_t width, size_t height ) {

  // Task 4: 
  // You may want to modify this for supersampling support
  this->render_target = render_target;
  this->target_w = width;
  this->target_h = height;
  resize_sample_buffer();

}

void SoftwareRendererImp::draw_element( SVGElement* element ) {

  // Task 5 (part 1):
  // Modify this to implement the transformation stack

  switch(element->type) {
    case POINT:
      draw_point(static_cast<Point&>(*element));
      break;
    case LINE:
      draw_line(static_cast<Line&>(*element));
      break;
    case POLYLINE:
      draw_polyline(static_cast<Polyline&>(*element));
      break;
    case RECT:
      draw_rect(static_cast<Rect&>(*element));
      break;
    case POLYGON:
      draw_polygon(static_cast<Polygon&>(*element));
      break;
    case ELLIPSE:
      draw_ellipse(static_cast<Ellipse&>(*element));
      break;
    case IMAGE:
      draw_image(static_cast<Image&>(*element));
      break;
    case GROUP:
      draw_group(static_cast<Group&>(*element));
      break;
    default:
      break;
  }

}


// Primitive Drawing //

void SoftwareRendererImp::draw_point( Point& point ) {

  Vector2D p = transform(point.position);
  rasterize_point( p.x, p.y, point.style.fillColor );

}

void SoftwareRendererImp::draw_line( Line& line ) { 

  Vector2D p0 = transform(line.from);
  Vector2D p1 = transform(line.to);
  rasterize_line( p0.x, p0.y, p1.x, p1.y, line.style.strokeColor );

}

void SoftwareRendererImp::draw_polyline( Polyline& polyline ) {

  Color c = polyline.style.strokeColor;

  if( c.a != 0 ) {
    int nPoints = polyline.points.size();
    for( int i = 0; i < nPoints - 1; i++ ) {
      Vector2D p0 = transform(polyline.points[(i+0) % nPoints]);
      Vector2D p1 = transform(polyline.points[(i+1) % nPoints]);
      rasterize_line( p0.x, p0.y, p1.x, p1.y, c );
    }
  }
}

void SoftwareRendererImp::draw_rect( Rect& rect ) {

  Color c;
  
  // draw as two triangles
  float x = rect.position.x;
  float y = rect.position.y;
  float w = rect.dimension.x;
  float h = rect.dimension.y;

  Vector2D p0 = transform(Vector2D(   x   ,   y   ));
  Vector2D p1 = transform(Vector2D( x + w ,   y   ));
  Vector2D p2 = transform(Vector2D(   x   , y + h ));
  Vector2D p3 = transform(Vector2D( x + w , y + h ));
  
  // draw fill
  c = rect.style.fillColor;
  if (c.a != 0 ) {
    rasterize_triangle( p0.x, p0.y, p1.x, p1.y, p2.x, p2.y, c );
    rasterize_triangle( p2.x, p2.y, p1.x, p1.y, p3.x, p3.y, c );
  }

  // draw outline
  c = rect.style.strokeColor;
  if( c.a != 0 ) {
    rasterize_line( p0.x, p0.y, p1.x, p1.y, c );
    rasterize_line( p1.x, p1.y, p3.x, p3.y, c );
    rasterize_line( p3.x, p3.y, p2.x, p2.y, c );
    rasterize_line( p2.x, p2.y, p0.x, p0.y, c );
  }

}

void SoftwareRendererImp::draw_polygon( Polygon& polygon ) {

  Color c;

  // draw fill
  c = polygon.style.fillColor;
  if( c.a != 0 ) {

    // triangulate
    vector<Vector2D> triangles;
    triangulate( polygon, triangles );

    // draw as triangles
    for (size_t i = 0; i < triangles.size(); i += 3) {
      Vector2D p0 = transform(triangles[i + 0]);
      Vector2D p1 = transform(triangles[i + 1]);
      Vector2D p2 = transform(triangles[i + 2]);
      rasterize_triangle( p0.x, p0.y, p1.x, p1.y, p2.x, p2.y, c );
    }
  }

  // draw outline
  c = polygon.style.strokeColor;
  if( c.a != 0 ) {
    int nPoints = polygon.points.size();
    for( int i = 0; i < nPoints; i++ ) {
      Vector2D p0 = transform(polygon.points[(i+0) % nPoints]);
      Vector2D p1 = transform(polygon.points[(i+1) % nPoints]);
      rasterize_line( p0.x, p0.y, p1.x, p1.y, c );
    }
  }
}

void SoftwareRendererImp::draw_ellipse( Ellipse& ellipse ) {

  // Extra credit 

}

void SoftwareRendererImp::draw_image( Image& image ) {

  Vector2D p0 = transform(image.position);
  Vector2D p1 = transform(image.position + image.dimension);

  rasterize_image( p0.x, p0.y, p1.x, p1.y, image.tex );
}

void SoftwareRendererImp::draw_group( Group& group ) {

  for ( size_t i = 0; i < group.elements.size(); ++i ) {
    draw_element(group.elements[i]);
  }

}

// Rasterization //

// The input arguments in the rasterization functions 
// below are all defined in screen space coordinates

void SoftwareRendererImp::rasterize_point( float x, float y, Color color ) {

  // fill the nearest pixel (all sub-samples) to preserve thickness at any sample rate
  int sx = (int) floor(x);
  int sy = (int) floor(y);

  // check bounds
  if ( sx < 0 || sx >= (int) target_w ) return;
  if ( sy < 0 || sy >= (int) target_h ) return;

  fill_pixel(sx, sy, color);

}

void SoftwareRendererImp::rasterize_line( float x0, float y0,
                                          float x1, float y1,
                                          Color color) {

  // Task 2: 
  // Implement line rasterization
  // 어떤 경사도의 선도 처리
  // O(length of line) 시간 복잡도

  // Bresenham's line algorithm (integer grid).
  int x = (int) std::floor(x0);
  int y = (int) std::floor(y0);
  int x_end = (int) std::floor(x1);
  int y_end = (int) std::floor(y1);

  int dx = std::abs(x_end - x);
  int sx = (x < x_end) ? 1 : -1;
  int dy = -std::abs(y_end - y);
  int sy = (y < y_end) ? 1 : -1;
  int err = dx + dy;

  int max_steps = dx + (-dy) + 2;

  for (int step = 0; step < max_steps; ++step) {
    rasterize_point(x, y, color);
    if (x == x_end && y == y_end) break;

    int e2 = 2 * err;
    if (e2 >= dy) {
      err += dy;
      x += sx;
    }
    if (e2 <= dx) {
      err += dx;
      y += sy;
    }
  }
}

void SoftwareRendererImp::rasterize_triangle( float x0, float y0,
                                              float x1, float y1,
                                              float x2, float y2,
                                              Color color ) {
  // Task 3: 
  // Implement triangle rasterization
  // 로직 순서
  // 1. 색상 알파가 0이면 종료
  // 2. 삼각형의 면적이 0이면 종료
  // 3. bounding box 계산
  // 4. bounding box 내의 픽셀에 대해 삼각형 내부에 있는지 판별
  // 5. 내부에 있으면 해당 픽셀 color로 칠하기

  // 알파가 0이면 그릴 필요 없음
  if (color.a == 0.0f) return;

  auto edge = [](float ax, float ay, float bx, float by, float px, float py) {
    float abx = bx - ax;
    float aby = by - ay;
    float apx = px - ax;
    float apy = py - ay;
    
    return (abx * apy) - (aby * apx);
  };

  // 삼각형 면적 0이면 종료
  const float area = edge(x0, y0, x1, y1, x2, y2);
  if (fabs(area) < 1e-12f) return;

  const float min_x = min({x0, x1, x2});
  const float min_y = min({y0, y1, y2});
  const float max_x = max({x0, x1, x2});
  const float max_y = max({y0, y1, y2});

  const float sr = (float) sample_rate;
  const int sample_w = (int) (target_w * sample_rate);
  const int sample_h = (int) (target_h * sample_rate);

  int sx_start = max(0, (int) ceil(min_x * sr - 0.5f));
  int sx_end   = min(sample_w - 1, (int) floor(max_x * sr - 0.5f));
  int sy_start = max(0, (int) ceil(min_y * sr - 0.5f));
  int sy_end   = min(sample_h - 1, (int) floor(max_y * sr - 0.5f));

  if (sx_start > sx_end || sy_start > sy_end) return;

  const float eps = 1e-6f;

  for (int sy = sy_start; sy <= sy_end; ++sy) {
    float py = ((float) sy + 0.5f) / sr;
    for (int sx = sx_start; sx <= sx_end; ++sx) {
      float px = ((float) sx + 0.5f) / sr;

      float e01 = edge(x0, y0, x1, y1, px, py);
      float e12 = edge(x1, y1, x2, y2, px, py);
      float e20 = edge(x2, y2, x0, y0, px, py);
    
      bool inside;
      if (area >= 0.0f) {
        inside = (e01 >= -eps && e12 >= -eps && e20 >= -eps);
      } else {
        inside = (e01 <= eps && e12 <= eps && e20 <= eps);
      }

      if (inside) {
        fill_sample(sx, sy, color);
      }
    }
  }
}

void SoftwareRendererImp::rasterize_image( float x0, float y0,
                                           float x1, float y1,
                                           Texture& tex ) {
  // Task 6: 
  // Implement image rasterization

}

// resolve samples to render target
void SoftwareRendererImp::resolve( void ) {

  // Task 4: 
  // Implement supersampling
  // You may also need to modify other functions marked with "Task 4".
  if (render_target == nullptr || target_w == 0 || target_h == 0) return;

  const size_t samples_per_pixel = sample_rate * sample_rate;
  if (samples_per_pixel == 0 || sample_buffer.empty()) return;

  const float inv_samples = 1.0f / (float) samples_per_pixel;
  auto clamp01 = [](float x) {
    return min(1.0f, max(0.0f, x));
  };

  for (size_t py = 0; py < target_h; ++py) {
    for (size_t px = 0; px < target_w; ++px) {
      size_t base = (px + py * target_w) * samples_per_pixel;
      Color accum(0, 0, 0, 0);
      for (size_t s = 0; s < samples_per_pixel; ++s) {
        accum += sample_buffer[base + s];
      }
      accum *= inv_samples;

      size_t out = 4 * (px + py * target_w);
      render_target[out    ] = (uint8_t) (255.0f * clamp01(accum.r));
      render_target[out + 1] = (uint8_t) (255.0f * clamp01(accum.g));
      render_target[out + 2] = (uint8_t) (255.0f * clamp01(accum.b));
      render_target[out + 3] = (uint8_t) (255.0f * clamp01(accum.a));
    }
  }

}

void SoftwareRendererImp::resize_sample_buffer( void ) {
  if (target_w == 0 || target_h == 0) {
    sample_buffer.clear();
    return;
  }

  size_t samples_per_pixel = sample_rate * sample_rate;
  size_t total_samples = target_w * target_h * samples_per_pixel;
  sample_buffer.assign(total_samples, Color::White);
}

void SoftwareRendererImp::fill_sample( int sx, int sy, Color color ) {
  if (sx < 0 || sy < 0) return;

  int sample_w = (int) (target_w * sample_rate);
  int sample_h = (int) (target_h * sample_rate);
  if (sx >= sample_w || sy >= sample_h) return;

  size_t px = (size_t) (sx / (int) sample_rate);
  size_t py = (size_t) (sy / (int) sample_rate);
  size_t sub_x = (size_t) (sx % (int) sample_rate);
  size_t sub_y = (size_t) (sy % (int) sample_rate);
  size_t samples_per_pixel = sample_rate * sample_rate;
  size_t sample_index = (px + py * target_w) * samples_per_pixel + (sub_x + sub_y * sample_rate);

  sample_buffer[sample_index] = color;
}

void SoftwareRendererImp::fill_pixel( int px, int py, Color color ) {
  if (px < 0 || py < 0) return;
  if (px >= (int) target_w || py >= (int) target_h) return;

  size_t samples_per_pixel = sample_rate * sample_rate;
  size_t base = ((size_t) px + (size_t) py * target_w) * samples_per_pixel;
  for (size_t s = 0; s < samples_per_pixel; ++s) {
    sample_buffer[base + s] = color;
  }

}


} // namespace CMU462
