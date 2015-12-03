
#ifndef COMMANDS_H
#define COMMANDS_H

#include <QUndoCommand>
#include "document.hxx"

///////////////////////////////////////////////////////////////////////////////

class AddDiagramItemCommand : public QUndoCommand
{
public:
    enum { Id = 2000 };
    virtual int id() const { return Id; }

    AddDiagramItemCommand(Document *doc, DiagramKey key, QPointF* pt = 0, QUndoCommand *parent = 0);
    ~AddDiagramItemCommand();
    void undo();
    void redo();

private:
    Document *m_doc;
    DiagramItem *m_item;
    bool m_undoed;
};

///////////////////////////////////////////////////////////////////////////////

class RemoveDiagramItemsCommand : public QUndoCommand
{
public:
    enum { Id = 2001 };
    virtual int id() const { return Id; }

    RemoveDiagramItemsCommand(Document *doc, QList<ResizableItem*> items, QUndoCommand *parent = 0);
    ~RemoveDiagramItemsCommand();
    void undo();
    void redo();

private:
    Document *m_doc;
    QList<ResizableItem*> m_items;
    bool m_undoed;
};

///////////////////////////////////////////////////////////////////////////////

class MoveDiagramItemCommand : public QUndoCommand
{
public:
    enum { Id = 2002 };
    virtual int id() const { return Id; }

    MoveDiagramItemCommand(ResizableItem *diagramItem, const QPointF &oldPos, const QPointF &newPos,
                QUndoCommand *parent = 0);

    virtual void undo();
    virtual void redo();
    virtual bool mergeWith(const QUndoCommand *command);

private:
    ResizableItem *m_item;
    QPointF m_oldPos;
    QPointF m_newPos;
};

///////////////////////////////////////////////////////////////////////////////

class ResizeDiagramItemCommand : public QUndoCommand
{
public:
    enum { Id = 2003 };
    virtual int id() const { return Id; }

    ResizeDiagramItemCommand(ResizableItem *diagramItem, const QRectF &oldPos,
                const QRectF &newPos, QUndoCommand *parent = 0);

    virtual void undo();
    virtual void redo();
    virtual bool mergeWith(const QUndoCommand *command);

private:
    ResizableItem *m_item;
    QRectF m_oldPos;
    QRectF m_newPos;
};

///////////////////////////////////////////////////////////////////////////////

class CutCommand : public QUndoCommand
{
public:
    enum { Id = 2004 };
    virtual int id() const { return Id; }

    CutCommand(QList<ResizableItem*> items, DiagramScene* scene, QUndoCommand *parent = 0);
    ~CutCommand();
    virtual void undo();
    virtual void redo();

private:
    QList<ResizableItem*> m_items;
    DiagramScene* m_scene;
    bool m_undoed;
};

///////////////////////////////////////////////////////////////////////////////

class PasteCommand : public QUndoCommand
{
public:
    enum { Id = 2005 };
    virtual int id() const { return Id; }

    PasteCommand(QList<DiagramItem*> items, QMap<int, QList<ResizableItem*> > groupMap, DiagramScene* scene, QUndoCommand *parent = 0);
    ~PasteCommand();
    virtual void undo();
    virtual void redo();

private:
    QList<DiagramItem*> m_items;
    QMap<int, QList<ResizableItem*> > m_groupMap;
    QList<DiagramItemGroup*> m_groups;
    DiagramScene* m_scene;
    bool m_undoed;
};

///////////////////////////////////////////////////////////////////////////////

class GroupCommand : public QUndoCommand
{
public:
    enum { Id = 2006 };
    virtual int id() const { return Id; }

    GroupCommand(QList<ResizableItem*> items, DiagramScene* scene, QUndoCommand *parent = 0);
    virtual void undo();
    virtual void redo();

private:
    QMap<DiagramItem*, int> m_itemGroupIds;
    QList<int> m_groups;
    QList<ResizableItem*> m_diagramItems;
    QList<ResizableItem*> m_groupItems;
    DiagramScene* m_scene;
    DiagramItemGroup *m_group;
};

///////////////////////////////////////////////////////////////////////////////

class UngroupCommand : public QUndoCommand
{
public:
    enum { Id = 2007 };
    virtual int id() const { return Id; }

    UngroupCommand(DiagramItemGroup* item, QUndoCommand *parent = 0);
    virtual void undo();
    virtual void redo();

private:
    QList<DiagramItem*> m_items;
    DiagramScene* m_scene;
    DiagramItemGroup *m_group;
};

///////////////////////////////////////////////////////////////////////////////

class LockCommand : public QUndoCommand
{
public:
    enum { Id = 2008 };
    virtual int id() const { return Id; }

    LockCommand(QList<ResizableItem*> items, QUndoCommand *parent = 0);
    virtual void undo();
    virtual void redo();

private:
    QList<ResizableItem*> m_items;
};

