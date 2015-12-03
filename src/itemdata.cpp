#include "itemdata.hxx"
#include <QtGui>
#include <QtXml>
#include "document.hxx"
#include "mainwindow.hxx"

#define MAX_WIDGET_WIDTH                    2000
#define DEF_FONT_SIZE                       11
#define DEF_FONT_NAME                       "Comic Sans MS"
#define WIDGET_BKCLR                        QColor(255, 255, 255)
#define SEL_WIDGET_BKCLR                    QColor(214, 206, 204)
#define SEL_ITEM_BKCLR                      QColor(129, 152, 193)
#define LINK_TEXT_CLR                       QColor(48,119,178)
#define SLIDER_MAX_VAL                      100

///////////////////////////////////////////////////////////////////////////////

#define ADD_STR_PROPERTY(type, name, str) \
    if (getProperties() & (type)) \
    { \
        QDomElement nd = doc.createElement(name); \
        QDomText t = doc.createTextNode(str); \
        nd.appendChild(t); \
        props.appendChild(nd); \
    }

#define ADD_PROPERTY(type, name, value) \
    ADD_STR_PROPERTY(type, name, QString("%1").arg(value))

#define SET_PROPERTY(type, name, prop, svalue) \
    if (getProperties() & type) \
    { \
        if (sProp == name) \
        { \
            prop = svalue; \
        } \
    }

#define SET_PROPERTY_INT(type, name, prop, svalue) \
    if (getProperties() & type) \
    { \
        if (sProp == name) \
        { \
            bool ok = false; int t; t = svalue.toInt(&ok); if (ok) prop = (t); \
        } \
    }

///////////////////////////////////////////////////////////////////////////////

int textWidth(const QString& t, const QFont* font = NULL)
{
    if (font == NULL)
        font = & (MainWindow::instance()->currentTheme()->font());
    QFontMetrics fm(*font, MainWindow::instance()->currentDocument()->viewport());
    return fm.width(t);
}

int textHeight(const QString& t, int w, const QFont* font = NULL)
{
    if (font == NULL)
        font = & (MainWindow::instance()->currentTheme()->font());
    QFontMetrics fm(*font, MainWindow::instance()->currentDocument()->viewport());
    QRect rc = fm.boundingRect(0,0, w, MAX_WIDGET_WIDTH, 
        Qt::AlignLeft | Qt::AlignTop | Qt::TextWrapAnywhere | Qt::TextWordWrap, t);
    return rc.height();
}

int textHeight(const QFont* font = NULL)
{
    if (font == NULL)
        font = & (MainWindow::instance()->currentTheme()->font());
    QFontMetrics fm(*font, MainWindow::instance()->currentDocument()->viewport());
    return fm.height();
}

///////////////////////////////////////////////////////////////////////////////

ThemeStyleSheet::ThemeStyleSheet()
{
    m_font = QFont(DEF_FONT_NAME, DEF_FONT_SIZE);
}

