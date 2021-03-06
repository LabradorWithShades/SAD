#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QStack>
#include <QMap>
#include <QPair>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pbLoad_clicked();

    void on_pbExec_clicked();

    void on_pbMemDump_clicked();

    void on_pbRegUpd_clicked();

    void on_pbDumpStack_clicked();

    void on_pbStop_clicked();

    void on_pbClearLog_clicked();

private:
    void showRegs();
    void updateRegs();
    uint32_t decode_addr(QString str, uint8_t& size);
    void execOP(QString op_str);

    Ui::MainWindow *ui;

    QVector<QPair<QString, uint32_t>> m_codeLines;
    uint16_t ax, bx, cx, dx,
             di, si,
             es, ds;
    uint8_t  s, z, ac, p, cy, o,
             d, i, t;
    int m_curExec;
    QString code_storage;
    int m_beforecall;

    uint8_t* m_memory;
    QStack<uint16_t> m_stack;
    QMap<QString, int> m_labels;
    QMap<QString, uint16_t*> m_genRegs;
    QMap<QString, uint8_t*> m_genpartRegs;
    QMap<QString, uint16_t*> m_indRegs;
    QMap<QString, uint16_t*> m_segRegs;

private:
    void err_op(QString msg);
    void mov_op(QString params);
    void push_op(QString params);
    void pop_op(QString params);
    void call_op(QString params);
    void ret_op();
    void add_op(QString params);
    void mul_op(QString params);

};
#endif // MAINWINDOW_H
