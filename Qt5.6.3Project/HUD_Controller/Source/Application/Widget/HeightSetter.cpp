#include "HeightSetter.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QSpinBox>

#include "../ParameterStore.h"

HeightSetter::HeightSetter(QWidget *parent) : QWidget(parent)
{
    QHBoxLayout *horizontalBoxLayout = new QHBoxLayout(this);
    //这里的this指的是父类为自己
    //创建在堆中

    horizontalBoxLayout->addWidget(new QLabel("气压高度",this));
    QSpinBox *qtSpinBox = new QSpinBox(this);
    qtSpinBox->setMinimum(0);
    qtSpinBox->setMaximum(0x7fff);//32767
    qtSpinBox->setValue(0);//初始值0

    horizontalBoxLayout->addWidget(qtSpinBox);
    setLayout(horizontalBoxLayout);

    connect(qtSpinBox,SIGNAL(valueChanged(int)),this,SLOT(onValueChange(int)));
}

void HeightSetter::onValueChange(int value)
{
    ParameterStore::getInstance()->setHeight((short int)value);
}
