#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <stdlib.h>

#include "memorydump.h"
#include "stackdump.h"

#define MEM_SIZE 1048576

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , ax(0)
    , bx(0)
    , cx(0)
    , dx(0)
    , di(0)
    , si(0)
    , es(0)
    , ds(0)
    , s(0)
    , z(0)
    , ac(0)
    , p(0)
    , cy(0)
    , o(0)
    , d(0)
    , i(0)
    , t(0)
    , m_curExec(0)
{
    ui->setupUi(this);
    m_memory = (uint8_t*)malloc(MEM_SIZE);
    memset(m_memory, 0, MEM_SIZE);
}

MainWindow::~MainWindow()
{
    delete ui;
    free(m_memory);
}

void MainWindow::showRegs() {
    ui->teAH->setPlainText("0x" + QString::number(ax >> 8, 16));
    ui->teAL->setPlainText("0x" + QString::number(ax & 0xF, 16));
    ui->teBH->setPlainText("0x" + QString::number(bx >> 8, 16));
    ui->teBL->setPlainText("0x" + QString::number(bx & 0xF, 16));
    ui->teCH->setPlainText("0x" + QString::number(cx >> 8, 16));
    ui->teCL->setPlainText("0x" + QString::number(cx & 0xF, 16));
    ui->teDH->setPlainText("0x" + QString::number(dx >> 8, 16));
    ui->teDL->setPlainText("0x" + QString::number(dx & 0xF, 16));

    ui->teSI->setPlainText("0x" + QString::number(si, 16));
    ui->teDI->setPlainText("0x" + QString::number(di, 16));

    ui->teES->setPlainText("0x" + QString::number(es, 16));
    ui->teDS->setPlainText("0x" + QString::number(ds, 16));

    ui->teS->setPlainText(QString::number(s, 2));
    ui->teZ->setPlainText(QString::number(z, 2));
    ui->teAC->setPlainText(QString::number(ac, 2));
    ui->teP->setPlainText(QString::number(p, 2));
    ui->teCY->setPlainText(QString::number(cy, 2));
    ui->teO->setPlainText(QString::number(o, 2));
    ui->teD->setPlainText(QString::number(d, 2));
    ui->teI->setPlainText(QString::number(i, 2));
    ui->teT->setPlainText(QString::number(t, 2));
}

void MainWindow::updateRegs() {

}

bool isComment(QString s) {
    QString ref = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    for (auto i : s) {
        if (i == ';')
            return true;
        if (ref.indexOf(i) != -1)
            return false;
    }
    return true;
}

void MainWindow::on_pbLoad_clicked()
{
    QString s = ui->teCode->toPlainText() + "\n";
    code_storage = s;

    while (true) {
        QString tmp = s;
        tmp.truncate(s.indexOf('\n'));
        if (!isComment(tmp))
            m_codeLines.append(tmp);
        if (s.length() == tmp.length())
            break;
        s = s.right(s.length() - tmp.length() - 1);
    }

    for (auto s : m_codeLines) {
        if (s.indexOf(';') != -1)
            s = s.left(s.indexOf(';'));
    }

    ui->teCode->clear();
    for (int i = 0; i < m_codeLines.size(); ++i) {
        QString hex = QString::number(i, 16);
        while (hex.length() != 4)
            hex = "0" + hex;
        QString addr = "[0x" + hex + "]";
        ui->teCode->appendPlainText(addr + " " + m_codeLines[i]);
    }
    m_curExec = 0;

    ui->pbLoad->setEnabled(false);

    ui->pbDumpStack->setEnabled(true);
    ui->pbExec->setEnabled(true);
    ui->pbMemDump->setEnabled(true);
    ui->pbRegUpd->setEnabled(true);
    ui->pbStop->setEnabled(true);

    ui->teCode->setReadOnly(true);
}

void MainWindow::on_pbExec_clicked()
{
    ++m_curExec;
    showRegs();
    if (m_curExec >= m_codeLines.size()) {
        ui->pbLoad->setEnabled(false);

        ui->pbDumpStack->setEnabled(true);
        ui->pbExec->setEnabled(false);
        ui->pbMemDump->setEnabled(true);
        ui->pbRegUpd->setEnabled(true);
        ui->pbStop->setEnabled(true);

        ui->teLog->appendPlainText("Программа завершена");
    }
}

void MainWindow::on_pbMemDump_clicked()
{
    MemoryDump mem_dump(this);
    mem_dump.printMemory(m_memory, MEM_SIZE);
    mem_dump.exec();
}

void MainWindow::on_pbRegUpd_clicked()
{

}

void MainWindow::on_pbDumpStack_clicked()
{
    StackDump stack_dump(this);
    stack_dump.printStack(m_stack);
    stack_dump.exec();
}

void MainWindow::on_pbStop_clicked()
{
    ui->pbLoad->setEnabled(true);

    ui->pbDumpStack->setEnabled(false);
    ui->pbExec->setEnabled(false);
    ui->pbMemDump->setEnabled(false);
    ui->pbRegUpd->setEnabled(false);
    ui->pbStop->setEnabled(false);

    m_codeLines.clear();
    ax = 0;
    bx = 0;
    cx = 0;
    dx = 0;
    di = 0;
    si = 0;
    es = 0;
    ds = 0;

    s = 0;
    z = 0;
    ac = 0;
    p = 0;
    cy = 0;
    o = 0;
    d = 0;
    i = 0;
    t = 0;
    showRegs();
    ui->teLog->clear();
    ui->teCode->setReadOnly(false);
    ui->teCode->setPlainText(code_storage);
}

void MainWindow::on_pbClearLog_clicked()
{
    ui->teLog->clear();
}
