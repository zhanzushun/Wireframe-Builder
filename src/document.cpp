#include <QtGui>
#include <QtCore>
#include <QtWidgets>
#include <QtXml>
#include "document.hxx"
#include "commands.h"
#include "itemdata.hxx"

const qreal GRIPSIZE = 6.0;
const qreal MIN_SIZE = 20.0;

QString s_empty;
QString s_key[] = {
    "Accordion",
    "Alert Box",
    "Arrow Line",
    "Breadcrumbs",
    "Browser Window",
    "Button",
    "ButtonBar TabBar",
    "Calendar",
    "Callout",
    "Bar Chart",
    "Column Chart",
    "Line Chart",
    "Pie Chart",
    "Checkbox",
    "Checkbox Group",
    "Color Picker",
    "ComboBox",
    "Comment",
    "Cover Flow",
    "Data Grid",
    "Date Chooser",
    "Field Set",
    "Formatting Toolbar",
    "Geometric Shape",
    "Help Button",
    "Horizontal Curly Brace",
    "Horizontal Rule",
    "Horizontal Scrollbar",
    "Horizontal Slider",
    "Horizontal Splitter",
    "Icon",
    "Icon And Text Label",
    "Image",
    "iPhone",
    "iPhone Keyboard",
    "iPhone Menu",
    "iPhone Picker",
    "Label",
    "Link",
    "Link Bar",
    "List",
    "Menu",
    "Menu Bar",
    "Modal Screen",
    "Multiline Button",
    "Numeric Stepper",
    "On Off Switch",
    "Paragraph Of Text",
    "Playback Controls",
    "Pointy Button",
    "Progress Bar",
    "Radio Button",
    "Radio Button Group",
    "Rectangle",
    "Red X",
    "Scratch Out",
    "Search Box",
    "Street Map",
    "Subtitle",
    "Tabs Bar",
    "Tag Cloud",
    "Text Area",
    "Text Input",
    "Title",
    "Tooltip",
    "Tree Pane",
    "Vertical Curly Brace",
    "Vertical Rule",
    "Vertical Scroll Bar",
    "Vertical Slider",
    "Vertical Splitter",
    "Vertical Tabs",
    "Video Player",
    "Volume Slider",
    "Webcam",
    "Window",
};

const QString& DiagramLibrary::diagramTypeFromKey(int key)
{
    if (key >= 0 && key < (int)KeyLast)
        return s_key[key];
    else
        return s_empty;
}

DiagramLibrary::DiagramLibrary(QObject *parent)
    : QAbstractListModel(parent)
{
    setSupportedDragActions(Qt::CopyAction);
}

int DiagramLibrary::rowCount(const QModelIndex & /* parent */) const
{
    return m_keyList.count();
}

QVariant DiagramLibrary::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    
    if (index.row() >= m_keyList.count() || index.row() < 0)
        return QVariant();
    
    if (role == Qt::DecorationRole)
        return QPixmap(m_imageList.at(index.row()));
    else if (role == Qt::UserRole)
        return (int)m_keyList.at(index.row());
    return QVariant();
}

Qt::ItemFlags DiagramLibrary::flags(const QModelIndex &index) const
{
    Qt::ItemFlags defaultFlags = QAbstractListModel::flags(index);
    if (index.isValid())
        return Qt::ItemIsDragEnabled | defaultFlags;
    return defaultFlags;
}

QMimeData* DiagramLibrary::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *mimeData = new QMimeData();
    QByteArray encodedData;

    QDataStream stream(&encodedData, QIODevice::WriteOnly);

    foreach (QModelIndex index, indexes) 
    {
        if (index.isValid()) {
            stream << data(index, Qt::UserRole).value<int>();
        }
    }
    mimeData->setData("application/diagram.library.key", encodedData);
    return mimeData;
}

QStringList DiagramLibrary::mimeTypes() const
{
    QStringList types;
    types << "application/diagram.library.key";
    return types;
}

///////////////////////////////////////////////////////////////////////////////

Grip::Grip(qreal len, PosType pos, QGraphicsItem *parent)
    :QGraphicsRectItem(0,0,len,len,parent), m_posType(pos), m_size(len)
{
    setAcceptsHoverEvents(true);
    setVisible(false);
    setPos();
}

Grip::~Grip()
{
}

void Grip::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    switch(m_posType)
    {
    case NorthMiddle:
    case SouthMiddle:
        setCursor(Qt::SizeVerCursor);
        break;
    case EastMiddle:
    case WestMiddle:
        setCursor(Qt::SizeHorCursor);
        break;
    case NorthWest:
    case SouthEast:
        setCursor(Qt::SizeFDiagCursor);
        break;
    case NorthEast:
    case SouthWest:
        setCursor(Qt::SizeBDiagCursor);
        break;
    default:
        break;
    }
    QGraphicsRectItem::hoverEnterEvent(event);
}