void ThemeStyleSheet::paint(const Graphy& graphy, QPainter *painter, const QStyleOptionGraphicsItem *)
{
    if (graphy.type == DrawFrame)
    {
        QStyleOptionFrameV2 option;
        option.rect = graphy.rc;
        QWidget* styleSheet = MainWindow::instance()->frameSample;
        if (graphy.styleSheet != NULL)
            styleSheet = graphy.styleSheet;
        QApplication::style()->drawPrimitive(QStyle::PE_Frame, &option, painter, styleSheet);
    }
    else if (graphy.type == DrawBackground)
    {
        QStyleOptionFrameV2 option;
        option.rect = graphy.rc;
        QColor clr;
        if (graphy.clrType == WidgetBackground)
            clr = WIDGET_BKCLR;
        else if (graphy.clrType == SelectedWidgetBackground)
            clr = SEL_WIDGET_BKCLR;
        else if (graphy.clrType == SelectedItemBackground)
            clr = SEL_ITEM_BKCLR;
        else if (graphy.clrType == UserColor)
            clr = graphy.userClr;
        painter->fillRect(option.rect, clr);
    }
    else if (graphy.type == DrawLinkText)
    {
        QPen pen(LINK_TEXT_CLR);
        QFont font = m_font;
        if (graphy.customFont != NULL)
            font = (*graphy.customFont);
        font.setUnderline(true);
        painter->setFont(font);
        painter->setPen(pen);
        painter->drawText(graphy.rc, graphy.textFlags, graphy.text);
    }
    else if (graphy.type == DrawTxt)
    {
        QPen pen(Qt::black);
        painter->setPen(pen);

        if (graphy.customFont != NULL)
            painter->setFont(*graphy.customFont);
        else
            painter->setFont(m_font);
        painter->drawText(graphy.rc, graphy.textFlags, graphy.text);
    }
    else if (graphy.type == DrawLine)
    {
        QStyleOptionFrameV2 option;
        option.rect = graphy.rc;
        QApplication::style()->drawPrimitive(QStyle::PE_Frame, &option, painter, MainWindow::instance()->lineSample);
    }
    else if (graphy.type == DrawVScrollBar)
    {
        QStyleOptionSlider opt;
        opt.state = QStyle::State_Active | QStyle::State_Enabled;
        opt.rect = graphy.rc;
        opt.orientation = Qt::Vertical;
        opt.sliderValue = graphy.value;
        opt.sliderPosition = opt.sliderValue;
        opt.minimum = 0;
        opt.maximum = SLIDER_MAX_VAL;
        QApplication::style()->drawComplexControl(QStyle::CC_ScrollBar, &opt, painter, 
            MainWindow::instance()->verticalScrollBarSample);
    }
    else if (graphy.type == DrawImage)
    {
        painter->drawPixmap(graphy.rc, *graphy.pm);
    }
}

///////////////////////////////////////////////////////////////////////////////

QString ItemDataBase::s_emptyString;

void ItemDataBase::paint(QPainter *painter, const QStyleOptionGraphicsItem *option)
{
    if (m_drawingSequence.length() == 0)
        calculateDrawingSequence();
    foreach (Graphy graphy, m_drawingSequence)
    {
        if (graphy.type == DrawBackground &&
            (graphy.clrType == WidgetBackground || graphy.clrType == SelectedWidgetBackground))
        {
            graphy.clrType = item()->isSelected() ? SelectedWidgetBackground : WidgetBackground;
        }
        MainWindow::instance()->currentTheme()->paint(graphy, painter, option);
    }
}

void trimTexts(QStringList & texts)
{
    QStringList tempTexts = texts;
    texts.clear();
    foreach (QString text, tempTexts)
    {
        QString t = text.trimmed();
        if (t.length() > 0)
            texts.append(t);
    }
}

void ItemDataBase::pTrimText()
{
    trimTexts(m_texts);
}

void ItemDataBase::addFrameGraphy(const QRect& rc)
{
    Graphy t;
    t.type = DrawFrame;
    t.rc = rc;
    m_drawingSequence.append(t);
}
void ItemDataBase::addBackgroundGraphy(const QRect& rc, ColorType clrType)
{
    Graphy t;
    t.type = DrawBackground;
    t.rc = rc;
    t.clrType = clrType;
    m_drawingSequence.append(t);

}
void ItemDataBase::addLineGraphy(const QRect& rc)
{
    Graphy t;
    t.type = DrawLine;
    t.rc = rc;
    m_drawingSequence.append(t);
}
void ItemDataBase::addTextGraphy(const QRect& rc, int textFlags, const QString& text)
{
    Graphy t;
    t.type = DrawTxt;
    t.rc = rc;
    t.text = text;
    t.textFlags = textFlags;
    m_drawingSequence.append(t);
}
void ItemDataBase::addVScrollbarGraphy(const QRect& rc, int value)
{
    Graphy t;
    t.type = DrawVScrollBar;
    t.rc = rc;
    t.value = value;
    m_drawingSequence.append(t);
}

void ItemDataBase::addImageGraphy(const QRect& rc, QPixmap* pm)
{
    Graphy t;
    t.type = DrawImage;
    t.rc = rc;
    t.pm = pm;
    m_drawingSequence.append(t);
}

