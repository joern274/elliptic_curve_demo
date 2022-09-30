#include "mainwindow.h"
#include "elliptictable.h"
#include "ellipticscene.h"
#include "ui_mainwindow.h"
#include <QTimer>
#include <math.h>

int g_mod_p = 59;
int g_par_a = 2;
int g_par_b = 4;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow), mDisableHandler(false), mFirstResize(true)
{
    ui->setupUi(this);
    mTable = new EllipticTable(this);
    ui->tableView->setMaximumWidth(160);
    ui->tableView->setModel(mTable);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    connect(ui->tableView->selectionModel(),&QItemSelectionModel::currentChanged,this,&MainWindow::handleCurrentChanged);
    for (int i=0; i<3; i++)
        ui->tableView->setColumnWidth(i,48);
    mScene = new EllipticScene(this);
    ui->graphicsView->setScene(mScene);
    QHBoxLayout*ll = new QHBoxLayout(ui->legend);
    ll->setMargin(3);
    ui->legend->setAutoFillBackground(true);
//    on_spinMod_valueChanged(g_mod_p);
    connect(mScene,&EllipticScene::pointSelected,this,&MainWindow::handlePointClicked);
    mTimer = new QTimer(this);
    connect(mTimer,&QTimer::timeout,this,&MainWindow::handleTimer);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::handlePointClicked(const QPoint& p)
{
    mTable->setEntries(p);
    selectRow(0);
}

void MainWindow::selectRow(int irow)
{
    QModelIndex inx = mTable->index(irow,0);
    ui->tableView->selectionModel()->select(inx,QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
    ui->tableView->setCurrentIndex(inx);
}

void MainWindow::handleCurrentChanged(const QModelIndex& current, const QModelIndex& previous = QModelIndex())
{
    Q_UNUSED(previous);
    QPoint selectedPoint = mTable->getPoint(current);
    mScene->selectPoint(selectedPoint);
}

void MainWindow::handleTimer()
{
    QModelIndex inx = ui->tableView->currentIndex();
    if (!inx.isValid()) return;
    int n = mTable->rowCount();
    int irow = inx.row() >= n-1 ? 0 : inx.row() + 1;
    selectRow(irow);
}

void MainWindow::on_checkLoop_stateChanged(int loopState)
{
    if (loopState)
        mTimer->start(500);
    else
        mTimer->stop();
}

void MainWindow::on_spinA_valueChanged(int val)
{
    if (mDisableHandler) return;
    mDisableHandler = true;
    g_par_a = val;
    resetTable();
    mDisableHandler = false;
}

void MainWindow::on_spinB_valueChanged(int val)
{
    if (mDisableHandler) return;
    mDisableHandler = true;
    g_par_b = val;
    resetTable();
    mDisableHandler = false;
}

bool MainWindow::isPrimeNumber(int x) const
{
    for (int j = 2; j <= sqrt(x); j++)
        if (x%j==0) return false;
    return true;
}

void MainWindow::on_spinMod_valueChanged(int val)
{
    if (mDisableHandler) return;
    mDisableHandler = true;

    if (!isPrimeNumber(val))
    {
        if (val < 7)
            val = 7;
        else
        {
            int step = val > g_mod_p ? 1 : -1;
            do {
                val += step;
            } while(!isPrimeNumber(val));
        }
        ui->spinMod->setValue(val);
    }
    g_mod_p = val;
    if (ui->spinA->value() >= val) ui->spinA->setValue(val-1);
    ui->spinA->setMaximum(val-1);
    if (ui->spinB->value() >= val) ui->spinB->setValue(val-1);
    ui->spinB->setMaximum(val-1);
    resetTable();
    mDisableHandler = false;
}

void MainWindow::resizeEvent(QResizeEvent *evt)
{
    if (mDisableHandler) return;
    Q_UNUSED(evt);

    if (mFirstResize)
    {
        mDisableHandler = true;
        ui->spinA->setValue(g_par_a);
        ui->spinB->setValue(g_par_b);
        ui->spinMod->setValue(g_mod_p);
        mFirstResize = false;
        resetTable();
        mDisableHandler = false;
    }
    else
    {
        QModelIndex inx = ui->tableView->currentIndex();
        mDisableHandler = true;
        updateCanvas();
        mDisableHandler = false;
        if (inx.isValid()) handleCurrentChanged(inx);
    }
}

void MainWindow::resetTable()
{
    // valid elliptic curve ?
    int valid = (4*g_par_a*g_par_a*g_par_a + 27*g_par_b*g_par_b) % g_mod_p;
    ui->labValid->setText(QString("4a³+27b² = %1 mod p").arg(valid));
    QPalette pal = ui->labValid->palette();
    pal.setColor(ui->labValid->foregroundRole(),valid ? Qt::black : Qt::red);
    ui->labValid->setPalette(pal);

    updateCanvas();
    mTable->setEntries(mScene->firstPoint());
    generateLegend();
    selectRow(0);
 }

void MainWindow::updateCanvas()
{
    float ftrans = height() - 90;
    ftrans /= (g_mod_p + 4);
    mScene->reset();
    ui->graphicsView->setTransform(QTransform(ftrans,0,0,ftrans,500,2));
    ui->graphicsView->update();
}

void MainWindow::generateLegend()
{
    // update legend
    QHBoxLayout* ll = static_cast<QHBoxLayout*>(ui->legend->layout());
    for (QLabel* l : mLegend)
    {
        ll->removeWidget(l);
        l->deleteLater();
    }
    mLegend.clear();

    QLabel* l= new QLabel("1",this);
    l->setMaximumWidth(36);
    l->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ll->addWidget(l);
    mLegend.append(l);

    for (auto it = mScene->colorTable().constBegin(); it != mScene->colorTable().constEnd(); ++it)
    {
        l = new QLabel(QString::number(it.key()),this);
        QPalette pal = l->palette();
        pal.setColor(QPalette::Background,it.value());
        l->setPalette(pal);
        l->setAutoFillBackground(true);
        l->setMaximumWidth(36);
        l->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ll->addWidget(l);
        mLegend.append(l);
    }
}
