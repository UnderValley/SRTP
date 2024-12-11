#ifndef MYGRAPHICSVIEW_H
#define MYGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QMouseEvent>
#include <qdebug.h>
#include <QTimer>
#include <QLabel>
#include "map.pb.h"
#include <QMutex>

class AGVCar
{
public:
    AGVCar();
    void load_path(std::vector<int> &path);
    void headfor_next_node();
    void change_vel(double v) {_vel = v;}
    void load_map(MapAGV &m);
    void draw_car(QPainter& painter);
    bool is_having_path() {return _has_path;}
    enum STATE{
        STOPPING = 0,
        RIGHT = 1,
        UP = 2,
        LEFT = 3,
        DOWN = 4,
        REACHED = 5
    };

private:
    std::vector<int> _path;
    int _cur_node_index_in_path;
    bool _has_path = false;
    double _vel = 10 / 73.0;
    double _battery;
    double _targetX;
    double _targetY;
    double _curX;
    double _curY;
    STATE _state;
    MapAGV _map;
};

class MyGraphicsView : public QLabel
{
    Q_OBJECT
public:
    explicit MyGraphicsView(QWidget *parent = nullptr);
    explicit MyGraphicsView();

    void load_map(MapAGV &m);
    void set_scale(double ratio);
    void set_offset(double x, double y);
    int clickedOnWhich(QPoint pos);
    AGVCar car;

signals:
    void mouseMoveEvent(QPoint point);//发送鼠标事件
    void mouseClickEvent(QPoint point);
    void chooseNode(int id);

    // QWidget interface
protected:
    void wheelEvent(QWheelEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *);

private:
    void draw_Map(QPainter &painter);
    QPoint mapToPainter(QPoint pos);
    void setFocusNodeId(int id);
    void updateOnce();
    std::vector<bool> isFocus;
    QTimer *timer;
    QMutex _map_mute;
    MapAGV _map;
    double _scale_ratio = 1;
    double _xoffset = 0;
    double _yoffset = 0;
};

#endif // MYGRAPHICSVIEW_H
