#include "fast_ln.h"

#include <math.h>
#include <stdint.h>
#include <string.h>

static float __int_as_float (int32_t a) { float r; memcpy (&r, &a, sizeof r); return r;}
static int32_t __float_as_int (float a) { int32_t r; memcpy (&r, &a, sizeof r); return r;}

/* compute natural logarithm, maximum error 0.85089 ulps */
static float my_logf (float a)
{
    float i, m, r, s, t;
    int e;

#if PORTABLE
    m = frexpf (a, &e);
    if (m < 0.666666667f) {
        m = m + m;
        e = e - 1;
    }
    i = (float)e;
#else // PORTABLE
    i = 0.0f;
    if (a < 1.175494351e-38f){ // 0x1.0p-126
        a = a * 8388608.0f; // 0x1.0p+23
        i = -23.0f;
    }
    e = (__float_as_int (a) - __float_as_int (0.666666667f)) & 0xff800000;
    m = __int_as_float (__float_as_int (a) - e);
    i = fmaf ((float)e, 1.19209290e-7f, i); // 0x1.0p-23
#endif // PORTABLE
    /* m in [2/3, 4/3] */
    m = m - 1.0f;
    s = m * m;
    /* Compute log1p(m) for m in [-1/3, 1/3] */
    r =             -0.130310059f;  // -0x1.0ae000p-3
    t =              0.140869141f;  //  0x1.208000p-3
    r = fmaf (r, s, -0.121483512f); // -0x1.f198b2p-4
    t = fmaf (t, s,  0.139814854f); //  0x1.1e5740p-3
    r = fmaf (r, s, -0.166846126f); // -0x1.55b36cp-3
    t = fmaf (t, s,  0.200120345f); //  0x1.99d8b2p-3
    r = fmaf (r, s, -0.249996200f); // -0x1.fffe02p-3
    r = fmaf (t, m, r);
    r = fmaf (r, m,  0.333331972f); //  0x1.5554fap-2
    r = fmaf (r, m, -0.500000000f); // -0x1.000000p-1  
    r = fmaf (r, s, m);
    r = fmaf (i,  0.693147182f, r); //  0x1.62e430p-1 // log(2)
    if (!((a > 0.0f) && (a < INFINITY))) {
        r = a + a;  // silence NaNs if necessary
        if (a  < 0.0f) r = INFINITY - INFINITY; //  NaN
        if (a == 0.0f) r = -INFINITY;
    }
    return r;
}

/* natural log on [0x1.f7a5ecp-127, 0x1.fffffep127]. Maximum relative error 9.4529e-5 */
static float my_faster_logf (float a)
{
    float m, r, s, t, i, f;
    int32_t e;

    e = (__float_as_int (a) - 0x3f2aaaab) & 0xff800000;
    m = __int_as_float (__float_as_int (a) - e);
    i = (float)e * 1.19209290e-7f; // 0x1.0p-23
    /* m in [2/3, 4/3] */
    f = m - 1.0f;
    s = f * f;
    /* Compute log1p(f) for f in [-1/3, 1/3] */
    r = fmaf (0.230836749f, f, -0.279208571f); // 0x1.d8c0f0p-3, -0x1.1de8dap-2
    t = fmaf (0.331826031f, f, -0.498910338f); // 0x1.53ca34p-2, -0x1.fee25ap-2
    r = fmaf (r, s, t);
    r = fmaf (r, s, f);
    r = fmaf (i, 0.693147182f, r); // 0x1.62e430p-1 // log(2) 
    return r;
}

float ln_impl2(float x)
{
  return my_logf(x);
}

float ln_impl3(float x)
{
  return my_faster_logf(x);
}
