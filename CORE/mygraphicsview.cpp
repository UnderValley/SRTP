#include "mygraphicsview.h"


namespace  {
    const double UPDATE_FREQUENCY = 60;
    const double UPDATE_PERIOD = 1.0 / UPDATE_FREQUENCY;
    const int NODEWIDTH = 50;
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

MyGraphicsView::MyGraphicsView(QWidget *parent) : QLabel(parent)
{
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
    for (int i = 0; i < _map.routes_size(); i++) {
        painter.setPen(QPen(Qt::white, 5));
        painter.drawLine(_map.nodes(_map.routes(i).node1_id()).x() + NODEWIDTH / 2,
                         _map.nodes(_map.routes(i).node1_id()).y() + NODEWIDTH / 2,
                         _map.nodes(_map.routes(i).node2_id()).x() + NODEWIDTH / 2,
                         _map.nodes(_map.routes(i).node2_id()).y() + NODEWIDTH / 2);
//         if (_map.routes(i).attr() == Attribution_Route::SINGLE) {
//             auto pen = QPen(Qt::gray, 1, Qt::PenStyle::DashLine);
//             painter.setPen(pen);
// //            painter.draw
//             painter.drawLine(_map.nodes(_map.routes(i).node1_id()).x() + NODEWIDTH / 2,
//                              (_map.nodes(_map.routes(i).node1_id()).y() + NODEWIDTH / 2 + _map.nodes(_map.routes(i).node2_id()).y() + NODEWIDTH / 2) / 2 - 5,
//                              _map.nodes(_map.routes(i).node2_id()).x() + NODEWIDTH / 2,
//                              (_map.nodes(_map.routes(i).node1_id()).y() + NODEWIDTH / 2 + _map.nodes(_map.routes(i).node2_id()).y() + NODEWIDTH / 2) / 2 + 5);

//             painter.drawLine(_map.nodes(_map.routes(i).node1_id()).x() + NODEWIDTH / 2 - 1,
//                              (_map.nodes(_map.routes(i).node1_id()).y() + NODEWIDTH / 2 + _map.nodes(_map.routes(i).node2_id()).y() + NODEWIDTH / 2) / 2 + 5,
//                              _map.nodes(_map.routes(i).node2_id()).x() + NODEWIDTH / 2 + 1,
//                              (_map.nodes(_map.routes(i).node1_id()).y() + NODEWIDTH / 2 + _map.nodes(_map.routes(i).node2_id()).y() + NODEWIDTH / 2) / 2 + 5);
//         }
    }
    for (int i = 0; i < _map.nodes_size(); i++) {
        painter.fillRect(_map.nodes(i).x(), _map.nodes(i).y(), NODEWIDTH, NODEWIDTH, QBrush(Qt::white));
        if (isFocus[i]) {
            painter.setPen(QPen(QColor(153, 51, 255),2 / _scale_ratio));
            painter.drawRect(_map.nodes(i).x(), _map.nodes(i).y(), NODEWIDTH, NODEWIDTH);
        } else {
            painter.setPen(QPen(Qt::gray,1));
            painter.drawRect(_map.nodes(i).x(), _map.nodes(i).y(), NODEWIDTH, NODEWIDTH);
        }
    }
//    painter.drawLine(0, 0, 200, 200);
//    for (int i = 0; i < 100; i++) {
//        for (int j = 0; j < 100; j++) {
//            painter.drawPoint(10 * i, 10 * j);
//        }
//    }
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
    _map.CopyFrom(m);
    isFocus.assign(_map.nodes_size(), false);
//    for (int i = 0; i < _map.nodes_size(); i++) {
//        isFocus.push_back(false);
//    }
    _map_mute.unlock();
}

void MyGraphicsView::wheelEvent(QWheelEvent *event)
{
    _scale_ratio += event->angleDelta().y() / 12000.0;
    if (!(_scale_ratio > 0)) _scale_ratio = 0.05;
//    qDebug() << event->angleDelta();
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
//    qDebug() << event->button();
//    qDebug() << pressx << pressy;
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
//    qDebug() << event->pos();
//    emit(mouseMoveEvent(event->pos()));
}

void MyGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_state & MouseState::LEFT_HOLDING) {
       setFocusNodeId(clickedOnWhich(mapToPainter(event->pos())));
       emit(chooseNode(clickedOnWhich(mapToPainter(event->pos()))));
    }
    m_state = MouseState::CLEAR;
//    if (event->button() == Qt::LeftButton) {
//        emit(mouseClickEvent(event->pos()));
//    }
//    _xoffset += event->pos().x() - pressx;
//    _yoffset += event->pos().y() - pressy;
//    qDebug() << mapToPainter(event->pos());
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
    for (int i = 0; i < _map.nodes_size(); i++) {
        if (pos.x() >= _map.nodes(i).x() &&
            pos.x() <= _map.nodes(i).x() + NODEWIDTH &&
            pos.y() <= _map.nodes(i).y() + NODEWIDTH &&
            pos.y() >= _map.nodes(i).y()) {
            return _map.nodes(i).id();
        }
    }
    return -1;
}

void MyGraphicsView::setFocusNodeId(int id)
{
    if (id == -1) {return;}
    isFocus.assign(_map.nodes_size(), false);
    isFocus[id] = true;
//    for (int i = 0; i < _map.nodes_size(); i++) {
//        isFocus[i] = false;
//        if (id == _map.nodes(i).id()) {
//            isFocus[i] = true;
//        }
//    }
}

QPoint MyGraphicsView::mapToPainter(QPoint pos)
{
    return QPoint((pos.x() - _xoffset) / _scale_ratio, (pos.y() - _yoffset) / _scale_ratio);
}
