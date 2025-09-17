#include "ejemplo1.h"

ejemplo1::ejemplo1(): Ui_Counter()
{
	num = 0;
	timer = new QTimer(this);
	setupUi(this);
	show();
	connect(button, SIGNAL(clicked()), this, SLOT(doButton()) );
	connect(timer, &QTimer::timeout, this, &ejemplo1::doCount);
	timer->start(1000);
}

void ejemplo1::doButton()
{
	qDebug() << "click on button";
	if (timer->isActive()) {
		timer->stop();
		button->setText("Start");
	} else {
		num = 0;
		timer->start(1000);
		button->setText("Stop");
		lcdNumber->display(num);
	}
}

void ejemplo1::doCount() {
	qDebug() << "click on count";
	lcdNumber->display(num++);
}


