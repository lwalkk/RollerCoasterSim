// linalg.cpp


#include "linalg.h"


// ---------------- vec2 ----------------

// I/O operators

std::ostream& operator << ( std::ostream& stream, vec2 const& p )

{
  stream << p.x << " " << p.y;
  return stream;
}

std::istream& operator >> ( std::istream& stream, vec2 & p )

{
  stream >> p.x >> p.y;
  return stream;
}



// ---------------- vec3 ----------------


vec3 operator * ( float k, vec3 const& p )

{
  vec3 q;

  q.x = p.x * k;
  q.y = p.y * k;
  q.z = p.z * k;

  return q;
}


// point-to-line distance

float vec3::distanceToLine( vec3 lineStart, vec3 lineDir )

{
  lineDir = lineDir.normalize();
  vec3 toPoint = *this - lineStart;
  return (toPoint - (toPoint * lineDir)*lineDir).length();
}



vec3 vec3::perp1()

{
  vec3 result(0,0,0);

  if (x == 0)
    if (y == 0 || z == 0)
      result.x = 1;     /* v = (0 0 z) or (0 y 0) */
    else {
      result.y = -z; /* v = (0 y z) */
      result.z = y;
    }
  else if (y == 0)
    if (z == 0)
      result.z = 1;     /* v = (x 0 0) */
    else {
      result.x = -z; /* v = (x 0 z) */
      result.z = x;
    }
  else {
    result.x = -y;   /* v = (x y z) or (x y 0) */
    result.y = x;
  }

  float lenRecip = 1.0 / sqrt( result.x*result.x + result.y*result.y + result.z*result.z );
  result.x *= lenRecip;
  result.y *= lenRecip;
  result.z *= lenRecip;

  return result;
}


vec3 vec3::perp2()

{
  vec3 result(0,0,0);

  if (x == 0)
    if (y == 0)
      result.y = 1;
    else if (z == 0)
      result.z = 1;
    else
      result.x = 1;
  else
    if (y == 0)
      result.y = 1;
    else if (z == 0)
      result.z = 1;
    else {
      result.x = x * z;
      result.y = y * z;
      result.z = - x*x - y*y;

      float lenRecip = 1.0 / sqrt( result.x*result.x + result.y*result.y + result.z*result.z );
      result.x *= lenRecip;
      result.y *= lenRecip;
      result.z *= lenRecip;
    }

  return result;
}


// I/O operators

std::ostream& operator << ( std::ostream& stream, vec3 const& p )

{
  stream << p.x << " " << p.y << " " << p.z;
  return stream;
}

std::istream& operator >> ( std::istream& stream, vec3 & p )

{
  stream >> p.x >> p.y >> p.z;
  return stream;
}


// ---------------- vec4 ----------------


vec4 operator * ( float k, vec4 const& p )

{
  vec4 q;

  q.x = p.x * k;
  q.y = p.y * k;
  q.z = p.z * k;
  q.w = p.w * k;

  return q;
}

// I/O operators

std::ostream& operator << ( std::ostream& stream, vec4 const& p )

{
  stream << p.x << " " << p.y << " " << p.z << " " << p.w;
  return stream;
}

std::istream& operator >> ( std::istream& stream, vec4 & p )

{
  stream >> p.x >> p.y >> p.z >> p.w;
  return stream;
}


// ---------------- quaternion ----------------


mat4 quaternion::toMatrix() const

{
  mat4 m;

  m.rows[0] = vec4( 2 * (q.w*q.w + q.x*q.x - .5),
                    2 * (q.x*q.y - q.w*q.z),
                    2 * (q.x*q.z + q.w*q.y),
                    0 );
  m.rows[1] = vec4( 2 * (q.x*q.y + q.w*q.z),
                    2 * (q.w*q.w + q.y*q.y - .5),
                    2 * (q.y*q.z - q.w*q.x),
                    0 );
  m.rows[2] = vec4( 2 * (q.x*q.z - q.w*q.y),
                    2 * (q.y*q.z + q.w*q.x),
                    2 * (q.w*q.w + q.z*q.z - .5),
                    0 );
  m.rows[3] = vec4( 0, 0, 0, 1 );

  return m;
}

quaternion operator * ( quaternion const& q1, quaternion const& q2 )

{
  quaternion result;

  result.q.w = q1.q.w*q2.q.w - q1.q.x*q2.q.x - q1.q.y*q2.q.y - q1.q.z*q2.q.z;
  result.q.x = q1.q.w*q2.q.x + q1.q.x*q2.q.w + q1.q.y*q2.q.z - q1.q.z*q2.q.y;
  result.q.y = q1.q.w*q2.q.y + q1.q.y*q2.q.w + q1.q.x*q2.q.z - q1.q.z*q2.q.x;
  result.q.z = q1.q.w*q2.q.z + q1.q.z*q2.q.w + q1.q.x*q2.q.y - q1.q.y*q2.q.x;

  return result;
}

