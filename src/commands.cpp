
#include "commands.h"
#include "itemdata.hxx"

///////////////////////////////////////////////////////////////////////////////

AddDiagramItemCommand::AddDiagramItemCommand(Document *doc, DiagramKey key, QPointF* pt, QUndoCommand *parent)
    : QUndoCommand(parent)
{
    m_doc = doc;
    QPointF p = QPointF(0, 0);
    if (pt != NULL)
        p = *pt;
    m_item = new DiagramItem(key, p);
    m_undoed = false;
}

AddDiagramItemCommand::~AddDiagramItemCommand()
{
    if (m_undoed)
        delete m_item;
}

void AddDiagramItemCommand::undo()
{
    m_doc->scene()->removeItem(m_item);
    m_undoed = true;
}

void AddDiagramItemCommand::redo()
{
    m_doc->scene()->addItemOnTop(m_item);
    m_undoed = false;
    //QString s = ("Add id(%1) in control type(%2) at (%3,%4)");
    //setText(s.arg(m_item->id())
    //    .arg(m_item->key())
    //    .arg(m_item->pos().x())
    //    .arg(m_item->pos().y()));
    QString s = ("Add %1");
    setText(s.arg(DiagramLibrary::diagramTypeFromKey(m_item->key())));
}

///////////////////////////////////////////////////////////////////////////////

RemoveDiagramItemsCommand::RemoveDiagramItemsCommand(Document *doc, QList<ResizableItem*> items, 
    QUndoCommand *parent) : QUndoCommand(parent)
{
    m_doc = doc;
    m_items = items;
    m_undoed = false;
}

RemoveDiagramItemsCommand::~RemoveDiagramItemsCommand()
{
    if (m_undoed)
        return;
    foreach (ResizableItem *item, m_items)
        delete item;
}

void RemoveDiagramItemsCommand::undo()
{
    foreach (ResizableItem *item, m_items)
        m_doc->scene()->addItem(item);
    m_undoed = true;
}

void RemoveDiagramItemsCommand::redo()
{
    foreach (ResizableItem *item, m_items)
        m_doc->scene()->removeItem(item); // memory leak?
    m_undoed = false;

    QString s = ("Remove %1 %2");
    setText(s.arg(m_items.length())
        .arg(m_items.length() > 1 ? "item" : "items"));
}

///////////////////////////////////////////////////////////////////////////////

MoveDiagramItemCommand::MoveDiagramItemCommand(ResizableItem *diagramItem, 
    const QPointF &oldPos, const QPointF &, QUndoCommand *parent)
    : QUndoCommand(parent)
{
    m_item = diagramItem;
    m_newPos = diagramItem->pos();
    m_oldPos = oldPos;
}

bool MoveDiagramItemCommand::mergeWith(const QUndoCommand *command)
{
    const MoveDiagramItemCommand *moveCommand = static_cast<const MoveDiagramItemCommand *>(command);
    ResizableItem *item = moveCommand->m_item;
    if (m_item != item)
        return false;
    m_newPos = item->pos();
    return true;
}

void MoveDiagramItemCommand::undo()
{
    m_item->setPos(m_oldPos);
    if (m_item->scene() != NULL)
        m_item->scene()->update();
}

void MoveDiagramItemCommand::redo()
{
    if (m_item->scene() != NULL)
        m_item->setPos(m_newPos);

    //QString s = ("Move id(%1) from (%2,%3) to (%3,%4)");
    //int id = -1;
    //if (m_item->type() == DiagramItemGroup::Type)
    //{
    //    s = ("Move group id(%1) from (%2,%3) to (%3,%4)");
    //    id = ((DiagramItemGroup*)m_item)->diagramItems().at(0)->id();
    //}
    //else
    //    id = ((DiagramItem*)m_item)->id();

    //setText(s.arg(id)
    //    .arg(m_oldPos.x())
    //    .arg(m_oldPos.y())
    //    .arg(m_newPos.x())
    //    .arg(m_newPos.y()));

    QString s = ("Move %1");
    int key = -1;
    if (m_item->type() == DiagramItemGroup::Type)
    {
        s = ("Move group %1");
        key = ((DiagramItemGroup*)m_item)->diagramItems().at(0)->key();
    }
    else
        key = ((DiagramItem*)m_item)->key();

    setText(s.arg(DiagramLibrary::diagramTypeFromKey(key)));
}