void Grip::hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{
    QGraphicsRectItem::hoverLeaveEvent(event);
}

void Grip::setPos()
{
    const QRectF parentRect = parentItem()->boundingRect();
    qreal x = 0, y = 0;
    switch(m_posType){
    case NorthMiddle:
        x = parentRect.width() / 2 - m_size / 2;
        y = 0;
        break;
    case SouthMiddle:
        x = parentRect.width() / 2 - m_size / 2;
        y = parentRect.height() - m_size;
        break;
    case EastMiddle:
        x = parentRect.width() - m_size;
        y = parentRect.height() / 2 - m_size / 2;
        break;
    case WestMiddle:
        x = 0;
        y = parentRect.height() / 2 - m_size / 2;
        break;
    case NorthWest:
        x = 0;
        y = 0;
        break;
    case SouthEast:
        x = parentRect.width() - m_size;
        y = parentRect.height() - m_size;
        break;
    case NorthEast:
        x = parentRect.width() - m_size;
        y = 0;
        break;
    case SouthWest:
        x = 0;
        y = parentRect.height() - m_size;
        break;
    default:
        break;
    }
    QGraphicsItem::setPos(x - m_size, y - m_size);
}

///////////////////////////////////////////////////////////////////////////////

ResizableItemHelper::ResizableItemHelper(ResizableItem* item)
{
    m_item = item;
}

QRectF ResizableItemHelper::boundingRect() const
{
    if (m_item->isSelected())
        return QRectF(-GRIPSIZE, -GRIPSIZE, m_size.width() + GRIPSIZE*2, m_size.height() + GRIPSIZE*2);
    else
        return QRectF(0, 0, m_size.width(), m_size.height());
}

QPainterPath ResizableItemHelper::shape() const
{
    QPainterPath path;
    path.addRect(boundingRect());
    return path;
}

void ResizableItemHelper::paint(QPainter*, const QStyleOptionGraphicsItem *option, QWidget *)
{
    if (option->state & QStyle::State_Selected)
        showGrips(true);
    else
        showGrips(false);
}

bool ResizableItemHelper::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    m_curGrip = qgraphicsitem_cast<Grip*>(m_item->scene()->itemAt(event->scenePos()));
    if(m_curGrip)
    {
        m_mouseMode = Resize;
        m_lastPoint.setX(event->scenePos().x());
        m_lastPoint.setY(event->scenePos().y());
        m_rubberBandRect = new QGraphicsRectItem();
        m_rubberBandRect->setPen(QPen(Qt::DashLine));
        m_rubberBandRect->setParentItem(m_item);
        m_rubberBandRect->setRect(0, 0, m_size.width(),m_size.height());
        return true;
    }
    else
    {
        m_mouseMode = Move;
        m_lastPos = m_item->pos();
        return false;
    }
}
bool ResizableItemHelper::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(m_mouseMode == Resize)
    {
        QPointF curPoint(event->scenePos());
        qreal wChanging = curPoint.x()-m_lastPoint.x(), hChanging = curPoint.y()-m_lastPoint.y();
        qreal curX = 0, curY = 0, curWidth = m_size.width(), curHeight = m_size.height();
        switch(m_curGrip->posType())
        {
        case Grip::NorthMiddle:
            curY += hChanging; curHeight-=hChanging;
            break;
        case Grip::SouthMiddle:
            curHeight+=hChanging;
            break;
        case Grip::EastMiddle:
            curWidth+=wChanging;
            break;
        case Grip::WestMiddle:
            curX+=wChanging; curWidth-=wChanging;
            break;
        case Grip::NorthWest:
            curX+=wChanging; curY+=hChanging; curWidth-=wChanging; curHeight-=hChanging;
            break;
        case Grip::SouthEast:
            curWidth+=wChanging; curHeight+=hChanging;
            break;
        case Grip::NorthEast:
            curY+=hChanging; curWidth+=wChanging; curHeight-=hChanging;
            break;
        case Grip::SouthWest:
            curX+=wChanging; curWidth-=wChanging; curHeight+=hChanging;
            break;
        default:
            break;
        }
        if(curWidth < MIN_SIZE ||curHeight <MIN_SIZE)
            return true;
        m_rubberBandRect->setRect(curX,curY,curWidth,curHeight);
        return true;
    }
    return false;
}
bool ResizableItemHelper::mouseReleaseEvent(QGraphicsSceneMouseEvent *)
{
    if(m_mouseMode == Resize)
    {
        QRectF oldPos = QRectF(m_item->pos(), size());

        qreal curX = m_rubberBandRect->rect().left();
        qreal curY = m_rubberBandRect->rect().top();
        QPointF curPos(curX,curY);
        m_item->setPos(m_item->mapToScene(curPos));
        m_size.setWidth(m_rubberBandRect->rect().width());
        m_size.setHeight(m_rubberBandRect->rect().height());

        if (oldPos != m_rubberBandRect->rect())
            ((DiagramScene*)m_item->scene())->raiseItemResized(m_item, oldPos, QRectF(m_item->pos(), m_size));
        m_item->scene()->update();
        delete m_rubberBandRect;
        return true;
    }
    else
    {
        if (m_item->pos() != m_lastPos)
            ((DiagramScene*)m_item->scene())->raiseItemMoved(m_item, m_lastPos, m_item->pos());
        return false;
    }
}
void ResizableItemHelper::showGrips(bool visible)
{
    ResizeMode mode = ResizeModeAll;
    if (m_item->type() == DiagramItem::Type)
        mode = ((DiagramItem*)m_item)->itemData()->resizeMode();
    if (mode == ResizeModeNone)
        return;

    if (m_grips.length() == 0)
        createGrips();

    for(int i = 0; i < m_grips.count(); i++)
    {
        Grip::PosType type = m_grips.at(i)->posType();
        if (mode == ResizeModeHorz)
        {
            if (type != Grip::EastMiddle && type != Grip::WestMiddle)
                continue;
        }
        if (mode == ResizeModeVert)
        {
            if (type != Grip::NorthMiddle && type != Grip::SouthMiddle)
                continue;
        }
        m_grips.at(i)->setPos();
        m_grips.at(i)->setVisible(visible);
    }
}

