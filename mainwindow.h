#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QTimer;
class QLabel;
class EllipticTable;
class EllipticScene;

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    bool isPrimeNumber(int x) const;
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
protected:
    void resizeEvent(QResizeEvent* evt) override;

private Q_SLOTS:
    void on_spinA_valueChanged(int val);
    void on_spinB_valueChanged(int val);
    void on_spinMod_valueChanged(int val);
    void on_checkLoop_stateChanged(int loopState);
    void handleCurrentChanged(const QModelIndex& current, const QModelIndex& previous);
    void handlePointClicked(const QPoint& p);
    void handleTimer();

private:
    Ui::MainWindow *ui;
    EllipticTable* mTable;
    EllipticScene* mScene;
    QList<QLabel*> mLegend;
    QTimer* mTimer;
    bool mDisableHandler;
    bool mFirstResize;
    void resetTable();
    void updateCanvas();
    void generateLegend();
    void selectRow(int irow);
};
#endif // MAINWINDOW_H
