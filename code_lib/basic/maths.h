#ifndef MATHS_H
#define MATHS_H

#include "basic.h"

#define EPSILON  0.0000001f
#define PI       3.14159265358979323846
#define PI_2     1.57079632679489661923

typedef r32 mat4[16];

inline internal s32
s32_distance(s32 n1, s32 n2) {
  s32 x = n1 ^ n2;
  s32 set_bits = 0;
 
  while (x > 0) {
    set_bits += x & 1;
    x >>= 1;
  }
 
  return(set_bits);
}

inline internal r64
to_radians(r64 value) {
  return(value * PI / 180.0);
}

inline internal r32
to_radians(r32 value) {
  return(value * (r32)PI / 180.0f);
}

inline internal r32
to_degrees(r32 value) {
  return(value * 180.0f / (r32) PI);
}

inline internal r32
lerp(float a, float b, float f) {
  return(a + f * (b - a));
}

inline internal r32
blend(r32 min, r32 max, float value) {
  return((1.0f - value) * min + value * max);
}

inline internal s32
clamp_int(s32 x, s32 upper, s32 lower) {
  if(x < lower)
    return lower;
  if(x > upper)
    return upper;
  return x;
}

inline internal r32
clamp(r32 x, r32 upper, r32 lower) {
  if(x < lower)
    return lower;
  if(x > upper)
    return upper;
  return x;
}

struct ivec2 {
  s32 x;
  s32 y;
};

struct vec2 {
  r32 x;
  r32 y;
};

vec2 operator+ (vec2 a, const vec2& b) {
  return(vec2{a.x + b.x, a.y + b.y});
}
vec2 operator- (vec2 a, const vec2& b) {
  return(vec2{a.x - b.x, a.y - b.y});
}
vec2 operator* (vec2 a, const vec2& b) {
  return(vec2{a.x * b.x, a.y * b.y});
}
vec2 operator/ (vec2 a, const vec2& b) {
  return(vec2{a.x / b.x, a.y / b.y});
}

bool operator==(vec2 a, vec2 b) {
  return(a.x == b.x && a.y == b.y);
}

b32 operator< (vec2 a, vec2 b) {
  return(a.x < b.x && a.y < b.y);
}

b32 operator> (vec2 a, vec2 b) {
  return(a.x > b.x && a.y > b.y);
}

inline internal r32
vec2_cross(vec2 a, vec2 b) {
  return(a.x * b.y - b.x * a.y);
}

inline internal r32
vec2_dot(vec2 a, vec2 b) {
  return(a.x * b.x + a.y * b.y);
}

inline internal r32
vec2_length(vec2 vec) {
  return(sqrtf(vec2_dot(vec, vec)));
}

inline internal vec2
vec2_normalize(vec2 vec) {
  r32 length = vec2_length(vec);
  r32 inverse_length = 1.0f / length;
  r32 x = vec.x * inverse_length;
  r32 y = vec.y * inverse_length;

  return(vec2{x, y});
}

inline internal r32
area(vec2 a, vec2 b, vec2 c) {
  r32 area = ((b.x - a.x)*(c.y - a.y) -
		 (c.x - a.x)*(b.y - a.y)) / 2.0f;
  return(area > 0.0f ? area : -area);
}
 
inline internal b32
point_on_triangle(vec2 point, vec2 a, vec2 b, vec2 c) {  
  float A = area(a, b, c);
 
  float A1 = area(point, b, c);
  float A2 = area(a, point, c); 
  float A3 = area(a, b, point);
  
  return(A == A1 + A2 + A3);
}

// @incomplete
struct ivec3 {
  s32 x;
  s32 y;
  s32 z;
};

struct vec3 {
  r32 x;
  r32 y;
  r32 z;
};

bool operator==(vec3 a, vec3 b) {
  return(a.x == b.x && a.y == b.y && a.z == b.z);
}
bool operator!=(vec3 a, vec3 b) {
  return(!(a == b));
}
vec3 operator+ (vec3 a, const vec3& b) {
  return(vec3{a.x + b.x, a.y + b.y, a.z + b.z});
}
vec3 operator- (vec3 a, const vec3& b) {
  return(vec3{a.x - b.x, a.y - b.y, a.z - b.z});
}
void operator+= (vec3& a, const vec3& b) {
  a = a+b;
}
void operator-= (vec3& a, const vec3& b) {
  a = a-b;
}
vec3 operator* (vec3 a, const vec3& b) {
  return(vec3{a.x * b.x, a.y * b.y, a.z * b.z});
}
vec3 operator* (vec3 a, r32 v) {
  return(vec3{a.x * v, a.y * v, a.z * v});
}
vec3 operator/ (vec3 a, const vec3& b) {
  return(vec3{a.x / b.x, a.y / b.y, a.z / b.z});
}
bool operator< (vec3 a, const vec3& b) {
  return(a.x < b.x || a.y < b.y || a.z < b.z);
}
bool operator> (vec3 a, const vec3& b) {
  return(a.x > b.x || a.y > b.y || a.z > b.z);
}

r32 max_vec3(r32 a, r32 b, r32 c) {
   return ((a > b)? (a > c ? a : c) : (b > c ? b : c));
}
r32 min_vec3(r32 a, r32 b, r32 c) {
   return ((a < b)? (a < c ? a : c) : (b < c ? b : c));
}

inline internal vec3
lerp_vec3(vec3 start, vec3 end, r32 progression) {
  vec3 result;
  result.x = start.x + (end.x - start.x) * progression;
  result.y = start.y + (end.y - start.y) * progression;
  result.z = start.z + (end.z - start.z) * progression;
  return(result);
}

inline internal r32
vec3_dot(vec3 a, vec3 b) {
  return(a.x * b.x + a.y * b.y + a.z * b.z);
}

inline internal r32
vec3_length(vec3 vec) {
  return(sqrtf(vec3_dot(vec, vec)));
}

inline internal vec3
vec3_normalize(vec3 vec) {
  r32 length = vec3_length(vec);
  //if(length == 0.0f)
  //  return(vec);
  r32 inverse_length = 1.0f / length;
  r32 x = vec.x * inverse_length;
  r32 y = vec.y * inverse_length;
  r32 z = vec.z * inverse_length;

  return(vec3{x, y, z});
}

inline internal vec3
vec3_cross(vec3 a, vec3 b) {
  vec3 result;
  result.x = a.y * b.z - a.z * b.y;
  result.y = a.z * b.x - a.x * b.z;
  result.z = a.x * b.y - a.y * b.x;
  return(result);
}

struct vec4 {
  r32 x;
  r32 y;
  r32 z;
  r32 w;
  
  /* @incomplete
  union{
    r32 data[4];
  };
  */
};

inline internal void
swap_vec4(vec4* a, vec4* b) {
  vec4 aux_a = *a;
  *a = *b;
  *b = aux_a;
}