DiagramItemGroup* ItemDataBase::group()
{
    if (item()->parentItem() != NULL)
    {
        Q_ASSERT(item()->parentItem()->type() == DiagramItemGroup::Type);
        return (DiagramItemGroup*)(item()->parentItem());
    }
    return NULL;
}

int ItemDataBase::groupId()
{
    int groupId = -1;
    if (group() != NULL)
        groupId = group()->diagramItems().at(0)->id();
    return groupId;
}

bool ItemDataBase::load(const QDomElement& element)
{
    // do nothing with isInGroup, controlID, controlTypeID.
    QPointF pos = item()->pos();
    QSizeF size = item()->size();
    bool ok = false;
    int t;
    t = element.attribute("x", "-2").toInt(&ok); if (ok && t >= 0) pos.setX(t);
    t = element.attribute("y", "-2").toInt(&ok); if (ok && t >= 0) pos.setY(t);
    t = element.attribute("w", "-2").toInt(&ok); if (ok && t >= 0) size.setWidth(t);
    t = element.attribute("h", "-2").toInt(&ok); if (ok && t >= 0) size.setHeight(t);
    item()->setPos(pos);
    item()->setSize(size);
    t = element.attribute("measuredW", "-2").toInt(&ok); if (ok && t >= 0) m_measuredSize.setWidth(t);
    t = element.attribute("measuredH", "-2").toInt(&ok); if (ok && t >= 0) m_measuredSize.setHeight(t);
    t = element.attribute("zOrder", QString("%1").arg(item()->zValue())).toDouble(&ok); if (ok) item()->setZValue(t);
    t = element.attribute("locked", QString("%1").arg(item()->locked())).toInt(&ok); if (ok) item()->setLocked(t ? true : false);
    if (element.childNodes().length() == 0)
        return true;

    QDomNode propsNd = element.childNodes().at(0);
    if (propsNd.nodeType() != QDomNode::ElementNode)
        return true;
    QDomElement* props = (QDomElement*) &propsNd;
    if (props->nodeName() != "controlProperties")
        return true;

    for(int i=0; i<(int)props->childNodes().length(); i++)
    {
        QDomNode nd = props->childNodes().at(i);
        if (nd.nodeType() != QDomNode::ElementNode)
            continue;
        QDomElement* prop = (QDomElement*) &nd;
        if (prop->childNodes().length() > 0)
        {
            QDomNode nd2 = prop->childNodes().at(0);
            if (nd2.nodeType() == QDomNode::TextNode)
            {
                QDomText* text = (QDomText*) &nd2;
                QString sProp = nd.nodeName();
                QString sValue = text->data();
                if (getProperties() & P_SingleLineText) 
                { 
                    if (sProp == "text") 
                    { 
                        m_texts.clear();
                        m_texts.append(sValue.trimmed());
                    } 
                }
                setProperty(sProp, sValue);
            }
        }
    }
    if (getProperties() & P_SelectedIndex)
    {
        if (m_selectedIndex < 0 || m_selectedIndex >= m_texts.length())
            m_selectedIndex = 0;
    }
    update(true);
    return true;
}

DiagramItem* ItemDataBase::sload(const QDomElement& element)
{
    // do nothing with isInGroup, controlID.
    bool ok = false;
    int t;
    t = element.attribute("controlTypeID", "-1").toInt(&ok); 
    if (ok && t >= 0 && t < KeyLast)
    {
        DiagramItem* item = new DiagramItem((DiagramKey)t, QPointF(0, 0));
        item->itemData()->load(element);
        return item;
    }
    return NULL;
}

QString ItemDataBase::joinTexts(const QStringList & texts, const QString& seperator)
{
    QString s;
    foreach (const QString& t, texts)
    {
        if (s.isEmpty())
            s = t;
        else
            s += seperator + t;
    }
    return s;
}

