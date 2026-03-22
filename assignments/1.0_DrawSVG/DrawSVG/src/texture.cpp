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

  // fill all 0 sub levels with interchanging colors (JUST AS A PLACEHOLDER)
  Color colors[3] = { Color(1,0,0,1), Color(0,1,0,1), Color(0,0,1,1) };
  for(size_t i = 1; i < tex.mipmap.size(); ++i) {

    Color c = colors[i % 3];
    MipLevel& mip = tex.mipmap[i];

    for(size_t i = 0; i < 4 * mip.width * mip.height; i += 4) {
      float_to_uint8( &mip.texels[i], &c.r );
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

  // return magenta for invalid level
  return Color(1,0,1,1);

}

} // namespace CMU462
