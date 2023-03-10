
//----------------------------------------------------------------------------
// Anti-Grain Geometry - Version 2.3
// Copyright (C) 2002-2005 Maxim Shemanarev (http://www.antigrain.com)
// Copyright (C) 2005 Tony Juricic (tonygeek@yahoo.com)
//
// Permission to copy, use, modify, sell and distribute this software
// is granted provided this copyright notice appears in all copies.
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.
//
//----------------------------------------------------------------------------
// Contact: mcseem@antigrain.com
//          mcseemagg@yahoo.com
//          http://www.antigrain.com
//----------------------------------------------------------------------------
#ifndef AGG_CURVES_INCLUDED
#define AGG_CURVES_INCLUDED
#include "agg_array.h"
namespace pdfium
{
namespace agg
{
struct curve4_points  {
    float cp[8];
    curve4_points() {}
    curve4_points(float x1, float y1,
                  float x2, float y2,
                  float x3, float y3,
                  float x4, float y4)
    {
        cp[0] = x1;
        cp[1] = y1;
        cp[2] = x2;
        cp[3] = y2;
        cp[4] = x3;
        cp[5] = y3;
        cp[6] = x4;
        cp[7] = y4;
    }
    void init(float x1, float y1,
              float x2, float y2,
              float x3, float y3,
              float x4, float y4)
    {
        cp[0] = x1;
        cp[1] = y1;
        cp[2] = x2;
        cp[3] = y2;
        cp[4] = x3;
        cp[5] = y3;
        cp[6] = x4;
        cp[7] = y4;
    }
    float  operator [] (unsigned i) const
    {
        return cp[i];
    }
    float& operator [] (unsigned i)
    {
        return cp[i];
    }
};
class curve4_div 
{
public:
    curve4_div() :
        m_count(0)
    {}
    curve4_div(float x1, float y1,
               float x2, float y2,
               float x3, float y3,
               float x4, float y4) :
        m_count(0)
    {
        init(x1, y1, x2, y2, x3, y3, x4, y4);
    }
    curve4_div(const curve4_points& cp) :
        m_count(0)
    {
        init(cp[0], cp[1], cp[2], cp[3], cp[4], cp[5], cp[6], cp[7]);
    }
    void reset()
    {
        m_points.remove_all();
        m_count = 0;
    }
    void init(float x1, float y1,
              float x2, float y2,
              float x3, float y3,
              float x4, float y4);
    void init(const curve4_points& cp)
    {
        init(cp[0], cp[1], cp[2], cp[3], cp[4], cp[5], cp[6], cp[7]);
    }
    void rewind(unsigned)
    {
        m_count = 0;
    }
    unsigned vertex(float* x, float* y)
    {
        if(m_count >= m_points.size()) {
            return path_cmd_stop;
        }
        const point_type& p = m_points[m_count++];
        *x = p.x;
        *y = p.y;
        return (m_count == 1) ? path_cmd_move_to : path_cmd_line_to;
    }
private:
    void bezier(float x1, float y1,
                float x2, float y2,
                float x3, float y3,
                float x4, float y4);
    void recursive_bezier(float x1, float y1,
                          float x2, float y2,
                          float x3, float y3,
                          float x4, float y4,
                          unsigned level);
    float    m_distance_tolerance_square;
    float    m_distance_tolerance_manhattan;
    unsigned              m_count;
    pod_deque<point_type> m_points;
};
class curve4 
{
public:
    curve4() {}
    curve4(float x1, float y1,
           float x2, float y2,
           float x3, float y3,
           float x4, float y4)
    {
        init(x1, y1, x2, y2, x3, y3, x4, y4);
    }
    curve4(const curve4_points& cp)
    {
        init(cp[0], cp[1], cp[2], cp[3], cp[4], cp[5], cp[6], cp[7]);
    }
    void reset()
    {
        m_curve_div.reset();
    }
    void init(float x1, float y1,
              float x2, float y2,
              float x3, float y3,
              float x4, float y4)
    {
        m_curve_div.init(x1, y1, x2, y2, x3, y3, x4, y4);
    }
    void init(const curve4_points& cp)
    {
        init(cp[0], cp[1], cp[2], cp[3], cp[4], cp[5], cp[6], cp[7]);
    }
    void rewind(unsigned path_id)
    {
        m_curve_div.rewind(path_id);
    }
    unsigned vertex(float* x, float* y)
    {
        return m_curve_div.vertex(x, y);
    }
private:
    curve4_div m_curve_div;
};
}
}  // namespace pdfium
#endif
