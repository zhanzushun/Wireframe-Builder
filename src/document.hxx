
#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <QWidget>
#include <QGraphicsPolygonItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QAbstractListModel>
#include <QList>
#include <QRect>
#include <QListView>
#include <QFile>
#include <QMap>

QT_FORWARD_DECLARE_CLASS(QUndoStack)
QT_FORWARD_DECLARE_CLASS(QTextStream)

enum DiagramKey
{
    KeyAccordion = 0,
    KeyAlertBox,
    KeyArrow_Line,
    KeyBreadcrumbs,
    KeyBrowserWindow,
    KeyButton,
    KeyButtonBar_TabBar,
    KeyCalendar,
    KeyCallout,
    KeyChartBar,
    KeyChartColumn,
    KeyChartLine,
    KeyChartPie,
    KeyCheckbox,
    KeyCheckboxGroup,
    KeyColorPicker,
    KeyComboBox_PulldownMenu_DropdownList,
    KeyComment_StickyNote,
    KeyCoverFlow,
    KeyDataGrid_Table,
    KeyDateChooser_DatePicker,
    KeyFieldSet_Group_Container,
    KeyFormattingToolbar_RichTextEditorToolbar,
    KeyGeometricShape,
    KeyHelpButton,
    KeyHorzCurlyBrace,
    KeyHorzRule_Separator_Line,
    KeyHorzScrollBar,
    KeyHorzSlider,
    KeyHorzSplitter_Separator_DragBar,
    KeyIcon,
    KeyIconAndTextLabel,
    KeyImage,
    KeyiPhone,
    KeyiPhoneKeyboard,
    KeyiPhoneMenu,
    KeyiPhonePicker,
    KeyLabel_StringOfText,
    KeyLink,
    KeyLinkBar,
    KeyList,
    KeyMenu,
    KeyMenuBar,
    KeyModalScreen_Overlay,
    KeyMultilineButton,
    KeyNumericStepper,
    KeyOnOffSwitch_Toggle,
    KeyParagraphOfText,
    KeyPlaybackControls,
    KeyPointyButton_iPhoneButton,
    KeyProgressBar,
    KeyRadioButton,
    KeyRadioButtonGroup,
    KeyRectangle_Canvas_Panel,
    KeyRedX_XNay,
    KeyScratchOut,
    KeySearchBox,
    KeyStreetMap,
    KeySubtitle,
    KeyTabsBar,
    KeyTagCloud,
    KeyTextArea,
    KeyTextInput_TextField,
    KeyTitle_Headline,
    KeyTooltip_Balloon,
    KeyTreePane,
    KeyVertCurlyBrace,
    KeyVertRule_Separator_Line,
    KeyVertScrollBar,
    KeyVertSlider,
    KeyVertSplitter_Separator_DragBar,
    KeyVertTabs,
    KeyVideoPlayer,
    KeyVolumeSlider,
    KeyWebcam,
    KeyWindow_Dialog,
    KeyLast,
};

enum DiagramGroup
{
    GroupBig = 0x01,
    GroupButtons = 0x02,
    GroupCommon = 0x04,
    GroupContainers = 0x08,
    GroupLayout = 0x10,
    GroupMarkup = 0x20,
    GroupMedia = 0x40,
    GroupText = 0x80,
    GroupiPhone = 0x100,
    GroupNone = 0x200,
    GroupAll = 0xfff,
};

///////////////////////////////////////////////////////////////////////////////

class DiagramLibraryView : public QListView
{
    Q_OBJECT
public:
    DiagramLibraryView(QWidget * parent = 0) : QListView(parent) {}
};

///////////////////////////////////////////////////////////////////////////////

class DiagramLibrary : public QAbstractListModel
{
    Q_OBJECT

public:
    DiagramLibrary(QObject *parent = 0);

    static const QString& diagramTypeFromKey(int key);

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    virtual Qt::ItemFlags flags(const QModelIndex &index) const;
    virtual QMimeData *mimeData(const QModelIndexList &indexes) const;
    virtual QStringList mimeTypes() const;