void ResizableItemHelper::createGrips()
{
    Grip *north_middle = new Grip(GRIPSIZE,Grip::NorthMiddle,m_item);
    m_grips.append(north_middle);
    Grip *north_east = new Grip(GRIPSIZE,Grip::NorthEast,m_item);
    m_grips.append(north_east);
    Grip *north_west = new Grip(GRIPSIZE,Grip::NorthWest,m_item);
    m_grips.append(north_west);
    Grip *south_middle = new Grip(GRIPSIZE,Grip::SouthMiddle,m_item);
    m_grips.append(south_middle);
    Grip *south_east = new Grip(GRIPSIZE,Grip::SouthEast,m_item);
    m_grips.append(south_east);
    Grip *south_west = new Grip(GRIPSIZE,Grip::SouthWest,m_item);
    m_grips.append(south_west);
    Grip *east_middle = new Grip(GRIPSIZE,Grip::EastMiddle,m_item);
    m_grips.append(east_middle);
    Grip *west_middle = new Grip(GRIPSIZE,Grip::WestMiddle,m_item);
    m_grips.append(west_middle);
}

///////////////////////////////////////////////////////////////////////////////

void ResizableItem::setLocked(bool v)
{
    if (v)
    {
        setFlags (flags() & ~(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable
            | QGraphicsItem::ItemIsFocusable));
    }
    else
    {
        setFlags (flags() | QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable
            | QGraphicsItem::ItemIsFocusable);
    }
}

bool ResizableItem::locked()
{
    return ((flags() & QGraphicsItem::ItemIsSelectable) == 0);
}


///////////////////////////////////////////////////////////////////////////////

DiagramItem::DiagramItem(DiagramKey key, QPointF pos, QGraphicsItem *parent) 
    : ResizableItem(parent)
{
    setAcceptDrops(true);
    setAcceptsHoverEvents(true);
    setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable
        | QGraphicsItem::ItemIsFocusable);
    m_key = key;
    m_helper = new ResizableItemHelper(this);
    setPos(pos.x(), pos.y());
    switch (m_key)
    {
    case KeyAccordion:
        m_data = new Accordion(this);
        break;
    case KeyAlertBox:
        m_data = new AlertBox(this);
        break;
    case KeyCalendar:
    case KeyChartBar:
    case KeyChartColumn:
    case KeyChartLine:
    case KeyChartPie:
    case KeyFormattingToolbar_RichTextEditorToolbar:
    case KeyHorzSplitter_Separator_DragBar:
    case KeyPlaybackControls:
    case KeyRedX_XNay:
    case KeyStreetMap:
    case KeyVertSplitter_Separator_DragBar:
    case KeyVolumeSlider:
    case KeyWebcam:
        m_data = new ImageItemData(this);
        break;
    case KeyBreadcrumbs:
        m_data = new Breadcrumbs(this);
        break;
    case KeyBrowserWindow:
        m_data = new BrowserWindow(this);
        break;
    case KeyButton:
        m_data = new Button(this);
        break;
    default: 
        m_data = new NotImplementedYet(this);
        break;
    }
    m_data->init();
    setSize(m_data->mesuredSize());
}

