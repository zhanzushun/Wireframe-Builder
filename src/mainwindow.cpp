#include <QtGui>
#include <QtWidgets>
#include "mainwindow.hxx"

#include "document.hxx"
#include "commands.h"
#include "flowlayout.h"
#include "palette.hxx"
#include "itemdata.hxx"

#define ADD_DIAGRAM_TO_LIB(key, flag, image) \
    if ((group & (flag)) > 0)\
    {\
        lib->keyList().append(key);\
        lib->imageList().append(":/icons/"#image".png");\
    }

MainWindow* MainWindow::m_instance = NULL;
static ThemeStyleSheet g_theme;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    m_instance = this;
    setupUi(this);
    m_palette = Palette::createPalette(this);
    setupButtonsLayout(widgetButtonsArea);
    setupDiagramLibrary(GroupAll);
    dockWidgetNeverShow->setVisible(false);
    m_undoGroup = new QUndoGroup(this);
    m_currentTheme = &g_theme;

    QWidget *w = documentTabs->widget(0);
    documentTabs->removeTab(0);
    delete w;

    // file menu and toolbar
    actionOpen->setShortcut(QString("Ctrl+O"));
    actionClose->setShortcut(QString("Ctrl+W"));
    actionNew->setShortcut(QString("Ctrl+N"));
    actionSave->setShortcut(QString("Ctrl+S"));
    actionExit->setShortcut(QString("Ctrl+Q"));
    connect(actionOpen, SIGNAL(triggered()), this, SLOT(openDocument()));
    connect(actionClose, SIGNAL(triggered()), this, SLOT(closeDocument()));
    connect(actionNew, SIGNAL(triggered()), this, SLOT(newDocument()));
    connect(actionSave, SIGNAL(triggered()), this, SLOT(saveDocument()));
    connect(actionExit, SIGNAL(triggered()), this, SLOT(close()));
    connect(actionSave_as_PNG, SIGNAL(triggered()), this, SLOT(saveAsPng()));
    connect(actionSave_as_PDF, SIGNAL(triggered()), this, SLOT(saveAsPdf()));
    connect(actionSave_as_XML, SIGNAL(triggered()), this, SLOT(saveAsXml()));

    // edit menu and toolbar
    QAction *m_undoAction = m_undoGroup->createUndoAction(this);
    QAction *m_redoAction = m_undoGroup->createRedoAction(this);
    m_undoAction->setIcon(QIcon(":/icons/undo.png"));
    m_redoAction->setIcon(QIcon(":/icons/redo.png"));
    menuEdit->insertAction(menuEdit->actions().at(0), m_undoAction);
    menuEdit->insertAction(m_undoAction, m_redoAction);
    toolBar->addAction(m_undoAction);
    toolBar->addAction(m_redoAction);

    connect(actionCut, SIGNAL(triggered()), this, SLOT(cut()));
    connect(actionCopy, SIGNAL(triggered()), this, SLOT(copy()));
    connect(actionPaste, SIGNAL(triggered()), this, SLOT(paste()));
    connect(actionDelete, SIGNAL(triggered()), this, SLOT(deleteObjects()));
    connect(actionSelectAll, SIGNAL(triggered()), this, SLOT(selectAll()));
    connect(actionSelect_None, SIGNAL(triggered()), this, SLOT(selectNone()));
    connect(actionGroup_Objects, SIGNAL(triggered()), this, SLOT(groupObjects()));
    connect(actionUngroup, SIGNAL(triggered()), this, SLOT(ungroup()));
    connect(actionLock, SIGNAL(triggered()), this, SLOT(lock()));
    connect(actionUnlock_All, SIGNAL(triggered()), this, SLOT(unlockAll()));

    // view 
    connect(actionShow_UI_Library, SIGNAL(triggered()), this, SLOT(showLibrary()));
    connect(actionShow_Paper, SIGNAL(triggered()), this, SLOT(showPaper()));
    connect(actionZoom_In, SIGNAL(triggered()), this, SLOT(zoomIn()));
    connect(actionZoom_Out, SIGNAL(triggered()), this, SLOT(zoomOut()));
    connect(actionZoom_1_1, SIGNAL(triggered()), this, SLOT(zoomActualSize()));
    connect(actionZoom_To_Fit, SIGNAL(triggered()), this, SLOT(zoomToFit()));
    connect(actionFull_Screen, SIGNAL(triggered()), this, SLOT(fullScreen()));

    // Help
    connect(actionAbout, SIGNAL(triggered()), this, SLOT(about()));
    connect(actionAboutQt, SIGNAL(triggered()), this, SLOT(aboutQt()));

    // buttons
    connect(btnAll, SIGNAL(clicked(bool)), this, SLOT(btnClicked()));
    connect(btnBig, SIGNAL(clicked(bool)), this, SLOT(btnClicked()));
    connect(btnButtons, SIGNAL(clicked(bool)), this, SLOT(btnClicked()));
    connect(btnCommon, SIGNAL(clicked(bool)), this, SLOT(btnClicked()));
    connect(btnContainers, SIGNAL(clicked(bool)), this, SLOT(btnClicked()));
    connect(btnIPhone, SIGNAL(clicked(bool)), this, SLOT(btnClicked()));
    connect(btnLayout, SIGNAL(clicked(bool)), this, SLOT(btnClicked()));
    connect(btnMarkup, SIGNAL(clicked(bool)), this, SLOT(btnClicked()));
    connect(btnMedia, SIGNAL(clicked(bool)), this, SLOT(btnClicked()));
    connect(btnText, SIGNAL(clicked(bool)), this, SLOT(btnClicked()));

    // palette
    connect(m_palette->btnUndo, SIGNAL(clicked(bool)), m_undoAction, SLOT(trigger()));
    connect(m_palette->btnRedo, SIGNAL(clicked(bool)), m_redoAction, SLOT(trigger()));
    connect(m_palette->btnCut, SIGNAL(clicked(bool)), actionCut, SLOT(trigger()));
    connect(m_palette->btnCopy, SIGNAL(clicked(bool)), actionCopy, SLOT(trigger()));
    connect(m_palette->btnPaste, SIGNAL(clicked(bool)), actionPaste, SLOT(trigger()));
    connect(m_palette->btnDelete, SIGNAL(clicked(bool)), actionDelete, SLOT(trigger()));
    connect(m_palette->btnGroup, SIGNAL(clicked(bool)), actionGroup_Objects, SLOT(trigger()));
    connect(m_palette->btnUngroup, SIGNAL(clicked(bool)), actionUngroup, SLOT(trigger()));
    connect(m_palette->btnLock, SIGNAL(clicked(bool)), actionLock, SLOT(trigger()));
    connect(m_palette->btnFront, SIGNAL(clicked(bool)), this, SLOT(moveFront()));
    connect(m_palette->btnBack, SIGNAL(clicked(bool)), this, SLOT(moveBack()));
    connect(m_palette->btnUp, SIGNAL(clicked(bool)), this, SLOT(moveUp()));
    connect(m_palette->btnDown, SIGNAL(clicked(bool)), this, SLOT(moveDown()));

    // tabs, libraryView
    connect(dockDiagramLibrary, SIGNAL(visibilityChanged(bool)), 
        actionShow_UI_Library, SLOT(setChecked(bool)));
    connect(documentTabs, SIGNAL(currentChanged(int)), this, SLOT(updateActions()));
    connect(diagramLibraryView, SIGNAL(doubleClicked(const QModelIndex &)), this, 
        SLOT(addDiagram(const QModelIndex &)));
    undoView->setGroup(m_undoGroup);
    undoView->setCleanIcon(QIcon(":/icons/ok.png"));

    newDocument();
    updateActions();
    setWindowState(Qt::WindowMaximized);
};