    QList<QString>& imageList() {return m_imageList;}
    QList<DiagramKey>& keyList() {return m_keyList;}

private:
    QList<QString> m_imageList;
    QList<DiagramKey> m_keyList;
};

///////////////////////////////////////////////////////////////////////////////

class Grip : public QGraphicsRectItem
{
public:
    enum {Type = UserType + 1};
    virtual int type() const {return Type;}

    enum PosType {NorthMiddle, NorthEast, EastMiddle, SouthEast, 
        SouthMiddle, SouthWest, WestMiddle, NorthWest};
    void setPosType(PosType posType){m_posType = posType;}
    PosType posType(){return m_posType;}

    Grip(qreal len, PosType pos, QGraphicsItem *parent);
    ~Grip();
    void setPos();
protected:
    virtual void hoverEnterEvent (QGraphicsSceneHoverEvent * event);
    virtual void hoverLeaveEvent (QGraphicsSceneHoverEvent * event);
private:
    PosType m_posType;
    qreal m_size;
};

///////////////////////////////////////////////////////////////////////////////

class ResizableItem;

class ResizableItemHelper
{
public:
    enum MouseMode{Move, Resize};

    ResizableItemHelper(ResizableItem* item);
    QSizeF size() {return m_size;}
    void setSize(QSizeF s) {m_size = s;}

    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget);

    bool mousePressEvent(QGraphicsSceneMouseEvent *event);
    bool mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    bool mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void showGrips(bool visible);
private:
    void createGrips();
private:
    ResizableItem* m_item;
    QSizeF m_size;
    MouseMode m_mouseMode;
    QGraphicsRectItem *m_rubberBandRect;
    QList<Grip*> m_grips;
    Grip * m_curGrip;
    QPointF m_lastPoint;
    QPointF m_lastPos;
};

///////////////////////////////////////////////////////////////////////////////

class ResizableItem : public QGraphicsItem
{
public:
    enum {Type = UserType + 2};
    virtual int type() const {return Type;}
    static bool isResizableItem(QGraphicsItem* item) { return item->type() >= ResizableItem::Type;}

    ResizableItem(QGraphicsItem *parent = 0) : QGraphicsItem(parent) {}

    ResizableItemHelper* resizeHelper() {return m_helper;}
    QSizeF size() {return m_helper->size();}
    void setSize(QSizeF s) {m_helper->setSize(s);}
    QRectF posRect() { return QRectF(pos(), size()); }
    void setPosRect(QRectF rc) {setPos(rc.topLeft()); setSize(rc.size());}
    bool locked();
    void setLocked(bool value);


    virtual void setPosRectAndNotify(QRectF rc) {setPosRect(rc);}
    virtual void subPaint(QPainter *, const QStyleOptionGraphicsItem *) {};

    // forward all to helper class
    virtual QRectF boundingRect() const { return m_helper->boundingRect();}
    virtual QPainterPath shape() const { return m_helper->shape(); }
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
    {
        subPaint(painter, option);
        m_helper->paint(painter, option, widget);
    }
protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) 
    {
        if (!m_helper->mousePressEvent(event))
            QGraphicsItem::mousePressEvent(event);
    }
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event)
    {
        if (!m_helper->mouseMoveEvent(event))
            QGraphicsItem::mouseMoveEvent(event);
    }
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
    {
        if (!m_helper->mouseReleaseEvent(event))
            QGraphicsItem::mouseReleaseEvent(event);
    }
protected:
    ResizableItemHelper* m_helper;
};

///////////////////////////////////////////////////////////////////////////////

class ItemDataBase;
class QLineEdit;
class QTextEdit;

class DiagramItem : public ResizableItem
{
public:
    enum {Type = UserType + 3};
    virtual int type() const {return Type;}

    DiagramItem(DiagramKey key, QPointF pos, QGraphicsItem *parent = 0);
    ~DiagramItem();

    DiagramKey key() {return m_key;}
    int id() {return m_id;}
    void setId(int v) {m_id = v;}
    ItemDataBase* itemData() {return m_data;}