DiagramItem::~DiagramItem()
{
    delete m_helper;
    delete m_data;
}

void DiagramItem::subPaint(QPainter *painter, const QStyleOptionGraphicsItem *option)
{
    if (m_data != NULL)
        m_data->paint(painter, option);
}

void DiagramItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *)
{
    ((DiagramScene*)scene())->raiseBeginEdit(this);
    if (m_data != NULL)
    {
        //bool ok;
        //QString str; 
        //m_data->save(str);
        //QString text = QInputDialog::getText(0, "set properties",
        //    "Do not change controlId, controlTypeId", QLineEdit::Normal,
        //    str, &ok);
        //if (ok && !text.isEmpty())
        //    m_data->load(text);
    }
}

void DiagramItem::setPosRectAndNotify(QRectF rc)
{
    setPosRect(rc); 
    if (m_data) m_data->posRect_changed();
}

void DiagramItem::setSizeAndNotify(QSizeF sz)
{
    setSize(sz);
    if (m_data) m_data->posRect_changed();
}

///////////////////////////////////////////////////////////////////////////////

DiagramItemGroup::DiagramItemGroup(QGraphicsItem *parent) : ResizableItem(parent)
{
    setAcceptDrops(true);
    setAcceptsHoverEvents(true);
    setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable
        | QGraphicsItem::ItemIsFocusable);
    m_helper = new ResizableItemHelper(this); // will set later.
}

DiagramItemGroup::~DiagramItemGroup()
{
    delete m_helper;
}

QList<DiagramItem*> DiagramItemGroup::diagramItems()
{
    QList<DiagramItem*> r;
    foreach(QGraphicsItem* item, children())
    {
        if (item->type() == DiagramItem::Type)
        {
            r.append((DiagramItem*)item);
        }
    }
    DiagramScene::sort(r);
    return r;
}

void DiagramItemGroup::resizeChildren(const QRectF &oldPos)
{
    qreal wRatio = posRect().width() / oldPos.width();
    qreal hRatio = posRect().height() / oldPos.height();

    foreach (DiagramItem* ditem, diagramItems())
    {
        QRectF rc = ditem->posRect();
        qreal x = rc.left() * wRatio;
        qreal y = rc.top() * hRatio;
        qreal x2 = rc.right() * wRatio;
        qreal y2 = rc.bottom() * hRatio;
        ditem->setPosRectAndNotify(QRectF(x, y, x2-x, y2-y));
    }
}

void DiagramItemGroup::subPaint(QPainter *, const QStyleOptionGraphicsItem *)
{
}

///////////////////////////////////////////////////////////////////////////////

DiagramScene::DiagramScene(QObject *parent)
    : QGraphicsScene(parent), m_showGrid(true)
{
    m_nextId = 0;
    m_snapOffset = 0;
}

void DiagramScene::drawBackground(QPainter *painter, const QRectF &rect)
{
    if (!m_showGrid)
        return;

    QGraphicsScene::drawBackground(painter, rect);

    const int gridSize = 50;
    const int realLeft = static_cast<int>(floor(rect.left()));
    const int realRight = static_cast<int>(ceil(rect.right()));
    const int realTop = static_cast<int>(floor(rect.top()));
    const int realBottom = static_cast<int>(ceil(rect.bottom()));

    const int firstLeftGridLine = realLeft - (realLeft % gridSize);
    const int firstTopGridLine = realTop - (realTop % gridSize);

    QVarLengthArray<QLine, 100> lines;

    for (qreal x = firstLeftGridLine; x <= realRight; x += gridSize)
        lines.append(QLine(x, realTop, x, realBottom));
    for (qreal y = firstTopGridLine; y <= realBottom; y += gridSize)
        lines.append(QLine(realLeft, y, realRight, y));

    painter->setPen(QPen(QColor(220, 220, 220), 0.0));
    painter->drawLines(lines.data(), lines.size());

    painter->setPen(QPen(Qt::lightGray, 0.0));
    painter->drawLine(0, realTop, 0, realBottom);
    painter->drawLine(realLeft, 0, realRight, 0);

    if (!qFuzzyIsNull(m_snapOffset))
    {
        painter->setPen(QColor(m_snapHorz ? 255 : 0, m_snapHorz ? 0 : 255, 0));
        if (m_snapHorz)
            painter->drawLine(m_snapLine, realTop, m_snapLine, realBottom);
        else
            painter->drawLine(realLeft, m_snapLine, realRight, m_snapLine);
    }
}

void DiagramScene::raiseItemMoved(ResizableItem *item, const QPointF &oldPos, const QPointF &newPos)
{
    emit itemMoved(item, oldPos, newPos);
}