void MainWindow::setupButtonsLayout(QWidget * pParent)
{
    FlowLayout *newLayout = new FlowLayout(2,2,2);
    foreach (const QObject* child, pParent->children())
        newLayout->addWidget((QWidget*)child);
    pParent->setLayout(newLayout);
}

void MainWindow::setupDiagramLibrary(int group)
{
    QAbstractItemModel * oldLib = diagramLibraryView->model();
    DiagramLibrary* lib = new DiagramLibrary(this);
    
    diagramLibraryView->setDragEnabled(true);
    diagramLibraryView->setAcceptDrops(false);
    diagramLibraryView->setModel(lib);
    delete oldLib;

    ADD_DIAGRAM_TO_LIB(KeyAccordion, GroupLayout, accordion)
    ADD_DIAGRAM_TO_LIB(KeyAlertBox, GroupiPhone, alertbox)
    ADD_DIAGRAM_TO_LIB(KeyArrow_Line, GroupMarkup, arrow)
    ADD_DIAGRAM_TO_LIB(KeyBreadcrumbs, GroupText, breadcrumbs)
    ADD_DIAGRAM_TO_LIB(KeyBrowserWindow, GroupBig | GroupCommon | GroupContainers, browserwindow)
    ADD_DIAGRAM_TO_LIB(KeyButton, GroupButtons | GroupCommon, button)
    ADD_DIAGRAM_TO_LIB(KeyButtonBar_TabBar, GroupButtons | GroupLayout, buttonbar)
    ADD_DIAGRAM_TO_LIB(KeyCalendar, GroupNone,calendar)
    ADD_DIAGRAM_TO_LIB(KeyCallout, GroupMarkup, callout)
    ADD_DIAGRAM_TO_LIB(KeyChartBar, GroupBig, chartbar)
    ADD_DIAGRAM_TO_LIB(KeyChartColumn, GroupBig, chartcolumn)
    ADD_DIAGRAM_TO_LIB(KeyChartLine, GroupBig, chartline)
    ADD_DIAGRAM_TO_LIB(KeyChartPie, GroupBig, chartpie)
    ADD_DIAGRAM_TO_LIB(KeyCheckbox, GroupButtons | GroupCommon, checkbox)
    ADD_DIAGRAM_TO_LIB(KeyCheckboxGroup, GroupButtons | GroupCommon, checkboxgroup)
    ADD_DIAGRAM_TO_LIB(KeyColorPicker, GroupButtons, colorpicker)
    ADD_DIAGRAM_TO_LIB(KeyComboBox_PulldownMenu_DropdownList, GroupButtons | GroupCommon | GroupText, combobox)
    ADD_DIAGRAM_TO_LIB(KeyComment_StickyNote, GroupMarkup, comment)
    ADD_DIAGRAM_TO_LIB(KeyCoverFlow, GroupBig | GroupMedia, coverflow)
    ADD_DIAGRAM_TO_LIB(KeyDataGrid_Table, GroupText, datagrid_table)
    ADD_DIAGRAM_TO_LIB(KeyDateChooser_DatePicker, GroupButtons, datechooser)
    ADD_DIAGRAM_TO_LIB(KeyFieldSet_Group_Container, GroupBig | GroupContainers | GroupLayout, fieldset)
    ADD_DIAGRAM_TO_LIB(KeyFormattingToolbar_RichTextEditorToolbar, GroupBig, formattingtoolbar)
    ADD_DIAGRAM_TO_LIB(KeyGeometricShape, GroupMarkup, geometricshape)
    ADD_DIAGRAM_TO_LIB(KeyHelpButton, GroupButtons, helpbutton)
    ADD_DIAGRAM_TO_LIB(KeyHorzCurlyBrace, GroupMarkup, horzcurlybrace)
    ADD_DIAGRAM_TO_LIB(KeyHorzRule_Separator_Line, GroupLayout, horzrule)
    ADD_DIAGRAM_TO_LIB(KeyHorzScrollBar, GroupLayout, horzscrollbar)
    ADD_DIAGRAM_TO_LIB(KeyHorzSlider, GroupMedia, horzslider)
    ADD_DIAGRAM_TO_LIB(KeyHorzSplitter_Separator_DragBar, GroupLayout, horzsplitter)
    ADD_DIAGRAM_TO_LIB(KeyIcon, GroupCommon | GroupMedia, icon)
    ADD_DIAGRAM_TO_LIB(KeyIconAndTextLabel, GroupCommon | GroupMedia | GroupText, iconandtextlabel)
    ADD_DIAGRAM_TO_LIB(KeyImage, GroupBig | GroupCommon | GroupMedia, image)
    ADD_DIAGRAM_TO_LIB(KeyiPhone, GroupBig | GroupiPhone, GroupiPhone)
    ADD_DIAGRAM_TO_LIB(KeyiPhoneKeyboard, GroupiPhone, iphonekeyboard)
    ADD_DIAGRAM_TO_LIB(KeyiPhoneMenu, GroupiPhone, iphonemenu)
    ADD_DIAGRAM_TO_LIB(KeyiPhonePicker, GroupiPhone, iphonepicker)
    ADD_DIAGRAM_TO_LIB(KeyLabel_StringOfText, GroupCommon | GroupText, label)
    ADD_DIAGRAM_TO_LIB(KeyLink, GroupCommon | GroupText, link)
    ADD_DIAGRAM_TO_LIB(KeyLinkBar, GroupLayout | GroupText, linkbar)
    ADD_DIAGRAM_TO_LIB(KeyList, GroupText, list)
    ADD_DIAGRAM_TO_LIB(KeyMenu, GroupText, menu)
    ADD_DIAGRAM_TO_LIB(KeyMenuBar, GroupCommon | GroupText, menubar)
    ADD_DIAGRAM_TO_LIB(KeyModalScreen_Overlay, GroupBig, modalscreen)
    ADD_DIAGRAM_TO_LIB(KeyMultilineButton, GroupBig | GroupButtons, multilinebutton)
    ADD_DIAGRAM_TO_LIB(KeyNumericStepper, GroupButtons | GroupText, numericstepper)
    ADD_DIAGRAM_TO_LIB(KeyOnOffSwitch_Toggle, GroupButtons | GroupiPhone, onoffswitch)
    ADD_DIAGRAM_TO_LIB(KeyParagraphOfText, GroupCommon | GroupText, paragraphoftext)
    ADD_DIAGRAM_TO_LIB(KeyPlaybackControls, GroupButtons | GroupMedia, playbackcontrols)
    ADD_DIAGRAM_TO_LIB(KeyPointyButton_iPhoneButton, GroupButtons | GroupiPhone, pointybutton)
    ADD_DIAGRAM_TO_LIB(KeyProgressBar, GroupMedia, progressbar)
    ADD_DIAGRAM_TO_LIB(KeyRadioButton, GroupButtons | GroupCommon, radiobutton)
    ADD_DIAGRAM_TO_LIB(KeyRadioButtonGroup, GroupButtons | GroupCommon, radiobuttongroup)
    ADD_DIAGRAM_TO_LIB(KeyRectangle_Canvas_Panel, GroupBig | GroupCommon | GroupContainers, rectangle)
    ADD_DIAGRAM_TO_LIB(KeyRedX_XNay, GroupMarkup, redx)
    ADD_DIAGRAM_TO_LIB(KeyScratchOut, GroupMarkup, scratchout)
    ADD_DIAGRAM_TO_LIB(KeySearchBox, GroupText, searchbox)
    ADD_DIAGRAM_TO_LIB(KeyStreetMap, GroupBig | GroupMedia, streetmap)
    ADD_DIAGRAM_TO_LIB(KeySubtitle, GroupText, subtitle)
    ADD_DIAGRAM_TO_LIB(KeyTabsBar, GroupBig | GroupContainers | GroupLayout, tabsbar)
    ADD_DIAGRAM_TO_LIB(KeyTagCloud, GroupText, tagcloud)
    ADD_DIAGRAM_TO_LIB(KeyTextArea, GroupBig | GroupCommon | GroupText, textarea)
    ADD_DIAGRAM_TO_LIB(KeyTextInput_TextField, GroupCommon | GroupText, textinput)
    ADD_DIAGRAM_TO_LIB(KeyTitle_Headline, GroupBig | GroupText, title)
    ADD_DIAGRAM_TO_LIB(KeyTooltip_Balloon, GroupText, tooltip)
    ADD_DIAGRAM_TO_LIB(KeyTreePane, GroupText, treepane)
    ADD_DIAGRAM_TO_LIB(KeyVertCurlyBrace, GroupMarkup, vertcurlybrace)
    ADD_DIAGRAM_TO_LIB(KeyVertRule_Separator_Line, GroupLayout, vertrule)
    ADD_DIAGRAM_TO_LIB(KeyVertScrollBar, GroupCommon | GroupLayout, vertscrollbar)
    ADD_DIAGRAM_TO_LIB(KeyVertSlider, GroupMedia, vertslider)
    ADD_DIAGRAM_TO_LIB(KeyVertSplitter_Separator_DragBar, GroupLayout, vertsplitter)
    ADD_DIAGRAM_TO_LIB(KeyVertTabs, GroupBig | GroupContainers | GroupLayout, verttabs)
    ADD_DIAGRAM_TO_LIB(KeyVideoPlayer, GroupBig | GroupMedia, videoplayer)
    ADD_DIAGRAM_TO_LIB(KeyVolumeSlider, GroupMedia, volumeslider)
    ADD_DIAGRAM_TO_LIB(KeyWebcam, GroupBig | GroupMedia, webcam)
    ADD_DIAGRAM_TO_LIB(KeyWindow_Dialog, GroupBig | GroupCommon | GroupContainers, window)
}