vec3 operator * ( quaternion const& q, vec3 const& v )

{
  vec4 result = q.toMatrix() * vec4( v, 1 );
  return vec3( result.x, result.y, result.z );
}

quaternion operator * ( float k, quaternion const& q )

{
  float angle = k * q.angle();
  vec3 axis = q.axis();

  return quaternion( cos(angle/2.0), sin(angle/2.0) * axis );
}

// I/O operators

std::ostream& operator << ( std::ostream& stream, quaternion const& q )

{
  stream << q.q.w << " " << q.q.x << " " << q.q.y << " " << q.q.z;
  return stream;
}

std::istream& operator >> ( std::istream& stream, quaternion & q )

{
  stream >> q.q.w >> q.q.x >> q.q.y >> q.q.z;
  return stream;
}




// ---------------- mat3 ----------------


mat3 operator * ( float k, mat3 const& m )

{
  mat3 out;

  out.rows[0] = k * m.rows[0];
  out.rows[1] = k * m.rows[1];
  out.rows[2] = k * m.rows[2];

  return out;
}

vec3 operator * ( mat3 const& m, vec3 const& v )

{
  vec3 out;

  out[0] = m.rows[0] * v;
  out[1] = m.rows[1] * v;
  out[2] = m.rows[2] * v;

  return out;
}

mat3 operator * ( mat3 const& m, mat3 const& n )

{
  mat3 out;

  for (int i=0; i<3; i++)
    for (int j=0; j<3; j++) {

      float sum=0;

      for (int k=0; k<3; k++)
        sum += m[i][k] * n[k][j];

      out[i][j] = sum;
    }

  return out;
}

// I/O operators

std::ostream& operator << ( std::ostream& stream, mat3 const& m )

{
  for (int i=0; i<3; i++) {
    for (int j=0; j<3; j++)
      stream << (j>0 ? " " : "") << m[i][j];
    stream << std::endl;
  }

  return stream;
}

std::istream& operator >> ( std::istream& stream, mat3 & m )

{
  for (int i=0; i<3; i++)
    for (int j=0; j<3; j++)
      stream >> m[i][j];

  return stream;
}


// ---------------- mat4 ----------------


mat4 operator * ( float k, mat4 const& m )

{
  mat4 out;

  out.rows[0] = k * m.rows[0];
  out.rows[1] = k * m.rows[1];
  out.rows[2] = k * m.rows[2];
  out.rows[3] = k * m.rows[3];

  return out;
}

vec4 operator * ( mat4 const& m, vec4 const& v )

{
  vec4 out;

  out[0] = m.rows[0] * v;
  out[1] = m.rows[1] * v;
  out[2] = m.rows[2] * v;
  out[3] = m.rows[3] * v;

  return out;
}

mat4 operator * ( mat4 const& m, mat4 const& n )

{
  mat4 out;

  for (int i=0; i<4; i++)
    for (int j=0; j<4; j++) {

      float sum=0;

      for (int k=0; k<4; k++)
        sum += m[i][k] * n[k][j];

      out[i][j] = sum;
    }

  return out;
}

mat4 scale( float x, float y, float z )

{
  mat4 out;

  out.rows[0] = vec4( x, 0, 0, 0 );
  out.rows[1] = vec4( 0, y, 0, 0 );
  out.rows[2] = vec4( 0, 0, z, 0 );
  out.rows[3] = vec4( 0, 0, 0, 1 );

  return out;
}

mat4 translate( float x, float y, float z )

{
  mat4 out;

  out.rows[0] = vec4( 1, 0, 0, x );
  out.rows[1] = vec4( 0, 1, 0, y );
  out.rows[2] = vec4( 0, 0, 1, z );
  out.rows[3] = vec4( 0, 0, 0, 1 );

  return out;
}


mat4 translate( vec3 v )

{
  mat4 out;

  out.rows[0] = vec4( 1, 0, 0, v.x );
  out.rows[1] = vec4( 0, 1, 0, v.y );
  out.rows[2] = vec4( 0, 0, 1, v.z );
  out.rows[3] = vec4( 0, 0, 0,   1 );

  return out;
}


mat4 rotate( float theta, vec3 axis )