    void setSizeAndNotify(QSizeF sz);
    virtual void subPaint(QPainter *painter, const QStyleOptionGraphicsItem *option);
    virtual void setPosRectAndNotify(QRectF rc);
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event);

private:
    DiagramKey m_key;
    ItemDataBase* m_data;
    int m_id;
};

///////////////////////////////////////////////////////////////////////////////

class DiagramItemGroup : public ResizableItem
{
public:
    enum {Type = UserType + 4};
    virtual int type() const {return Type;}

    DiagramItemGroup(QGraphicsItem *parent = 0);
    ~DiagramItemGroup();

    QList<DiagramItem*> diagramItems();
    void resizeChildren(const QRectF &oldPos);
    virtual void subPaint(QPainter *painter, const QStyleOptionGraphicsItem *option);
};

///////////////////////////////////////////////////////////////////////////////

class DiagramScene : public QGraphicsScene
{
    Q_OBJECT

public:
    static void sort(QList<ResizableItem*> & items);
    static void sort(QList<DiagramItem*> & items);

    DiagramScene(QObject *parent = 0);
    
    virtual void drawBackground(QPainter *painter, const QRectF &rect);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);

    bool isGridVisible() {return m_showGrid;}
    void setGridVisible(bool v) {m_showGrid = v;}

    void destroyItemGroup(DiagramItemGroup *group);
    DiagramItemGroup *createItemGroup(const QList<ResizableItem *> &items);
    QList<ResizableItem*> topLevelSortedItems();
    QList<ResizableItem*> selectedSortedItems();
    QList<DiagramItem*> sortedDiagramItems();

    void addItemOnTop(ResizableItem* item);

    void raiseItemMoved(ResizableItem *item, const QPointF &oldPos, const QPointF &newPos);
    void raiseItemResized(ResizableItem *item, const QRectF &oldPos, const QRectF &newPos);
    void raiseBeginEdit(DiagramItem* item);
Q_SIGNALS:
    void itemMoved(ResizableItem *item, const QPointF &oldPos, const QPointF &newPos);
    void itemResized(ResizableItem *item, const QRectF &oldPos, const QRectF &newPos);
    void beginEdit(DiagramItem* item);
    void endEdit();

private:
    bool m_showGrid;
    int m_nextId;
    qreal m_snapOffset;
    qreal m_snapLine;
    bool m_snapHorz;
};

///////////////////////////////////////////////////////////////////////////////

class Document : public QGraphicsView
{
    Q_OBJECT

protected:
    Document(QGraphicsScene* );

public:
    static bool isValidItemsText(const QString& text);
    static QList<DiagramItem*> createItemsByText(const QString& text, QMap<int, QList<ResizableItem*> > & groupMap);
    static QString serializeItemsToText(QList<ResizableItem*> items);

public:
    static Document* createDocument(QObject* parent);
    bool load(QFile &stream);
    void save(QTextStream &stream);

    QString fileName() const;
    void setFileName(const QString &fileName);

    QUndoStack *undoStack() const;
    DiagramScene* scene();
    bool saveImage(const QString &fileName, const char *fileFormat);

Q_SIGNALS:
    void deleteKeyPressed();
protected:
    virtual void keyPressEvent(QKeyEvent *e);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);

public Q_SLOTS:
    void itemMoved(ResizableItem *item, const QPointF &oldPos, const QPointF &newPos);
    void itemResized(ResizableItem *item, const QRectF &oldPos, const QRectF &newPos);
    void beginEdit(DiagramItem* item);
    void endEdit();

    //////////////////////////////////////////////////////////////
    // properties
    void setSelectedIndex(int);
    void setValue(int);
    void setVScrollBar(bool);
    void autoResize();
    void setFontBold(bool);
    void setFontItalic(bool);
    void setFontUnderline(bool);
    void setFontSize(const QString &);
    void setColor();
    void setIcon();

private:
    QString m_fileName;
    QUndoStack *m_undoStack;
    DiagramScene* m_scene;
    QLineEdit* m_stextEdit;
    QTextEdit* m_mtextEdit;
    DiagramItem* m_editingItem;
};

#endif // DOCUMENT_H
