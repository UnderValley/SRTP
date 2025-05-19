#include "mygraphicsview.h"
#include <algorithm>

namespace  {
    const double UPDATE_FREQUENCY = 20;
    const double UPDATE_PERIOD = 1.0 / UPDATE_FREQUENCY;
    const int NODEWIDTH = 50;
    const int CARWIDTH = 75;
    int pressx = 0;
    int pressy = 0;
    struct MouseState {
        static const int CLEAR = 0x00;
        static const int RIGHT_HODING = 0x01;
        static const int MIDDLE_HOLDING = 0x02;
        static const int LEFT_HOLDING = 0x04;
        static const int MOVING = 0x08;
    };
    int m_state = 0;
}

AGVCar::AGVCar()
{
    _cur_node_index_in_path = 0;
}

AGVCar::AGVCar(int id)
{
    _cur_node_index_in_path = 0;
    _index = id;
}

void AGVCar::load_map(MapAGV &m)
{
    _map = &m;
}

void AGVCar::draw_car(QPainter &painter, double scale)
{
    painter.fillRect(_curX + (NODEWIDTH - CARWIDTH) / 2.0, _curY + (NODEWIDTH - CARWIDTH) / 2.0, CARWIDTH / scale , CARWIDTH / scale, QBrush(Qt::blue));
    painter.fillRect(_curX - (NODEWIDTH - CARWIDTH) / 4.0, _curY - (NODEWIDTH - CARWIDTH) / 4.0, CARWIDTH / scale / 2.0 , CARWIDTH / scale / 2.0, QBrush(Qt::white));
    painter.setPen(QPen(Qt::gray, 1));
    painter.drawText(_curX + NODEWIDTH / 3, _curY + NODEWIDTH / 2, QString("%1").arg(_index));
}

void AGVCar::draw_path(QPainter& painter, double scale) {
    if (_path.empty() || !_has_path) return;

    painter.setPen(QPen(QColor(144, 238, 144), 2 / scale)); // 浅绿色
    double currentX = _curX + NODEWIDTH / 2;
    double currentY = _curY + NODEWIDTH / 2;

    for (size_t i = _cur_node_index_in_path + 1; i < _path.size(); ++i) {
        int next_node_id = _path[i];
        double nextX = _map->nodes(next_node_id).x() + NODEWIDTH / 2;
        double nextY = _map->nodes(next_node_id).y() + NODEWIDTH / 2;

        painter.drawLine(currentX, currentY, nextX, nextY);

        currentX = nextX;
        currentY = nextY;
    }
}

void AGVCar::reserve_nodes(std::vector<int> &path) {
    auto minnum = path.size() - _cur_node_index_in_path;
    if (minnum > 3) minnum = 3;
    for (size_t i = _cur_node_index_in_path; i < _cur_node_index_in_path + minnum; ++i) {
        int node_id = path[i];
        auto* node = _map->mutable_nodes(node_id);

        // Skip nodes already reserved by this AGV
        if (node->occupied_by() == _index) {
            qDebug() << "Node " << node_id << " is already reserved by car " << _index << ", skipping.";
            continue;
        }

        // Check if the node is occupied by another AGV
        if (node->occupied()) {
            qDebug() << "Node " << node_id << " is already occupied by AGV " << node->occupied_by() << ", cannot reserve for car " << _index;
            return;
        }
        // Reserve the node for this AGV
        node->set_occupied(true);
        node->set_occupied_by(_index);
        qDebug() << "Node " << node_id << " reserved by car " << _index << " with: " << node->occupied_by();

    }
}

void AGVCar::load_path(std::vector<int> &path) {
    if (path.empty()) {
        qDebug() << "Error: Path is empty, cannot load.";
        _has_path = false;
        return;
    }

    // reserve_nodes(path); // Reserve nodes for the path
    // if (!_has_path) return; // If reservation failed, stop loading

    _path = path;
    qDebug() << "Loaded path starting at node:" << _path[0];
    _cur_node_index_in_path = 0;
    _curX = _map->nodes(_path[_cur_node_index_in_path]).x();
    _curY = _map->nodes(_path[_cur_node_index_in_path]).y();
    _state = STATE::REACHED;
    _has_path = true;

    for (int node_id : _path) {
        qDebug() << "Path node:" << node_id;
    }
}

void AGVCar::reload_path(std::vector<int> &path) {
    if (path.empty()) {
        qDebug() << "Error: Path is empty, cannot reload.";
        _has_path = false;
        return;
    }
    if (path == _path) {
        qDebug() << "Path is the same, no need to reload.";
        return;
    }

    // reserve_nodes(path); // Reserve nodes for the new path
    // if (!_has_path) return; // If reservation failed, stop reloading

    _path = path;
    qDebug() << "Reloaded path starting at node:" << _path[0];
    _cur_node_index_in_path = 0;
    _state = STATE::REACHED;
    _has_path = true;
}