///////////////////////////////////////////////////////////////////////////////

ResizeDiagramItemCommand::ResizeDiagramItemCommand(ResizableItem *diagramItem, 
    const QRectF &oldPos, const QRectF &, QUndoCommand *parent) : QUndoCommand(parent)
{
    m_item = diagramItem;
    m_newPos = m_item->posRect();
    m_oldPos = oldPos;
}

bool ResizeDiagramItemCommand::mergeWith(const QUndoCommand *command)
{
    const ResizeDiagramItemCommand *cmd = static_cast<const ResizeDiagramItemCommand *>(command);
    ResizableItem *item = cmd->m_item;
    if (m_item != item)
        return false;
    m_newPos = QRectF(item->pos(), item->size());
    return true;
}

void ResizeDiagramItemCommand::undo()
{
    m_item->setPosRectAndNotify(m_oldPos);
    if (qgraphicsitem_cast<DiagramItemGroup*> (m_item) != NULL)
    {
        DiagramItemGroup* ditem = qgraphicsitem_cast<DiagramItemGroup*>(m_item);
        ditem->resizeChildren(m_newPos);
    }
    if (m_item->scene() != NULL)
        m_item->scene()->update();
}

void ResizeDiagramItemCommand::redo()
{
    m_item->setPosRectAndNotify(m_newPos);
    if (qgraphicsitem_cast<DiagramItemGroup*> (m_item) != NULL)
    {
        DiagramItemGroup* ditem = qgraphicsitem_cast<DiagramItemGroup*>(m_item);
        ditem->resizeChildren(m_oldPos);
    }
    if (m_item->scene() != NULL)
        m_item->scene()->update();

    //QString s = ("Resize id(%1) from (%2,%3,%4,%5) to (%6,%7,%8,%9).");
    //int id = -1;
    //if (m_item->type() == DiagramItemGroup::Type)
    //{
    //    s = ("Resize group id(%1) from (%2,%3,%4,%5) to (%6,%7,%8,%9).");
    //    id = ((DiagramItemGroup*)m_item)->diagramItems().at(0)->id();
    //}
    //else
    //    id = ((DiagramItem*)m_item)->id();

    //setText(s.arg(id)
    //    .arg(m_oldPos.x())
    //    .arg(m_oldPos.y())
    //    .arg(m_oldPos.width())
    //    .arg(m_oldPos.height())
    //    .arg(m_newPos.x())
    //    .arg(m_newPos.y())
    //    .arg(m_newPos.width())
    //    .arg(m_newPos.height()));

    QString s = ("Resize %1");
    int key = -1;
    if (m_item->type() == DiagramItemGroup::Type)
    {
        s = ("Resize group %1");
        key = ((DiagramItemGroup*)m_item)->diagramItems().at(0)->key();
    }
    else
        key = ((DiagramItem*)m_item)->key();

    setText(s.arg(DiagramLibrary::diagramTypeFromKey(key)));
}

///////////////////////////////////////////////////////////////////////////////

CutCommand::CutCommand(QList<ResizableItem*> items, DiagramScene* scene,
    QUndoCommand *parent) : QUndoCommand(parent)
{
    m_items = items;
    m_scene = scene;
    m_undoed = false;
}

CutCommand::~CutCommand()
{
    if (m_undoed)
        return;
    foreach (ResizableItem *item, m_items)
        delete item;
}

void CutCommand::undo()
{
    foreach (QGraphicsItem *item, m_items)
        m_scene->addItem(item);
    m_undoed = true;
}