void MainWindow::btnClicked()
{
    int value = sender()->property("value").value<int>();
    setupDiagramLibrary(value);
}

void MainWindow::openDocument()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), QString(), 
        tr("Text files (*.txt)"));
    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, tr("File error"), tr("Failed to open\n%1").arg(fileName));
        return;
    }
    //QTextStream stream(&file);

    Document *doc = Document::createDocument(this);
    if (!doc->load(file)) 
    {
        QMessageBox::warning(this, tr("Parse error"), tr("Failed to parse\n%1").arg(fileName));
        delete doc;
        return;
    }

    doc->setFileName(fileName);
    addDocument(doc);
}

QString MainWindow::getWindowTitle(const Document *doc) const
{
    QString title = doc->fileName();

    if (title.isEmpty())
        title = tr("Unnamed");
    else
        title = QFileInfo(title).fileName();

    QString result;

    for (int i = 0; ; ++i) {
        result = title;
        if (i > 0)
            result += QString::number(i);

        bool unique = true;
        for (int j = 0; j < documentTabs->count(); ++j) {
            const QWidget *widget = documentTabs->widget(j);
            if (widget == doc)
                continue;
            if (result == documentTabs->tabText(j)) {
                unique = false;
                break;
            }
        }
        if (unique)
            break;
    }
    return result;
}

