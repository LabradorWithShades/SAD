#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QStack>
#include <QMap>

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
    void execOP(QString op_str);

    Ui::MainWindow *ui;

    QVector<QString> m_codeLines;
    uint16_t ax, bx, cx, dx,
             di, si,
             es, ds;
    uint8_t  s, z, ac, p, cy, o,
             d, i, t;
    int m_curExec;
    QString code_storage;

    uint8_t* m_memory;
    QStack<uint16_t> m_stack;
    QMap<QString, int> m_labels;
    QMap<QString, uint16_t*> m_genRegs;
    QMap<QString, uint16_t*> m_indRegs;
    QMap<QString, uint16_t*> m_segRegs;

private:
    void err_op(QString msg);
    void mov_op(QString params);

};
#endif // MAINWINDOW_H
