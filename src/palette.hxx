#ifndef PALETTE_H
#define PALETTE_H

#include "ui_palette.h"

class QDockWidget;
class MainWindow;
class Document;
class ItemDataBase;

class Palette : public QWidget, public Ui::Palette
{
    Q_OBJECT

public:
    explicit Palette(QWidget *parent = 0);
    ~Palette();

public:
    static Palette* createPalette(QWidget* mainWindow);
    QDockWidget* dockWidget() {return m_dock;}
    void showWindow(bool show, Document* doc, int props = 0, ItemDataBase* itemData = NULL);

private:
    void mousePressEvent(QMouseEvent *mouse);
    void mouseMoveEvent(QMouseEvent *mouse);
    MainWindow* m_mainWindow;
    QDockWidget* m_dock;
    QPoint m_currentPos;
    QList<QWidget*> m_widgets;
};

#endif // PALETTE_H
