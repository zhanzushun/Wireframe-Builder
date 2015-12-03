#ifndef ITEMDATA_H
#define ITEMDATA_H

#include <QObject>
#include <QColor>
#include <QRect>
#include <QPoint>
#include <QString>
#include <QImage>
#include <QPixmap>
#include <QFont>

///////////////////////////////////////////////////////////////////////////////

class QPixmap;

enum ColorType
{
    SelectedItemBackground,
    WidgetBackground,
    SelectedWidgetBackground,
    UserColor,
};

enum DrawType
{
    DrawBackground, //rc, clrType
    DrawFrame, // rc
    DrawLine, // rc
    DrawTxt, // rc, text, textFlags
    DrawImage, // rc
    DrawVScrollBar, // rc, value
    DrawLinkText, // rc, text
};

struct Graphy
{
    Graphy() : pm(0), customFont(0), styleSheet(0) {}
    DrawType type;
    QRect rc;
    ColorType clrType;
    QString text;
    int textFlags;
    QPixmap* pm;
    int value;
    QFont* customFont;
    QWidget* styleSheet;
    QColor userClr;
};

///////////////////////////////////////////////////////////////////////////////

class QPainter;
class QStyleOptionGraphicsItem;

class ThemeInterface
{
public:
    virtual void paint(const Graphy& g, QPainter *p, const QStyleOptionGraphicsItem *opt) = 0;
    virtual const QFont & font() = 0;

};

class ThemeStyleSheet : public ThemeInterface
{
public:
    ThemeStyleSheet();
    virtual void paint(const Graphy& g, QPainter *p, const QStyleOptionGraphicsItem *opt);
    virtual const QFont & font() {return m_font;}
private:
    QFont m_font;
};

///////////////////////////////////////////////////////////////////////////////

class DiagramItem;
class QDomElement;
class QDomDocument;
class DiagramItemGroup;

enum PropertyType
{
    P_SingleLineText = 0x1,
    P_MultilineTexts = 0x2,
    P_SelectedIndex = 0x4,
    P_VScrollBar = 0x8,
    P_Value = 0x10,
    P_AutoSize = 0x20,
    P_FontBold = 0x40,
    P_FontItalic = 0x80,
    P_FontUnderline = 0x100,
    P_FontSize = 0x200,
    P_Font = P_FontBold | P_FontItalic | P_FontUnderline | P_FontSize,
    P_Color = 0x400,
    P_Icon = 0x800,
    P_State = 0x1000,
};

enum ResizeMode
{
    ResizeModeNone,
    ResizeModeHorz,
    ResizeModeVert,
    ResizeModeAll,
};

enum StateMode
{
    StateNormal,
    StateInFocus,
    StateSelected,
    StateDisabled,
};

///////////////////////////////////////////////////////////////////////////////

class ItemDataBase : public QObject
{
    Q_OBJECT
public:
    static QString joinTexts(const QStringList & texts, const QString& seperator);
    explicit ItemDataBase(DiagramItem *item) : QObject(0) {m_item = item;}
    DiagramItem* item() {return m_item;}

    void init();
    void update(bool toMeasuredSize);
    bool load(const QDomElement& element);
    static DiagramItem* sload(const QDomElement& element);
    bool save(QDomDocument& doc, QDomElement& element);
    bool load(const QString& xml);
    bool save(QString& xml);
    void setProperty(const QString& sProp, const QString& sValue);
    const QSize& mesuredSize() {return m_measuredSize;}
    int groupId();
    DiagramItemGroup* group();
    QRect posRect();
    void posRect_changed() {calculateDrawingSequence();}
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option);

protected:
    void addPropertyToDomElement(QDomDocument& doc, QDomElement& props);
    const QList<Graphy>& drawingSequence() {return m_drawingSequence;}
    DiagramItem* m_item;
    QList<Graphy> m_drawingSequence;
    QSize m_measuredSize;
    void pTrimText();
    void addFrameGraphy(const QRect& rc);
    void addBackgroundGraphy(const QRect& rc, ColorType clrType);
    void addLineGraphy(const QRect& rc);
    void addTextGraphy(const QRect& rc, int textFlags, const QString& text);
    void addVScrollbarGraphy(const QRect& rc, int value);
    void addImageGraphy(const QRect& rc, QPixmap* pm);

public:
    virtual ResizeMode resizeMode() {return ResizeModeAll;}
    virtual void propertyChanged(PropertyType type) {if ((type & getProperties()) > 0) update(false);}
    virtual int getProperties() = 0;
    virtual void setDefaultData() = 0;
    virtual void calculateDrawingSequence() = 0;
    virtual void calculateMesuredSize() = 0;
    virtual void parseData() {};

    ///////////////////////////////////////////////////////////////////////////////
    // properties

public:
    const QString& text() const     {return (m_texts.length() > 0) ? m_texts[0] : s_emptyString;}
    const QStringList& texts() const {return m_texts;}
    virtual const QStringList& selectableTexts() const {return m_texts;}
    virtual int selectedIndex() const       {return m_selectedIndex;}
    bool vScrollbar() const         {return m_vScrollbar;}
    int value()                     {return m_value;}
    bool fontBold()                 {return m_fontBold;}
    bool fontItalic()               {return m_fontItalic;}
    bool fontUnderline()            {return m_fontUnderline;}
    int fontSize()                  {return m_fontSize;}
    QColor color()                  {return m_color;}