void MainWindow::addDocument(Document *doc)
{
    if (documentTabs->indexOf(doc) != -1)
        return;
    m_undoGroup->addStack(doc->undoStack());
    documentTabs->addTab(doc, getWindowTitle(doc));
    connect(doc->undoStack(), SIGNAL(indexChanged(int)), this, SLOT(updateActions()));
    connect(doc->undoStack(), SIGNAL(cleanChanged(bool)), this, SLOT(updateActions()));
    connect(doc->undoStack(), SIGNAL(canUndoChanged(bool)), m_palette->btnUndo, SLOT(setEnabled(bool)));
    connect(doc->undoStack(), SIGNAL(canRedoChanged(bool)), m_palette->btnRedo, SLOT(setEnabled(bool)));
    connect(doc->scene(), SIGNAL(selectionChanged()), this, SLOT(updateActions()));
    connect(doc, SIGNAL(deleteKeyPressed()), this, SLOT(deleteObjects()));
    setCurrentDocument(doc);
    m_currentScale = 1;
    m_originalMatrix = doc->matrix();
}

void MainWindow::setCurrentDocument(Document *doc)
{
    documentTabs->setCurrentWidget(doc);
}

Document *MainWindow::currentDocument() const
{
    return qobject_cast<Document*>(documentTabs->currentWidget());
}

