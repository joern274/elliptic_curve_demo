#include "ellipticscene.h"
#include "ellipticpoint.h"
#include <QGraphicsRectItem>
#include <QGraphicsEllipseItem>

extern int g_mod_p;
extern int g_par_a;
extern int g_par_b;

EllipticItem::EllipticItem(const QPoint& p, const QColor& col)
    : QGraphicsRectItem(p.x(),p.y(),1,1), mCoordinates(p)
{
    setPen(QPen(Qt::black,0));
    setFlag(QGraphicsItem::ItemIsSelectable);
    if (col.isValid()) setColor(col);
    else setBrush(Qt::gray);
}

void EllipticItem::setColor(const QColor &col)
{
    setBrush(QBrush(col));
    update();
}

EllipticScene::EllipticScene(QObject *parent)
    : QGraphicsScene(parent), mMarker(nullptr), mInfinity(nullptr), mIgnoreSelection(false)
{
    setBackgroundBrush(QBrush(Qt::black));
    reset();
    connect(this,&QGraphicsScene::selectionChanged,this,&EllipticScene::handleSelectionChanged);
}

void EllipticScene::reset()
{
    mOrdinalHash.clear();
    mOrdColor.clear();
    if (mMarker)
    {
        removeItem(mMarker);
        delete mMarker;
        mMarker = nullptr;
    }
    addInfinity();

    setSceneRect(-1,-1,g_mod_p+2, g_mod_p+2);
    clear();
    QGraphicsRectItem* r = new QGraphicsRectItem(0,0,g_mod_p,g_mod_p);
    r->setPen(QPen(Qt::blue,0));
    addItem(r);

    setPoints();
    for (const QPoint& p : mPoints)
    {
        EllipticPoint elp(p);
        int ord = elp.ordinal();
        if (!ord) continue;
        EllipticItem* it = new EllipticItem(p);
        mOrdinalHash.insert(it,ord);
        mOrdColor.insert(ord,QColor());
        addItem(it);
    }

    int ncol = mOrdColor.size();
    int hueStep = 200;
    if (ncol > 2) hueStep = 250 / (ncol-1);

    for (int ord : mOrdColor.keys())
    {
        mOrdColor[ord] = QColor::fromHsv((ncol-1)*hueStep,255,255);
        --ncol;
    }

    setColor();
    mInfinity = new QGraphicsSimpleTextItem(QString(QChar(8734)));
    QFont font = mInfinity->font();
    font.setPixelSize(14);
    font.setBold(true);
    mInfinity->setFont(font);
    mInfinity->setPen(Qt::NoPen);
    mInfinity->setBrush(QBrush(Qt::white));
    mInfinity->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    mInfinity->setPos(QPointF(g_mod_p+3.5,g_mod_p/2.));
    addItem(mInfinity);
}

void EllipticScene::addInfinity()
{
    if (mInfinity)
    {
        removeItem(mInfinity);
        delete mInfinity;
    }
    mInfinity = new QGraphicsSimpleTextItem(QString(QChar(8734)));
    QFont font = mInfinity->font();
    font.setPixelSize(14);
    font.setBold(true);
    mInfinity->setFont(font);
    mInfinity->setPen(Qt::NoPen);
    mInfinity->setBrush(QBrush(Qt::white));
    mInfinity->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    mInfinity->setPos(QPointF(g_mod_p+3.5,g_mod_p/2.));
    addItem(mInfinity);
}

void EllipticScene::selectPoint(const QPoint& p)
{
    mIgnoreSelection = true;

    QPointF mpos = p + QPointF(-1.5,-1.5);
    if (p.x() >= g_mod_p && mInfinity)
    {
        mpos = mInfinity->pos() - QPointF(1.25,0.75);
    }
    if (mMarker)
        mMarker->setPos(mpos);
    else
    {
        mMarker = new QGraphicsEllipseItem(0,0,4,4);
        mMarker->setBrush(QColor::fromRgb(255,255,255,128));
        mMarker->setPen(Qt::NoPen);
        addItem(mMarker);
        mMarker->setPos(mpos);
    }
    mIgnoreSelection = false;
}

void EllipticScene::setColor()
{
    for (auto it = mOrdinalHash.begin(); it != mOrdinalHash.end(); ++it)
    {
        EllipticItem* item = it.key();
        int ord = it.value();
        if (!ord) continue;
        QColor col = mOrdColor.value(ord);
        if (!col.isValid()) continue;
        item->setColor(col);
    }
    update();
}

void EllipticScene::handleSelectionChanged()
{
    if (mIgnoreSelection) return;
    setColor();

    EllipticItem* firstSelected = nullptr;

    for (QGraphicsItem* si : selectedItems())
    {
        EllipticItem* item = dynamic_cast<EllipticItem*>(si);
        if (!item) continue;
        firstSelected = item;
        break;
    }

    if (firstSelected)
    {
        firstSelected->setSelected(false);
        QPoint p = firstSelected->coordinates();
        selectPoint(p);
        Q_EMIT pointSelected(p);
        firstSelected->update();
    }
    update();
}

QPoint EllipticScene::firstPoint() const
{
    if (mPoints.empty()) return QPoint();
    return mPoints.at(0);
}

void EllipticScene::setPoints()
{
    mPoints.clear();
    int yy[g_mod_p];
    int cnt[g_mod_p];
    memset(cnt, 0, sizeof(cnt));
    for (int y=0; y<g_mod_p; y++)
    {
        yy[y] = y*y%g_mod_p;
        cnt[yy[y]]++;
    }
    int sum = 0;
    for (int x=0; x<g_mod_p; x++)
    {
        int y2 = (x*x*x + g_par_a*x + g_par_b)%g_mod_p;
        if (cnt[y2])
        {
            for (int y=0; y<g_mod_p; y++)
                if ( yy[y] == y2 )
                {
                    mPoints.append(QPoint(x,y));
                    ++sum;
                }
        }
    }
}