void CutCommand::redo()
{
    foreach (QGraphicsItem *item, m_items)
        m_scene->removeItem(item);
    m_undoed = false;

    QString s = ("Cut %1 %2");
    setText(s.arg(m_items.length())
        .arg(m_items.length() > 1 ? "item" : "items"));
}

///////////////////////////////////////////////////////////////////////////////

PasteCommand::PasteCommand(QList<DiagramItem*> items, QMap<int, QList<ResizableItem*> > groupMap, 
    DiagramScene* scene, QUndoCommand *parent) : QUndoCommand(parent)
{
    m_items = items;
    m_scene = scene;
    m_undoed = false;
    m_groupMap = groupMap;
}

PasteCommand::~PasteCommand()
{
    if (m_undoed)
    {
        foreach (ResizableItem *item, m_items)
            delete item;
    }
}

void PasteCommand::undo()
{
    foreach (QGraphicsItem *item, m_items)
        m_scene->removeItem(item);
    foreach (DiagramItemGroup* group, m_groups)
    {
        m_scene->removeItem(group);
        delete group;
    }
    m_groups.clear();
    m_undoed = true;
}

void PasteCommand::redo()
{
    m_groups.clear();
    foreach (QGraphicsItem *item, m_items)
    {
        if (ResizableItem::isResizableItem(item))
            m_scene->addItemOnTop((ResizableItem*)item);
    }
    foreach (int group, m_groupMap.keys())
        m_groups.append(m_scene->createItemGroup(m_groupMap[group]));
    m_undoed = false;

    QString s = ("Paste %1 %2");
    setText(s.arg(m_items.length())
        .arg(m_items.length() > 1 ? "item" : "items"));
}

///////////////////////////////////////////////////////////////////////////////

GroupCommand::GroupCommand(QList<ResizableItem*> items, DiagramScene* scene,
    QUndoCommand *parent) : QUndoCommand(parent)
{
    foreach (ResizableItem *item, items)
    {
        if (item->type() == DiagramItemGroup::Type)
        {
            m_groupItems.append(item);
            DiagramItemGroup* gitem = ((DiagramItemGroup*)item);
            Q_ASSERT(gitem->diagramItems().length() > 1);
            bool bGroup = gitem->diagramItems().length() > 1;
            int groupId = -1;
            if (bGroup)
            {
                groupId = gitem->diagramItems().at(0)->id();
                m_groups.append(groupId);
            }
            foreach (DiagramItem* ditem, gitem->diagramItems())
                m_itemGroupIds.insert(ditem, groupId);
        }
        else
        {
            m_itemGroupIds.insert((DiagramItem*)item, -1);
            m_diagramItems.append(item);
        }
    }
    m_scene = scene;
    m_group = NULL;
}

void GroupCommand::undo()
{
    m_groupItems.clear();

    m_scene->destroyItemGroup(m_group);
    m_group = NULL;
    foreach (int groupId, m_groups)
    {
        QList<ResizableItem*> items;
        foreach (DiagramItem *item, m_itemGroupIds.keys())
        {
            if (m_itemGroupIds.value(item) == groupId)
                items.append(item);
        }
        DiagramScene::sort(items);
        DiagramItemGroup* group = m_scene->createItemGroup(items);
        group->setSelected(true);
        m_groupItems.append(group);
    }
}

void GroupCommand::redo()
{
    QList<ResizableItem*> resizableItems = m_groupItems;
    resizableItems.append(m_diagramItems);

    DiagramScene::sort(resizableItems);
    m_group = m_scene->createItemGroup(resizableItems);
    m_group->setSelected(true);

    QString s = ("Group items");
    setText(s);
}

///////////////////////////////////////////////////////////////////////////////

UngroupCommand::UngroupCommand(DiagramItemGroup* item, QUndoCommand *parent)
     : QUndoCommand(parent)
{
    m_items = item->diagramItems();
    m_group = item;
    m_scene = (DiagramScene*)(item->scene());
}

void UngroupCommand::undo()
{
    QList<ResizableItem*> items;
    foreach (DiagramItem* item, m_items)
        items.append(item);
    m_group = m_scene->createItemGroup(items);
    m_group->setSelected(true);
}