void MainWindow::removeDocument(Document *doc)
{
    int index = documentTabs->indexOf(doc);
    if (index == -1)
        return;

    documentTabs->removeTab(index);
    m_undoGroup->removeStack(doc->undoStack());
    disconnect(doc->undoStack(), SIGNAL(indexChanged(int)), this, SLOT(updateActions()));
    disconnect(doc->undoStack(), SIGNAL(cleanChanged(bool)), this, SLOT(updateActions()));

    if (documentTabs->count() == 0) {
        newDocument();
        updateActions();
    }
}

void MainWindow::saveDocument()
{
    Document *doc = currentDocument();
    if (doc == 0)
        return;

    for (;;) {
        QString fileName = doc->fileName();

        if (fileName.isEmpty())
            fileName = QFileDialog::getSaveFileName(this, tr("Save File"), QString(), 
                tr("Text files (*.txt)"));
        if (fileName.isEmpty())
            break;

        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly)) {
            QMessageBox::warning(this,
                                tr("File error"),
                                tr("Failed to open\n%1").arg(fileName));
            doc->setFileName(QString());
        } else {
            QTextStream stream(&file);
            doc->save(stream);
            doc->setFileName(fileName);

            int index = documentTabs->indexOf(doc);
            Q_ASSERT(index != -1);
            documentTabs->setTabText(index, getWindowTitle(doc));

            break;
        }
    }
}