bool ItemDataBase::save(QDomDocument& doc, QDomElement& element)
{
    int groupx(0), groupy(0);
    if (group() != NULL)
    {
        groupx = group()->pos().x();
        groupy = group()->pos().y();
    }
    element.setAttribute("controlID", item()->id());
    element.setAttribute("controlTypeID", item()->key());
    element.setAttribute("x", groupx + (int)item()->pos().x());
    element.setAttribute("y", groupy + (int)item()->pos().y());
    element.setAttribute("w", (posRect().size() == m_measuredSize) ? -1 : posRect().width());
    element.setAttribute("h", (posRect().size() == m_measuredSize) ? -1 : posRect().height());
    element.setAttribute("measuredW", m_measuredSize.width());
    element.setAttribute("measuredH", m_measuredSize.height());
    element.setAttribute("zOrder", item()->zValue());
    element.setAttribute("locked", item()->locked());
    element.setAttribute("isInGroup", groupId());
    QDomElement props = doc.createElement("controlProperties");
    addPropertyToDomElement(doc, props);
    element.appendChild(props);
    return true;
}

bool ItemDataBase::load(const QString& xml)
{
    QDomDocument doc;
    if (doc.setContent(xml))
        if (doc.documentElement().nodeName() == "control")
            return load(doc.documentElement());
    return false;
}
bool ItemDataBase::save(QString& xml)
{
    QDomDocument doc;
    QDomElement rootElm = doc.createElement("control");
    if(save(doc, rootElm))
    {
        doc.appendChild(rootElm);
        xml = doc.toString();
        return true;
    }
    return false;
}

QRect ItemDataBase::posRect()
{
    int w = (int)item()->size().width();
    int h = (int)item()->size().height();
    return QRect(0, 0, w, h);
}

void ItemDataBase::init()
{
    setDefaultData();
    update(true);
}

void ItemDataBase::update(bool toMeasuredSize)
{
    QSize oldMeasuredSize = m_measuredSize;
    parseData();
    calculateMesuredSize(); 
    if (toMeasuredSize)
        m_item->setSize(m_measuredSize);
    if (m_measuredSize.width() > oldMeasuredSize.width() ||
        m_measuredSize.height() > oldMeasuredSize.height())
        m_item->setSize(m_measuredSize);
    calculateDrawingSequence();
    m_item->update();
}

void ItemDataBase::autoResize()
{
    m_item->setSize(m_measuredSize);
    calculateDrawingSequence();
    m_item->scene()->update();
}

///////////////////////////////////////////////////////////////////////////////

void ItemDataBase::setProperty(const QString& sProp, const QString& sValue)
{
    SET_PROPERTY(P_MultilineTexts, "text", m_texts, sValue.trimmed().split("%0A"));
    SET_PROPERTY_INT(P_SelectedIndex, "selectedIndex", m_selectedIndex, sValue);
    SET_PROPERTY_INT(P_VScrollBar, "verticalScrollBar", m_vScrollbar, sValue);
    SET_PROPERTY_INT(P_Value, "value", m_value, sValue);
    SET_PROPERTY_INT(P_FontBold, "bold", m_fontBold, sValue);
    SET_PROPERTY_INT(P_FontItalic, "italic", m_fontItalic, sValue);
    SET_PROPERTY_INT(P_FontUnderline, "underline", m_fontUnderline, sValue);
    SET_PROPERTY_INT(P_FontSize, "fontsize", m_fontSize, sValue);
    SET_PROPERTY_INT(P_State, "state", m_state, sValue);
    if ((getProperties() & P_Color) && (sProp == "color"))
        m_color.setNamedColor(sValue);
}

void ItemDataBase::addPropertyToDomElement(QDomDocument& doc, QDomElement& props)
{
    ADD_STR_PROPERTY(P_SingleLineText, "text", (m_texts[0]));
    ADD_STR_PROPERTY(P_MultilineTexts, "text", (joinTexts(m_texts, "%0A")));
    ADD_PROPERTY(P_SelectedIndex, "selectedIndex", m_selectedIndex);
    ADD_PROPERTY(P_MultilineTexts, "verticalScrollBar", m_vScrollbar);
    ADD_PROPERTY(P_Value, "value", m_value);
    ADD_PROPERTY(P_FontBold, "bold", m_fontBold);
    ADD_PROPERTY(P_FontItalic, "italic", m_fontItalic);
    ADD_PROPERTY(P_FontUnderline, "underline", m_fontUnderline);
    ADD_PROPERTY(P_FontSize, "fontsize", m_fontSize);
    ADD_PROPERTY(P_State, "state", m_state);
    ADD_STR_PROPERTY(P_Color, "color", m_color.name());
}