void UngroupCommand::redo()
{
    m_scene->destroyItemGroup(m_group);
    m_group = NULL;
    QString s = ("Ungroup");
    setText(s);
}

///////////////////////////////////////////////////////////////////////////////

LockCommand::LockCommand(QList<ResizableItem*> items, QUndoCommand *parent)
     : QUndoCommand(parent)
{
    m_items = items;
}

void LockItems(QList<ResizableItem*> items)
{
    foreach (ResizableItem *item, items)
    {
        item->setSelected(false);
        item->setLocked(true);
    }
}

void UnlockItems(QList<ResizableItem*> items, bool selectedAfterUnlock)
{
    foreach (ResizableItem *item, items)
    {
        item->setLocked(false);
        item->setSelected(selectedAfterUnlock);
    }
}

void LockCommand::undo()
{
    UnlockItems(m_items, true);
}

void LockCommand::redo()
{
    LockItems(m_items);
    QString s = ("Lock %1 %2");
    setText(s.arg(m_items.length())
        .arg(m_items.length() > 1 ? "item" : "items"));
}

///////////////////////////////////////////////////////////////////////////////

UnlockCommand::UnlockCommand(QList<ResizableItem*> items, QUndoCommand *parent)
     : QUndoCommand(parent)
{
    m_items = items;
}

void UnlockCommand::undo()
{
    LockItems(m_items);
}

void UnlockCommand::redo()
{
    UnlockItems(m_items, false);
    QString s = ("Unlock all");
    setText(s);
}

///////////////////////////////////////////////////////////////////////////////

MoveFrontCommand::MoveFrontCommand(QList<ResizableItem*> items, QUndoCommand *parent)
     : QUndoCommand(parent)
{
    m_items = items;
    foreach (ResizableItem* item, items)
        m_zValues.append(item->zValue());
}

void MoveFront(QList<ResizableItem*> items)
{
    DiagramScene* scene = NULL;
    if (items.length() == 0)
        return;
    scene = (DiagramScene*)(items[0]->scene());
    if (scene == NULL)
        return;
    qreal maxzInAll = 0.0;
    int count = scene->topLevelSortedItems().length();
    if (count > 0)
        maxzInAll = scene->topLevelSortedItems()[count - 1]->zValue();
    maxzInAll += 0.1;

    qreal minzInItems = items[0]->zValue();
    foreach (ResizableItem* item, items)
    {
        qreal z = maxzInAll + (item->zValue() - minzInItems);
        item->setZValue(z);
        if (item->type() == DiagramItemGroup::Type)
        {
            DiagramItemGroup* g = (DiagramItemGroup*)item;
            Q_ASSERT(g->diagramItems().length() > 0);
            g->diagramItems().at(0)->setZValue(z);
        }
    }
    scene->update();
}

void MoveBack(QList<ResizableItem*> items)
{
    DiagramScene* scene = NULL;
    if (items.length() == 0)
        return;
    scene = (DiagramScene*)(items[0]->scene());
    if (scene == NULL)
        return;
    qreal minzInAll = 0.0;
    if (scene->topLevelSortedItems().length() > 0)
        minzInAll = scene->topLevelSortedItems()[0]->zValue();
    minzInAll -= 0.1;

    qreal maxzInItems = items[items.length() -1]->zValue();
    foreach (ResizableItem* item, items)
    {
        qreal z = minzInAll - (maxzInItems - item->zValue());
        item->setZValue(z);
        if (item->type() == DiagramItemGroup::Type)
        {
            DiagramItemGroup* g = (DiagramItemGroup*)item;
            Q_ASSERT(g->diagramItems().length() > 0);
            g->diagramItems().at(0)->setZValue(z);
        }
    }
    scene->update();
}

void MoveFrontCommand::undo()
{
    int i=0;
    foreach (ResizableItem* item, m_items)
    {
        item->setZValue(m_zValues[i]);
        i++;
    }
}

