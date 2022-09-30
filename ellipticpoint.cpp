#include "ellipticpoint.h"

extern int g_mod_p;
extern int g_par_a;
extern int g_par_b;

EuklidEntry::EuklidEntry(int a_, int b_, int t_)
    : a(a_), b(b_), t(t_)
{
    q = b ? a/b : 0;
}

EuklidEntry::EuklidEntry(const EuklidEntry& lastEntry, int tLast)
{
    a = lastEntry.b;
    b = a ? lastEntry.a % a : 0;
    q = b ? a/b : 0;

    t =  q ? tLast - lastEntry.t * lastEntry.q : 0;
}

//-------------------------------------------

EllipticPoint::EllipticPoint(int x, int y)
    : QPoint(x,y)
{
    setOnCurve();
}

EllipticPoint::EllipticPoint(const QPoint& qp)
    : QPoint(qp)
{
    setOnCurve();
}

void EllipticPoint::setOnCurve()
{
    mOnCurve = ( EllipticPoint::modulo(y()*y()) ==
                 EllipticPoint::modulo(x()*x()*x() + g_par_a*x() + g_par_b));
}

EllipticPoint::EllipticPoint(char)
    : QPoint(g_mod_p, g_mod_p), mOnCurve(true)
{;}

bool EllipticPoint::isValid() const
{
    return (x() >= 0 && y() >= 0);
}

bool EllipticPoint::isInfinity() const
{
    return (x() == g_mod_p && y() == g_mod_p);
}
EllipticPoint EllipticPoint::operator+(const EllipticPoint& other) const
{
    if (! (isValid() && other.isValid()) ) return EllipticPoint();
    if (isInfinity()) return other;
    if (other.isInfinity()) return *this;

    int x0 = x();
    int x1 = other.x();
    int y0 = y();
    int y1 = other.y();

    int numerator = 0;
    int denominator = 1;

    if (x1 == x0)
    {
        if (y1 == y0)
        {
            numerator = modulo((3 * x0 * x0 + g_par_a));
            denominator = modulo(y0*2);
            if (!denominator) return EllipticPoint('I');
        }
        else if (y0*y0%g_mod_p == y1*y1%g_mod_p)
            return EllipticPoint('I');
        else
            return EllipticPoint();
    }
    else
    {
        numerator = modulo(y1-y0);
        denominator = modulo(x1-x0);
    }

    int s = modulo(numerator * inverse(denominator));

    int xp = modulo(s*s - x0 - x1);
    int yp = modulo(s * (x0 - xp) - y0);
    return EllipticPoint(xp,yp);
}

int EllipticPoint::modulo(int q)
{
    int retval = q%g_mod_p;
    if (retval < 0) retval += g_mod_p;
    return retval;
}

int EllipticPoint::ordinal() const
{
    if (!isValid() || !isOnCurve()) return 0;
    int count = 0;
    EllipticPoint p('I');
    do {
        p = p + *this;
        ++count;
    } while (!p.isInfinity());
    return count;
}

int EllipticPoint::inverse(int q)
{
    int last = 0;
    int next = 0;
    EuklidEntry ee(g_mod_p,q,1);
    while (! ee.isNull())
    {
        last = next;
        next = ee.result();
        ee = EuklidEntry(ee, last);
    }
    return last;
}

