#include "stackdump.h"
#include "ui_stackdump.h"

StackDump::StackDump(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StackDump)
{
    ui->setupUi(this);
}

StackDump::~StackDump()
{
    delete ui;
}

void StackDump::printStack(QStack<uint16_t> stack) {
    ui->teDump->clear();
    ui->teDump->appendPlainText("====TOP====");
    if (stack.empty())
        ui->teDump->appendPlainText("Стек пуст!");
    while (!stack.empty()) {
        uint16_t x = stack.pop();
        QString cur = QString::number(x, 16);
        while (cur.length() < 4)
            cur = "0" + cur;
        cur = "[0x" + cur + "]";
        ui->teDump->appendPlainText(cur);
    }
    ui->teDump->appendPlainText("==BOTTOM==");
}