vec4 operator+ (vec4 a, const vec4& b) {
  return(vec4{a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w});
}
vec4 operator- (vec4 a, const vec4& b) {
  return(vec4{a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w});
}
vec4 operator* (vec4 a, const vec4& b) {
  return(vec4{a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w});
}
vec4 operator/ (vec4 a, const vec4& b) {
  return(vec4{a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w});
}

vec4 operator* (vec4 a, r32 value) {
  return(vec4{a.x * value, a.y * value, a.z * value, a.w * value});
}

vec4 operator* (r32* mat, vec4 vec) {
  return(vec4{  mat[0] * vec.x + mat[4] * vec.y + mat[8]  * vec.z + mat[12] * vec.w,
		mat[1] * vec.x + mat[5] * vec.y + mat[9]  * vec.z + mat[13] * vec.w,
		mat[2] * vec.x + mat[6] * vec.y + mat[10] * vec.z + mat[14] * vec.w,
		mat[3] * vec.x + mat[7] * vec.y + mat[11] * vec.z + mat[15] * vec.w});
}

// @temporary: position * mat4 operator
vec3 operator* (r32* mat, vec3 pos) {
  vec4 vec = {pos.x, pos.y, pos.z, 1.0f};
  vec4 result = mat * vec;
  return(vec3{result.x, result.y, result.z});
}

// @temporary
inline internal vec4
lerp(vec4 a, vec4 b, r32 t){
  vec4 lerp_value;
  lerp_value.x = lerp(a.x, b.x, t);
  lerp_value.y = lerp(a.y, b.y, t);
  lerp_value.z = lerp(a.z, b.z, t);
  lerp_value.w = lerp(a.w, b.w, t);
  return(lerp_value);
}

struct rect {
  r32 x, y;
  r32 w, h;

  r32 layer; // @temporary
};

rect make_rect(r32 x, r32 y, r32 w, r32 h) {
  rect r;
  
  r.x = x;
  r.y = y;
  
  r.w = (r32) fabs(w);
  r.h = (r32) fabs(h);
  
  return(r);
}

inline internal b32
point_on_rect(r32 x, r32 y, rect r) {
  return((x >= r.x) && (x <= r.x + r.w) && (y >= r.y) && (y <= r.y + r.h));
}

inline internal b32
rect_overlap(rect a, rect b) {
  return(a.x < b.x + b.w  	&&
	 a.x 	   + a.w > b.x  &&
	 a.y < b.y + b.h 	&&
	 a.h 	   + a.y > b.y);
}

inline internal b32
rect_on_rect(rect a, rect b) {
  return(point_on_rect(a.x, a.y, b) &&
	 point_on_rect(a.x + a.w, a.y      , b) &&
	 point_on_rect(a.x      , a.y + a.h, b) &&
	 point_on_rect(a.x + a.w, a.y + a.h, b));
}

struct quat {
  r32 x, y, z, w;
};

internal quat
mul_quat(quat a, quat b) {
  quat q;
  
  vec3 va = {a.x, a.y, a.z};
  vec3 vb = {b.x, b.y, b.z};
  float wa = a.w; 
  float wb = b.w;  
      
  vec3 va_x_vb = vec3_cross(va, vb);
  q.x = va.x * wb  + vb.x * wa + va_x_vb.x;
  q.y = va.y * wb  + vb.y * wa + va_x_vb.y;
  q.z = va.z * wb  + vb.z * wa + va_x_vb.z;
  q.w = wa * wb - vec3_dot(va, vb);

  return(q);
}

internal r32
quat_dot(quat a, quat b) {
  return(a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w);
}

internal quat
quat_conjugate(quat q) {
  return(quat{-q.x, -q.y, -q.z, q.w});
}

internal quat
quat_axis_angle(vec3 angles, r32 theta) {
  quat q;
  r32 cos_ht = cosf(theta * 0.5f);
  r32 sin_ht = sinf(theta * 0.5f);
      
  q.x = angles.x * sin_ht;
  q.y = angles.y * sin_ht;
  q.z = angles.z * sin_ht;
  q.w = cos_ht;

  return(q);
}

internal quat
quat_inverse(quat q) {
  quat result = q;

  result = quat_conjugate(result);
  r32 dot_quat = quat_dot(q, q);
  
  result.x = result.x / dot_quat;
  result.y = result.y / dot_quat;
  result.z = result.z / dot_quat;
  result.w = result.w / dot_quat;

  return(result);
}

internal inline quat
slerp_quat(quat a, quat b, r32 t) {
  quat q;
  
  r32 cos_omega = a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
  if(cos_omega < 0.0f) {
    a.x = -a.x;
    a.y = -a.y;
    a.z = -a.z;
    a.w = -a.w;
    cos_omega = -cos_omega;
  }
      
  r32 k0, k1;
  if(cos_omega > 0.9999f) {
    k0 = 1.0f - t;
    k1 = t;
  }	else {
    r32 sinOmega = sqrtf(1.0f - cos_omega * cos_omega);
    r32 omega = atan2f(sinOmega, cos_omega);
    r32 oneOverSinOmega = 1.0f / sinOmega;
	
    k0 = sinf((1.0f - t) * omega) * oneOverSinOmega;
    k1 = sinf(t * omega) * oneOverSinOmega;
  }
      
  q.x = a.x * k0 + b.x * k1;
  q.y = a.y * k0 + b.y * k1;
  q.z = a.z * k0 + b.z * k1;
  q.w = a.w * k0 + b.w * k1;

  return(q);
}

internal vec3
quat_vertex_rotate(quat q, vec3 pos) {
  quat pos_quat = {pos.x, pos.y, pos.z, 0.0f}; 
  quat q_star   = {-q.x, -q.y, -q.z, q.w};
      
  quat result, temp;
  temp   = mul_quat(pos_quat, q_star);
  result = mul_quat(q, temp);
      
  return(vec3{result.x, result.y, result.z});
}
    
internal quat
quat_from_euler(vec3 euler) {
  quat q;

  r64 yaw   = euler.x;
  r64 pitch = euler.y;
  r64 roll  = euler.z;
  
  q.x = (r32) (sin(roll/2.0) * cos(pitch/2.0) * cos(yaw/2.0) - cos(roll/2.0) * sin(pitch/2.0) * sin(yaw/2.0));
  q.y = (r32) (cos(roll/2.0) * sin(pitch/2.0) * cos(yaw/2.0) + sin(roll/2.0) * cos(pitch/2.0) * sin(yaw/2.0));
  q.z = (r32) (cos(roll/2.0) * cos(pitch/2.0) * sin(yaw/2.0) - sin(roll/2.0) * sin(pitch/2.0) * cos(yaw/2.0));
  q.w = (r32) (cos(roll/2.0) * cos(pitch/2.0) * cos(yaw/2.0) + sin(roll/2.0) * sin(pitch/2.0) * sin(yaw/2.0));
  
  return(q);
}