void MoveFrontCommand::redo()
{
    MoveFront(m_items);
    QString s = ("Move %1 %2 to front");
    setText(s.arg(m_items.length())
        .arg(m_items.length() > 1 ? "item" : "items"));
}

///////////////////////////////////////////////////////////////////////////////

MoveBackCommand::MoveBackCommand(QList<ResizableItem*> items, QUndoCommand *parent)
     : QUndoCommand(parent)
{
    m_items = items;
    foreach (ResizableItem* item, items)
        m_zValues.append(item->zValue());
}

void MoveBackCommand::undo()
{
    int i=0;
    foreach (ResizableItem* item, m_items)
    {
        item->setZValue(m_zValues[i]);
        i++;
    }
}

void MoveBackCommand::redo()
{
    MoveBack(m_items);
    QString s = ("Move %1 %2 to back");
    setText(s.arg(m_items.length())
        .arg(m_items.length() > 1 ? "item" : "items"));
}

///////////////////////////////////////////////////////////////////////////////

MoveUpCommand::MoveUpCommand(ResizableItem* item, QUndoCommand *parent)
     : QUndoCommand(parent)
{
    m_item = item;
    m_zValue = item->zValue();
}

ResizableItem* MoveUp(ResizableItem* item)
{
    DiagramScene* scene = (DiagramScene*)(item->scene());

    if (scene == NULL)
        return NULL;
    bool isfound = false;
    int index = 0;
    foreach (ResizableItem* tempItem, scene->topLevelSortedItems())
    {
        if (tempItem == item)
        {
            isfound = true;
            break;
        }
        index ++;
    }
    if (!isfound)
        return NULL;
    if (index >= scene->topLevelSortedItems().length() - 1)
        return NULL;
    ResizableItem* upItem = scene->topLevelSortedItems()[index+1];
    qreal zTemp = item->zValue();
    item->setZValue(upItem->zValue());
    upItem->setZValue(zTemp);
    scene->update();
    return upItem;
}

void MoveUpCommand::undo()
{
    if (m_upItem == NULL)
        return;
    qreal zTemp = m_item->zValue();
    m_item->setZValue(m_upItem->zValue());
    m_upItem->setZValue(zTemp);
    m_item->scene()->update();
}

void MoveUpCommand::redo()
{
    m_upItem = MoveUp(m_item);
    QString s = ("Move %1 up");
    int key = -1;
    if (m_item->type() == DiagramItemGroup::Type)
    {
        s = ("Move group %1 up");
        key = ((DiagramItemGroup*)m_item)->diagramItems().at(0)->key();
    }
    else
        key = ((DiagramItem*)m_item)->key();
    setText(s.arg(DiagramLibrary::diagramTypeFromKey(key)));
}


///////////////////////////////////////////////////////////////////////////////

MoveDownCommand::MoveDownCommand(ResizableItem* item, QUndoCommand *parent)
     : QUndoCommand(parent)
{
    m_item = item;
    m_zValue = item->zValue();
}

ResizableItem* MoveDown(ResizableItem* item)
{
    DiagramScene* scene = (DiagramScene*)(item->scene());

    if (scene == NULL)
        return NULL;
    bool isfound = false;
    int index = 0;
    foreach (ResizableItem* tempItem, scene->topLevelSortedItems())
    {
        if (tempItem == item)
        {
            isfound = true;
            break;
        }
        index ++;
    }
    if (!isfound)
        return NULL;
    if (index >= scene->topLevelSortedItems().length() || index == 0)
        return NULL;
    ResizableItem* downItem = scene->topLevelSortedItems()[index-1];
    qreal zTemp = item->zValue();
    item->setZValue(downItem->zValue());
    downItem->setZValue(zTemp);
    scene->update();
    return downItem;
}

void MoveDownCommand::undo()
{
    if (m_downItem == NULL)
        return;
    qreal zTemp = m_item->zValue();
    m_item->setZValue(m_downItem->zValue());
    m_downItem->setZValue(zTemp);
    m_item->scene()->update();
}

