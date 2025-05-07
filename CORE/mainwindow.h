#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "mygraphicsview.h"
#include <QTimer>
#include "map.pb.h"
#include <QLineEdit>
#include "mywidget.h"
#include <QDomDocument>
#include "chageattr.h"
#include <QVector>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    MapAGV editableMap;


protected:
    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent *event) override;

private:
    Ui::MainWindow *ui;
    MyGraphicsView *graphicsView;
    QLineEdit *testFrame1;
    QLineEdit *testFrame2;
    mywidget w;
    chageattr changeW;
    bool isInMap(double x, double y);

private slots:
    void startSim();
    void restartSim();
    void pauseSim();
    void addNode();
    void enterRouteMode();
    void changeAttribution();
    void recChangeAttrInfo(QVector<int> ids, Attribution_Node category);
    void exitRouteMode();
    void recordChoosedID(int id);
    void batchAddingNodes();
    void recBatchMap(MapAGV tmpMap);
    void saveMap();
    void loadMap();
};

#endif // MAINWINDOW_H