internal vec3
euler_from_quat(quat q) {
  vec3 euler;
  
  // roll (x-axis rotation)
  r64 sinr_cosp = 2 * (q.w * q.x + q.y * q.z);
  r64 cosr_cosp = 1 - 2 * (q.x * q.x + q.y * q.y);
  euler.x = (r32) atan2(sinr_cosp, cosr_cosp);

  // pitch (y-axis rotation)
  r64 sinp = 2 * (q.w * q.y - q.z * q.x);
  if (fabs(sinp) >= 1.0)
    euler.y = (r32) copysign(PI / 2, sinp); // use 90 degrees if out of range
  else
    euler.y = (r32) asin(sinp);

  // yaw (z-axis rotation)
  r64 siny_cosp = 2 * (q.w * q.z + q.x * q.y);
  r64 cosy_cosp = 1 - 2 * (q.y * q.y + q.z * q.z);
  euler.z = (r32) atan2(siny_cosp, cosy_cosp);

  return(euler);
}

#if 0
inline internal quat
quat_from_mat4(r32* a) {
  quat q;
  
  r32 diagonal = a[0] + a[1 + 1 * 4] + a[2 + 2 * 4];
  if(diagonal > 0) {
    r32 s = 0.5f / sqrtf(diagonal + 1.0f);
    q.w = 0.25f / s;
    q.x = (a[1 + 2 * 4] - a[2 + 1 * 4]) * s;
    q.y = (a[2 + 0 * 4] - a[0 + 2 * 4]) * s;
    q.z = (a[0 + 1 * 4] - a[1 + 0 * 4]) * s;
  } else {
    if ( a[0] > a[1 + 1 * 4] && a[0] > a[2 + 2 * 4] ) {
      r32 s = 2.0f * sqrtf( 1.0f + a[0] - a[1 + 1 * 4] - a[2 + 2 * 4]);
      q.w = (a[1 + 2 * 4] - a[2 + 1 * 4]) / s;
      q.x = 0.25f * s;
      q.y = (a[1 + 0 * 4] + a[0 + 1 * 4] ) / s;
      q.z = (a[2 + 0 * 4] + a[0 + 2 * 4] ) / s;
    } else if (a[1 + 1 * 4] > a[2 + 2 * 4]) {
      r32 s = 2.0f * sqrtf( 1.0f + a[1 + 1 * 4] - a[0] - a[2 + 2 * 4]);
      q.w = (a[2 + 0 * 4] - a[0 + 2 * 4] ) / s;
      q.x = (a[1 + 0 * 4] + a[0 + 1 * 4] ) / s;
      q.y = 0.25f * s;
      q.z = (a[2 + 1 * 4] + a[1 + 2 * 4] ) / s;
    } else {
      r32 s = 2.0f * sqrtf( 1.0f + a[2 + 2 * 4] - a[0] - a[1 + 1 * 4] );
      q.w = (a[0 + 1 * 4] - a[1 + 0 * 4] ) / s;
      q.x = (a[2 + 0 * 4] + a[0 + 2 * 4] ) / s;
      q.y = (a[2 + 1 * 4] + a[1 + 2 * 4] ) / s;
      q.z = 0.25f * s;
    }
  }
  
  return(q);
}
#endif

inline internal quat
quat_from_mat4(r32* a) {
  quat q;
  
  r32 trace = a[0] + a[1 + 1 * 4] + a[2 + 2 * 4]; // I removed + 1.0f; see discussion with Ethan
  if(trace > EPSILON) {
    r32 s = 0.5f / sqrtf(trace + 1.0f);
    q.w = 0.25f / s;
    q.x = (a[2 + 1 * 4] - a[1 + 2 * 4]) * s;
    q.y = (a[0 + 2 * 4] - a[2 + 0 * 4]) * s;
    q.z = (a[1 + 0 * 4] - a[0 + 1 * 4]) * s;
  } else {
    if ( a[0] > a[1 + 1 * 4] && a[0] > a[2 + 2 * 4] ) {
      r32 s = 2.0f * sqrtf( 1.0f + a[0] - a[1 + 1 * 4] - a[2 + 2 * 4]);
      q.w = (a[2 + 1 * 4] - a[1 + 2 * 4]) / s;
      q.x = 0.25f * s;
      q.y = (a[0 + 1 * 4] + a[1 + 0 * 4] ) / s;
      q.z = (a[0 + 2 * 4] + a[2 + 0 * 4] ) / s;
    } else if (a[1 + 1 * 4] > a[2 + 2 * 4]) {
      r32 s = 2.0f * sqrtf( 1.0f + a[1 + 1 * 4] - a[0] - a[2 + 2 * 4]);
      q.w = (a[0 + 2 * 4] - a[2 + 0 * 4] ) / s;
      q.x = (a[0 + 1 * 4] + a[1 + 0 * 4] ) / s;
      q.y = 0.25f * s;
      q.z = (a[1 + 2 * 4] + a[2 + 1 * 4] ) / s;
    } else {
      r32 s = 2.0f * sqrtf( 1.0f + a[2 + 2 * 4] - a[0] - a[1 + 1 * 4] );
      q.w = (a[1 + 0 * 4] - a[0 + 1 * 4] ) / s;
      q.x = (a[0 + 2 * 4] + a[2 + 0 * 4] ) / s;
      q.y = (a[1 + 2 * 4] + a[2 + 1 * 4] ) / s;
      q.z = 0.25f * s;
    }
  }
  
  return(q);
}


internal void
mat4_from_quat(float* M, quat Q) {
  M[0]  = 1.0f - 2.0f * (Q.y * Q.y  + Q.z * Q.z);
  M[1]  = 2.0f * (Q.x * Q.y + Q.w * Q.z);
  M[2]  = 2.0f * (Q.x * Q.z - Q.w * Q.y);
  M[3]  = 0.0f;
      
  M[4]  = 2.0f * (Q.x * Q.y - Q.w * Q.z);
  M[5]  = 1.0f - 2.0f * (Q.x * Q.x  + Q.z * Q.z);
  M[6]  = 2.0f * (Q.y * Q.z + Q.w * Q.x);
  M[7]  = 0.0f;
      
  M[8]  = 2.0f * (Q.x * Q.z + Q.w * Q.y);
  M[9]  = 2.0f * (Q.y * Q.z - Q.w * Q.x);
  M[10] = 1.0f - 2.0f * (Q.x * Q.x + Q.y * Q.y);
  M[11] = 0.0f;
      
  M[12] = 0.0f;
  M[13] = 0.0f;
  M[14] = 0.0f;
  M[15] = 1.0f;
}

inline internal void
imat4(r32* out) {
  memset(out, 0, sizeof(r32) * 16);
  out[0]  = 1.0f;
  out[5]  = 1.0f;
  out[10] = 1.0f;
  out[15] = 1.0f;
}

