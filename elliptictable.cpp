#include "elliptictable.h"
#include <QColor>

extern int g_mod_p;
extern int g_par_a;
extern int g_par_b;

//-------------------------------------------
EllipticTable::EllipticTable(QObject *parent) : QAbstractTableModel(parent)
{;}

void EllipticTable::setEntries(const QPoint &startP)
{
    mEditIndex = QModelIndex();
    beginResetModel();

    mEntries.clear();
    mEntries.append(EllipticPoint(startP.x(),startP.y()));

    calculate();
    endResetModel();
}

void EllipticTable::calculate()
{
    if (mEntries.isEmpty()) return;

    int inx = mEntries.size()-1;

    int badcount = 0;
    EllipticPoint startPoint = mEntries.at(inx);
    EllipticPoint nextPoint = startPoint + mEntries.at(0);

    while (nextPoint.isValid() && nextPoint != startPoint)
    {
        if (!nextPoint.isOnCurve()) ++badcount;
        if (badcount >= 3) return;
        mEntries.append(nextPoint);
        inx++;
        nextPoint = nextPoint + mEntries.at(0);
    }
}

QPoint EllipticTable::getPoint(const QModelIndex& inx) const
{
    if (!inx.isValid() || inx.row() >= mEntries.size()) return QPoint();
    return mEntries.at(inx.row());
}

int EllipticTable::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return mEntries.size();
}

int EllipticTable::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 3;
}

QVariant EllipticTable::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) return QVariant();
    if (orientation != Qt::Horizontal) return QVariant();
    switch (section)
    {
    case 0: return "FAC";
    case 1: return "X";
    case 2: return "Y";
    }
    return QVariant();
}

bool EllipticTable::smallerIndex(const QModelIndex& inx) const
{
    if (!mEditIndex.isValid()) return true;
    if (inx.row() < mEditIndex.row()) return true;
    if (inx.row() > mEditIndex.row()) return false;
    return (inx.column() < mEditIndex.column());
}

QVariant EllipticTable::data(const QModelIndex &index, int role) const
{
    if (role == Qt::TextAlignmentRole)
        return Qt::AlignRight;
    if (index.row() >= mEntries.size()) return QVariant();
    const EllipticPoint& pdata =  mEntries.at(index.row());
    if (role == Qt::TextColorRole)
    {
        if (pdata.isOnCurve()) return QColor(Qt::black);
        return QColor(Qt::darkRed);
    }
    if (role!= Qt::DisplayRole) return QVariant();

    if (!index.column()) return index.row() + 1;
    if (pdata.isInfinity()) return QString(QChar(8734));

    return (index.column()==1 ? pdata.x() : pdata.y());
}

bool EllipticTable::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role != Qt::EditRole) return false;

    int irow = index.row();
    if (irow >= mEntries.size()) return false;

    bool ok;
    int ival = value.toInt(&ok);
    if (!ok) return false;
    EllipticPoint lastP = mEntries.at(irow);
    switch(index.column())
    {
    case 1:
        lastP.setX(ival);
        break;
    case 2:
        lastP.setY(ival);
        break;
    default:
        return false;
    }
    beginResetModel();
    mEntries[irow] = lastP;
    for (int k = mEntries.size()-1; k>irow; k--)
        mEntries.removeAt(k);

    calculate();
    endResetModel();
    return true;
}

Qt::ItemFlags EllipticTable::flags(const QModelIndex &index) const
{
    Q_UNUSED(index)
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}
