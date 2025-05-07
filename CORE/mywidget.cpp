#include "mywidget.h"
#include "ui_mywidget.h"

mywidget::mywidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::mywidget)
{
    ui->setupUi(this);
    connect(ui->btn_finish, SIGNAL(clicked()), this, SLOT(closeWidget()));
    connect(this, SIGNAL(test(int)), this, SLOT(rec(int)));
}

mywidget::~mywidget()
{
    delete ui;
}

void mywidget::closeWidget()
{
    double curX = ui->tx_startX->text().toDouble();
    double curY = ui->tx_startY->text().toDouble();
    int rows = ui->tx_rows->text().toInt();
    int cols = ui->tx_cols->text().toInt();
    // int num = ui->tx_noedNum->text().toInt();
    double intervalX = ui->tx_intervalX->text().toDouble();
    double intervalY = ui->tx_intervalY->text().toDouble();
    for (int i = 0; i < rows; i++) {
        curX = ui->tx_startX->text().toDouble();
        for (int j = 0; j < cols; j++) {
//        if (i != num) {
//            auto t_route = tmpMap.add_routes();
//            t_route->set_node1_id(i);
//        }
            auto t_node = tmpMap.add_nodes();
            t_node->set_id(0);
            t_node->set_x(curX);
            t_node->set_y(curY);
            curX += intervalX;
        }
        curY += intervalY;
    }
    emit(sendMap(tmpMap));
    qDebug() << "batch map sent";
    this->close();
}

void mywidget::rec(int ew)
{
    qDebug() << ew;
}