inline internal void
mat_ortho(r32* mat, r32 b, r32 t, r32 l, r32 r, r32 n, r32 f) { 
  mat[0] = 2.0f / (r - l);
  mat[1] = 0.0f; 
  mat[2] = 0.0f; 
  mat[3] = 0.0f; 
  
  mat[4] = 0.0f; 
  mat[5] = 2.0f / (t - b);
  mat[6] = 0.0f; 
  mat[7] = 0.0f; 
  
  mat[8] = 0.0f; 
  mat[9] = 0.0f;
  mat[10] = 2.0f / (n - f);
  mat[11] = 0.0f; 
  
  mat[12] = (l + r) / (l - r);
  mat[13] = (b + t) / (b - t); 
  mat[14] = (f + n) / (f - n); 
  mat[15] = 1.0f;
}

inline internal void
mat_perspective(r32* mat, r32 fov, r32 aspect_ratio, r32 n, r32 f) {
  imat4(mat);
  r32 q = 1.0f / (r32) tan(to_radians(0.5f * fov));
  mat[0] = q / aspect_ratio;
  mat[1] = 0.0f; 
  mat[2] = 0.0f; 
  mat[3] = 0.0f; 
  
  mat[4] = 0.0f; 
  mat[5] = q;
  mat[6] = 0.0f; 
  mat[7] = 0.0f; 
  
  mat[8] = 0.0f; 
  mat[9] = 0.0f;
  mat[10] = (n + f) / (n - f);
  mat[11] = -1.0f; 

  mat[12] = 0.0f;
  mat[13] = 0.0f; 
  mat[14] = (2.0f * n * f) / (n - f); 
  mat[15] = 0.0f;
}

#if 0
internal void
mat_cofac(r32* matrix, r32* sub_matrix, s32 p, s32 q, s32 n, s32 TOTAL_N) {
  s32 i = 0, j = 0;

  for(s32 row = 0; row < n; row++) {
    for(s32 col = 0; col < n; col++) {
      if(row != p && col != q) {
	sub_matrix[j + i * TOTAL_N] = matrix[col + row * TOTAL_N];
	j++;
	if(j == n - 1) {
	  j = 0;
	  i++;
	}
      }
    }
  }
}

internal r32
mat_determinant(r32* matrix, s32 n, s32 TOTAL_N) {
  r32 determinant = 0;
  if(n == 1) {
    determinant = matrix[0];
  } else {
    s32 sign = 1;
    r32 sub_matrix[10 * 10];
    // r32* sub_matrix = static_cast<r32*>(temporary_alloc(pool, sizeof(r32) * TOTAL_N * TOTAL_N));
    
    for(s32 f = 0; f < n; f++) {
      mat_cofac(matrix, sub_matrix, 0, f, n, TOTAL_N);
      determinant += sign * matrix[f + 0 * TOTAL_N] * mat_determinant(sub_matrix, n - 1, TOTAL_N);
      sign = -sign;
    }
  }
  
  return(determinant);
}

internal void
mat_adjoint(r32* matrix, r32* adj_matrix, u32 n, u32 TOTAL_N) {
  if(n == 1) {
    adj_matrix[0] = 1;
  }
  
  s32 sign = 1;
  r32 aux_matrix[10 * 10];
  // r32* aux_matrix = static_cast<r32*>(temporary_alloc(pool, sizeof(r32) * n * n));

  for(u32 i = 0; i < n; i++) {
    for(u32 j = 0; j < n; j++) {
      mat_cofac(matrix, aux_matrix, i, j, n, TOTAL_N);
      sign = ((i + j) % 2 == 0) ? 1 : -1;
      adj_matrix[i + j * n] = sign * (mat_determinant(aux_matrix, n - 1, TOTAL_N));
    }
  }
}

internal void
mat_inverse(r32* out, r32* in, s32 n = 4) {
  s32 TOTAL_N = n;
  r32 determinant = mat_determinant(in, n, TOTAL_N);
  
  r32 adj_matrix[10 * 10];
  // r32* adj_matrix = static_cast<r32*>(temporary_alloc(pool, sizeof(r32) * TOTAL_N * TOTAL_N));
  mat_adjoint(in, adj_matrix, n, TOTAL_N);
  
  for(s32 i = 0; i < n; i++) {
    for(s32 j = 0; j < n; j++) {
      out[i + j * n] = adj_matrix[i + j * n] / determinant;
    }
  }
}
#endif

internal void
mat4_transpose(r32* out, r32* in) {
  mat4 temp;
  for(u32 i = 0; i < 4; i++) {
    for(u32 j = 0; j < 4; j++) {
      temp[i + j * 4] = in[j + i * 4];
    }
  }
  memcpy(out, temp, sizeof(mat4));
}