bool AGVCar::can_move_to_next_node() {
    if (_cur_node_index_in_path + 1 >= _path.size()) return false; // No next node
    int next_node_id = _path[_cur_node_index_in_path + 1];
    return _map->nodes(next_node_id).occupied_by() == _index; // Check if the next node is reserved by this AGV
}

void AGVCar::headfor_next_node() {
    _cur_node_id = _path[_cur_node_index_in_path];
    if (_state == STOPPING) {
        qDebug() << "Car " << _index << " stopped at node:" << _path[_cur_node_index_in_path];
        return;
    }
    reserve_nodes(_path); // Reserve nodes for the path
    if (!can_move_to_next_node()) {
        qDebug() << "Car " << _index << " cannot move to the next node yet.";
        this->replan();
        return;
    }
    qDebug() << "index:" << _index << "heading for:" << _path[_cur_node_index_in_path + 1] << "statee:" << _state << " node occupied by:" << _map->nodes(_path[_cur_node_index_in_path + 1]).occupied_by();

    if (_state == REACHED) {
        if (_map->nodes(_path[_cur_node_index_in_path + 1]).x() < _curX) {
            _state = STATE::LEFT;
        } else if (_map->nodes(_path[_cur_node_index_in_path + 1]).x() > _curX) {
            _state = STATE::RIGHT;
        } else if (_map->nodes(_path[_cur_node_index_in_path + 1]).y() < _curY) {
            _state = STATE::UP;
        } else if (_map->nodes(_path[_cur_node_index_in_path + 1]).y() > _curY) {
            _state = STATE::DOWN;
        }
    } else if (_state == LEFT) {
        _curX -= _vel;
        if (_curX <= _map->nodes(_path[_cur_node_index_in_path + 1]).x()) {
            // Release the current node's resource
            auto* current_node = _map->mutable_nodes(_cur_node_id);
            current_node->set_occupied(false);
            current_node->set_occupied_by(0);
            qDebug() << "Node " << _cur_node_id << " released by car " << _index;
            _curX = _map->nodes(_path[_cur_node_index_in_path + 1]).x();
            _cur_node_index_in_path++;
            if (_cur_node_index_in_path == _path.size() - 1) {
                _state = STOPPING;
            } else {
                _state = REACHED;
            }
        }
    } else if (_state == RIGHT) {
        _curX += _vel;
        if (_curX >= _map->nodes(_path[_cur_node_index_in_path + 1]).x()) {
            // Release the current node's resource
            auto* current_node = _map->mutable_nodes(_cur_node_id);
            current_node->set_occupied(false);
            current_node->set_occupied_by(0);
            qDebug() << "Node " << _cur_node_id << " released by car " << _index;
            _curX = _map->nodes(_path[_cur_node_index_in_path + 1]).x();
            _cur_node_index_in_path++;
            if (_cur_node_index_in_path == _path.size() - 1) {
                _state = STOPPING;
            } else {
                _state = REACHED;
            }
        }
    } else if (_state == UP) {
        _curY -= _vel;
        if (_curY <= _map->nodes(_path[_cur_node_index_in_path + 1]).y()) {
            // Release the current node's resource
            auto* current_node = _map->mutable_nodes(_cur_node_id);
            current_node->set_occupied(false);
            current_node->set_occupied_by(0);
            qDebug() << "Node " << _cur_node_id << " released by car " << _index;
            _curY = _map->nodes(_path[_cur_node_index_in_path + 1]).y();
            _cur_node_index_in_path++;
            if (_cur_node_index_in_path == _path.size() - 1) {
                _state = STOPPING;
            } else {
                _state = REACHED;
            }
        }
    } else if (_state == DOWN) {
        _curY += _vel;
        if (_curY >= _map->nodes(_path[_cur_node_index_in_path + 1]).y()) {
            // Release the current node's resource
            auto* current_node = _map->mutable_nodes(_cur_node_id);
            current_node->set_occupied(false);
            current_node->set_occupied_by(0);
            qDebug() << "Node " << _cur_node_id << " released by car " << _index;
            _curY = _map->nodes(_path[_cur_node_index_in_path + 1]).y();
            _cur_node_index_in_path++;
            if (_cur_node_index_in_path == _path.size() - 1) {
                _state = STOPPING;
            } else {
                _state = REACHED;
            }
        }
    }
}

double AGVCar::heuristic(int node1_id, int node2_id) {
    const auto& node1 = _map->nodes(node1_id);
    const auto& node2 = _map->nodes(node2_id);
    return std::abs(node1.x() - node2.x()) + std::abs(node1.y() - node2.y());
}

