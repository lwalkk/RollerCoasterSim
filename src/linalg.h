// linalg.h


#ifndef LINALG_H
#define LINALG_H


#include <iostream>
#include <cmath>

#ifdef _WIN32
  #pragma warning(disable : 4244 4305 4996)
#endif


class mat4;
class vec4;



// ---------------- vec2 ----------------


class vec2 {
public:

  float x, y;

  vec2() {}

  vec2( float xx, float yy )
    { x = xx; y = yy; }

  bool operator == (const vec2 p) {
    return x == p.x && y == p.y;
  }

  bool operator != (const vec2 p) {
    return x != p.x || y != p.y; 
  }

  vec2 operator + (vec2 p)
    { return vec2( x+p.x, y+p.y ); }

  vec2 operator - (vec2 p)
    { return vec2( x-p.x, y-p.y ); }

  float operator * (vec2 p)     /* dot product */
    { return x * p.x + y * p.y; }

  vec2 normalize() {
    float len;
    len = sqrt( x*x + y*y );
    return vec2( x/len, y/len );
  }

  float length()
    { return sqrt( x*x + y*y ); }

  float squaredLength()
  { return x*x + y*y; }

  float & operator[]( unsigned int index ) {
    return (&x)[index];
  }
};

// Scalar/vec3 multiplication

vec2 operator * ( float k, vec2 const& p );

// I/O operators

std::ostream& operator << ( std::ostream& stream, vec2 const& p );
std::istream& operator >> ( std::istream& stream, vec2 & p );


// ---------------- vec3 ----------------


class vec3 {
public:

  float x, y, z;

  vec3() {}

  vec3( float xx, float yy, float zz )
    { x = xx; y = yy; z = zz; }

  vec3( float *v )
    { x = v[0]; y = v[1]; z = v[2]; }

  bool operator == (const vec3 p) const {
    return x == p.x && y == p.y && z == p.z;
  }

  bool operator != (const vec3 p) const {
    return x != p.x || y != p.y || z != p.z; 
  }

  vec3 operator + (vec3 p) const
    { return vec3( x+p.x, y+p.y, z+p.z ); }

  vec3 operator - (vec3 p) const
    { return vec3( x-p.x, y-p.y, z-p.z ); }

  float operator * (vec3 p) const /* dot product */
    { return x * p.x + y * p.y + z * p.z; }

  vec3 operator ^ (vec3 p) const /* cross product */
    { return vec3( y*p.z-p.y*z, -(x*p.z-p.x*z), x*p.y-p.x*y ); }

  vec3 operator % (vec3 p) const /* component-wise product */
    { return vec3( x*p.x, y*p.y, z*p.z ); }

  vec3 normalize() const {
    float len;
    len = sqrt( x*x + y*y + z*z );
    return vec3( x/len, y/len, z/len );
  }

  float length() const
    { return sqrt( x*x + y*y + z*z ); }

  float squaredLength() const
    { return x*x + y*y + z*z; }

  float & operator[]( unsigned int index ) {
    return (&x)[index];
  }
  
  float distanceToLine( vec3 lineStart, vec3 lineDir );

  vec3 perp1();
  vec3 perp2();
};


mat4 identity4();


// Scalar/vec3 multiplication

vec3 operator * ( float k, vec3 const& p );


// I/O operators

std::ostream& operator << ( std::ostream& stream, vec3 const& p );
std::istream& operator >> ( std::istream& stream, vec3 & p );



// ---------------- vec4 ----------------



class vec4 {
public:

  float x, y, z, w;
  vec4(){}

  vec4( const float (&arr)[4] ) 
  {
      x = arr[0]; y = arr[1]; z = arr[2]; w = arr[3];
  }

  vec4( float xx, float yy, float zz, float ww )
    { x = xx; y = yy; z = zz; w = ww; }

  vec4( vec3 v, float ww )
    { x = v.x; y = v.y; z = v.z; w = ww; }

  vec4( vec3 v )
    { x = v.x; y = v.y; z = v.z; w = 1; }

  bool operator == (const vec4 p) const
  { return x == p.x && y == p.y && z == p.z && w == p.w; }

  bool operator != (const vec4 p) const
  { return x != p.x || y != p.y || z != p.z || w != p.w; }

  vec4 operator + (vec4 p) const
  { return vec4( x+p.x, y+p.y, z+p.z, w+p.w ); }

  vec4 operator - (vec4 p) const
  { return vec4( x-p.x, y-p.y, z-p.z, w-p.w ); }