inline internal void
mat4_inverse(r32* out, r32* in) {
  mat4 temp = {};

  temp[0] = in[5] * in[10] * in[15] -
    in[5] * in[11] * in[14] -
    in[9] * in[6] * in[15] +
    in[9] * in[7] * in[14] +
    in[13] * in[6] * in[11] -
    in[13] * in[7] * in[10];

  temp[4] = -in[4] * in[10] * in[15] +
    in[4] * in[11] * in[14] +
    in[8] * in[6] * in[15] -
    in[8] * in[7] * in[14] -
    in[12] * in[6] * in[11] +
    in[12] * in[7] * in[10];

  temp[8] = in[4] * in[9] * in[15] -
    in[4] * in[11] * in[13] -
    in[8] * in[5] * in[15] +
    in[8] * in[7] * in[13] +
    in[12] * in[5] * in[11] -
    in[12] * in[7] * in[9];

  temp[12] = -in[4] * in[9] * in[14] +
    in[4] * in[10] * in[13] +
    in[8] * in[5] * in[14] -
    in[8] * in[6] * in[13] -
    in[12] * in[5] * in[10] +
    in[12] * in[6] * in[9];

  temp[1] = -in[1] * in[10] * in[15] +
    in[1] * in[11] * in[14] +
    in[9] * in[2] * in[15] -
    in[9] * in[3] * in[14] -
    in[13] * in[2] * in[11] +
    in[13] * in[3] * in[10];

  temp[5] = in[0] * in[10] * in[15] -
    in[0] * in[11] * in[14] -
    in[8] * in[2] * in[15] +
    in[8] * in[3] * in[14] +
    in[12] * in[2] * in[11] -
    in[12] * in[3] * in[10];

  temp[9] = -in[0] * in[9] * in[15] +
    in[0] * in[11] * in[13] +
    in[8] * in[1] * in[15] -
    in[8] * in[3] * in[13] -
    in[12] * in[1] * in[11] +
    in[12] * in[3] * in[9];

  temp[13] = in[0] * in[9] * in[14] -
    in[0] * in[10] * in[13] -
    in[8] * in[1] * in[14] +
    in[8] * in[2] * in[13] +
    in[12] * in[1] * in[10] -
    in[12] * in[2] * in[9];

  temp[2] = in[1] * in[6] * in[15] -
    in[1] * in[7] * in[14] -
    in[5] * in[2] * in[15] +
    in[5] * in[3] * in[14] +
    in[13] * in[2] * in[7] -
    in[13] * in[3] * in[6];

  temp[6] = -in[0] * in[6] * in[15] +
    in[0] * in[7] * in[14] +
    in[4] * in[2] * in[15] -
    in[4] * in[3] * in[14] -
    in[12] * in[2] * in[7] +
    in[12] * in[3] * in[6];

  temp[10] = in[0] * in[5] * in[15] -
    in[0] * in[7] * in[13] -
    in[4] * in[1] * in[15] +
    in[4] * in[3] * in[13] +
    in[12] * in[1] * in[7] -
    in[12] * in[3] * in[5];

  temp[14] = -in[0] * in[5] * in[14] +
    in[0] * in[6] * in[13] +
    in[4] * in[1] * in[14] -
    in[4] * in[2] * in[13] -
    in[12] * in[1] * in[6] +
    in[12] * in[2] * in[5];

  temp[3] = -in[1] * in[6] * in[11] +
    in[1] * in[7] * in[10] +
    in[5] * in[2] * in[11] -
    in[5] * in[3] * in[10] -
    in[9] * in[2] * in[7] +
    in[9] * in[3] * in[6];

  temp[7] = in[0] * in[6] * in[11] -
    in[0] * in[7] * in[10] -
    in[4] * in[2] * in[11] +
    in[4] * in[3] * in[10] +
    in[8] * in[2] * in[7] -
    in[8] * in[3] * in[6];

  temp[11] = -in[0] * in[5] * in[11] +
    in[0] * in[7] * in[9] +
    in[4] * in[1] * in[11] -
    in[4] * in[3] * in[9] -
    in[8] * in[1] * in[7] +
    in[8] * in[3] * in[5];

  temp[15] = in[0] * in[5] * in[10] -
    in[0] * in[6] * in[9] -
    in[4] * in[1] * in[10] +
    in[4] * in[2] * in[9] +
    in[8] * in[1] * in[6] -
    in[8] * in[2] * in[5];

  r32 determinant = in[0] * temp[0] + in[1] * temp[4] + in[2] * temp[8] + in[3] * temp[12];
  determinant = 1.0f / determinant;

  for (u32 i = 0; i < 16; i++)
    out[i] = temp[i] * determinant;
}

internal void
mat_multiply(r32* out, r32* mat, r32* other, u32 row_n = 4, u32 col_n = 4) { // @speedup
  for(u32 i = 0; i < row_n; i++) {
    for(u32 j = 0; j < col_n; j++) {
      out[i + j * row_n] = 0;
      
      for (u32 k = 0; k < row_n; k++) {
        out[i + j * row_n] += mat[i + k * row_n] * other[k + j * col_n];
      }
    }
  }
}

inline internal void
mat_multiply(r32* mat, r32* other) {
  mat4 data;
  for(s32 row = 0; row < 4; row++) {
    for(s32 col = 0; col < 4; col++) {
      r32 sum = 0.0f;
      for(s32 e = 0; e < 4; e++) {
	sum += mat[e + row * 4] * other[col + e * 4];
      }
      data[col + row * 4] = sum;
    }
  }
  
  memcpy(mat, data, sizeof(mat4));
}

inline internal void
mat_translate(r32* out, vec3 pos) {
  
  mat4 translation;
  imat4(translation);
  translation[12] = pos.x;
  translation[13] = pos.y;
  translation[14] = pos.z;
  mat_multiply(out, translation);
}

inline internal void
mat_scale(r32* out, vec3 pos) {
  mat4 scale;
  imat4(scale);

  scale[0]  = pos.x;
  scale[5]  = pos.y;
  scale[10] = pos.z;
  mat_multiply(out, scale);
}

inline internal void
mat_scale(r32* out, r32 scalar) {
  
  mat4 scale;
  imat4(scale);
  scale[0]  = scalar;
  scale[5]  = scalar;
  scale[10] = scalar;
  mat_multiply(out, scale);
}

inline internal void
mat_rotate(r32* out, r32 angle, r32 x, r32 y, r32 z) {
  mat4 rotation;
  imat4(rotation);
 
  r32 r = to_radians(angle);
  r32 c = (r32) cos(r);
  r32 s = (r32) sin(r);
  r32 omc = 1.0f - c;
		
  rotation[0 + 0 * 4]  = x * x * omc + c;
  rotation[0 + 1 * 4]  = y * x * omc + z * s;
  rotation[0 + 2 * 4]  = x * z * omc - y * s;
	
  rotation[1 + 0 * 4]  = x * y * omc - z * s;
  rotation[1 + 1 * 4]  = y * y * omc + c;
  rotation[1 + 2 * 4]  = y * z * omc + x * s;

  rotation[2 + 0 * 4]  = x * z * omc + y * s;
  rotation[2 + 1 * 4]  = y * z * omc - x * s;
  rotation[2 + 2 * 4]  = z * z * omc + c;
  mat_multiply(out, rotation);
}

inline internal void
mat_look_at(r32* out, vec3 eye, vec3 target, vec3 up_dir) {
  vec3 z_axis = vec3_normalize(eye - target);
  vec3 x_axis = vec3_normalize(vec3_cross(up_dir, z_axis));
  vec3 y_axis = vec3_cross(z_axis, x_axis);
  
  out[0]  = x_axis.x;
  out[1]  = y_axis.x;
  out[2]  = z_axis.x;
  out[3]  = 0.0f;
  
  out[4]  = x_axis.y;
  out[5]  = y_axis.y;
  out[6]  = z_axis.y;
  out[7]  = 0.0f;
  
  out[8]  = x_axis.z;
  out[9]  = y_axis.z;
  out[10] = z_axis.z;
  out[11] = 0.0f;
  
  out[12] = 0.0f;
  out[13] = 0.0f;
  out[14] = 0.0f;
  out[15] = 1.0f;
  
  mat4 mat_translation;
  imat4(mat_translation);
  mat_translation[12] = -vec3_dot(x_axis, eye);
  mat_translation[13] = -vec3_dot(y_axis, eye);
  mat_translation[14] = -vec3_dot(z_axis, eye);

  mat_multiply(out, mat_translation);
}

//*****GET MOUSE LOCATION*****//
inline internal vec2
calculate_normalized_device_coords(r64 mousex, r64 mousey, u32 width, u32 height) {
  r32 x = (r32) (2.0 * mousex) / (r32) width  - 1.0f;
  r32 y = (r32) (2.0 * mousey) / (r32) height - 1.0f;
  return(vec2{x, y});
}

