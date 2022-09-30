#pragma once

#include <QPoint>

class EuklidEntry
{
    int a;
    int b;
    int q;
    int t;
public:
    EuklidEntry(int a_= 0, int b_=0, int t_=0);
    EuklidEntry(const EuklidEntry& lastEntry, int tLast);
    int result() const { return t; }
    bool isNull() const { return a==0 && b==0; }
};

class EllipticPoint : public QPoint
{
    bool mOnCurve;

    void setOnCurve();
public:
    EllipticPoint(int x=-1, int y=-1);
    EllipticPoint(const QPoint& qp);
    EllipticPoint(char cc);
    bool isValid() const;
    bool isInfinity() const;
    bool isOnCurve() const { return mOnCurve; }
    EllipticPoint operator+(const EllipticPoint& other) const;
    int ordinal() const;
    static int modulo(int q);
    static int inverse(int q);
};