std::vector<int> AGVCar::find_path_astar(int start_id, int goal_id) {
    std::priority_queue<std::pair<double, int>, std::vector<std::pair<double, int>>, std::greater<>> open_set;
    std::unordered_map<int, double> g_score;
    std::unordered_map<int, int> came_from;
    std::unordered_map<int, bool> visited;
    _start_node_id = start_id;
    _goal_node_id = goal_id;
    g_score[start_id] = 0.0;
    open_set.emplace(heuristic(start_id, goal_id), start_id);

    while (!open_set.empty()) {
        int current = open_set.top().second;
        open_set.pop();

        if (current == goal_id) {
            std::vector<int> path;
            while (came_from.find(current) != came_from.end()) {
                path.push_back(current);
                current = came_from[current];
            }
            path.push_back(start_id);
            std::reverse(path.begin(), path.end());

            return path;
        }

        if (visited[current]) continue;
        visited[current] = true;

        const auto& node = _map->nodes(current);
        for (int route_id : node.exit_routes()) {
            const auto& route = _map->routes(route_id);
            int neighbor = (route.node1_id() == current) ? route.node2_id() : route.node1_id();

            // Skip nodes that are occupied
            if (_map->nodes(neighbor).occupied()) continue;

            // Skip nodes with SHELF attribution only if it's not the goal
            if (neighbor != goal_id && _map->nodes(neighbor).attr() == Attribution_Node::SHELF) continue;

            if (visited[neighbor]) continue;

            double tentative_g_score = g_score[current] + heuristic(current, neighbor);
            if (g_score.find(neighbor) == g_score.end() || tentative_g_score < g_score[neighbor]) {
                g_score[neighbor] = tentative_g_score;
                came_from[neighbor] = current;
                double f_score = tentative_g_score + heuristic(neighbor, goal_id);
                open_set.emplace(f_score, neighbor);
            }
        }
    }

    return {}; // 如果没有找到路径，返回空路径
}

void AGVCar::replan() {
    auto path = this->find_path_astar(this->get_cur_node_id(), this->get_goal_node_id());
    this->reload_path(path);
    // if (path.empty()) {
    //     qDebug() << "No path found for car " << this->get_index();
    // } else {
    //     qDebug() << "Path found for car " << this->get_index();
    //     this->reload_path(path);
    // }
}

MyGraphicsView::MyGraphicsView(QWidget *parent) : QLabel(parent)
{
    car.set_index(1);
    car2.set_index(2);
    car3.set_index(3);
    setMouseTracking(true);
    setStyleSheet("border-width: 2px;border-style: solid;border-color: rgb(0, 0, 0);"
                  "background-color: rgb(150, 150, 150)");

    timer = new QTimer(this);
    connect(timer,&QTimer::timeout,this,[=](){
//        update();
        this->repaint();
    });
    timer->setInterval(UPDATE_PERIOD);
    timer->start();
}

void MyGraphicsView::draw_Map(QPainter &painter)
{
    QTransform tr;
    tr.translate(_xoffset, _yoffset);
    tr.scale(_scale_ratio, _scale_ratio);
    painter.setTransform(tr);
    painter.setTransform(tr);
    painter.setPen(QPen(Qt::white,2));
    // draw routes
    for (int i = 0; i < _map->routes_size(); i++) {
        painter.setPen(QPen(Qt::white, 5));
        painter.drawLine(_map->nodes(_map->routes(i).node1_id()).x() + NODEWIDTH / 2,
                         _map->nodes(_map->routes(i).node1_id()).y() + NODEWIDTH / 2,
                         _map->nodes(_map->routes(i).node2_id()).x() + NODEWIDTH / 2,
                         _map->nodes(_map->routes(i).node2_id()).y() + NODEWIDTH / 2);
    }
    // draw nodes
    for (int i = 0; i < _map->nodes_size(); i++) {
        const auto& node = _map->nodes(i);

        // Determine node color based on occupation
        QColor nodeColor = Qt::white;
        if (node.occupied()) {
            nodeColor = QColor(255, 165, 0); // Light orange for occupied nodes
        } else {
            switch (node.attr()) {
                case Attribution_Node::ROAD: nodeColor = Qt::white; break;
                case Attribution_Node::CHARGE: nodeColor = Qt::red; break;
                case Attribution_Node::IMPORT: nodeColor = Qt::cyan; break;
                case Attribution_Node::EXPORT: nodeColor = Qt::yellow; break;
                case Attribution_Node::SHELF: nodeColor = Qt::green; break;
                default: nodeColor = Qt::white; break;
            }
        }

        // Draw the node
        painter.fillRect(node.x(), node.y(), NODEWIDTH, NODEWIDTH, QBrush(nodeColor));
        painter.setPen(QPen(Qt::gray, 1));
        painter.drawRect(node.x(), node.y(), NODEWIDTH, NODEWIDTH);

        // Draw occupied info if the node is occupied
        if (node.occupied()) {
            draw_occupied_info(painter, node);
        }

        // Draw node ID
        painter.setPen(QPen(Qt::gray, 1));
        painter.drawText(node.x() + NODEWIDTH / 3, node.y() + NODEWIDTH / 2, QString("%1").arg(node.id()));
    }

    // draw cars
    car.draw_car(painter);
    car2.draw_car(painter);
    car3.draw_car(painter);

    if (car.is_having_path()) {
        car.draw_path(painter, 0.3); // 调用 AGVCar 的路径绘制方法
        
        if (_flag_car_running) {
            car.headfor_next_node();
        }
        // car.replan();
    }
    if (car2.is_having_path()) {
        car2.draw_path(painter, 0.3); // 调用 AGVCar 的路径绘制方法
        // car2.draw_car(painter);
        if (_flag_car_running) {
            car2.headfor_next_node();
        }
        // car2.replan();
    }
    if (car3.is_having_path()) {
        car3.draw_path(painter, 0.3); // 调用 AGVCar 的路径绘制方法
        // car3.draw_car(painter);
        if (_flag_car_running) {
            car3.headfor_next_node();
        }
        // car3.replan();
    }
}

