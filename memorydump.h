#ifndef MEMORYDUMP_H
#define MEMORYDUMP_H

#include <QDialog>

namespace Ui {
class MemoryDump;
}

class MemoryDump : public QDialog
{
    Q_OBJECT

public:
    explicit MemoryDump(QWidget *parent = nullptr);
    ~MemoryDump();

    void printMemory(uint8_t* mem, uint32_t size);

private:
    Ui::MemoryDump *ui;
};

#endif // MEMORYDUMP_H
