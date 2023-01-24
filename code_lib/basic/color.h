#ifndef COLOR_H
#define COLOR_H

#include "maths.h"

//
// @temporary: move to other file??
//

struct color {
  vec4 value;
  
  vec3 rgb 	  = {1.0f, 1.0f, 1.0f};
  r32  tone 	  = 0.0f;
  r32  saturation = 1.0f;
  r32  brightness = 1.0f;
};

internal vec3
rgb_to_hsv(r32 r, r32 g, r32 b) {
  r32 h, s, v;
  r32 cmax = max_vec3(r, g, b);
  r32 cmin = min_vec3(r, g, b);
  r32 diff = cmax-cmin;
   
  if(cmax == cmin) {
    h = 0.0f;
  } else if(cmax == r) {
    h = (r32) fmod((60.0f * ((g - b) / diff) + 360.0f), 360.0f);
  } else if(cmax == g) {
    h = (r32) fmod((60.0f * ((b - r) / diff) + 120.0f), 360.0f);
  } else if(cmax == b) {
    h = (r32) fmod((60.0f * ((r - g) / diff) + 240.0f), 360.0f);
  }
  
  if(cmax == 0.0f) {
    s = 0;
  } else {
    s = diff / cmax;
  }
  v = cmax;
  
  return(vec3{h, s, v});
}

internal vec3
hsv_to_rgb(r32 H, r32 S,r32 V) {
  r32 s = S;
  r32 v = V;
  r32 C = s * v;
  r32 X = C * (1.0f - (r32) fabs(fmod(H / 60.0f, 2.0f) - 1.0f));
  r32 m = v - C;
  r32 r,g,b;
    
  if(H >= 0.0f && H < 60.0f) {
    r = C,g = X,b = 0.0f;
  } else if(H >= 60.0f && H < 120.0f) {
    r = X,g = C,b = 0.0f;
  } else if(H >= 120.0f && H < 180.0f) {
    r = 0,g = C,b = X;
  } else if(H >= 180.0f && H < 240.0f) {
    r = 0,g = X,b = C;
  } else if(H >= 240.0f && H < 300.0f) {
    r = X,g = 0,b = C;
  } else {
    r = C,g = 0,b = X;
  }
  
  return(vec3{r+m, g+m, b+m});
}

internal color
make_color(vec4 rgb) {
  color c;
  vec3 hsv = rgb_to_hsv(rgb.x, rgb.y, rgb.z);
  
  c.rgb   = {rgb.x, rgb.y, rgb.z};
  c.value = rgb;
  c.tone       = hsv.x;
  c.saturation = hsv.y;
  c.brightness = hsv.z;
  return(c);
}

internal void
update_color(color* color) {
  vec3 rgb_color = hsv_to_rgb(color->tone, color->saturation, color->brightness);
  color->rgb = hsv_to_rgb(color->tone, 1.0f, 1.0f);

  color->value.x = rgb_color.x;
  color->value.y = rgb_color.y;
  color->value.z = rgb_color.z;
}

internal void
u32_to_color_array(float* color_array, u32 color) {
  u8* value = (uint8_t*) &color;
  u8 r = *value; value++;
  u8 g = *value; value++;
  u8 b = *value; value++;
  u8 a = *value;

  color_array[0] = (float) r / 255.0f;
  color_array[1] = (float) g / 255.0f;
  color_array[2] = (float) b / 255.0f;
  color_array[3] = (float) a / 255.0f;
}

#define set_alpha_value(_vec4_color, _new_alpha) _vec4_color.w = _new_alpha

#endif // !COLOR_H
