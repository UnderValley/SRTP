#ifndef MYGRAPHICSVIEW_H
#define MYGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QMouseEvent>
#include <qdebug.h>
#include <QTimer>
#include <QLabel>
#include "map.pb.h"
#include <QMutex>
#include <queue>
#include <unordered_map>
#include <cmath>

class AGVCar
{
public:
    AGVCar();
    AGVCar(int id);
    void load_path(std::vector<int> &path);
    void reload_path(std::vector<int> &path);
    void headfor_next_node();
    void change_vel(double v) {_vel = v;}
    void load_map(MapAGV &m);
    void draw_car(QPainter& painter, double scale = 1.0);
    void draw_path(QPainter& painter, double scale = 1.0);
    bool is_having_path() {return _has_path;}
    int get_cur_node_id() {return _cur_node_id;}
    int get_cur_node_index_in_path() {return _cur_node_index_in_path;}
    int get_goal_node_id() {return _goal_node_id;}
    int get_start_node_id() {return _start_node_id;}
    int get_index() {return _index;}
    void replan();
    std::vector<int> find_path_astar(int start_id, int goal_id);
    enum STATE{
        STOPPING = 0,
        RIGHT = 1,
        UP = 2,
        LEFT = 3,
        DOWN = 4,
        REACHED = 5
    };
    void set_index(int index) {_index = index;}
    void reserve_nodes(std::vector<int> &path);
    bool can_move_to_next_node();

private:
    std::vector<int> _path;
    int _cur_node_id;
    int _cur_node_index_in_path;
    int _start_node_id;
    int _goal_node_id;
    bool _has_path = false;
    double _vel = 100 / 73.0;
    double _battery;
    double _targetX;
    double _targetY;
    double _curX;
    double _curY;
    STATE _state;
    MapAGV* _map;
    int _index;
    double heuristic(int node1_id, int node2_id);
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
    void start_sim() {_flag_car_running = true;}
    void pause_sim() {_flag_car_running = false;}
    int clickedOnWhich(QPoint pos);
    AGVCar car;
    AGVCar car2;
    AGVCar car3;

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
    void draw_occupied_info(QPainter &painter, const Node &node); // 新增方法
    QPoint mapToPainter(QPoint pos);
    void setFocusNodeId(int id);
    void updateOnce();
    bool _flag_car_running = false;
    std::vector<bool> isFocus;
    QTimer *timer;
    QMutex _map_mute;
    MapAGV* _map;
    double _scale_ratio = 0.15;
    double _xoffset = 0;
    double _yoffset = 0;
};

#endif // MYGRAPHICSVIEW_H
