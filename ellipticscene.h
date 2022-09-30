#pragma once

#include <QGraphicsScene>
#include <QPoint>
#include <QList>
#include <QHash>
#include <QMap>
#include <QColor>
#include <QGraphicsEllipseItem>
#include <QGraphicsRectItem>
#include <QGraphicsSimpleTextItem>

class EllipticItem : public QGraphicsRectItem
{
    QPoint mCoordinates;
public:
    EllipticItem(const QPoint& p, const QColor& col = QColor());
    void setColor(const QColor& col);
    QPoint coordinates() const { return mCoordinates; }
};

class EllipticScene : public QGraphicsScene
{
    Q_OBJECT
    QList<QPoint> mPoints;
    QHash<EllipticItem*,int> mOrdinalHash;
    QMap<int,QColor> mOrdColor;
    QGraphicsEllipseItem* mMarker;
    QGraphicsSimpleTextItem* mInfinity;
    bool mIgnoreSelection;

    void setPoints();
    void setColor();
    void addInfinity();

Q_SIGNALS:
    void pointSelected(QPoint pnt);

private Q_SLOTS:
    void handleSelectionChanged();

public:
    EllipticScene(QObject* parent = nullptr);
    void reset();
    QPoint firstPoint() const;
    void selectPoint(const QPoint& p);
    const QMap<int,QColor>& colorTable() const { return mOrdColor; }
};

