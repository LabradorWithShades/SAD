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

    m_genRegs.insert("AX", &ax);
    m_genRegs.insert("BX", &bx);
    m_genRegs.insert("CX", &cx);
    m_genRegs.insert("DX", &dx);

    m_indRegs.insert("DI", &di);
    m_indRegs.insert("SI", &si);

    m_segRegs.insert("DS", &ds);
    m_segRegs.insert("ES", &es);
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

void MainWindow::err_op(QString msg) {
    ui->teLog->appendPlainText("Программа завершилась из-за ошибки:");
    ui->teLog->appendPlainText(msg);
    m_curExec = m_codeLines.size();
}

uint32_t MainWindow::decode_addr(QString str, uint8_t& size) {
    if (str.indexOf("word") != -1)
        size = 2;
    else
        size = 1;
    str = str.right(str.length() - str.indexOf('[') + 1);
    str = str.left(str.indexOf(']'));

    if (m_labels.find(str) != m_labels.cend())
        return m_codeLines[*(m_labels.find(str))].second;

    QString left_part = str.left(str.indexOf(':'));
    QString right_part = str.right(str.length() - left_part.length() - 1);
    uint32_t left = left_part.toUInt(nullptr, 16);
    uint32_t right = right_part.toUInt(nullptr, 16);
    left = left << 4;
    return left + right;
}

void MainWindow::mov_op(QString params) {
    QString param1 = params;
    param1.truncate(param1.indexOf(','));
    QString param2 = params.right(params.length() - param1.length() - 1);

    if (m_genRegs.find(param1) != m_genRegs.cend()) {
        auto reg_to = m_genRegs.find(param1);
        if (m_genRegs.find(param2) != m_genRegs.cend()) {
            auto reg_from = m_genRegs.find(param2);
            *(*reg_to) = *(*reg_from);
        } else if (m_segRegs.find(param2) != m_segRegs.cend()) {
            auto reg_from = m_segRegs.find(param2);
            *(*reg_to) = *(*reg_from);
        } else if (m_indRegs.find(param2) != m_indRegs.cend()) {
            auto reg_from = m_indRegs.find(param2);
            *(*reg_to) = *(*reg_from);
        } else if (param2.indexOf('[') != -1) {
            //TODO: mov ax, word[a:b]
            uint8_t size = 0;
            uint32_t addr = decode_addr(param2, size);
            err_op("Данные операнды оператора MOV ещё не поддерживаются");
            return;
        } else {
            uint16_t x = param2.toUInt(nullptr, 16);
            *(*reg_to) = x;
        }
    } else if (m_segRegs.find(param1) != m_segRegs.cend()) {
        auto reg_to = m_segRegs.find(param1);
        if (m_genRegs.find(param2) != m_genRegs.cend()) {
            auto reg_from = m_genRegs.find(param2);
            *(*reg_to) = *(*reg_from);
        } else if (m_segRegs.find(param2) != m_segRegs.cend()) {
            auto reg_from = m_segRegs.find(param2);
            *(*reg_to) = *(*reg_from);
        } else if (m_indRegs.find(param2) != m_indRegs.cend()) {
            auto reg_from = m_indRegs.find(param2);
            *(*reg_to) = *(*reg_from);
        } else
            err_op("Недопустимые операнды оператора MOV");
    } else if (m_indRegs.find(param1) != m_indRegs.cend()) {
        auto reg_to = m_indRegs.find(param1);
        if (m_genRegs.find(param2) != m_genRegs.cend()) {
            auto reg_from = m_genRegs.find(param2);
            *(*reg_to) = *(*reg_from);
        } else if (m_segRegs.find(param2) != m_segRegs.cend()) {
            auto reg_from = m_segRegs.find(param2);
            *(*reg_to) = *(*reg_from);
        } else if (m_indRegs.find(param2) != m_indRegs.cend()) {
            auto reg_from = m_indRegs.find(param2);
            *(*reg_to) = *(*reg_from);
        } else {
            uint8_t size = 0;
            uint32_t addr = decode_addr(param2, size);
            err_op("Недопустимые операнды оператора MOV");
            return;
        }
    } else if (param2.indexOf('[') != -1) {
        //TODO: mov word[a:b], ax
        err_op("Данные операнды оператора MOV ещё не поддерживаются");
    } else {
        err_op("Недопустимые операнды оператора MOV");
    }
}

