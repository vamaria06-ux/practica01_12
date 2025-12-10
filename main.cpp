#include <iostream>
#include <cstddef>
namespace top
{
  struct p_t
  {
    int x,y;
  };
  struct IDraw
  {
    virtual p_t begin() const = 0;
    virtual p_t next(p_t) const = 0;
    virtual ~IDraw() = default;
  };
  struct frame_t
  {
    p_t left_bot;
    p_t right_top;
  };
  void make_f(IDraw ** b,size_t k);
  void get_points (IDraw* b, p_t ** ps, size_t & s);
  frame_t build_frame(const p_t * p, size_t s);
  char * build_canvas(frame_t f, char fill);
  void paint_canvas(char * cnv,top::frame_t fr, p_t p, char fill);
  void print_canvas(const char * cnv, top::frame_t fr);
  bool operator == (p_t a, p_t b)
  {
    return a.x == b.x && a.y == b.y;
  }
  bool operator != (p_t a, p_t b)
  {
    return !(a == b);
  }
  struct Dot:IDraw
  {
    p_t begin() const override;
    p_t next(p_t) const override;
    p_t o;
    explicit Dot(p_t dd);
    Dot(int x, int y);
  };
  struct VerSeg:IDraw
  {
    p_t begin() const override;
    p_t next(p_t) const override;
    p_t start_v;
    size_t len_v;
    VerSeg(p_t,size_t);
  };
  struct HorSeg:IDraw
  {
    p_t begin() const override;
    p_t next(p_t) const override;
    p_t start_h;
    size_t len_h;
    HorSeg(p_t,size_t);
  };
  struct DiagSeg:IDraw
  {
    p_t begin() const override;
    p_t next(p_t) const override;
    p_t start_d;
    size_t len_d;
    DiagSeg(p_t,size_t);
  };
}
int main()
{
  using namespace top;
  int err = 0;
  IDraw * f[4] = {};
  p_t * pts = nullptr;
  size_t s = 0;
  char * cnv = nullptr;
  try
  {
    make_f(f,4);
    for (size_t i = 0; i < 4; ++i)
    {
      get_points(f[i], &pts, s);
    }
    frame_t fr = build_frame(pts,s);
    cnv = build_canvas(fr,'.');
    for (size_t i=0; i<s;++i)
    {
      paint_canvas(cnv, fr, pts[i],'#');
    }
    print_canvas(cnv, fr);
    delete[] pts;
    delete[] cnv;
  } catch(...)
  {
    err = 1;
    std::cerr << "Error\n";
  }
  delete f[0];
  delete f[1];
  delete f[2];
  delete f[3];
  return err;
}
void top::make_f(IDraw ** b, size_t k)
{
  b[0] = new HorSeg({0,0},5);
  b[1] = new VerSeg({4,1},6);
  b[2] = new DiagSeg({-12,5},9);
  b[3] = new Dot(3,3);
}
void extend(top::p_t ** pts, size_t s, top::p_t p) {
  top::p_t * res = new top::p_t[s+1];
  for (size_t i = 0;i < s; i++)
  {
    res[i]=(*pts)[i];
  }
  res[s] = p;
  delete [] *pts;
  *pts = res;
}
void top::get_points(IDraw *b, p_t** pts, size_t & s)
{
  p_t p = b ->begin();
  extend(pts, s ,p);
  size_t delta = 1;
  while (b->next(p) != b->begin()) {
    p = b->next(p);
    extend(pts,s + delta,p);
    ++delta;
  }
  s += delta;
}
top::frame_t top::build_frame(const p_t* p, size_t s)
{
  if (!s)
  {
    throw std::logic_error("bad size");
  }
  int minx = p[0].x, maxx= minx;
  int miny = p[0].y, maxy = miny;
  for (size_t i = 1; i < s; i++)
  {
    minx= std::min(minx,p[i].x);
    maxx= std::max(maxx, p[i].x);
    miny = std::min(miny, p[i].y);
    maxy = std::max(maxy, p[i].y);
  }
  p_t aa = {minx,miny};
  p_t bb = {maxx,maxy};
  return frame_t{aa,bb};
}
size_t rows(top::frame_t fr) {
  return (fr.right_top.y - fr.left_bot.y + 1);
}
size_t cols(top::frame_t fr) {
  return (fr.right_top.x - fr.left_bot.x + 1);
}
char * top::build_canvas(top::frame_t fr, char fill)
{
  char* cnv = new char [rows(fr)*cols(fr)];
  for (size_t i=0;i<rows(fr)*cols(fr); ++i)
  {
    cnv[i]=fill;
  }
  return cnv;
}
void top::paint_canvas(char* cnv,top::frame_t fr, top::p_t p, char fill) {
  int dx = p.x - fr.left_bot.x;
  int dy = fr.right_top.y - p.y;
  cnv[dy*cols(fr)+dx] = fill;
}
void top::print_canvas(const char * cnv, top::frame_t fr) {
  for (size_t i = 0; i < rows(fr);++i)
  {
    for(size_t j=0; j < cols(fr);++j)
    {
      std::cout<< cnv[i * cols(fr)+j];
    }
    std::cout << "\n";
  }
}
top::Dot::Dot(int x, int y) :
  IDraw(),
  o{x,y}
{
}
top::p_t top::Dot::begin() const
{
  return o;
}
top::p_t top::Dot::next(p_t) const
{
  return begin();
}
top::Dot::Dot(p_t dd):
  IDraw(),
  o{dd}
{
}
top::DiagSeg::DiagSeg(p_t a, size_t b):
  start_d(a),
  len_d(b)
{
}
top::p_t top::DiagSeg::begin() const
{
  return start_d;
}
top::p_t top::DiagSeg::next(p_t a) const
{
  if (a.x + 1 == start_d.x + len_d && a.y + 1 == start_d.y + len_d)
  {
    return begin();
  }
  return {a.x +1 , a.y + 1};
}
top::HorSeg::HorSeg(p_t a, size_t b):
  start_h(a),
  len_h(b)
{
}
top::p_t top::HorSeg::begin() const
{
  return start_h;
}
top::p_t top::HorSeg::next(p_t a) const
{
  if (a.x + 1 == start_h.x + len_h)
  {
    return begin();
  }
  return {a.x + 1,a.y};
}
top::VerSeg::VerSeg(p_t a, size_t b):
  start_v(a),
  len_v(b)
{
}
top::p_t top::VerSeg::begin() const
{
  return start_v;
}
top::p_t top::VerSeg::next(p_t a) const
{
  if (a.y + 1 == start_v.y + len_v)
  {
    return begin();
  }
  return {a.x, a.y + 1};
}