void MoveDownCommand::redo()
{
    m_downItem = MoveDown(m_item);
    QString s = ("Move %1 down");
    int key = -1;
    if (m_item->type() == DiagramItemGroup::Type)
    {
        s = ("Move group %1 down");
        key = ((DiagramItemGroup*)m_item)->diagramItems().at(0)->key();
    }
    else
        key = ((DiagramItem*)m_item)->key();
    setText(s.arg(DiagramLibrary::diagramTypeFromKey(key)));
}

///////////////////////////////////////////////////////////////////////////////

ChangeMultiLineTextsCommand::ChangeMultiLineTextsCommand(DiagramItem* item, 
    const QStringList& newTexts, QUndoCommand* parent) : QUndoCommand(parent)
{
    m_item = item;
    m_newTexts = newTexts;
    m_oldTexts = item->itemData()->texts();
    m_oldSize = item->size();
}

void ChangeMultiLineTextsCommand::undo()
{
    m_item->itemData()->setTexts(m_oldTexts);
    m_item->setSizeAndNotify(m_oldSize);
}

void ChangeMultiLineTextsCommand::redo()
{
    m_item->itemData()->setTexts(m_newTexts);
    QString s = ("Change %1 text");
    setText(s.arg(DiagramLibrary::diagramTypeFromKey(m_item->key())));
}

bool ChangeMultiLineTextsCommand::mergeWith(const QUndoCommand *command)
{
    const ChangeMultiLineTextsCommand *cmd = static_cast<const ChangeMultiLineTextsCommand *>(command);
    DiagramItem *item = cmd->m_item;
    if (m_item != item)
        return false;
    m_newTexts = cmd->m_newTexts;
    return true;
}

///////////////////////////////////////////////////////////////////////////////

AutosizeCommand::AutosizeCommand(DiagramItem* item, QUndoCommand* parent) : QUndoCommand(parent)
{
    m_item = item;
    m_oldSize = item->size();
}

void AutosizeCommand::undo()
{
    m_item->setSizeAndNotify(m_oldSize);
}

void AutosizeCommand::redo()
{
    m_item->itemData()->autoResize();
    QString s = ("Auto-Resize %1");
    setText(s.arg(DiagramLibrary::diagramTypeFromKey(m_item->key())));
}

bool AutosizeCommand::mergeWith(const QUndoCommand *command)
{
    const AutosizeCommand *cmd = static_cast<const AutosizeCommand *>(command);
    DiagramItem *item = cmd->m_item;
    if (m_item != item)
        return false;
    return true;
}

///////////////////////////////////////////////////////////////////////////////

ChangeIntPropertyCommand::ChangeIntPropertyCommand(DiagramItem* item, int newValue, 
    int prop, QUndoCommand* parent) : QUndoCommand(parent)
{
    m_item = item;
    m_prop = prop;
    m_newValue = newValue;
    if (m_prop == P_SelectedIndex)
        m_oldValue = item->itemData()->selectedIndex();
    else if (m_prop == P_Value)
        m_oldValue = item->itemData()->value();
    else if (m_prop == P_FontSize)
        m_oldValue = item->itemData()->fontSize();
}

void ChangeIntPropertyCommand::undo()
{
    if (m_prop == P_SelectedIndex)
        m_item->itemData()->setSelectedIndex(m_oldValue);
    else if (m_prop == P_Value)
        m_item->itemData()->setValue(m_oldValue);
    else if (m_prop == P_FontSize)
        m_item->itemData()->setFontSize(m_oldValue);
}

void ChangeIntPropertyCommand::redo()
{
    QString s = "%1";
    if (m_prop == P_SelectedIndex)
    {
        m_item->itemData()->setSelectedIndex(m_newValue);
        s = ("Set %1 selected index");
    }
    else if (m_prop == P_Value)
    {
        m_item->itemData()->setValue(m_newValue);
        s = ("Set %1 value");
    }
    else if (m_prop == P_FontSize)
    {
        m_item->itemData()->setFontSize(m_newValue);
        s = ("Set %1 font size");
    }
    setText(s.arg(DiagramLibrary::diagramTypeFromKey(m_item->key())));
}

