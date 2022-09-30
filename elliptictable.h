#pragma once

#include <QObject>
#include <QAbstractTableModel>
#include <QList>
#include <QPoint>
#include "ellipticpoint.h"

class EllipticTable : public QAbstractTableModel
{
    Q_OBJECT

    /**
     * @brief mEntries table content
     */
    QList<EllipticPoint> mEntries;

    /**
     * @brief index of first edited (tampered) cell, invalid if not edited
     */
    QModelIndex mEditIndex;

    /**
     * @brief smallerIndex compare index location with mEditIndex
     * @param inx index to be compared
     * @return true if index is located to the left or above mEditIndex or mEditIndex invalid
     * If internal method returns true cell is painted in black.
     */
    bool smallerIndex(const QModelIndex& inx) const;

public Q_SLOTS:

    void setEntries(const QPoint& startP);

public:

    /**
     * @brief EllipticTable constructor
     * @param parent object
     */
    EllipticTable(QObject *parent = nullptr);

    void calculate();

    QPoint getPoint(const QModelIndex& inx) const;

    /**
     * @brief rowCount number of table rows
     * @param parent not used
     * @return number of rows
     */
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    /**
     * @brief columnCount number of table columns
     * @param parent not used
     * @return number of columns (always 3)
     */
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    /**
     * @brief headerData header text
     * @param section column number
     * @param orientation only horizontal is implemented
     * @param role only display role is implemented
     * @return string header text
     */
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    /**
     * @brief data table data content and style
     * @param index cell index
     * @param role implemented roles: display, alignment, text color
     * @return content or style value
     */
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    /**
     * @brief setData change content of cell and recalculate
     * @param index cell index
     * @param value new numerical value
     * @param role must be edit role
     * @return true on success
     */
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    /**
     * @brief flags manage cell access
     * @param index cell index
     * @return allow selection and edit access
     */
    Qt::ItemFlags flags(const QModelIndex &index) const override;

};