void DiagramScene::raiseItemResized(ResizableItem *item, const QRectF &oldPos, const QRectF &newPos)
{
    emit itemResized(item, oldPos, newPos);
}

void DiagramScene::raiseBeginEdit(DiagramItem* item)
{
    emit beginEdit(item);
}

DiagramItemGroup *DiagramScene::createItemGroup(const QList<ResizableItem *> &items)
{
    qreal z = 0;
    if (items.length() > 0)
        z = items.at(0)->zValue();

    QRectF rc;
    foreach (ResizableItem* item, items)
        rc |= item->posRect();

    QList<DiagramItem*> ditems;
    foreach (ResizableItem *item, items)
    {
        if (item->type() == DiagramItemGroup::Type)
        {
            DiagramItemGroup* gitem = (DiagramItemGroup*)item;
            ditems.append(gitem->diagramItems());
            destroyItemGroup(gitem);
        }
        else
            ditems.append((DiagramItem*) item);
    }

    DiagramItemGroup *group = new DiagramItemGroup();
    addItem(group);

    DiagramScene::sort(ditems);
    foreach (DiagramItem *ditem, ditems)
    {
        removeItem(ditem);
        ditem->setSelected(false);
        ditem->setParentItem(group);
        ditem->setPos(ditem->pos() - rc.topLeft());
        ditem->setLocked(true);
    }
    group->setZValue(z);
    group->setLocked(false);
    group->setPosRect(rc);
    return group;
}

void DiagramScene::destroyItemGroup(DiagramItemGroup *group)
{
    bool oldSelected = group->isSelected();
    group->setSelected(false);
    group->resizeHelper()->showGrips(false);
    foreach (DiagramItem *item, group->diagramItems())
    {
        item->setParentItem(0);
        item->setPos(item->pos() + group->pos());
        item->setLocked(false);
        item->setSelected(oldSelected);
    }
    removeItem(group);
    delete group;
}

bool zValueLessThan(ResizableItem* s1, ResizableItem* s2)
{
    return s1->zValue() < s2->zValue();
}

void DiagramScene::sort(QList<ResizableItem*> & r)
{
    qSort(r.begin(), r.end(), zValueLessThan);
}

void DiagramScene::sort(QList<DiagramItem*> & r)
{
    qSort(r.begin(), r.end(), zValueLessThan);
}

QList<ResizableItem*> DiagramScene::topLevelSortedItems()
{
    QList<ResizableItem*> r;
    foreach(QGraphicsItem* item, items())
    {
        if (ResizableItem::isResizableItem(item) && item->parentItem() == NULL)
            r.append((ResizableItem*)item);
    }
    qSort(r.begin(), r.end(), zValueLessThan);
    return r;
}

QList<DiagramItem*> DiagramScene::sortedDiagramItems()
{
    QList<DiagramItem*> r;
    foreach(QGraphicsItem* item, items())
    {
        if (item->type() == DiagramItem::Type)
            r.append((DiagramItem*)item);
    }
    qSort(r.begin(), r.end(), zValueLessThan);
    return r;
}

QList<ResizableItem*> DiagramScene::selectedSortedItems()
{
    QList<ResizableItem*> r;
    foreach(QGraphicsItem* item, selectedItems())
    {
        if (ResizableItem::isResizableItem(item))
        {
            r.append((ResizableItem*)item);
            Q_ASSERT(item->parentItem() == NULL);
        }
    }
    Q_ASSERT(r.length() == selectedItems().length());
    qSort(r.begin(), r.end(), zValueLessThan);
    return r;
}

void DiagramScene::addItemOnTop(ResizableItem* item)
{
    qreal zValue = 0;
    if (topLevelSortedItems().length() > 0)
        zValue = topLevelSortedItems()[topLevelSortedItems().length() - 1]->zValue();
    zValue += 0.1;
    item->setZValue(zValue);
    if (qgraphicsitem_cast<DiagramItem*> (item) != NULL)
        (qgraphicsitem_cast<DiagramItem*> (item))->setId(m_nextId);
    m_nextId ++;
    addItem(item);
}

void DiagramScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mouseMoveEvent(event);

    bool hasOldSnapLine = !qFuzzyIsNull(m_snapOffset);
    m_snapOffset = 0;
    if (mouseGrabberItem() != NULL)
    {
        if (qgraphicsitem_cast<DiagramItem*> (mouseGrabberItem()))
        {
            DiagramItem* ditem = (DiagramItem*)mouseGrabberItem();
            foreach(DiagramItem* t, sortedDiagramItems())
            {
                if (ditem == t)
                    continue;
                qreal x = ditem->posRect().left();
                qreal y = ditem->posRect().top();
                qreal x1 = ditem->posRect().right();
                qreal y1 = ditem->posRect().bottom();

                qreal tx = t->posRect().left();
                qreal ty = t->posRect().top();
                qreal tx1 = t->posRect().right();
                qreal ty1 = t->posRect().bottom();
                qreal snap = 5;

                if (abs((tx + tx1) / 2 - (x + x1) / 2) < snap)
                {
                    m_snapOffset = (tx + tx1) / 2 - (x + x1) / 2;
                    m_snapLine = (tx + tx1) / 2;
                    m_snapHorz = true;
                }
                if (abs((ty + ty1) / 2 - (y + y1) / 2) < snap)
                {
                    m_snapOffset = (ty + ty1) / 2 - (y + y1) / 2;
                    m_snapLine = (ty + ty1) / 2;
                    m_snapHorz = false;
                }
                if (abs(tx - x) < snap) {m_snapOffset = tx - x; m_snapLine = tx; m_snapHorz = true;}
                if (abs(tx1 - x) < snap) {m_snapOffset = tx1 - x; m_snapLine = tx1; m_snapHorz = true;}
                if (abs(tx - x1) < snap) {m_snapOffset = tx - x1; m_snapLine = tx; m_snapHorz = true;}
                if (abs(tx1 - x1) < snap) {m_snapOffset = tx1 - x1; m_snapLine = tx1; m_snapHorz = true;}

                if (abs(ty - y) < snap) {m_snapOffset = ty - y; m_snapLine = ty; m_snapHorz = false;}
                if (abs(ty1 - y) < snap) {m_snapOffset = ty1 - y; m_snapLine = ty1; m_snapHorz = false;}
                if (abs(ty - y1) < snap) {m_snapOffset = ty - y1; m_snapLine = ty; m_snapHorz = false;}
                if (abs(ty1 - y1) < snap) {m_snapOffset = ty1 - y1; m_snapLine = ty1; m_snapHorz = false;}

                if (!qFuzzyIsNull(m_snapOffset))
                {
                    update();
                    break;
                }
            }
        }
    }
    if (qFuzzyIsNull(m_snapOffset) && hasOldSnapLine)
        update();
}

void DiagramScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    DiagramItem* ditem = NULL;
    if (mouseGrabberItem() != NULL)
        if (qgraphicsitem_cast<DiagramItem*> (mouseGrabberItem()))
            ditem = (DiagramItem*)mouseGrabberItem();
    
    QGraphicsScene::mouseReleaseEvent(event);
    if (ditem != NULL)
    {
        if (m_snapHorz)
            ditem->setPos(ditem->pos().x() + m_snapOffset, ditem->pos().y());
        else
            ditem->setPos(ditem->pos().x(), ditem->pos().y() + m_snapOffset);
    }
    m_snapOffset = 0;
}

void DiagramScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    emit endEdit();
    QGraphicsScene::mousePressEvent(event);
}

///////////////////////////////////////////////////////////////////////////////

Document* Document::createDocument(QObject* mainWindow)
{
    DiagramScene* scene = new DiagramScene(mainWindow);
    scene->setSceneRect(QRectF(0, 0, 2000, 1000));
    //scene->setBackgroundBrush(Qt::white);
    QLinearGradient gradient(scene->sceneRect().topLeft(), scene->sceneRect().bottomLeft());
    gradient.setColorAt(0, Qt::white);
    gradient.setColorAt(1, QColor(230, 230, 230));
    scene->setBackgroundBrush(gradient);

    Document* doc = new Document(scene);

    connect(scene, SIGNAL(itemMoved(ResizableItem*,QPointF,QPointF)),
        doc, SLOT(itemMoved(ResizableItem*,QPointF,QPointF)));
    connect(scene, SIGNAL(itemResized(ResizableItem*,QRectF,QRectF)),
        doc, SLOT(itemResized(ResizableItem*,QRectF,QRectF)));
    connect(scene, SIGNAL(beginEdit(DiagramItem*)), doc, SLOT(beginEdit(DiagramItem*)));
    connect(scene, SIGNAL(endEdit()), doc, SLOT(endEdit()));
    doc->centerOn(0,0);

    return doc;
}

Document::Document(QGraphicsScene* scene) : QGraphicsView(scene)
{
    m_undoStack = new QUndoStack(this);
    m_scene = (DiagramScene*)scene;
    setDragMode(RubberBandDrag);
    setRubberBandSelectionMode(Qt::ContainsItemShape);
    m_stextEdit = NULL;
    m_mtextEdit = NULL;
    m_editingItem = NULL;
}

QUndoStack *Document::undoStack() const
{
    return m_undoStack;
}