inline internal vec4
calculate_eye_coords(vec4 clip_coords, r32* proj_matrix) {
  mat4 inverse_projection;
  mat4_inverse(inverse_projection, proj_matrix);
  vec4 eye_coords = inverse_projection * clip_coords;
  return(vec4{eye_coords.x, eye_coords.y, -1.0f, 0.0f});
}

inline internal vec3
calculate_world_coords(vec4 eye_coords, r32* view_matrix) {
  mat4 inverse_view;
  mat4_inverse(inverse_view, view_matrix);
  vec4 ray_world = inverse_view * eye_coords;
  vec3 mouse_ray = {ray_world.x, ray_world.y, ray_world.z};
  mouse_ray = vec3_normalize(mouse_ray);
  return(mouse_ray);
}

inline internal vec3
get_mouse_ray(u32 width, u32 height, r32 mousex, r32 mousey,
			 r32* proj_matrix, r32* view_matrix) {
  vec2 normalized_device_coords = calculate_normalized_device_coords(mousex, mousey, width, height);
  vec4 clip_coords = {normalized_device_coords.x, normalized_device_coords.y, -1.0f, 1.0f};
  vec4 eye_coords = calculate_eye_coords(clip_coords, proj_matrix);
  vec3 ray = calculate_world_coords(eye_coords, view_matrix);
  return(ray);
}

inline internal vec3
get_camera_ray(r32* proj_matrix, r32* view_matrix) {
  vec2 normalized_device_coords = {0.0f, 0.0f};
  vec4 clip_coords = {normalized_device_coords.x, normalized_device_coords.y, -1.0f, 1.0f};
  vec4 eye_coords = calculate_eye_coords(clip_coords, proj_matrix);
  vec3 ray = calculate_world_coords(eye_coords, view_matrix);
  return(ray);
}
/*****GET MOUSE LOCATION*****/

inline internal bool
sphere_aabb(vec3 point, vec3 sphere, r32 radius) {
  r32 distance = (r32) sqrt((point.x - sphere.x) * (point.x - sphere.x) +
			 (point.y - sphere.y) * (point.y - sphere.y) +
			 (point.z - sphere.z) * (point.z - sphere.z));
  return(distance < radius);
}

inline internal bool
aabb(vec3 a_min, vec3 a_max, vec3 b_min, vec3 b_max) {
  return((a_min.x <= b_max.x && a_max.x >= b_min.x) &&
	 (a_min.y <= b_max.y && a_max.y >= b_min.y) &&
	 (a_min.z <= b_max.z && a_max.z >= b_min.z));
}

inline internal bool
vec3_aabb(vec3 min, vec3 max, vec3 point) {
  return((point.x >= min.x && point.x <= max.x) &&
	 (point.y >= min.y && point.y <= max.y) &&
	 (point.z >= min.z && point.z <= max.z));
}

inline internal bool
hit_cube(vec3 ray, vec3 camPos, vec3 vMin, vec3 vMax) {
  vec3 start = {camPos.x, camPos.y, camPos.z};
  vec3 scaledRay = {ray.x * 600.0f , ray.y * 600.0f, ray.z * 600.0f};
  vec3 rayDir = ray;
  r32 tmp;

  /*X */
  r32 txMin = (vMin.x - start.x) / rayDir.x;
  r32 txMax = (vMax.x - start.x) / rayDir.x;
  if (txMax < txMin) { tmp = txMax; txMax = txMin; txMin = tmp; }

  r32 tyMin = (vMin.y - start.y) / rayDir.y;
  r32 tyMax = (vMax.y - start.y) / rayDir.y;
  if (tyMax < tyMin) { tmp = tyMax; tyMax = tyMin; tyMin = tmp; }

  r32 tzMin = (vMin.z - start.z) / rayDir.z;
  r32 tzMax = (vMax.z - start.z) / rayDir.z;
  if (tzMax < tzMin) { tmp = tzMax; tzMax = tzMin; tzMin = tmp; }

  r32 tMin = (txMin > tyMin)? txMin : tyMin; //Get Greatest Min
  r32 tMax = (txMax < tyMax)? txMax : tyMax; //Get Smallest Max

  if (txMin > tyMax || tyMin > txMax) return false;
  if (tMin > tzMax || tzMin > tMax) return false;
  if(tzMin > tMin) tMin = tzMin;
  if(tzMax < tMax) tMax = tzMax;

  return true;
}

inline internal vec3
hit_plane(vec3 ray_direction, vec3 ray_origin, vec3 plane_normal, vec3 plane_origin) {
	vec3 diff = ray_origin - plane_origin;
        r32 prod1 = vec3_dot(diff, plane_normal);
	r32 prod2 = vec3_dot(ray_direction, plane_normal);
	r32 prod3 = prod1 / prod2;
	vec3 ray_x_prod = ray_direction * prod3;
	return(ray_origin - ray_x_prod);
}

inline internal r32
ray_to_point_distance(vec3 ray_origin, vec3 ray_direction, vec3 point) {
  vec3 l = point-ray_origin;
  r32 tca = vec3_dot(l, ray_direction);
  r32 distance = vec3_dot(l,l) - (tca * tca);
  return(distance);
}

inline internal b32
hit_sphere(vec3 center, r32 radius, vec3 ray_origin, vec3 ray_direction){
  r32 distance = ray_to_point_distance(ray_origin, ray_direction, center);
  return(distance < radius);
  /*vec3 l = center-ray_origin;
  r32 tca = vec3_dot(l, ray_direction);
  r32 d2 = vec3_dot(l,l) - (tca * tca);
  return(d2 < radius);*/
  /*
  r32 t = vec3_dot(center-ray_origin, ray_direction);
  vec3 rd_x_t = {ray_direction.x*t, ray_direction.y*t, ray_direction.z*t};
  vec3 p = ray_origin + rd_x_t;

  r32 y = vec3_length(center-p);
  return(y < radius);*/
}

inline internal bool
hit_triangle(vec3 ray_origin, vec3 ray_vector, vec3 vertex0, vec3 vertex1, vec3 vertex2, vec3* out_intersection_point) {
  vec3 edge1, edge2, h, s, q;
  r32 a, f, u, v;
  edge1 = vertex1 - vertex0;
  edge2 = vertex2 - vertex0;
            
  h = vec3_cross(ray_vector, edge2);
  a = vec3_dot(edge1, h);
  if (a > -EPSILON && a < EPSILON)
    return false;    // This ray is parallel to this triangle.
  f = 1.0f / a;
  s = ray_origin - vertex0;

  u = f * vec3_dot(s, h);
  if (u < 0.0f || u > 1.0f)
    return false;
            
  q = vec3_cross(s, edge1);
  v = f * vec3_dot(ray_vector, q);
  if (v < 0.0 || u + v > 1.0f)
    return false;
  // At this stage we can compute t to find out where the intersection point is on the line.
            
  r32 t = f * vec3_dot(edge2, q);
  if (t > EPSILON) { // ray intersection
    ray_vector.x *= t;
    ray_vector.y *= t;
    ray_vector.z *= t;
    (*out_intersection_point) = ray_origin + ray_vector;
    return true;
  } else {// This means that there is a line intersection but not a ray intersection.
    return false;
  }
}

