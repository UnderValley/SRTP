#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QGraphicsRectItem>
#include <QPainter>
#include <QTimer>
#include <qdebug.h>
#include <QStyleFactory>

namespace  {
    int curNodeID = 0;
    int curRouteID = 0;
    bool addingRouteMode = false;
    std::vector<int> choosedNodes;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
//    w.initial1();
    testFrame1 = new QLineEdit(this);
    testFrame2 = new QLineEdit(this);
    testFrame1->setGeometry(870, 100, 100, 20);
    testFrame1->setText("200");
    testFrame2->setGeometry(1000, 100, 100, 20);
    testFrame2->setText("200");

    graphicsView = new MyGraphicsView(this);
    graphicsView->setGeometry(80, 50, 700, 500);
    connect(&w, SIGNAL(sendMap(MapAGV)), this, SLOT(recBatchMap(MapAGV)));
    connect(ui->btn_addRouteControl, SIGNAL(clicked()), this, SLOT(enterRouteMode()));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addNode()
{
    if (!isInMap(testFrame1->text().toInt(), testFrame2->text().toInt())) {
        auto node = editableMap.add_nodes();
        node->set_id(curNodeID);
        node->set_x(testFrame1->text().toInt());
        node->set_y(testFrame2->text().toInt());
        graphicsView->load_map(editableMap);
        qDebug() << "node" << curNodeID << "succeed";
        curNodeID++;

    } else {
        qDebug() << "the node is alredy existing!";
    }
}

void MainWindow::batchAddingNodes()
{
    w.show();
}

void MainWindow::recBatchMap(MapAGV tmpMap)
{
    for (int i = 0; i < tmpMap.nodes_size(); i++) {
        if (i != 0) {
            auto t_route = editableMap.add_routes();
            t_route->set_node1_id(curNodeID - 1);
            t_route->set_node2_id(curNodeID);
            t_route->set_id(curRouteID);
            t_route->set_attr(Attribution_Route::SINGLE);
            curRouteID++;
        }
        auto t_node = editableMap.add_nodes();
        t_node->set_id(curNodeID);
        t_node->set_x(tmpMap.nodes(i).x());
        t_node->set_y(tmpMap.nodes(i).y());
        if (i == 0) {
            t_node->add_exit_routes(curRouteID);
        } else if (i == tmpMap.nodes_size()) {
            t_node->add_entry_routes(curRouteID - 1);
        } else {
            t_node->add_exit_routes(curRouteID);
            t_node->add_entry_routes(curRouteID - 1);
        }
        curNodeID++;
    }
    graphicsView->load_map(editableMap);
}

void MainWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
//    painter.fillRect(this->rect(), QColor(66, 66, 66));
//    painter.drawLine(100, 100, 200, 200);
}

bool MainWindow::isInMap(double x, double y)
{
    for (int i = 0; i < editableMap.nodes_size(); i++) {
        if (editableMap.nodes(i).x() == x &&
            editableMap.nodes(i).y() == y) {
            return true;
        }
    }
    return false;
}

void MainWindow::recordChoosedID(int id)
{
    if (id != -1) {
        choosedNodes.push_back(id);
    }
    if (choosedNodes.size() == 2) {
        auto t_route = editableMap.add_routes();
        t_route->set_node1_id(choosedNodes[0]);
        t_route->set_node2_id(choosedNodes[1]);
        t_route->set_id(curRouteID);
        graphicsView->load_map(editableMap);
        curRouteID++;
        choosedNodes.clear();
    }
}

void MainWindow::enterRouteMode()
{
    addingRouteMode = true;
    connect(graphicsView, SIGNAL(chooseNode(int)), this, SLOT(recordChoosedID(int)));
    disconnect(ui->btn_addRouteControl, SIGNAL(clicked()), this, SLOT(enterRouteMode()));
    connect(ui->btn_addRouteControl, SIGNAL(clicked()), this, SLOT(exitRouteMode()));
    ui->btn_addRouteControl->setText("exit");
}

void MainWindow::exitRouteMode()
{
    addingRouteMode = false;
    disconnect(graphicsView, SIGNAL(chooseNode(int)), this, SLOT(recordChoosedID(int)));
    connect(ui->btn_addRouteControl, SIGNAL(clicked()), this, SLOT(enterRouteMode()));
    disconnect(ui->btn_addRouteControl, SIGNAL(clicked()), this, SLOT(exitRouteMode()));
    ui->btn_addRouteControl->setText("addRouteMode");
}