  float operator * (vec4 const &p) const
    { return x * p.x + y * p.y + z * p.z + w * p.w; }

  vec4 normalize() const {
    float len;
    len = sqrt( x*x + y*y + z*z + w*w );
    return vec4( x/len, y/len, z/len, w/len );
  }

  vec3 toVec3() const {
    if (w != 0)
      return vec3( x/w, y/w, z/w );
    else
      return vec3( x, y, z );   
  }

  float length() const
  { return sqrt( x*x + y*y + z*z + w*w ); }

  float squaredLength() const
  { return x*x + y*y + z*z + w*w; }

  float & operator[]( unsigned int index ) {
    return (&x)[index];
  }
  
};

// Scalar/vec4 multiplication

vec4 operator * ( float k, vec4 const& p );

// I/O operators

std::ostream& operator << ( std::ostream& stream, vec4 const& p );
std::istream& operator >> ( std::istream& stream, vec4 & p );


// ---------------- quaternions ----------------


class quaternion {
 public:

  vec4 q;                       // w = cos(theta/2), (x,y,z) = sin(theta/w) * axis

  quaternion() {};

  quaternion( float ww, float xx, float yy, float zz ) // NOTE THE ORDER: w,x,y,z = q0,q1,q2,q3
    { q.x = xx; q.y = yy; q.z = zz; q.w = ww; }

  quaternion( float angle, const vec3 axis ) {
    vec3 n = sin(angle/2.0) * axis.normalize();
    q.w = cos(angle/2.0);
    q.x = n.x;
    q.y = n.y;
    q.z = n.z;
  }

  bool operator == ( const quaternion q2 ) const
  { return q == q2.q; }

  bool operator != ( const quaternion q2 ) const
  { return q != q2.q; }

  float angle() const
  { return 2 * acos(q.w); }

  vec3 axis() const
  { return vec3( q.x, q.y, q.z ); }

  quaternion normalize() {
    float len = q.length();
    return quaternion( q.w/len, q.x/len, q.y/len, q.z/len );
  }

  mat4 toMatrix() const;
};


#define Z_AXIS quaternion( cos(0.5), sin(0.5) * vec3(0,0,1) ) // rotation of 1 radian about z axis


// operators

quaternion operator * ( float k, quaternion const& q );
quaternion operator * ( quaternion const& q1, quaternion const& q2 );
vec3 operator * ( quaternion const& q, vec3 const& v );

// I/O operators

std::ostream& operator << ( std::ostream& stream, quaternion const& q );
std::istream& operator >> ( std::istream& stream, quaternion & q );


// ---------------- mat3 ----------------


class mat3 {

public:
  
  vec3 rows[3];

  mat3() {}

  vec3 & operator[]( unsigned int index ) const {
    return ((vec3*)(&rows[0]))[index];
  }
};


// operations

mat3 operator * (       float k, mat3 const& m );
vec3 operator * ( mat3 const& m, vec3 const& v );
mat3 operator * ( mat3 const& m, mat3 const& n );

// I/O operators

std::ostream& operator << ( std::ostream& stream, mat3 const& m );
std::istream& operator >> ( std::istream& stream, mat3 & m );


// ---------------- mat4 ----------------


class mat4 {

public:
  
  vec4 rows[4];

  mat4() {}

  float *data() {
    return & rows[0][0];
  }

  vec4 & operator[]( unsigned int index ) const {
    return ((vec4*)(&rows[0]))[index];
  }

  mat4 inverse();
};


// I/O operators

std::ostream& operator << ( std::ostream& stream, mat4 const& m );
std::istream& operator >> ( std::istream& stream, mat4 & m );


// operations

mat4 operator * (       float k, mat4 const& m );
vec4 operator * ( mat4 const& m, vec4 const& v );
mat4 operator * ( mat4 const& m, mat4 const& n );

mat4 identity4();

mat4 scale( float x, float y, float z );
mat4 translate( float x, float y, float z );
mat4 translate( vec3 v );
mat4 rotate( float theta, vec3 axis );

mat4 frustum( float l, float r, float b, float t, float n, float f );
mat4 ortho( float l, float r, float b, float t, float n, float f );
mat4 perspective( float fovy, float aspect, float n, float f );

// I/O operators

std::ostream& operator << ( std::ostream& stream, mat4 const& m );
std::istream& operator >> ( std::istream& stream, mat4 & m );

#endif