void MainWindow::execOP(QString op_str) {
    QString op = op_str;
    op.truncate(op.indexOf(' '));
    QString params = op_str.right(op_str.length() - op.length() - 1) + ' ';

    if (op == "MOV")
        mov_op(params);
    else if (op == "INT") {
        m_curExec = m_codeLines.size();
    } else {
        ui->teLog->appendPlainText("ОШИБКА! Нераспознання команда: \"" + op_str + "\"");
        ui->teLog->appendPlainText("Если команда верна - это означает, что на данный момент она не поддерживается");
    }
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

void capitalize(QString& str) {
    QString small = "abcdefghijklmnopqrstuvwxyz";
    QString big   = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    QString nums  = "0123456789";
    bool num_started = false;
    for (auto& i : str) {
        if ((small.indexOf(i) != -1) && !num_started)
            i = big.at(small.indexOf(i));
        if (nums.indexOf(i) != -1)
            num_started = true;
        else
            num_started = false;
    }
}

void clean_spaces(QString& str) {
    bool last_space = false;
    bool last_comma = false;
    for (int i = 0; i < str.length();) {
        if (str.at(i) == ' ') {
            if (last_space || last_comma) {
                str.remove(i, 1);
                continue;
            }
            else
                last_space = true;
        } else
            last_space = false;

        if (str.at(i) == ',')
            last_comma = true;
        else
            last_comma = false;

        ++i;
    }
}

void sanitize_str(QString& str) {
    capitalize(str);
    clean_spaces(str);
}

void inc_num(QString str, uint32_t& line_num) {
    if ((str.indexOf("DB") == -1) &&
        (str.indexOf("DW") == -1))
        line_num++;
    else {
        //TODO: Add db/dw memory counter;
    }
}

void MainWindow::on_pbLoad_clicked()
{
    QString s = ui->teCode->toPlainText() + "\n";
    code_storage = s;

    bool mode16bit = false;
    bool org100 = false;

    bool label_started = false;
    QString label_name = "";
    uint32_t line_num = 256;
    while (true) {
        QString tmp = s;
        tmp.truncate(s.indexOf('\n'));
        if (!isComment(tmp)) {
            if (tmp.indexOf(':') != -1) {
                label_name = tmp;
                label_name.truncate(label_name.indexOf(':'));
                tmp = tmp.right(tmp.length() - label_name.length() - 1);
                label_started = true;
                s = s.right(s.length() - label_name.length() - 1);
                if (!isComment(tmp)) {
                    sanitize_str(tmp);
                    if (tmp.indexOf(';') != -1)
                        tmp = tmp.left(tmp.indexOf(';'));
                    m_codeLines.append(QPair<QString, uint32_t>(tmp, line_num));
                    inc_num(tmp, line_num);
                    if (label_started) {
                        m_labels.insert(label_name, m_codeLines.size() - 1);
                        label_started = true;
                    }
                }
            } else if (tmp.indexOf("use16") != -1) {
                ui->teLog->appendPlainText("Использован 16-битный режим");
                mode16bit = true;
            } else if ((tmp.indexOf("org") != -1) && (tmp.indexOf("0x100") != -1)){
                ui->teLog->appendPlainText("Смещение в 0x100 установлено");
                org100 = true;
            } else {
                sanitize_str(tmp);
                if (tmp.indexOf(';') != -1)
                    tmp = tmp.left(tmp.indexOf(';'));
                m_codeLines.append(QPair<QString, uint32_t>(tmp, line_num));
                inc_num(tmp, line_num);
                if (label_started) {
                    m_labels.insert(label_name, m_codeLines.size() - 1);
                    label_started = false;
                    label_name = "";
                }
            }
        }
        if (s.length() == tmp.length())
            break;
        s = s.right(s.length() - tmp.length() - 1);
    }

    if (!mode16bit)
        ui->teLog->appendPlainText("Внимание! Не задан 16-битный режим. Отладчик не поддерживает другие режимы. Возможны ошибки");
    if (!org100)
        ui->teLog->appendPlainText("Внимание! Не установлен отступ в 0x100 для COM-формата!");

    ui->teCode->clear();
    for (int i = 0; i < m_codeLines.size(); ++i) {
        QString hex = QString::number(m_codeLines[i].second, 16);
        while (hex.length() != 4)
            hex = "0" + hex;
        QString addr = "[0x" + hex + "]";
        ui->teCode->appendPlainText(addr + " " + m_codeLines[i].first);
    }
    m_curExec = 0;

    if (m_codeLines.size() == 0) {
        ui->teLog->appendPlainText("Ошибка! Код пуст!");
        return;
    }

    ui->pbLoad->setEnabled(false);

    ui->pbDumpStack->setEnabled(true);
    ui->pbExec->setEnabled(true);
    ui->pbMemDump->setEnabled(true);
    ui->pbRegUpd->setEnabled(true);
    ui->pbStop->setEnabled(true);

    ui->teCode->setReadOnly(true);

    ui->teExecAddr->setPlainText(QString("0x0100"));
    ui->teExecCommand->setPlainText(m_codeLines[0].first);
}

void MainWindow::on_pbExec_clicked()
{
    execOP(m_codeLines[m_curExec].first);
    ++m_curExec;
    showRegs();
    if (m_curExec < m_codeLines.size()) {
        QString line_num = QString::number(m_codeLines[m_curExec].second + 256, 16);
        while (line_num.length() < 4)
            line_num = "0" + line_num;
        line_num = "0x" + line_num;
        ui->teExecAddr->setPlainText(line_num);
        ui->teExecCommand->setPlainText(m_codeLines[m_curExec].first);
    } else {
        ui->pbLoad->setEnabled(false);

        ui->pbDumpStack->setEnabled(true);
        ui->pbExec->setEnabled(false);
        ui->pbMemDump->setEnabled(true);
        ui->pbRegUpd->setEnabled(true);
        ui->pbStop->setEnabled(true);

        ui->teLog->appendPlainText("Программа завершена");

        ui->teExecAddr->setPlainText("");
        ui->teExecCommand->setPlainText("");
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