bool Document::load(QFile & file)
{
    QDomDocument doc;
    doc.setContent(&file);

    if (doc.documentElement().nodeName() == "controls")
    {
        QMap<int, QList<ResizableItem*> > groups;
        for(int i=0; i<(int)doc.documentElement().childNodes().length(); i++)
        {
            QDomNode nd = doc.documentElement().childNodes().at(i);
            if (nd.nodeType() != QDomNode::ElementNode)
                continue;
            QDomElement* e = (QDomElement*) &nd;
            DiagramItem* newItem = ItemDataBase::sload(*e);
            m_scene->addItemOnTop(newItem);
            bool ok = false;
            int group;
            group = e->attribute("isInGroup", "-1").toInt(&ok); 
            if (ok && group >= 0)
            {
                groups[group].append(newItem);
            }
        }
        foreach (int group, groups.keys())
        {
            scene()->createItemGroup(groups[group]);
        }
        return true;
    }
    else
        return false;
}

void Document::save(QTextStream & textStream)
{
    const int Indent = 4;
    QDomDocument doc;
    QDomElement ctrls = doc.createElement("controls");
    foreach (DiagramItem* item, scene()->sortedDiagramItems())
    {
        QDomElement ctrl = doc.createElement("control");
        if (!item->itemData()->save(doc, ctrl))
            return;
        ctrls.appendChild(ctrl);
    }
    doc.appendChild(ctrls);

    QDomNode xmlNode = doc.createProcessingInstruction("xml",
        "version=\"1.0\" encoding=\"UTF-8\"");
    doc.insertBefore(xmlNode, ctrls);
    doc.save(textStream, Indent);
    m_undoStack->setClean();
}

bool Document::saveImage(const QString &fileName, const char *fileFormat)
{
    QImage newImage((int)scene()->sceneRect().width(), (int)scene()->sceneRect().height(), 
        QImage::Format_RGB32);
    QPainter painter(&newImage);

    scene()->render(&painter, scene()->sceneRect());
    if (newImage.save(fileName,fileFormat,80))
    {
        qDebug()<<"SAVE SUCCESS!";
        return true;
    }
    else
        return false;
}

QString Document::fileName() const
{
    return m_fileName;
}

void Document::setFileName(const QString &fileName)
{
    m_fileName = fileName;
}

DiagramScene* Document::scene()
{
    return m_scene;
}

void Document::itemMoved(ResizableItem *item, const QPointF &oldPos, const QPointF &newPos)
{
    undoStack()->push(new MoveDiagramItemCommand(item, oldPos, newPos));
}

void Document::itemResized(ResizableItem *item, const QRectF &oldPos, const QRectF &newPos)
{
    undoStack()->push(new ResizeDiagramItemCommand(item, oldPos, newPos));
}

QList<DiagramItem*> Document::createItemsByText(const QString& text, QMap<int, QList<ResizableItem*> > & groupMap)
{
    QList<DiagramItem*> items;
    QDomDocument doc;
    groupMap.clear();
    if (doc.setContent(text))
    {
        if (doc.documentElement().nodeName() == "controls")
        {
            for(int i=0; i<(int)doc.documentElement().childNodes().length(); i++)
            {
                QDomNode nd = doc.documentElement().childNodes().at(i);
                if (nd.nodeType() != QDomNode::ElementNode)
                    continue;
                QDomElement* e = (QDomElement*) &nd;
                DiagramItem* newItem = ItemDataBase::sload(*e);
                bool ok = false;
                int group;
                group = e->attribute("isInGroup", "-1").toInt(&ok); 
                if (ok && group >= 0)
                    groupMap[group].append(newItem);
                items.append(newItem);
            }
        }
    }
    return items;
}

bool Document::isValidItemsText(const QString& text)
{
    QString s = "<control";
    return (text.left(s.length()) == s);
}

QString Document::serializeItemsToText(QList<ResizableItem*> items)
{
    QString s = "<controls>";
    foreach(ResizableItem* item, items)
    {
        if (item->type() == DiagramItem::Type)
        {
            DiagramItem* ditem = qgraphicsitem_cast<DiagramItem *>(item);
            QString xml;
            if (ditem->itemData()->save(xml))
                s += xml;
        }
        else
        {
            DiagramItemGroup* gitem = qgraphicsitem_cast<DiagramItemGroup *>(item);
            foreach (DiagramItem* ditem, gitem->diagramItems())
            {
                QString xml;
                if (ditem->itemData()->save(xml))
                    s += xml;
            }
        }
    }
    s += "</controls>";
    return s;
}

void Document::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Delete)
        emit deleteKeyPressed();
}

void Document::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("application/diagram.library.key"))
        event->accept();
    else
        event->ignore();
}

void Document::dragLeaveEvent(QDragLeaveEvent *event)
{
    event->accept();
}

void Document::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasFormat("application/diagram.library.key"))
    {
        event->setDropAction(Qt::MoveAction);
        event->accept();
    } 
    else 
    {
        event->ignore();
    }
}