void MainWindow::closeDocument()
{
    Document *doc = currentDocument();
    if (doc == 0)
        return;

    if (!doc->undoStack()->isClean()) {
        int button
            = QMessageBox::warning(this,
                            tr("Unsaved changes"),
                            tr("Would you like to save this document?"),
                            QMessageBox::Yes, QMessageBox::No);
        if (button == QMessageBox::Yes)
            saveDocument();
    }

    removeDocument(doc);
    delete doc;
}

void MainWindow::newDocument()
{
    addDocument(Document::createDocument(this));
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About Wireframe Builder"), tr("Wireframe Builder, yet another UI prototyping tool."));
}

void MainWindow::aboutQt()
{
    QMessageBox::aboutQt(this, tr("About Qt"));
}

void MainWindow::addDiagram(const QModelIndex & index)
{
    Document *doc = currentDocument();
    DiagramLibrary* diagramLibrary = qobject_cast<DiagramLibrary*> (diagramLibraryView->model());
    Q_ASSERT(diagramLibrary != NULL);
    if (diagramLibrary == NULL)
        return;
    DiagramKey key = (DiagramKey)(diagramLibrary->data(index, Qt::UserRole).value<int>());
    doc->undoStack()->push(new AddDiagramItemCommand(doc, key));
}

void MainWindow::updateActions()
{
    Document *doc = currentDocument();
    m_undoGroup->setActiveStack(doc == 0 ? 0 : doc->undoStack());
    actionClose->setEnabled(doc != 0);
    actionSave->setEnabled(doc != 0 && !doc->undoStack()->isClean());
    actionSave_as_PNG->setEnabled(doc != 0);

    bool hasSelection = doc != 0 && doc->scene()->selectedSortedItems().length() > 0;
    bool hasSingleSelection = hasSelection && doc->scene()->selectedSortedItems().length() == 1;
    bool hasSingleDiagramItemSelection = hasSingleSelection && (doc->scene()->selectedSortedItems().at(0)->type() == DiagramItem::Type);
    bool hasMultiSelection = doc != 0 && doc->scene()->selectedSortedItems().length() > 1;
    bool hasItems = doc != 0 && doc->scene()->items().length() > 0;
    if (hasSelection)
    {
        int props = 0;
        ItemDataBase* itemData = NULL;
        if (hasSingleDiagramItemSelection)
        {
            itemData = ((DiagramItem*)(doc->scene()->selectedSortedItems().at(0)))->itemData();
            props = itemData->getProperties();
        }
        m_palette->showWindow(true, doc, props, itemData);
    }
    else
    {
        if (m_palette->dockWidget()->isVisible())
            m_palette->showWindow(false, doc);
    }
    actionCut->setEnabled(hasSelection);
    actionCopy->setEnabled(hasSelection);

    QString clipboardText = QApplication::clipboard()->text();
    actionPaste->setEnabled(doc != 0 && Document::isValidItemsText(clipboardText));

    actionDelete->setEnabled(hasSelection);
    actionSelectAll->setEnabled(hasItems);
    actionSelect_None->setEnabled(hasSelection);
    actionGroup_Objects->setEnabled(hasMultiSelection);
    actionUngroup->setEnabled(false);
    
    actionLock->setEnabled(hasSelection);
    actionUnlock_All->setEnabled(hasItems);

    actionShow_Paper->setChecked(doc != 0 && doc->scene()->isGridVisible());

    actionZoom_In->setEnabled(doc != 0);
    actionZoom_Out->setEnabled(doc != 0);
    actionZoom_1_1->setEnabled(doc != 0);

    if (hasSingleSelection)
    {
        QGraphicsItem* item = doc->scene()->selectedSortedItems().at(0);
        if (qgraphicsitem_cast<DiagramItemGroup*> (item) != NULL)
            actionUngroup->setEnabled(true);
    }
    if (doc != 0)
    {
        int index = documentTabs->indexOf(doc);
        Q_ASSERT(index != -1);
        static const QIcon unsavedIcon(":/icons/filesave.png");
        documentTabs->setTabIcon(index, doc->undoStack()->isClean() ? QIcon() : unsavedIcon);
    }

    m_palette->btnCut->setEnabled(actionCut->isEnabled());
    m_palette->btnCopy->setEnabled(actionCopy->isEnabled());
    m_palette->btnPaste->setEnabled(actionPaste->isEnabled());
    m_palette->btnDelete->setEnabled(actionDelete->isEnabled());
    m_palette->btnGroup->setEnabled(actionGroup_Objects->isEnabled());
    m_palette->btnUngroup->setEnabled(actionUngroup->isEnabled());
    m_palette->btnLock->setEnabled(actionLock->isEnabled());
    m_palette->btnFront->setEnabled(hasSelection);
    m_palette->btnBack->setEnabled(hasSelection);
    m_palette->btnUp->setEnabled(hasSingleSelection);
    m_palette->btnDown->setEnabled(hasSingleSelection);

    actionSave_as_PDF->setEnabled(false);
    actionSave_as_XML->setEnabled(false);
    actionZoom_To_Fit->setEnabled(false);
    actionFull_Screen->setEnabled(false);
}