///////////////////////////////////////////////////////////////////////////////

void NotImplementedYet::setDefaultData()
{
    QString s = ("%1: Not implemented yet");
    m_texts.append(s.arg(DiagramLibrary::diagramTypeFromKey(m_item->key())));
}

void NotImplementedYet::calculateDrawingSequence()
{
    m_drawingSequence.clear();
    addFrameGraphy(posRect());
    addTextGraphy(posRect(), Qt::AlignCenter | Qt::TextWrapAnywhere, text());
}

void NotImplementedYet::calculateMesuredSize()
{
    m_measuredSize = QSize(300, 100);
}

///////////////////////////////////////////////////////////////////////////////

void ImageItemData::calculateDrawingSequence()
{
    m_drawingSequence.clear();
    QRect rc = posRect();
    addImageGraphy(rc, &m_pixmap);
}

void ImageItemData::setDefaultData()
{
    if (item()->key() == KeyCalendar)
        m_sImage = ":/images/calendar.png";
    else if (item()->key() == KeyChartBar)
        m_sImage = ":/images/chartbar.png";
    else if (item()->key() == KeyChartColumn)
        m_sImage = ":/images/chartcolumn.png";
    else if (item()->key() == KeyChartLine)
        m_sImage = ":/images/chartline.png";
    else if (item()->key() == KeyChartPie)
        m_sImage = ":/images/chartpie.png";
    else if (item()->key() == KeyFormattingToolbar_RichTextEditorToolbar)
        m_sImage = ":/images/formattingtoolbar.png";
    else if (item()->key() == KeyHorzSplitter_Separator_DragBar)
        m_sImage = ":/images/horzsplitter.png";
    else if (item()->key() == KeyPlaybackControls)
        m_sImage = ":/images/playbackcontrols.png";
    else if (item()->key() == KeyRedX_XNay)
        m_sImage = ":/images/redx.png";
    else if (item()->key() == KeyStreetMap)
        m_sImage = ":/images/streetmap.png";
    else if (item()->key() == KeyVertSplitter_Separator_DragBar)
        m_sImage = ":/images/vertsplitter.png";
    else if (item()->key() == KeyVolumeSlider)
        m_sImage = ":/images/volumeslider.png";
    else if (item()->key() == KeyWebcam)
        m_sImage = ":/images/webcam.png";
    else
        m_sImage = ":/icons/diagramdemo.png";

    m_image.load(m_sImage);
    m_pixmap = QPixmap::fromImage(m_image, Qt::OrderedAlphaDither);
    m_measuredSize.setWidth(m_image.width());
    m_measuredSize.setHeight(m_image.height());
}

///////////////////////////////////////////////////////////////////////////////

void Accordion::setDefaultData()
{
    m_texts.append("Item One");
    m_texts.append("Item Two");
    m_texts.append("- SubItem 2.1");
    m_texts.append("- SubItem 2.2");
    m_texts.append("Item Three");
    m_texts.append("Item Four");
    m_selectedIndex = 1;
    m_selectedParent = -1;
    m_vScrollbar = true;
    m_value = 20;
    m_measuredSize.setWidth(200);
}

void Accordion::parseData()
{
    m_selectedParent = -1;
    m_nextParent = -1;

    int lastParent = 0;
    for (int i=0; i<m_texts.length(); i++)
    {
        const QString& s = m_texts[i];
        bool isSubItem = (s.left(2) == "- ");
        if (i <= m_selectedIndex && !isSubItem)
            lastParent = i;
        if (i == m_selectedIndex)
            m_selectedParent = lastParent;
    }

    m_visibleTexts.clear();
    bool enterSelectedParent = false;
    for (int i=0; i<m_texts.length(); i++)
    {
        const QString& s = m_texts[i];
        bool isSubItem = (s.left(2) == "- ");
        if (!isSubItem) m_visibleTexts.append(s);

        if (enterSelectedParent && !isSubItem)
        {
            m_nextParent = m_visibleTexts.length() - 1;
            enterSelectedParent = false;
        }
        if (i == m_selectedParent)
        {
            enterSelectedParent = true;
            m_selectedParent = m_visibleTexts.length() - 1;
        }
        if (enterSelectedParent && isSubItem)
            m_visibleTexts.append(s);
        if (i == m_selectedIndex)
            m_selectedVisibleIndex = m_visibleTexts.length() - 1;
    }
    if (m_nextParent == -1)
        m_nextParent = m_visibleTexts.length();
}