void Document::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasFormat("application/diagram.library.key"))
    {
        QByteArray pieceData = event->mimeData()->data("application/diagram.library.key");

        QPoint pt = event->pos();
        QPointF pt2 = mapToScene(pt);
        QDataStream stream(&pieceData, QIODevice::ReadOnly);
        int key = -1;
        stream >> key;
        if (key != -1)
            undoStack()->push(new AddDiagramItemCommand(this, (DiagramKey)key, &pt2));

        event->setDropAction(Qt::MoveAction);
        event->accept();
    } 
    else 
    {
        event->ignore();
    }
}

void Document::beginEdit(DiagramItem *item)
{
    QPoint pos = mapFromScene(item->pos());
    if (item->itemData()->getProperties() & P_SingleLineText)
    {
        m_stextEdit = new QLineEdit(this);
        m_stextEdit->setGeometry(pos.x(), pos.y(), 400, 30);
        m_stextEdit->setText(item->itemData()->text());
        m_stextEdit->show();
        m_editingItem = item;
    }
    else if (item->itemData()->getProperties() & P_MultilineTexts)
    {
        int h = item->size().height() - 20;
        if (h < 50) h = 50;
        m_mtextEdit = new QTextEdit(this);
        m_mtextEdit->setFontFamily("Comic Sans MS");
        m_mtextEdit->setFontPointSize(11);
        m_mtextEdit->setGeometry(pos.x(), pos.y(), 400, h);
        m_mtextEdit->setPlainText(ItemDataBase::joinTexts(item->itemData()->texts(), "\n"));
        m_mtextEdit->show();
        m_editingItem = item;
    }
}

void Document::endEdit()
{
    if (m_stextEdit != NULL)
    {
        m_stextEdit->hide();
        m_editingItem->itemData()->setText(m_stextEdit->text());
        delete m_stextEdit;
        m_stextEdit = NULL;
        m_editingItem = NULL;
    }
    if (m_mtextEdit != NULL)
    {
        m_mtextEdit->hide();
        QString t = m_mtextEdit->toPlainText();
        QStringList texts = t.split("\n");
        if (texts != m_editingItem->itemData()->texts())
            undoStack()->push(new ChangeMultiLineTextsCommand(m_editingItem, texts));
        delete m_mtextEdit;
        m_mtextEdit = NULL;
        m_editingItem = NULL;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

#define SET_INT_PROP_IMPL(prop) \
    void Document::set##prop(int value) \
{ \
    ResizableItem* ritem = scene()->selectedSortedItems()[0]; \
    Q_ASSERT(ritem->type() == DiagramItem::Type); \
    DiagramItem* ditem = (DiagramItem*) ritem; \
    undoStack()->push(new ChangeIntPropertyCommand(ditem, value, P_##prop)); \
}

#define SET_BOOL_PROP_IMPL(prop) \
void Document::set##prop(bool value) \
{ \
    ResizableItem* ritem = scene()->selectedSortedItems()[0]; \
    Q_ASSERT(ritem->type() == DiagramItem::Type); \
    DiagramItem* ditem = (DiagramItem*) ritem; \
    undoStack()->push(new ChangeBoolPropertyCommand(ditem, value, P_##prop)); \
}
void Document::autoResize()
{
    ResizableItem* ritem = scene()->selectedSortedItems()[0];
    Q_ASSERT(ritem->type() == DiagramItem::Type);
    DiagramItem* ditem = (DiagramItem*) ritem;
    undoStack()->push(new AutosizeCommand(ditem));
}
void Document::setFontSize(const QString & snumber)
{
    ResizableItem* ritem = scene()->selectedSortedItems()[0];
    Q_ASSERT(ritem->type() == DiagramItem::Type);
    DiagramItem* ditem = (DiagramItem*) ritem;
    undoStack()->push(new ChangeIntPropertyCommand(ditem, snumber.toInt(), P_FontSize));
}
void Document::setColor()
{
    ResizableItem* ritem = scene()->selectedSortedItems()[0];
    Q_ASSERT(ritem->type() == DiagramItem::Type);
    DiagramItem* ditem = (DiagramItem*) ritem;
    QColor color = QColorDialog::getColor(ditem->itemData()->color(), this);
    if (color.isValid())
    {
        undoStack()->push(new ChangeColorPropertyCommand(ditem, color));
    }
}
void Document::setIcon()
{
}
SET_INT_PROP_IMPL(SelectedIndex)
SET_INT_PROP_IMPL(Value)
SET_BOOL_PROP_IMPL(VScrollBar)
SET_BOOL_PROP_IMPL(FontBold)
SET_BOOL_PROP_IMPL(FontItalic)
SET_BOOL_PROP_IMPL(FontUnderline)

