#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_SrioVideoTest.h"

class SrioVideoTest : public QMainWindow
{
    Q_OBJECT

public:
    SrioVideoTest(QWidget *parent = Q_NULLPTR);

private:
    Ui::SrioVideoTestClass ui;
};
