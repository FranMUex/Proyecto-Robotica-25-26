#include "ejemplo1.h"

ejemplo1::ejemplo1(): Ui_Counter()
{
	count = 0;
	period = 1000;
	timer = new QTimer(this);
	setupUi(this);
	show();
	connect(button, SIGNAL(clicked()), this, SLOT(doButton()) );
	connect(timer, &QTimer::timeout, this, &ejemplo1::doCount);
	connect(button_2, SIGNAL(clicked()), this, SLOT(doReset()) );
	connect(horizontalSlider, SIGNAL(valueChanged(int)), this, SLOT(changePeriod()) );
	timer->start(period);
}

void ejemplo1::doButton()
{
	qDebug() << "click on button";
	if (timer->isActive()) {
		timer->stop();
		button->setText("Start");
	} else {
		//count = 0;
		timer->start(period);
		button->setText("Stop");
		lcdNumber->display(count);
	}
}

void ejemplo1::doCount() {
	qDebug() << "click on count";
	lcdNumber->display(count++);
}
void ejemplo1::doReset() {
    count = 0;
    lcdNumber->display(count);
}

void ejemplo1::changePeriod()
{
	period = horizontalSlider->value();
	lcdNumber_2->display(period);
	timer->start(period);
}


