#ifndef STACKDUMP_H
#define STACKDUMP_H

#include <QDialog>
#include <QStack>

namespace Ui {
class StackDump;
}

class StackDump : public QDialog
{
    Q_OBJECT

public:
    explicit StackDump(QWidget *parent = nullptr);
    ~StackDump();

    //Need a copy since can only push and pop
    void printStack(QStack<uint16_t> stack);

private:
    Ui::StackDump *ui;
};

#endif // STACKDUMP_H