inline internal r32
barry_centric(vec3 p1, vec3 p2, vec3 p3, vec2 pos) {
  float det = (p2.z - p3.z) * (p1.x - p3.x) + (p3.x - p2.x) * (p1.z - p3.z);
  float l1 = ((p2.z - p3.z) * (pos.x - p3.x) + (p3.x - p2.x) * (pos.y - p3.z)) / det;
  float l2 = ((p3.z - p1.z) * (pos.x - p3.x) + (p1.x - p3.x) * (pos.y - p3.z)) / det;
  float l3 = 1.0f - l1 - l2;
  return l1 * p1.y + l2 * p2.y + l3 * p3.y;
}

inline internal r32
angle_between_lines(vec2 p1, vec2 p2, vec2 p3, vec2 p4) {
  r32 a = p1.x - p2.x;
  r32 b = p1.y - p2.y;
  r32 c = p3.x - p4.x;
  r32 d = p3.y - p4.y;

  r32 cos_angle , angle;
  r32 mag_v1 = (r32) sqrt(a*a + b*b);
  r32 mag_v2 = (r32) sqrt(c*c + d*d);

  cos_angle = (a*c + b*d) / (mag_v1 * mag_v2);
  angle = (r32) acos(cos_angle);
  
  return angle;
}

inline internal r32
angle_vec2(vec2 point, vec2 target) {
  return((r32) atan2(target.y - point.y, target.x - point.x));
}

inline internal r32
angle_between_vec2(vec2 v0, vec2 v1) {
  r32 dot_value = v0.x*v1.x + v0.y*v1.y;
  r32 det_value = v0.x*v1.y - v0.y*v1.x;
  r32 result = (r32) atan2(det_value, dot_value);
  return(result);
}

inline internal r32
angle_between_vec3(vec3 v0, vec3 v1) {
  r32 dot_value = v0.x*v1.x + v0.y*v1.y + v0.z*v1.z;
  r32 len_sq1   = v0.x*v0.x + v0.y*v0.y + v0.z*v0.z;
  r32 len_sq2   = v1.x*v1.x + v1.y*v1.y + v1.z*v1.z;

  r32 result = (r32) acos(dot_value/ (r32) sqrt(len_sq1 * len_sq2));
  return(result);
}

inline internal vec3
center(vec3 a, vec3 b) {
  vec3 start = {b.x - a.x, b.y - a.y, b.z - a.z};
  return(a + (start * 0.5f));
}

inline internal r32
distance(int x1, int y1, int x2, int y2) { 
  r32 result = (r32) sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2) * 1.0);
  return(result); 
}

inline internal r32
distance(vec2 v0, vec2 v1) {
  r32 result = (r32) (sqrt(pow(v1.x - v0.x, 2) + pow(v1.y - v0.y, 2) * 1.0));
  return(result); 
}

inline internal r32
distance(vec3 v0, vec3 v1) {
  r32 result = (r32) sqrt(pow(v1.x - v0.x, 2) +
			  pow(v1.y - v0.y, 2) +
			  pow(v1.z - v0.z, 2) * 1.0);
  return(result);
}

inline internal r32
round_by(r32 value, r32 factor) {
  return((r32) round(value / factor) * factor);  
}

inline internal r32
floor_by(r32 value, r32 factor) {
  return((r32) floor(value / factor) * factor);  
}

inline internal r32
ceil_by(r32 value, r32 factor) {
  return((r32) ceil(value / factor) * factor);  
}

inline internal vec2
round_vec2_by(vec2 v, r32 factor) {
  return(vec2{round_by(v.x, factor), round_by(v.y, factor)});  
}

inline internal bool
point_on_area(vec2 point, vec2 v0, vec2 v1) {
  if((v0.x < point.x && v1.x > point.x) && (v0.y < point.y && v1.y > point.y)) {
    return(true);
  } else {
    return(false);
  }
}

// @temporary
inline internal r32
calculate_spherical_cov(r32 distance, r32 nugget, r32 sill, r32 range) {  
  r32 result = 0.0f;

  if(distance == 0.0f) {
    result = 0.0f;
  } else if(distance <= range) {
    result = nugget + (sill - nugget) * ((1.5f * (distance / range) - 0.5f * ((r32) pow(distance / range, 3.0f))));
  } else {
    result = sill;
  }
  
  return(result);
}


inline internal r32
angle_between_points(vec2 a, vec2 b) {
  r32 result = (r32) atan2(b.x - a.x, b.y - a.y);
  return(result);
}

internal u32
get_polygon(vec2* out, vec2* point_array, u32 n) {
  // @cleanup
  u32 count = 0;
  
  u32 init_id = 0;
  for(u32 i = 1; i < n; i++) {
    if(point_array[i].x < point_array[init_id].x) {
      init_id = i;
    }
  }
  out[count++] = point_array[init_id];

  for(u32 i = 0; i < n; i++) {
    s32 curr = 0;
    b32 add_point = false;
    
    for(u32 j = 0; j < n; j++) {
      if(point_array[j].x > out[count-1].x) {
	if(!add_point || angle_between_points(point_array[curr], out[count-1]) < angle_between_points(point_array[j],    out[count-1])) {
	  add_point = true;
	  curr = j;
	}
      }
    }
    if(add_point) {
      out[count++] = point_array[curr];
      i = 0;
    }
  }
  
  for(u32 i = 0; i < n; i++) {
    s32 curr = 0;
    b32 add_point = false;
    
    for(u32 j = 0; j < n; j++) {
      if(point_array[j].y > out[count-1].y) {
	if(!add_point || angle_between_points(point_array[curr], out[count-1]) < angle_between_points(point_array[j],    out[count-1])) {
	  add_point = true;
	  curr = j;
	}
      }
    }
    if(add_point) {
      out[count++] = point_array[curr];
      i = 0;
    }
  }
  
  for(u32 i = 0; i < n; i++) {
    s32 curr = 0;
    b32 add_point = false;
    
    for(u32 j = 0; j < n; j++) {
      if(point_array[j].x < out[count-1].x) {
	if(!add_point || angle_between_points(point_array[curr], out[count-1]) < angle_between_points(point_array[j],    out[count-1])) {
	  add_point = true;
	  curr = j;
	}
      }
    }
    if(add_point) {
      out[count++] = point_array[curr];
      i = 0;
    }
  }
  
  return(count);
}

