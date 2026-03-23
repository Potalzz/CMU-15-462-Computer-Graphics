#include "texture.h"
#include "color.h"

#include <assert.h>
#include <cmath>
#include <iostream>
#include <algorithm>

using namespace std;

namespace CMU462 {

static inline Color texel_to_color(const MipLevel& mip, int tx, int ty) {
  tx = max(0, min(tx, (int) mip.width - 1));
  ty = max(0, min(ty, (int) mip.height - 1));

  size_t offset = 4 * ((size_t) tx + (size_t) ty * mip.width);
  const unsigned char* texel = &mip.texels[offset];
  return Color(texel[0] / 255.f, texel[1] / 255.f, texel[2] / 255.f, texel[3] / 255.f);
}

inline void uint8_to_float( float dst[4], unsigned char* src ) {
  uint8_t* src_uint8 = (uint8_t *)src;
  dst[0] = src_uint8[0] / 255.f;
  dst[1] = src_uint8[1] / 255.f;
  dst[2] = src_uint8[2] / 255.f;
  dst[3] = src_uint8[3] / 255.f;
}

inline void float_to_uint8( unsigned char* dst, float src[4] ) {
  uint8_t* dst_uint8 = (uint8_t *)dst;
  dst_uint8[0] = (uint8_t) ( 255.f * max( 0.0f, min( 1.0f, src[0])));
  dst_uint8[1] = (uint8_t) ( 255.f * max( 0.0f, min( 1.0f, src[1])));
  dst_uint8[2] = (uint8_t) ( 255.f * max( 0.0f, min( 1.0f, src[2])));
  dst_uint8[3] = (uint8_t) ( 255.f * max( 0.0f, min( 1.0f, src[3])));
}

void Sampler2DImp::generate_mips(Texture& tex, int startLevel) {

  // NOTE: 
  // This starter code allocates the mip levels and generates a level 
  // map by filling each level with placeholder data in the form of a 
  // color that differs from its neighbours'. You should instead fill
  // with the correct data!

  // Task 7: Implement this

  // check start level
  if ( startLevel >= tex.mipmap.size() ) {
    std::cerr << "Invalid start level"; 
    return;
  }

  // allocate sublevels
  int baseWidth  = tex.mipmap[startLevel].width;
  int baseHeight = tex.mipmap[startLevel].height;
  int numSubLevels = (int)(log2f( (float)max(baseWidth, baseHeight)));

  numSubLevels = min(numSubLevels, kMaxMipLevels - startLevel - 1);
  tex.mipmap.resize(startLevel + numSubLevels + 1);

  int width  = baseWidth;
  int height = baseHeight;
  for (int i = 1; i <= numSubLevels; i++) {

    MipLevel& level = tex.mipmap[startLevel + i];

    // handle odd size texture by rounding down
    width  = max( 1, width  / 2); assert(width  > 0);
    height = max( 1, height / 2); assert(height > 0);

    level.width = width;
    level.height = height;
    level.texels = vector<unsigned char>(4 * width * height);

  }

  for (size_t level = startLevel + 1; level < tex.mipmap.size(); ++level) {
    const MipLevel& parent = tex.mipmap[level - 1];
    MipLevel& mip = tex.mipmap[level];

    for (size_t y = 0; y < mip.height; ++y) {
      for (size_t x = 0; x < mip.width; ++x) {
        Color accum(0, 0, 0, 0);
        accum += texel_to_color(parent, (int) (2 * x),     (int) (2 * y));
        accum += texel_to_color(parent, (int) (2 * x + 1), (int) (2 * y));
        accum += texel_to_color(parent, (int) (2 * x),     (int) (2 * y + 1));
        accum += texel_to_color(parent, (int) (2 * x + 1), (int) (2 * y + 1));
        accum *= 0.25f;

        float rgba[4] = { accum.r, accum.g, accum.b, accum.a };
        size_t offset = 4 * (x + y * mip.width);
        float_to_uint8(&mip.texels[offset], rgba);
      }
    }
  }

}

Color Sampler2DImp::sample_nearest(Texture& tex, 
                                   float u, float v, 
                                   int level) {

  // Task 6: Implement nearest neighbour interpolation
  if (level < 0 || level >= (int) tex.mipmap.size()) {
    return Color(1,0,1,1);
  }

  const MipLevel& mip = tex.mipmap[level];
  if (mip.width == 0 || mip.height == 0 || mip.texels.empty()) {
    return Color(1,0,1,1);
  }

  u = max(0.0f, min(1.0f, u));
  v = max(0.0f, min(1.0f, v));

  int tx = min((int) mip.width - 1, (int) floor(u * mip.width));
  int ty = min((int) mip.height - 1, (int) floor(v * mip.height));

  return texel_to_color(mip, tx, ty);

}

Color Sampler2DImp::sample_bilinear(Texture& tex, 
                                    float u, float v, 
                                    int level) {
  
  // Task 6: Implement bilinear filtering
  if (level < 0 || level >= (int) tex.mipmap.size()) {
    return Color(1,0,1,1);
  }

  const MipLevel& mip = tex.mipmap[level];
  if (mip.width == 0 || mip.height == 0 || mip.texels.empty()) {
    return Color(1,0,1,1);
  }

  u = max(0.0f, min(1.0f, u));
  v = max(0.0f, min(1.0f, v));

  float tx = u * mip.width - 0.5f;
  float ty = v * mip.height - 0.5f;

  int x0 = (int) floor(tx);
  int y0 = (int) floor(ty);
  int x1 = x0 + 1;
  int y1 = y0 + 1;

  float sx = tx - x0;
  float sy = ty - y0;

  Color c00 = texel_to_color(mip, x0, y0);
  Color c10 = texel_to_color(mip, x1, y0);
  Color c01 = texel_to_color(mip, x0, y1);
  Color c11 = texel_to_color(mip, x1, y1);

  Color c0 = c00 * (1.0f - sx) + c10 * sx;
  Color c1 = c01 * (1.0f - sx) + c11 * sx;
  return c0 * (1.0f - sy) + c1 * sy;

}

Color Sampler2DImp::sample_trilinear(Texture& tex, 
                                     float u, float v, 
                                     float u_scale, float v_scale) {

  // Task 7: Implement trilinear filtering
  if (tex.mipmap.empty()) {
    return Color(1,0,1,1);
  }

  float scale = max(u_scale, v_scale);
  scale = max(scale, 1e-8f);

  float level = log2f(scale);
  level = max(0.0f, level);

  int low_level = (int) floor(level);
  int high_level = min((int) tex.mipmap.size() - 1, low_level + 1);
  low_level = min(low_level, (int) tex.mipmap.size() - 1);

  if (low_level == high_level) {
    return sample_bilinear(tex, u, v, low_level);
  }

  float t = level - low_level;
  Color low = sample_bilinear(tex, u, v, low_level);
  Color high = sample_bilinear(tex, u, v, high_level);
  return low * (1.0f - t) + high * t;

}

} // namespace CMU462