///////////////////////////////////////////////////////////////////////////////

class UnlockCommand : public QUndoCommand
{
public:
    enum { Id = 2009 };
    virtual int id() const { return Id; }

    UnlockCommand(QList<ResizableItem*> items, QUndoCommand *parent = 0);
    virtual void undo();
    virtual void redo();

private:
    QList<ResizableItem*> m_items;
};

///////////////////////////////////////////////////////////////////////////////

class MoveFrontCommand : public QUndoCommand
{
public:
    enum { Id = 2010 };
    virtual int id() const { return Id; }

    MoveFrontCommand(QList<ResizableItem*> items, QUndoCommand *parent = 0);
    virtual void undo();
    virtual void redo();

private:
    QList<ResizableItem*> m_items;
    QList<qreal> m_zValues;
};

///////////////////////////////////////////////////////////////////////////////

class MoveBackCommand : public QUndoCommand
{
public:
    enum { Id = 2011 };
    virtual int id() const { return Id; }

    MoveBackCommand(QList<ResizableItem*> items, QUndoCommand *parent = 0);
    virtual void undo();
    virtual void redo();

private:
    QList<ResizableItem*> m_items;
    QList<qreal> m_zValues;
};

///////////////////////////////////////////////////////////////////////////////

class MoveUpCommand : public QUndoCommand
{
public:
    enum { Id = 2012 };
    virtual int id() const { return Id; }

    MoveUpCommand(ResizableItem* item, QUndoCommand *parent = 0);
    virtual void undo();
    virtual void redo();

private:
    ResizableItem* m_item;
    qreal m_zValue;
    ResizableItem* m_upItem;
};

///////////////////////////////////////////////////////////////////////////////

class MoveDownCommand : public QUndoCommand
{
public:
    enum { Id = 2013 };
    virtual int id() const { return Id; }

    MoveDownCommand(ResizableItem* item, QUndoCommand *parent = 0);
    virtual void undo();
    virtual void redo();

private:
    ResizableItem* m_item;
    qreal m_zValue;
    ResizableItem* m_downItem;
};

///////////////////////////////////////////////////////////////////////////////

class ChangeMultiLineTextsCommand : public QUndoCommand
{
public:
    enum { Id = 2014 };
    virtual int id() const { return Id; }
    virtual bool mergeWith(const QUndoCommand *command);

    ChangeMultiLineTextsCommand(DiagramItem* item, const QStringList& newTexts, QUndoCommand *parent = 0);
    virtual void undo();
    virtual void redo();

private:
    DiagramItem* m_item;
    QSizeF m_oldSize;
    QStringList m_oldTexts;
    QStringList m_newTexts;
};

///////////////////////////////////////////////////////////////////////////////

class AutosizeCommand : public QUndoCommand
{
public:
    enum { Id = 2015 };
    virtual int id() const { return Id; }
    virtual bool mergeWith(const QUndoCommand *command);

    AutosizeCommand(DiagramItem* item, QUndoCommand *parent = 0);
    virtual void undo();
    virtual void redo();

private:
    DiagramItem* m_item;
    QSizeF m_oldSize;
};

///////////////////////////////////////////////////////////////////////////////

class ChangeIntPropertyCommand : public QUndoCommand
{
public:
    enum { Id = 2016 };
    virtual int id() const { return Id; }
    virtual bool mergeWith(const QUndoCommand *command);
    
    ChangeIntPropertyCommand(DiagramItem* item, int newValue, int prop, QUndoCommand *parent = 0);
    virtual void undo();
    virtual void redo();

private:
    DiagramItem* m_item;
    int m_oldValue;
    int m_newValue;
    int m_prop;
};

///////////////////////////////////////////////////////////////////////////////

class ChangeBoolPropertyCommand : public QUndoCommand
{
public:
    enum { Id = 2017 };
    virtual int id() const { return Id; }
    virtual bool mergeWith(const QUndoCommand *command);

    ChangeBoolPropertyCommand(DiagramItem* item, bool newValue, int prop, QUndoCommand *parent = 0);
    virtual void undo();
    virtual void redo();

private:
    DiagramItem* m_item;
    bool m_oldValue;
    bool m_newValue;
    int m_prop;
};

///////////////////////////////////////////////////////////////////////////////

class ChangeColorPropertyCommand : public QUndoCommand
{
public:
    enum { Id = 2018 };
    virtual int id() const { return Id; }
    virtual bool mergeWith(const QUndoCommand *command);
    
    ChangeColorPropertyCommand(DiagramItem* item, QColor newValue, QUndoCommand *parent = 0);
    virtual void undo();
    virtual void redo();

private:
    DiagramItem* m_item;
    QColor m_oldValue;
    QColor m_newValue;
};

#endif // COMMANDS_H
