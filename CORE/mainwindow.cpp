#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QGraphicsRectItem>
#include <QPainter>
#include <QTimer>
#include <qdebug.h>
#include <QStyleFactory>
#include <QFile>
#include <QFileDialog>
#include <fstream>

namespace  {
    int curNodeID = 0;
    int curRouteID = 0;
    bool addingRouteMode = false;
    std::vector<int> choosedNodes;
    double ratioBtn = 1;
    double ratioText1 = 1;
    double ratioText2 = 1;
    std::vector<double> offsetBtn;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->btn_simControl->setEnabled(false);
//    w.initial1();
    testFrame1 = new QLineEdit(this);
    testFrame2 = new QLineEdit(this);
    testFrame1->setGeometry(1500, 100, 100, 20);
    testFrame1->setText("200");
    ratioText1 = 1500.0 / (double)this->width();
    testFrame2->setGeometry(1630, 100, 100, 20);
    testFrame2->setText("200");
    ratioText2 = 1630.0 / (double)this->width();
    ratioBtn = (double)(ui->btn_addNode->x()) / (double)this->width();
    for (auto& tmpbtn : ui->buttonGroup->buttons()) {
        offsetBtn.push_back(tmpbtn->x() - ui->btn_addNode->x());
    }
qDebug() << this->width() << this->height();
    graphicsView = new MyGraphicsView(this);
    graphicsView->setGeometry(80, 50, this->width()*0.6, this->height()*0.75);
    connect(&w, SIGNAL(sendMap(MapAGV)), this, SLOT(recBatchMap(MapAGV)));
    connect(ui->btn_addRouteControl, SIGNAL(clicked()), this, SLOT(enterRouteMode()));
    connect(&changeW, SIGNAL(sendChangedInfo(QVector<int>, Attribution_Node)), this, SLOT(recChangeAttrInfo(QVector<int>, Attribution_Node)));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::startSim()
{
    std::vector<int> pathList;
    QString filename ="../MAP/path.txt";
    QFile file(filename);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        while (!file.atEnd()) {
            QByteArray line = file.readLine();
//            QString str(line);
            pathList.push_back(line.toInt());
        }

        file.close();
    }
    graphicsView->car.load_path(pathList);
    graphicsView->start_sim();
    ui->btn_simControl->setEnabled(true);
    connect(ui->btn_simControl, SIGNAL(clicked()), this, SLOT(pauseSim()));
}

void MainWindow::pauseSim()
{
    disconnect(ui->btn_simControl, SIGNAL(clicked()), this, SLOT(pauseSim()));
    connect(ui->btn_simControl, SIGNAL(clicked()), this, SLOT(restartSim()));
    ui->btn_simControl->setText("RESTART");
    graphicsView->pause_sim();
}