void MainWindow::saveAsPng()
{
    Document *doc = currentDocument();
    if (doc == 0)
        return;

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), QString(), 
        tr("Image files (*.png)"));
    if (fileName.isEmpty())
        return;
    // check access right
    {
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly)) {
            QMessageBox::warning(this,
                tr("File error"),
                tr("Failed to open\n%1").arg(fileName));
            doc->setFileName(QString());
        }
    }
    if (!doc->saveImage(fileName, "PNG"))
    {
        QMessageBox::warning(this,
            tr("File error"),
            tr("Failed to save\n%1").arg(fileName));
    }
}

void MainWindow::saveAsPdf()
{
}
void MainWindow::saveAsXml()
{
}
void MainWindow::cut()
{
    Document *doc = currentDocument();
    if (doc == NULL)
        return;
    QClipboard *clipboard = QApplication::clipboard();
    if (clipboard)
    {
        QMimeData *mimeData = new QMimeData;
        QList<ResizableItem*> items = doc->scene()->selectedSortedItems();
        mimeData->setText(Document::serializeItemsToText(items));
        clipboard->setMimeData(mimeData);
        doc->undoStack()->push(new CutCommand(items, doc->scene()));
    }
    updateActions();
}
void MainWindow::copy()
{
    Document *doc = currentDocument();
    if (doc == NULL)
        return;
    QClipboard *clipboard = QApplication::clipboard();
    if (clipboard)
    {
        QMimeData *mimeData = new QMimeData;
        mimeData->setText(Document::serializeItemsToText(doc->scene()->selectedSortedItems()));
        clipboard->setMimeData(mimeData);
    }
    updateActions();
}

void MainWindow::paste()
{
    Document *doc = currentDocument();
    if (doc == NULL)
        return;
    QClipboard *clipboard = QApplication::clipboard();
    if (clipboard)
    {
        const QMimeData *mimeData = clipboard->mimeData();

        if (mimeData->hasText())
        {
            QMap<int, QList<ResizableItem*> > groupMap;
            QList<DiagramItem*> items = Document::createItemsByText(mimeData->text(), groupMap);
            doc->undoStack()->push(new PasteCommand(items, groupMap, doc->scene()));
        }
    }
    updateActions();
}

void MainWindow::deleteObjects()
{
    Document *doc = currentDocument();
    if (doc == NULL)
        return;
    doc->undoStack()->push(new RemoveDiagramItemsCommand(doc, doc->scene()->selectedSortedItems()));
    updateActions();
}