{
  axis = axis.normalize();

  float v1 = axis.x;
  float v2 = axis.y;
  float v3 = axis.z;
  
  float t1 =  cos(theta);
  float t2 =  1 - t1;
  float t3 =  v1*v1;
  float t6 =  t2*v1;
  float t7 =  t6*v2;
  float t8 =  sin(theta);
  float t9 =  t8*v3;
  float t11 = t6*v3;
  float t12 = t8*v2;
  float t15 = v2*v2;
  float t19 = t2*v2*v3;
  float t20 = t8*v1;
  float t24 = v3*v3;

  mat4 out;

  out.rows[0] = vec4( t1 + t2*t3,     t7 - t9,   t11 + t12, 0 );
  out.rows[1] = vec4(    t7 + t9, t1 + t2*t15,   t19 - t20, 0 );
  out.rows[2] = vec4(  t11 - t12,   t19 + t20, t1 + t2*t24, 0 );
  out.rows[3] = vec4(          0,           0,           0, 1 );

  return out;
}


mat4 identity4()

{
  mat4 M;

  for (int i=0; i<4; i++)
    for (int j=0; j<4; j++)
      if (i==j)
        M[i][j] = 1;
      else
        M[i][j] = 0;

  return M;
}


mat4 frustum( float l, float r, float b, float t, float n, float f )

{
  mat4 out;

  out.rows[0] = vec4( 2*n/(r-l),         0, (r+l)/(r-l),           0 );
  out.rows[1] = vec4(         0, 2*n/(t-b), (t+b)/(t-b),           0 );
  out.rows[2] = vec4(         0,         0, (f+n)/(n-f), 2*f*n/(n-f) );
  out.rows[3] = vec4(         0,         0,          -1,           0 );

  return out;
}


mat4 perspective( float fovy, float aspect, float n, float f )

{
  mat4 out;

  float s = 1 / atan2( fovy, 2.0 );

  out.rows[0] = vec4( s/aspect,          0,           0,           0 );
  out.rows[1] = vec4(         0,         s,           0,           0 );
  out.rows[2] = vec4(         0,         0, (f+n)/(n-f), 2*f*n/(n-f) );
  out.rows[3] = vec4(         0,         0,          -1,           0 );

  return out;
}
    

mat4 ortho( float l, float r, float b, float t, float n, float f )

{
  mat4 out;

  out.rows[0] = vec4( 2/(r-l),       0,       0, (l+r)/(l-r) );
  out.rows[1] = vec4(       0, 2/(t-b),       0, (b+t)/(b-t) );
  out.rows[2] = vec4(       0,       0, 2/(n-f), (n+f)/(n-f) );
  out.rows[3] = vec4(       0,       0,       0,           1 );

  return out;
}
    

// I/O operators

std::ostream& operator << ( std::ostream& stream, mat4 const& m )

{
  for (int i=0; i<4; i++) {
    for (int j=0; j<4; j++)
      stream << (j>0 ? " " : "") << m[i][j];
    stream << std::endl;
  }

  return stream;
}

std::istream& operator >> ( std::istream& stream, mat4 & m )

{
  for (int i=0; i<4; i++)
    for (int j=0; j<4; j++)
      stream >> m[i][j];

  return stream;
}


// 4x4 inverse
//
// Code from the Mesa OpenGL library

mat4 mat4::inverse()