inline internal r32
distance_point_to_line(vec2 a, vec2 b, vec2 e)
{
  // vector AB
  vec2 a_b;
  a_b.x = b.x - a.x;
  a_b.y = b.y - a.y;
 
  // vector BP
  vec2 b_e;
  b_e.x = e.x - b.x;
  b_e.y = e.y - b.y;
 
  // vector AP
  vec2 a_e;
  a_e.x = e.x - a.x;
  a_e.y = e.y - a.y;
 
  // Calculating the dot product
  r32 ab_be = vec2_dot(a_b, b_e);
  r32 ab_ae = vec2_dot(a_b, a_e);

  r32 result;
 
  if (ab_be > 0) {
    r32 x = e.x - b.x;
    r32 y = e.y - b.y;
    result = (r32) sqrt(x * x + y * y);
  } else if (ab_ae < 0) {
    r32 x = e.x - a.x;
    r32 y = e.y - a.y;
    result = (r32) sqrt(x * x + y * y);
  } else {
 
    r32 x1 = a_b.x;
    r32 y1 = a_b.y;
    r32 x2 = a_e.x;
    r32 y2 = a_e.y;
    r32 mod = (r32) sqrt(x1 * x1 + y1 * y1);
    result = (r32) fabs(x1 * y2 - y1 * x2) / mod;
  }
  return(result);
}

internal r32
distance_point_to_polygon(vec2 p, vec2* polygon, u32 n) {
  r32 distance = distance_point_to_line(polygon[0], polygon[1], p);
  for(u32 i = 1; i < n-1; i++) {
    r32 new_distance = distance_point_to_line(polygon[i], polygon[i+1], p);
    if(distance > new_distance) {
      distance = new_distance;
    }
  }
  return(distance);
}

internal b32
point_on_polygon(vec2 p, vec2* polygon, s32 n) {
  // @cleanup
  // source: https://github.com/ksanderRND/PointInPolygon/blob/main/pointInPolygon.cpp
  //
  b32 result = false;
  s32 j = n - 1;
    
  for(s32 i = 0; i < n; i++) {
    if(polygon[i].x == p.x && polygon[i].y == p.y) {
      return true;
    }

    if( (fabs((polygon[j].x-polygon[i].x)*(p.y-polygon[i].y) - (p.x-polygon[i].x)*(polygon[j].y-polygon[i].y)) <= EPSILON) &&
       ((p.x<=polygon[i].x && p.y<=polygon[i].y && p.x>=polygon[j].x && p.y>=polygon[j].y) || (p.x<=polygon[j].x && p.y<=polygon[j].y && p.x>=polygon[i].x && p.y>=polygon[i].y) ) ){
      return true;
    }
	 
    if( (polygon[i].y < p.y && polygon[j].y >= p.y || polygon[j].y < p.y && polygon[i].y >= p.y) &&
	(polygon[i].x + (p.y - polygon[i].y) / (polygon[j].y - polygon[i].y) * (polygon[j].x - polygon[i].x) < p.x) ) {
      result = !result; 
    }
    j = i;
  }
    
  return result;
}

// https://en.wikipedia.org/wiki/LU_decomposition#C_code_examples
/* INPUT: A - array of pointers to rows of a square matrix having dimension N
 *        Tol - small tolerance number to detect failure when the matrix is near degenerate
 * OUTPUT: Matrix A is changed, it contains a copy of both matrices L-E and U as A=(L-E)+U such that P*A=L*U.
 *        The permutation matrix is not stored as a matrix, but in an integer vector P of size N+1 
 *        containing column indexes where the permutation matrix has "1". The last element P[N]=S+N, 
 *        where S is the number of row exchanges needed for determinant computation, det(P)=(-1)^S    
 */
internal void
lup_decompose(r32 **A, s32 N, r32 Tol, s32 *P) {
  s32 i, j, k, imax; 
  r32 maxA, *ptr, absA;

  for (i = 0; i <= N; i++)
    P[i] = i; //Unit permutation matrix, P[N] initialized with N

  for (i = 0; i < N; i++) {
    maxA = 0.0;
    imax = i;

    for (k = i; k < N; k++)
      if ((absA = (r32) fabs(A[k][i])) > maxA) { 
	maxA = absA;
	imax = k;
      }

    // if (maxA < Tol) return 0; //failure, matrix is degenerate

    if (imax != i) {
      //pivoting P
      j = P[i];
      P[i] = P[imax];
      P[imax] = j;

      //pivoting rows of A
      ptr = A[i];
      A[i] = A[imax];
      A[imax] = ptr;

      //counting pivots starting from N (for determinant)
      P[N]++;
    }

    for (j = i + 1; j < N; j++) {
      A[j][i] /= A[i][i];

      for (k = i + 1; k < N; k++)
	A[j][k] -= A[j][i] * A[i][k];
    }
  }
}

/* INPUT: A,P filled in LUPDecompose; N - dimension
 * OUTPUT: IA is the inverse of the initial matrix
 */
internal void
lup_invert(r32 **A, s32 *P, s32 N, r32 **IA) {  
  for(s32 j = 0; j < N; j++) {
    for(s32 i = 0; i < N; i++) {
      IA[i][j] = P[i] == j ? 1.0f : 0.0f;

      for(s32 k = 0; k < i; k++)
	IA[i][j] -= A[i][k] * IA[k][j];
    }

    for(s32 i = N - 1; i >= 0; i--) {
      for(s32 k = i + 1; k < N; k++)
	IA[i][j] -= A[i][k] * IA[k][j];

      IA[i][j] /= A[i][i];
    }
  }
}

internal void
mat_inverse(r32* out, r32* in, s32 N, temporary_pool* pool) {
  s32 permutation_N = N + 1; 
  s32* permutation_array = static_cast<s32*>(temporary_alloc(pool, sizeof(s32) * permutation_N));

  r32** matrix_A = static_cast<r32**>(temporary_alloc(pool, sizeof(r32*) * N));
  for(s32 i = 0; i < N; i++) {
    matrix_A[i] = static_cast<r32*>(temporary_alloc(pool, sizeof(r32) * N));
  }
  for(s32 j = 0; j < N; j++) {
    for(s32 i = 0; i < N; i++) { 
      matrix_A[i][j] = in[i + j * N];
    }
  }
  
  lup_decompose(matrix_A, N, 0.0f, permutation_array);
  
  r32** matrix_IA = static_cast<r32**>(temporary_alloc(pool, sizeof(r32*) * N));
  for(s32 i = 0; i < N; i++) {
    matrix_IA[i] = static_cast<r32*>(temporary_alloc(pool, sizeof(r32) * N));
  }
  lup_invert(matrix_A, permutation_array, N, matrix_IA);

  for(s32 j = 0; j < N; j++) {
    for(s32 i = 0; i < N; i++) { 
      out[i + j * N] = matrix_IA[i][j];
    }
  }
}


#endif // !MATHS_H