bool ChangeIntPropertyCommand::mergeWith(const QUndoCommand *command)
{
    const ChangeIntPropertyCommand *cmd = static_cast<const ChangeIntPropertyCommand *>(command);
    DiagramItem *item = cmd->m_item;
    if (m_item != item)
        return false;
    if (cmd->m_prop != m_prop)
        return false;
    m_newValue = cmd->m_newValue;
    return true;
}

///////////////////////////////////////////////////////////////////////////////

ChangeBoolPropertyCommand::ChangeBoolPropertyCommand(DiagramItem* item, bool newValue, 
    int prop, QUndoCommand* parent) : QUndoCommand(parent)
{
    m_item = item;
    m_prop = prop;
    m_newValue = newValue;
    if (m_prop == P_VScrollBar)
        m_oldValue = item->itemData()->vScrollbar();
    else if (m_prop == P_FontBold)
        m_oldValue = item->itemData()->fontBold();
    else if (m_prop == P_FontItalic)
        m_oldValue = item->itemData()->fontItalic();
    else if (m_prop == P_FontUnderline)
        m_oldValue = item->itemData()->fontUnderline();
}

void ChangeBoolPropertyCommand::undo()
{
    if (m_prop == P_VScrollBar)
        m_item->itemData()->setVScrollbar(m_oldValue);
    else if (m_prop == P_FontBold)
        m_item->itemData()->setFontBold(m_oldValue);
    else if (m_prop == P_FontItalic)
        m_item->itemData()->setFontItalic(m_oldValue);
    else if (m_prop == P_FontUnderline)
        m_item->itemData()->setFontUnderline(m_oldValue);
}

void ChangeBoolPropertyCommand::redo()
{
    QString s = "%1";
    if (m_prop == P_VScrollBar)
    {
        m_item->itemData()->setVScrollbar(m_newValue);
        s = ("Set %1 scrollbar");
    }
    else if (m_prop == P_FontBold)
    {
        m_item->itemData()->setFontBold(m_newValue);
        s = ("Set %1 bold");
    }    
    else if (m_prop == P_FontItalic)
    {
        m_item->itemData()->setFontItalic(m_newValue);
        s = ("Set %1 italic");
    }
    else if (m_prop == P_FontUnderline)
    {
        m_item->itemData()->setFontUnderline(m_newValue);
        s = ("Set %1 underline");
    }
    setText(s.arg(DiagramLibrary::diagramTypeFromKey(m_item->key())));
}

bool ChangeBoolPropertyCommand::mergeWith(const QUndoCommand *command)
{
    const ChangeBoolPropertyCommand *cmd = static_cast<const ChangeBoolPropertyCommand *>(command);
    DiagramItem *item = cmd->m_item;
    if (m_item != item)
        return false;
    if (cmd->m_prop != m_prop)
        return false;
    m_newValue = cmd->m_newValue;
    return true;
}

///////////////////////////////////////////////////////////////////////////////

ChangeColorPropertyCommand::ChangeColorPropertyCommand(DiagramItem* item, QColor newValue, 
    QUndoCommand* parent) : QUndoCommand(parent)
{
    m_item = item;
    m_newValue = newValue;
    m_oldValue = item->itemData()->color();
}

void ChangeColorPropertyCommand::undo()
{
    m_item->itemData()->setColor(m_oldValue);
}

void ChangeColorPropertyCommand::redo()
{
    m_item->itemData()->setColor(m_newValue);
    QString s = ("Set %1 color");
    setText(s.arg(DiagramLibrary::diagramTypeFromKey(m_item->key())));
}

bool ChangeColorPropertyCommand::mergeWith(const QUndoCommand *command)
{
    const ChangeColorPropertyCommand *cmd = static_cast<const ChangeColorPropertyCommand *>(command);
    DiagramItem *item = cmd->m_item;
    if (m_item != item)
        return false;
    m_newValue = cmd->m_newValue;
    return true;
}