void MainWindow::selectAll()
{
    Document *doc = currentDocument();
    if (doc == NULL)
        return;
    foreach(ResizableItem* item, doc->scene()->topLevelSortedItems())
        item->setSelected(true);
    updateActions();
}
void MainWindow::selectNone()
{
    Document *doc = currentDocument();
    if (doc == NULL)
        return;
    foreach(ResizableItem* item, doc->scene()->topLevelSortedItems())
        item->setSelected(false);
    updateActions();
}
void MainWindow::groupObjects()
{
    Document *doc = currentDocument();
    if (doc == NULL)
        return;
    doc->undoStack()->push(new GroupCommand(doc->scene()->selectedSortedItems(), doc->scene()));
    updateActions();
}
void MainWindow::ungroup()
{
    Document *doc = currentDocument();
    if (doc == NULL)
        return;
    if (doc->scene()->selectedSortedItems().length() > 0)
    {
        ResizableItem* item = doc->scene()->selectedSortedItems().at(0);
        DiagramItemGroup* gitem = qgraphicsitem_cast<DiagramItemGroup*> (item);
        if (gitem != NULL)
            doc->undoStack()->push(new UngroupCommand(gitem));
    }
    updateActions();
}

void MainWindow::lock()
{
    Document *doc = currentDocument();
    if (doc == NULL)
        return;
    doc->undoStack()->push(new LockCommand(doc->scene()->selectedSortedItems()));
    updateActions();
}

void MainWindow::unlockAll()
{
    Document *doc = currentDocument();
    if (doc == NULL)
        return;
    doc->undoStack()->push(new UnlockCommand(doc->scene()->topLevelSortedItems()));
    updateActions();
}

void MainWindow::showLibrary()
{
    if (actionShow_UI_Library->isChecked())
        dockDiagramLibrary->show();
    else
        dockDiagramLibrary->hide();
}
void MainWindow::showPaper()
{
    currentDocument()->scene()->setGridVisible(actionShow_Paper->isChecked());
    currentDocument()->scene()->update();
}
void ScaleView(Document* view, double scale)
{
    QMatrix oldMatrix = view->matrix();
    view->resetMatrix();
    view->translate(oldMatrix.dx(), oldMatrix.dy());
    view->scale(scale, scale);
}

void MainWindow::zoomIn()
{
    Document *view = currentDocument();
    if (view == NULL)
        return;
    double newScale = m_currentScale * 1.25;
    ScaleView(view, newScale);
    m_currentScale = newScale;
}

void MainWindow::zoomOut()
{
    Document *view = currentDocument();
    if (view == NULL)
        return;
    double newScale = m_currentScale * 0.8;
    ScaleView(view, newScale);
    m_currentScale = newScale;
}

void MainWindow::zoomActualSize()
{
    Document *view = currentDocument();
    if (view == NULL)
        return;
    view->resetMatrix();
    view->translate(m_originalMatrix.dx(), m_originalMatrix.dy());
}

void MainWindow::zoomToFit()
{
    //todo
}
void MainWindow::fullScreen()
{
    //todo
}

void MainWindow::moveFront()
{
    Document *doc = currentDocument();
    if (doc == NULL)
        return;
    doc->undoStack()->push(new MoveFrontCommand(doc->scene()->selectedSortedItems()));
    updateActions();
}

void MainWindow::moveBack()
{
    Document *doc = currentDocument();
    if (doc == NULL)
        return;
    doc->undoStack()->push(new MoveBackCommand(doc->scene()->selectedSortedItems()));
    updateActions();
}

void MainWindow::moveUp()
{
    Document *doc = currentDocument();
    if (doc == NULL)
        return;
    if (doc->scene()->selectedSortedItems().length() != 1)
        return;
    doc->undoStack()->push(new MoveUpCommand(doc->scene()->selectedSortedItems()[0]));
    updateActions();
}

void MainWindow::moveDown()
{
    Document *doc = currentDocument();
    if (doc == NULL)
        return;
    if (doc->scene()->selectedSortedItems().length() != 1)
        return;
    doc->undoStack()->push(new MoveDownCommand(doc->scene()->selectedSortedItems()[0]));
    updateActions();
}