void Accordion::calculateMesuredSize()
{
    int unitHeight = 30;
    int selectedMinHeight = 150;
    m_measuredSize.setHeight(unitHeight * (m_visibleTexts.length()-1) + selectedMinHeight);
}

void Accordion::calculateDrawingSequence()
{
    m_drawingSequence.clear();

    int borderOffset = 3;
    int unitHeight = 30;
    int textOffsetY = 3;
    int textOffsetX = 10;
    int scrollbarWidth = 16;

    QRect rc = posRect();
    int width = rc.width();
    int height = rc.height();
    int selectedHeight = height - unitHeight * (m_visibleTexts.length()-1) + 
        ((m_nextParent - 1) - m_selectedParent) * unitHeight;

    addFrameGraphy(rc); // frame

    QRect curRc = QRect(borderOffset, borderOffset, width - borderOffset * 2, height - borderOffset * 2);
    addBackgroundGraphy(curRc, WidgetBackground); // background

    int n = m_visibleTexts.length();
    if (n == 0)
        return;
    int y = 0;
    int nextParentY = -1;

    for(int i=0; i<m_visibleTexts.length(); i++)
    {
        QString text = m_visibleTexts[i];
        bool isSubItem = (text.left(2) == "- ");
        if (isSubItem)
           text = "    " + text.mid(2);
        if (y >= height)
            break;
        if (i == m_nextParent)
            y = nextParentY;
        if ((i != n-1 && !isSubItem) || ((i == n-1) && i == m_selectedParent))
        {
            curRc = QRect(borderOffset, y + unitHeight, width-borderOffset*2, 0);
            addLineGraphy(curRc); // draw the line below current item
        }

        if (i == m_selectedParent)
        {
            if (m_nextParent < n)
            {
                nextParentY = y + selectedHeight;
                curRc = QRect(borderOffset, nextParentY, width-borderOffset*2, 0);
                addLineGraphy(curRc); // draw the line above next item
            }
            curRc = QRect(borderOffset, y + borderOffset, width - borderOffset * 2, unitHeight - borderOffset);
            addBackgroundGraphy(curRc, SelectedItemBackground); // draw background

            if (vScrollbar())
            {
                curRc = QRect(width - borderOffset - scrollbarWidth, y + borderOffset + unitHeight, 
                    scrollbarWidth, selectedHeight - unitHeight - borderOffset);
                addVScrollbarGraphy(curRc, value()); // vscrollbar
            }
        }
        if (i == m_selectedVisibleIndex && i != m_selectedParent)
        {
            curRc = QRect(borderOffset, y + borderOffset, width - borderOffset * 2 - scrollbarWidth, unitHeight - borderOffset);
            addBackgroundGraphy(curRc, SelectedItemBackground); // draw background
        }
        curRc = QRect(textOffsetX, y + textOffsetY, width - textOffsetX * 2, y + unitHeight - textOffsetY * 2);
        addTextGraphy(curRc, Qt::AlignLeft | Qt::AlignTop, text); // text
        y += unitHeight;
    }
}

///////////////////////////////////////////////////////////////////////////////

void AlertBox::setDefaultData()
{
    m_texts.append("Alert");
    m_texts.append("Alert text goes here");
    m_texts.append("No, Yes");
}

void AlertBox::parseData()
{
    if (m_texts.length() > 0)
        m_title = m_texts[0];
    if (m_texts.length() > 1)
    {
        QStringList l = m_texts[m_texts.length() - 1].split(',');
        if (l.length() > 0)
            m_button1 = l[0];
        if (l.length() > 1)
            m_button2 = l[1];
    }
    if (m_texts.length() > 2)
    {
        int i=0;
        foreach (QString s, m_texts)
        {
            if (i != 0 && i!= m_texts.length() - 1)
                m_text = (i == 1) ? m_texts[i] : (m_text + "\n" + m_texts[i]);
            i++;
        }
    }
}

