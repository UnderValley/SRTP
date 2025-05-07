#include "chageattr.h"
#include "ui_chageattr.h"

chageattr::chageattr(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::chageattr)
{
    ui->setupUi(this);
    connect(ui->btn_cancel, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui->btn_OK, SIGNAL(clicked()), this, SLOT(sendResult()));
}

chageattr::~chageattr()
{
    delete ui;
}

// 提取文本中所有整型数字（返回 int 类型列表）
QVector<int> chageattr::extractIntegersFromLineEdit(QLineEdit* lineEdit) {
    QVector<int> integers;
    QString text = lineEdit->text();
    
    // 正则表达式只匹配整数（包括正负数）
    QRegularExpression re(R"(-?\d+)"); // 匹配：123, -456
    QRegularExpressionMatchIterator i = re.globalMatch(text);

    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        bool ok;
        int num = match.captured().toInt(&ok);
        if (ok) {
            integers.append(num);
        }
    }

    return integers;
}

void chageattr::sendResult()
{
    QVector<int> nums = extractIntegersFromLineEdit(ui->lineEdit);
    
    if (nums.isEmpty()) {
        qDebug() << "未找到整数";
    } else {
        qDebug() << "找到整数:";
        for (int num : nums) {
            qDebug() << num;
        }
    }
    qDebug() << "category: " << (Attribution_Node)ui->comboBox->currentIndex();
    this->close();
    emit(sendChangedInfo(nums, (Attribution_Node)ui->comboBox->currentIndex()));
}
