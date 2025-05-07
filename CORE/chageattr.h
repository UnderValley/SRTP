#ifndef CHAGEATTR_H
#define CHAGEATTR_H

#include <QWidget>
#include <QLineEdit>
#include <QRegularExpression>
#include <QDebug>
#include "map.pb.h"

namespace Ui {
class chageattr;
}

class chageattr : public QWidget
{
    Q_OBJECT

public:
    explicit chageattr(QWidget *parent = nullptr);
    ~chageattr();

signals:
    void sendChangedInfo(QVector<int>, Attribution_Node);

private:
    Ui::chageattr *ui;
    QVector<int> extractIntegersFromLineEdit(QLineEdit* lineEdit);

private slots:
    void sendResult();
};

#endif // CHAGEATTR_H