void MainWindow::restartSim()
{
    connect(ui->btn_simControl, SIGNAL(clicked()), this, SLOT(pauseSim()));
    disconnect(ui->btn_simControl, SIGNAL(clicked()), this, SLOT(restartSim()));
    ui->btn_simControl->setText("PAUSE");
    graphicsView->start_sim();
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

void MainWindow::changeAttribution()
{
    changeW.show();
}

void MainWindow::recChangeAttrInfo(QVector<int> ids, Attribution_Node category)
{
    qDebug() << category;
    MapAGV tmpM;
    if (editableMap.nodes_size() == 0) return;
    for (int i = 0; i < editableMap.nodes_size(); i++) {
        if (ids.contains(i)) {
            editableMap.mutable_nodes(i)->set_attr(category);
        }
    }
    graphicsView->load_map(editableMap);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    graphicsView->setGeometry(80, 50, this->width()*0.6, this->height()*0.75);
    testFrame1->setGeometry(this->width()*ratioText1, testFrame1->y(), testFrame1->width(), testFrame1->height());
    testFrame2->setGeometry(this->width()*ratioText2, testFrame2->y(), testFrame2->width(), testFrame2->height());
    int i = 0;
    for (auto& tmpbtn : ui->buttonGroup->buttons()) {
        tmpbtn->setGeometry(this->width()*ratioBtn + offsetBtn[i], tmpbtn->y(), tmpbtn->width(), tmpbtn->height());
    }
}
void MainWindow::recBatchMap(MapAGV tmpMap)
{
    for (int i = 0; i < tmpMap.nodes_size(); i++) {
        // if (i != 0) {
        //     auto t_route = editableMap.add_routes();
        //     t_route->set_node1_id(curNodeID - 1);
        //     t_route->set_node2_id(curNodeID);
        //     t_route->set_id(curRouteID);
        //     t_route->set_attr(Attribution_Route::SINGLE);
        //     curRouteID++;
        // }
        auto t_node = editableMap.add_nodes();
        t_node->set_id(curNodeID);
        t_node->set_x(tmpMap.nodes(i).x());
        t_node->set_y(tmpMap.nodes(i).y());
        // if (i == 0) {
        //     t_node->add_exit_routes(curRouteID);
        // } else if (i == tmpMap.nodes_size()) {
        //     t_node->add_entry_routes(curRouteID - 1);
        // } else {
        //     t_node->add_exit_routes(curRouteID);
        //     t_node->add_entry_routes(curRouteID - 1);
        // }
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
//        editableMap.mutable_node
        editableMap.mutable_nodes(choosedNodes[0])->add_exit_routes(curRouteID);
        editableMap.mutable_nodes(choosedNodes[0])->add_entry_routes(curRouteID);
        editableMap.mutable_nodes(choosedNodes[1])->add_entry_routes(curRouteID);
        editableMap.mutable_nodes(choosedNodes[1])->add_exit_routes(curRouteID);
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

void MainWindow::saveMap()
{
    std::ofstream output("../MAP/savemap.bin", std::ios::binary);
    if (!editableMap.SerializePartialToOstream(&output)) {
        qDebug() << "save failed";
        return;
    }
    output.close();
}

void MainWindow::loadMap()
{
//    QString filename ="../MAP/savemap.bin";// QFileDialog::getOpenFileName(this, "打开文件", "", "二进制文件 (*.bin);;所有文件 (*)", 0, QFileDialog::DontUseNativeDialog);
////    QFile file("QT_XML.xml");
////    qDebug() << filename;
//    QFile file(filename);
    std::ifstream input("../MAP/savemap.bin", std::ios::binary);
    MapAGV tempMap;
    tempMap.ParseFromIstream(&input);
    editableMap.CopyFrom(tempMap);
    graphicsView->load_map(editableMap);
//     if (!file.open(QFileDevice::ReadOnly)) {
//         qDebug() << "文件打开失败！";
//         return;
//     }
//     QDomDocument doc;
//     if (!doc.setContent(&file)) {
//        qDebug() <<  "操作的文件不是XML文件！";
//         file.close();
//         return;
//     }
//     QDomNodeList list = doc.elementsByTagName("node");
//     for (int i = 0; i < list.count(); i++) {
//         auto t_node = editableMap.add_nodes();
//         t_node->set_id(i);
//         t_node->set_x(list.at(i).toElement().attribute("x").toDouble() / 10);
//         t_node->set_y( -list.at(i).toElement().attribute("y").toDouble() / 10);
// //        qDebug() << list.at(i).toElement().attribute("code") << list.at(i).toElement().attribute("x").toDouble();
//     }
//     list = doc.elementsByTagName("arc");
//     for (int i = 0; i < list.count(); i++) {
//         auto t_node = editableMap.add_routes();
//         t_node->set_id(i);
//         t_node->set_node1_id(list.at(i).toElement().attribute("snode").toDouble());
//         t_node->set_node2_id(list.at(i).toElement().attribute("enode").toDouble());
//         t_node->set_entry(list.at(i).toElement().attribute("snode").toDouble());
//         t_node->set_exit(list.at(i).toElement().attribute("enode").toDouble());
// //        qDebug() << list.at(i).toElement().attribute("code") << list.at(i).toElement().attribute("x").toDouble();
//     }
//     graphicsView->load_map(editableMap);

    //    if (filename.isEmpty()) {
//        qDebug() << "not select file";
//        return;
//    }
//    MapAGV t_map;
//    std::ifstream input(filename.toStdString(), std::ios::binary);
//    if (!t_map.ParseFromIstream(&input)) {
//        qDebug() << "open failed";
//        return;
//    }
//    input.close();
//    editableMap.Clear();
//    editableMap.CopyFrom(t_map);
//    graphicsView->load_map(editableMap);
    //    auto key = QFileDialog::getOpenFileName(this,"打开文件","../");
//    QFile file;
//    file.setFileName(key);
//    QTextStream in(&file);
//    editableMap.Clear();
//    editableMap.ParseFromString(in.readAll().toStdString());
////    editableMap.ParseFromString(key.toStdString());
//    graphicsView->load_map(editableMap);
}

void MainWindow::exitRouteMode()
{
    addingRouteMode = false;
    choosedNodes.clear();
    disconnect(graphicsView, SIGNAL(chooseNode(int)), this, SLOT(recordChoosedID(int)));
    connect(ui->btn_addRouteControl, SIGNAL(clicked()), this, SLOT(enterRouteMode()));
    disconnect(ui->btn_addRouteControl, SIGNAL(clicked()), this, SLOT(exitRouteMode()));
    ui->btn_addRouteControl->setText("addRouteMode");
}