void AlertBox::calculateMesuredSize()
{
    int spacing = 10;
    int titleHeight = 20;
    int btnPadding = 10;
    int btnHeight = 40;
    int btnMinWidth = 100;

    int minWidth = btnMinWidth * 2 + spacing * 3;
    int w1 = textWidth(m_title) + spacing * 2;
    if (w1 < minWidth)
        w1 = minWidth;
    int w2 = textWidth(m_button1) + textWidth(m_button2) + btnPadding * 4 + spacing * 3;
    int w = (w1 > w2 ? w1 : w2);
    int h = textHeight(m_text, w) + spacing * 6 + btnHeight + titleHeight;
    w += spacing * 2;
    
    m_measuredSize.setWidth(w);
    m_measuredSize.setHeight(h);
}

void AlertBox::calculateDrawingSequence()
{
    m_drawingSequence.clear();
    
    int spacing = 10;
    int titleHeight = 20;
    int btnHeight = 40;
    int borderOffset = 3;

    QRect rc = posRect();
    int w = rc.width();
    int h = rc.height();

    addFrameGraphy(rc); // frame

    QRect curRc = QRect(borderOffset, borderOffset, w - borderOffset * 2, h - borderOffset * 2);
    addBackgroundGraphy(curRc, WidgetBackground); // background

    curRc = QRect(spacing, spacing, w - spacing * 2, titleHeight);
    addTextGraphy(curRc, Qt::AlignHCenter | Qt::AlignTop, m_title); // title

    curRc = QRect(spacing, spacing*3 + titleHeight, w - spacing * 2, h);
    addTextGraphy(curRc, Qt::AlignHCenter | Qt::AlignTop | Qt::TextWrapAnywhere | Qt::TextWordWrap, m_text); // text

    int btnW = (w - spacing * 3) / 2;
    QRect rc1 = QRect(spacing, h - btnHeight - spacing, btnW, btnHeight);
    QRect rc2 = QRect(spacing*2 + btnW, h - btnHeight - spacing, btnW, btnHeight);
    addFrameGraphy(rc1);
    addFrameGraphy(rc2);
    addTextGraphy(rc1, Qt::AlignCenter, m_button1);
    addTextGraphy(rc2, Qt::AlignCenter, m_button2);
}

///////////////////////////////////////////////////////////////////////////////

void Breadcrumbs::setDefaultData()
{
    m_fontBold = false;
    m_fontItalic = false;
    m_fontUnderline = false;
    m_fontSize = 11;
    m_texts.append("One, Two, Three");
}

void Breadcrumbs::calculateDrawingSequence()
{
    m_drawingSequence.clear();

    if (m_texts.length() <= 0)
        return;
    QStringList texts = m_texts[0].split(',');
    trimTexts(texts);
    QRect rc = posRect();
    QPoint pt = rc.topLeft();
    int h = rc.height();
    int w = 0;
    int i = 0;
    int textFlags = Qt::AlignLeft | Qt::AlignVCenter | Qt::TextDontClip;
    foreach(QString s, texts)
    {
        if (i < texts.length() - 1)
        {
            w = textWidth(s, &m_font);
            Graphy t;
            t.type = DrawLinkText;
            t.rc = QRect(pt, QSize(w, h));
            t.text = s;
            t.textFlags = textFlags;
            t.customFont = &m_font;
            m_drawingSequence.append(t);
            pt.setX(pt.x() + w);

            s = " > ";
            w = textWidth(s, &m_font);
            addTextGraphy(QRect(pt, QSize(w, h)), textFlags, s);
            m_drawingSequence[m_drawingSequence.length() - 1].customFont = &m_font;
            pt.setX(pt.x() + w);
        }
        else
        {
            w = textWidth(s, &m_font);
            addTextGraphy(QRect(pt, QSize(w, h)), textFlags, s);
            m_drawingSequence[m_drawingSequence.length() - 1].customFont = &m_font;
            pt.setX(pt.x() + w);
        }
        i++;
    }
}

