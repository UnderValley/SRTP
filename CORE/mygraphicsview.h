#ifndef MYGRAPHICSVIEW_H
#define MYGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QMouseEvent>
#include <qdebug.h>
#include <QTimer>
#include <QLabel>
#include "map.pb.h"
#include <QMutex>


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