public Q_SLOTS:
    void autoResize();
    void setText(const QString& v)  {m_texts.clear(); m_texts.append(v); pTrimText(); propertyChanged(P_SingleLineText);}
    void setTexts(const QStringList& v)  {m_texts = v; pTrimText(); propertyChanged(P_MultilineTexts);}
    virtual void setSelectedIndex(int v)    {m_selectedIndex = v; propertyChanged(P_SelectedIndex);}
    void setVScrollbar(int v)       {setVScrollbar(v ? true : false);}
    void setVScrollbar(bool v)      {m_vScrollbar = v; propertyChanged(P_VScrollBar);}
    void setValue(int v)            {m_value = v; propertyChanged(P_Value);}
    void setFontBold(bool v)        {m_fontBold = v; propertyChanged(P_FontBold);}
    void setFontItalic(bool v)      {m_fontItalic = v; propertyChanged(P_FontItalic);}
    void setFontUnderline(bool v)   {m_fontUnderline = v; propertyChanged(P_FontUnderline);}
    void setFontSize(int v)         {m_fontSize = v; propertyChanged(P_FontSize);}
    void setColor(QColor v)         {m_color = v; propertyChanged(P_Color);}

protected:
    static QString s_emptyString;
    QStringList m_texts;
    int m_selectedIndex;
    bool m_vScrollbar;
    int m_value;
    bool m_fontBold;
    bool m_fontItalic;
    bool m_fontUnderline;
    int m_fontSize;
    int m_state;
    QColor m_color;
};


///////////////////////////////////////////////////////////////////////////////

class NotImplementedYet : public ItemDataBase
{
public:
    explicit NotImplementedYet(DiagramItem *item) : ItemDataBase(item) {}
    virtual int getProperties() {return P_SingleLineText; }
    virtual void setDefaultData();
    virtual void calculateDrawingSequence();
    virtual void calculateMesuredSize();
};

// calendar, chart bar, chart column, chart line, chart pie, formatting toolbar, playback controls,
// red x, street map, horz splitter, vert splitter, web cam, volume slider
///////////////////////////////////////////////////////////////////////////////

class ImageItemData : public ItemDataBase
{
public:
    explicit ImageItemData(DiagramItem *item) : ItemDataBase(item) {}
    virtual int getProperties() {return 0;}
    virtual void calculateMesuredSize() {}
    virtual void setDefaultData();
    virtual void calculateDrawingSequence();
protected:
    QString m_sImage;
    QImage m_image;
    QPixmap m_pixmap;
};

///////////////////////////////////////////////////////////////////////////////

class Accordion : public ItemDataBase
{
public:
    explicit Accordion(DiagramItem *item) : ItemDataBase(item) {}
    virtual int getProperties() {return P_MultilineTexts|P_SelectedIndex|P_VScrollBar|P_Value|P_AutoSize; }
    virtual void setDefaultData();
    virtual void calculateDrawingSequence();
    virtual void calculateMesuredSize();
    virtual void parseData();
private:
    QStringList m_visibleTexts;
    int m_selectedParent;
    int m_nextParent;
    int m_selectedVisibleIndex;
};

///////////////////////////////////////////////////////////////////////////////

class AlertBox : public ItemDataBase
{
public:
    explicit AlertBox(DiagramItem *item) : ItemDataBase(item) {}
    virtual int getProperties() {return P_MultilineTexts | P_AutoSize; }
    virtual void setDefaultData();
    virtual void calculateDrawingSequence();
    virtual void calculateMesuredSize();
    virtual void parseData();
private:
    QString m_title;
    QString m_text;
    QString m_button1;
    QString m_button2;
};

///////////////////////////////////////////////////////////////////////////////

class Breadcrumbs : public ItemDataBase
{
public:
    explicit Breadcrumbs(DiagramItem *item) : ItemDataBase(item) {}
    virtual int getProperties() {return P_SingleLineText | P_Font; }
    virtual void setDefaultData();
    virtual void calculateDrawingSequence();
    virtual void calculateMesuredSize();
    virtual void parseData();
private:
    QFont m_font;
};

///////////////////////////////////////////////////////////////////////////////

class BrowserWindow : public ItemDataBase
{
public:
    explicit BrowserWindow(DiagramItem *item) : ItemDataBase(item) {}
    virtual int getProperties() {return P_VScrollBar | P_Value | P_MultilineTexts; }
    virtual void setDefaultData();
    virtual void calculateDrawingSequence();
    virtual void calculateMesuredSize();
};

///////////////////////////////////////////////////////////////////////////////

class Button : public ItemDataBase
{
public:
    explicit Button(DiagramItem *item) : ItemDataBase(item) {}
    virtual int getProperties() {return P_AutoSize | P_Color | P_SingleLineText | P_Font | P_Icon | P_State; }
    virtual void setDefaultData();
    virtual void calculateDrawingSequence();
    virtual void calculateMesuredSize();
    virtual const QStringList& selectableTexts() const {return m_states;}
    virtual void setSelectedIndex(int v)    {m_state = v; propertyChanged(P_State);}
    virtual int selectedIndex() const       {return m_state;}
    virtual void parseData();
private:
    QStringList m_states;
    QFont m_font;
};

#endif // ITEMDATA_H