void Breadcrumbs::calculateMesuredSize()
{
    if (m_texts.length() <= 0)
        return;
    QStringList texts = m_texts[0].split(',');
    trimTexts(texts);
    QString text = joinTexts(texts, " > ");
    int offset = 0;
    int h = textHeight(&m_font);
    int w = textWidth(text, &m_font);
    m_measuredSize = QSize(w + offset *2, h + offset *2);
}

void Breadcrumbs::parseData()
{
    m_font = MainWindow::instance()->currentTheme()->font();
    m_font.setBold(m_fontBold);
    m_font.setItalic(m_fontItalic);
    m_font.setPointSize(m_fontSize);
}


///////////////////////////////////////////////////////////////////////////////

void BrowserWindow::setDefaultData()
{
    m_texts.append("A web page");
    m_texts.append("http://www.google.com");
    m_vScrollbar = false;
    m_value = 50;
}

void BrowserWindow::calculateMesuredSize()
{
    m_measuredSize = QSize(625, 375);
}

void BrowserWindow::calculateDrawingSequence()
{
    m_drawingSequence.clear();
    QRect rc = posRect();
    addFrameGraphy(rc);
    m_drawingSequence[m_drawingSequence.length() - 1].styleSheet = MainWindow::instance()->frameBrowserWindow;

    QPoint ptRow1(160, 6);
    QPoint ptRow2(160, 32);
    int borderRight = 95;
    int row1Height = 19;
    int row2Height = 15;
    int scrollbarWidth = 16;
    int borderOffset = 3;
    int topHeight = 76;
    int bottomHeight = 28;

    int w = rc.right() - borderRight - ptRow1.x();
    if (m_texts.length() > 0)
        addTextGraphy(QRect(ptRow1, QSize(w, row1Height)), Qt::AlignCenter, m_texts[0]);
    if (m_texts.length() > 1)
        addTextGraphy(QRect(ptRow2, QSize(w, row2Height)), Qt::AlignLeft | Qt::AlignVCenter, m_texts[1]);

    if (vScrollbar())
    {
        QRect vrc = QRect(
            rc.width() - borderOffset - scrollbarWidth, 
            topHeight, 
            scrollbarWidth, 
            rc.height() - bottomHeight - topHeight);
        addVScrollbarGraphy(vrc, value()); // vscrollbar
    }
}

///////////////////////////////////////////////////////////////////////////////

void Button::setDefaultData()
{
    m_states.append("Normal");
    m_states.append("In focus");
    m_states.append("Selected");
    m_states.append("Disabled");
    m_state = 0;
    m_fontBold = false;
    m_fontItalic = false;
    m_fontUnderline = false;
    m_fontSize = 11;
    m_texts.append("button");
    m_color = Qt::white;
}
void Button::calculateDrawingSequence()
{
    m_drawingSequence.clear();
    QRect rc = posRect();

    addFrameGraphy(rc);
    m_drawingSequence[m_drawingSequence.length() - 1].styleSheet = MainWindow::instance()->buttonSample;

    int t(2), r(5), b(4), l(2);
    QRect rc2 = rc;
    rc2.setTopLeft(rc.topLeft() + QPoint(l, t));
    rc2.setSize(rc.size() - QSize(l+r, t+b));
    addBackgroundGraphy(rc2, UserColor);
    m_drawingSequence[m_drawingSequence.length() - 1].userClr = m_color;

    addTextGraphy(rc, Qt::AlignCenter, text());
    m_drawingSequence[m_drawingSequence.length() - 1].customFont = &m_font;
}
void Button::calculateMesuredSize()
{
    if (m_texts.length() <= 0)
        return;
    QString t = text();
    int offset = 10;
    int h = textHeight(&m_font);
    int w = textWidth(t, &m_font);
    m_measuredSize = QSize(w + offset *2, h + offset *2);
}
void Button::parseData()
{
    m_font = MainWindow::instance()->currentTheme()->font();
    m_font.setBold(m_fontBold);
    m_font.setItalic(m_fontItalic);
    m_font.setPointSize(m_fontSize);
    m_font.setUnderline(m_fontUnderline);
}