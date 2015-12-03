
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ui_mainwindow.h"

QT_FORWARD_DECLARE_CLASS(QUndoGroup)
class Document;
class Palette;
class ThemeInterface;

class MainWindow : public QMainWindow, public Ui::MainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);

    static MainWindow* instance() {return m_instance;}

    void addDocument(Document *doc);
    void removeDocument(Document *doc);
    void setCurrentDocument(Document *doc);
    Document *currentDocument() const;
    ThemeInterface* currentTheme() {return m_currentTheme;}

public Q_SLOTS:
    void openDocument();
    void saveDocument();
    void closeDocument();
    void newDocument();
    void saveAsPng();
    void saveAsPdf();
    void saveAsXml();
    void cut();
    void copy();
    void paste();
    void deleteObjects();
    void selectAll();
    void selectNone();
    void groupObjects();
    void ungroup();
    void lock();
    void unlockAll();
    void showLibrary();
    void showPaper();
    void zoomIn();
    void zoomOut();
    void zoomActualSize();
    void zoomToFit();
    void fullScreen();
    void about();
    void aboutQt();
    void moveFront();
    void moveBack();
    void moveUp();
    void moveDown();

    void btnClicked();
    void updateActions();
    void addDiagram(const QModelIndex & index);

private:
    void setupButtonsLayout(QWidget * pButtonsArea);
    void setupDiagramLibrary(int group);
    QString getWindowTitle(const Document *doc) const;
    Palette *m_palette;
    QAction *m_undoAction;
    QAction *m_redoAction;

    QUndoGroup *m_undoGroup;
    double m_currentScale;
    QMatrix m_originalMatrix;

    ThemeInterface* m_currentTheme;

    static MainWindow* m_instance;
};

#endif // MAINWINDOW_H
