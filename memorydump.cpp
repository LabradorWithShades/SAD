#include "memorydump.h"
#include "ui_memorydump.h"

MemoryDump::MemoryDump(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MemoryDump)
{
    ui->setupUi(this);
    QFont font;
    font.setFamily(QStringLiteral("Monospace"));
    font.setPointSize(12);
    font.setBold(false);
    font.setItalic(false);
    font.setWeight(50);
    ui->teDump->setFont(font);
}

MemoryDump::~MemoryDump()
{
    delete ui;
}

void MemoryDump::printMemory(uint8_t* mem, uint32_t size) {
    ui->teDump->clear();
    for (uint32_t i = 0; i < size / 64; ++i) {
        QString cur_line = "";
        for (uint32_t j = 64 * i; j < 64 * i + 64; j+=2) {
            QString cur = QString::number((mem[j] << 8) + mem[j + 1], 16);
            if (cur.length() < 2)
                cur = "0" + cur;
            cur_line += cur + " ";
        }
        QString line_num = QString::number(i * 64, 16);
        while (line_num.length() < 5)
            line_num = "0" + line_num;
        line_num = "[0x" + line_num + "]";
        ui->teDump->appendPlainText(line_num + ":" + cur_line);
    }
}
