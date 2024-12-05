#ifndef MYWIDGET_H
#define MYWIDGET_H

#include <QWidget>
#include "map.pb.h"

namespace Ui {
class mywidget;
}

class mywidget : public QWidget
{
    Q_OBJECT

public:
    explicit mywidget(QWidget *parent = nullptr);
    ~mywidget();

private:
    Ui::mywidget *ui;
    MapAGV tmpMap;

protected:
    void closeEvent(QCloseEvent*event);


signals:
    void test(int ew);
    void sendMap(MapAGV map);

private slots:
    void rec(int ew);

};

#endif // MYWIDGET_H