void MyGraphicsView::draw_occupied_info(QPainter &painter, const Node &node) {
    painter.setPen(QPen(Qt::black, 1));
    painter.drawText(node.x() + NODEWIDTH / 4, node.y() + NODEWIDTH / 2, QString("AGV %1").arg(node.occupied_by()));
}

void MyGraphicsView::updateOnce()
{
    this->repaint();
}

void MyGraphicsView::set_scale(double ratio)
{
    _scale_ratio = ratio;
}

void MyGraphicsView::set_offset(double x, double y)
{
    _xoffset = x;
    _yoffset = y;
}

void MyGraphicsView::load_map(MapAGV &m)
{
    _map_mute.lock();
    _map = &m;
    car.load_map(m);
    car2.load_map(m);
    car3.load_map(m);
    isFocus.assign(_map->nodes_size(), false);
    _map_mute.unlock();
}

void MyGraphicsView::wheelEvent(QWheelEvent *event)
{
    _scale_ratio += event->angleDelta().y() / 12000.0;
    if (!(_scale_ratio > 0)) _scale_ratio = 0.05;
}

void MyGraphicsView::mousePressEvent(QMouseEvent *event)
{
    switch (event->button()) {
        case Qt::MouseButton::LeftButton: {
            m_state |= MouseState::LEFT_HOLDING;
            break;
        }
        case Qt::MouseButton::RightButton: {
            m_state |= MouseState::RIGHT_HODING;
            break;
        }
        case Qt::MouseButton::MiddleButton: {
            m_state |= MouseState::MIDDLE_HOLDING;
            pressx = event->pos().x();
            pressy = event->pos().y();

            break;
        }
        default:
            break;
    }
}

void MyGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    m_state |= MouseState::MOVING;
    if (m_state & MouseState::MIDDLE_HOLDING) {
        _xoffset += event->pos().x() - pressx;
        _yoffset += event->pos().y() - pressy;
        pressx = event->pos().x();
        pressy = event->pos().y();
    }
}

void MyGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_state & MouseState::LEFT_HOLDING) {
       setFocusNodeId(clickedOnWhich(mapToPainter(event->pos())));
       emit(chooseNode(clickedOnWhich(mapToPainter(event->pos()))));
    }
    m_state = MouseState::CLEAR;
}

void MyGraphicsView::paintEvent(QPaintEvent *)
{
    _map_mute.lock();

    QPixmap pixmap;
    QPainter painter(this);
    draw_Map(painter);
    _map_mute.unlock();
}

int MyGraphicsView::clickedOnWhich(QPoint pos)
{
    for (int i = 0; i < _map->nodes_size(); i++) {
        if (pos.x() >= _map->nodes(i).x() &&
            pos.x() <= _map->nodes(i).x() + NODEWIDTH &&
            pos.y() <= _map->nodes(i).y() + NODEWIDTH &&
            pos.y() >= _map->nodes(i).y()) {
            return _map->nodes(i).id();
        }
    }
    return -1;
}

void MyGraphicsView::setFocusNodeId(int id)
{
    if (id == -1) {return;}
    isFocus.assign(_map->nodes_size(), false);
    isFocus[id] = true;
    qDebug() << "focused id: " << id << QString("(%1, %2)").arg(_map->nodes(id).x()).arg(_map->nodes(id).y()).toLatin1();
}

QPoint MyGraphicsView::mapToPainter(QPoint pos)
{
    return QPoint((pos.x() - _xoffset) / _scale_ratio, (pos.y() - _yoffset) / _scale_ratio);
}