{
  float m[16], invOut[16];

  m[ 0] = rows[0].x;
  m[ 1] = rows[1].x;
  m[ 2] = rows[2].x;
  m[ 3] = rows[3].x;
  m[ 4] = rows[0].y;
  m[ 5] = rows[1].y;
  m[ 6] = rows[2].y;
  m[ 7] = rows[3].y;
  m[ 8] = rows[0].z;
  m[ 9] = rows[1].z;
  m[10] = rows[2].z;
  m[11] = rows[3].z;
  m[12] = rows[0].w;
  m[13] = rows[1].w;
  m[14] = rows[2].w;
  m[15] = rows[3].w;
    
  float inv[16], det;
  int i;

  inv[0] = m[5]  * m[10] * m[15] - 
    m[5]  * m[11] * m[14] - 
    m[9]  * m[6]  * m[15] + 
    m[9]  * m[7]  * m[14] +
    m[13] * m[6]  * m[11] - 
    m[13] * m[7]  * m[10];

  inv[4] = -m[4]  * m[10] * m[15] + 
    m[4]  * m[11] * m[14] + 
    m[8]  * m[6]  * m[15] - 
    m[8]  * m[7]  * m[14] - 
    m[12] * m[6]  * m[11] + 
    m[12] * m[7]  * m[10];

  inv[8] = m[4]  * m[9] * m[15] - 
    m[4]  * m[11] * m[13] - 
    m[8]  * m[5] * m[15] + 
    m[8]  * m[7] * m[13] + 
    m[12] * m[5] * m[11] - 
    m[12] * m[7] * m[9];

  inv[12] = -m[4]  * m[9] * m[14] + 
    m[4]  * m[10] * m[13] +
    m[8]  * m[5] * m[14] - 
    m[8]  * m[6] * m[13] - 
    m[12] * m[5] * m[10] + 
    m[12] * m[6] * m[9];

  inv[1] = -m[1]  * m[10] * m[15] + 
    m[1]  * m[11] * m[14] + 
    m[9]  * m[2] * m[15] - 
    m[9]  * m[3] * m[14] - 
    m[13] * m[2] * m[11] + 
    m[13] * m[3] * m[10];

  inv[5] = m[0]  * m[10] * m[15] - 
    m[0]  * m[11] * m[14] - 
    m[8]  * m[2] * m[15] + 
    m[8]  * m[3] * m[14] + 
    m[12] * m[2] * m[11] - 
    m[12] * m[3] * m[10];

  inv[9] = -m[0]  * m[9] * m[15] + 
    m[0]  * m[11] * m[13] + 
    m[8]  * m[1] * m[15] - 
    m[8]  * m[3] * m[13] - 
    m[12] * m[1] * m[11] + 
    m[12] * m[3] * m[9];

  inv[13] = m[0]  * m[9] * m[14] - 
    m[0]  * m[10] * m[13] - 
    m[8]  * m[1] * m[14] + 
    m[8]  * m[2] * m[13] + 
    m[12] * m[1] * m[10] - 
    m[12] * m[2] * m[9];

  inv[2] = m[1]  * m[6] * m[15] - 
    m[1]  * m[7] * m[14] - 
    m[5]  * m[2] * m[15] + 
    m[5]  * m[3] * m[14] + 
    m[13] * m[2] * m[7] - 
    m[13] * m[3] * m[6];

  inv[6] = -m[0]  * m[6] * m[15] + 
    m[0]  * m[7] * m[14] + 
    m[4]  * m[2] * m[15] - 
    m[4]  * m[3] * m[14] - 
    m[12] * m[2] * m[7] + 
    m[12] * m[3] * m[6];

  inv[10] = m[0]  * m[5] * m[15] - 
    m[0]  * m[7] * m[13] - 
    m[4]  * m[1] * m[15] + 
    m[4]  * m[3] * m[13] + 
    m[12] * m[1] * m[7] - 
    m[12] * m[3] * m[5];

  inv[14] = -m[0]  * m[5] * m[14] + 
    m[0]  * m[6] * m[13] + 
    m[4]  * m[1] * m[14] - 
    m[4]  * m[2] * m[13] - 
    m[12] * m[1] * m[6] + 
    m[12] * m[2] * m[5];

  inv[3] = -m[1] * m[6] * m[11] + 
    m[1] * m[7] * m[10] + 
    m[5] * m[2] * m[11] - 
    m[5] * m[3] * m[10] - 
    m[9] * m[2] * m[7] + 
    m[9] * m[3] * m[6];

  inv[7] = m[0] * m[6] * m[11] - 
    m[0] * m[7] * m[10] - 
    m[4] * m[2] * m[11] + 
    m[4] * m[3] * m[10] + 
    m[8] * m[2] * m[7] - 
    m[8] * m[3] * m[6];

  inv[11] = -m[0] * m[5] * m[11] + 
    m[0] * m[7] * m[9] + 
    m[4] * m[1] * m[11] - 
    m[4] * m[3] * m[9] - 
    m[8] * m[1] * m[7] + 
    m[8] * m[3] * m[5];

  inv[15] = m[0] * m[5] * m[10] - 
    m[0] * m[6] * m[9] - 
    m[4] * m[1] * m[10] + 
    m[4] * m[2] * m[9] + 
    m[8] * m[1] * m[6] - 
    m[8] * m[2] * m[5];

  det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

  // if (det == 0)
  //     return false;

  det = 1.0 / det;

  for (i = 0; i < 16; i++)
    invOut[i] = inv[i] * det;

  mat4 r;

  r.rows[0] = vec4( invOut[0], invOut[4], invOut[ 8], invOut[12] );
  r.rows[1] = vec4( invOut[1], invOut[5], invOut[ 9], invOut[13] );
  r.rows[2] = vec4( invOut[2], invOut[6], invOut[10], invOut[14] );
  r.rows[3] = vec4( invOut[3], invOut[7], invOut[11], invOut[15] );

  return r;
    
  // return true;
}
