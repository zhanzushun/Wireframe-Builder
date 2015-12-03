#include <QMouseEvent>
#include <QtGui>
#include "palette.hxx"
#include "ui_palette.h"
#include "mainwindow.hxx"
#include "itemdata.hxx"

Palette::Palette(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);
}

Palette::~Palette()
{
}

void Palette::mousePressEvent(QMouseEvent *mouse)
{
    if (mouse->buttons() == Qt::LeftButton)
    {
        m_currentPos = mouse->pos();
    }
}

void Palette::mouseMoveEvent(QMouseEvent *mouse)
{
    if (mouse->buttons() == Qt::LeftButton && mouse->pos().y() < 30)
        m_dock->move(mouse->globalPos() - m_currentPos);
}

Palette* Palette::createPalette(QWidget* mainWindow)
{
    QDockWidget *dockWidget = new QDockWidget(mainWindow);
    QWidget *dockWidgetContents = new QWidget();
    Palette* widgetPalette = new Palette(dockWidgetContents);
    QHBoxLayout *horizontalLayout = new QHBoxLayout(dockWidget);

    widgetPalette->setObjectName("myPalette");

    horizontalLayout->setMargin(0);
    horizontalLayout->setSpacing(0);
    horizontalLayout->addWidget(widgetPalette);
    horizontalLayout = new QHBoxLayout(dockWidgetContents);

    dockWidget->setFloating(true);
    dockWidget->setVisible(false);
    dockWidget->setFeatures(QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
    dockWidget->setAllowedAreas(Qt::NoDockWidgetArea);
    dockWidget->setWidget(dockWidgetContents);
    dockWidget->resize(widgetPalette->rect().width(), widgetPalette->rect().height());
    dockWidget->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
    dockWidget->setStyleSheet(QString::fromUtf8(" \
    #myPalette .QFrame { \
	border-width:15px 18px 8px 15px; \
	border-image:url(:/icons/palettebg.png) 15 18 8 15 repeat stretch; \
    }"
    ));

    widgetPalette->m_mainWindow = (MainWindow*)mainWindow;
    widgetPalette->m_mainWindow->addDockWidget(static_cast<Qt::DockWidgetArea>(0), dockWidget);
    widgetPalette->m_dock = dockWidget;
    return widgetPalette;
}

void Palette::showWindow(bool show, Document* doc, int props, ItemDataBase* itemData)
{
    if (!show)
    {
        dockWidget()->hide();
        return;
    }
    if (show && itemData != NULL)
    {
        labelTitle->setText(DiagramLibrary::diagramTypeFromKey(itemData->item()->key()));
        QRectF pos = itemData->item()->posRect();
        QString s = ("Pos(%1,%2), Size(%3x%4)");
        labelPosSize->setText(s.arg(pos.x())
            .arg(pos.y())
            .arg(pos.width())
            .arg(pos.height()));
    }
    else if (show && doc && doc->scene()->selectedSortedItems().length() > 0)
    {
        labelTitle->setText("multiple items or group");
        QRectF pos;
        foreach (ResizableItem* item, doc->scene()->selectedSortedItems())
            pos |= item->posRect();
        QString s = ("Pos(%1,%2), Size(%3x%4)");
        labelPosSize->setText(s.arg(pos.x())
            .arg(pos.y())
            .arg(pos.width())
            .arg(pos.height()));
    }

    int rows = 2;
    int rowMargin = 2;

    // set properties

    //autosize
    if ((props & P_AutoSize) == 0)
    {
        labelAutoSize->hide();
        btnAutoSize->hide();
        hlayoutAutoSize->setMargin(0);
    }
    else
    {
        btnAutoSize->disconnect();
        connect(btnAutoSize, SIGNAL(clicked(bool)), doc, SLOT(autoResize()));
        labelAutoSize->show();
        btnAutoSize->show();
        hlayoutAutoSize->setContentsMargins(0,rowMargin,0,rowMargin);
        rows ++;
    }

    // color
    if ((props & P_Color) == 0)
    {
        labelColor->hide();
        btnColor->hide();
        hLayoutColor->setMargin(0);
    }
    else
    {
        btnColor->disconnect();
        connect(btnColor, SIGNAL(clicked(bool)), doc, SLOT(setColor()));
        btnColor->setStyleSheet(QString("color:%1").arg(itemData->color().name()));
        labelColor->show();
        btnColor->show();
        hLayoutColor->setContentsMargins(0,rowMargin,0,rowMargin);
        rows ++;
    }

    // icon
    if ((props & P_Icon) == 0)
    {
        labelIcon->hide();
        btnIcon->hide();
        hLayoutIcon->setMargin(0);
    }
    else
    {
        btnIcon->disconnect();
        connect(btnIcon, SIGNAL(clicked(bool)), doc, SLOT(setIcon()));
        labelIcon->show();
        btnIcon->show();
        hLayoutIcon->setContentsMargins(0,rowMargin,0,rowMargin);
        rows ++;
    }

    // state
    if ((props & P_State) == 0)
    {
        labelState->hide();
        cmbState->hide();
        hLayoutState->setMargin(0);
    }
    else
    {
        cmbState->disconnect();
        cmbState->clear();
        cmbState->addItems(itemData->selectableTexts());
        cmbState->setCurrentIndex(itemData->selectedIndex());
        connect(cmbState, SIGNAL(currentIndexChanged(int)), doc, SLOT(setSelectedIndex(int)));
        labelState->show();
        cmbState->show();
        hLayoutState->setContentsMargins(0,rowMargin,0,rowMargin);
        rows ++;
    }

    // selected index
    if ((props & P_SelectedIndex) == 0)
    {
        labelSelection->hide();
        cmbSelection->hide();
        hlayoutSelection->setMargin(0);
    }
    else
    {
        cmbSelection->disconnect();
        cmbSelection->clear();
        cmbSelection->addItems(itemData->selectableTexts());
        cmbSelection->setCurrentIndex(itemData->selectedIndex());
        connect(cmbSelection, SIGNAL(currentIndexChanged(int)), doc, SLOT(setSelectedIndex(int)));
        labelSelection->show();
        cmbSelection->show();
        hlayoutSelection->setContentsMargins(0,rowMargin,0,rowMargin);
        rows ++;
    }

    // vscroll bar
    if ((props & P_VScrollBar) == 0)
    {
        labelScrollbar->hide();
        btnVScrollbar->hide();
        sliderValue->hide();
        hlayoutScrollbar->setMargin(0);
    }
    else
    {
        btnVScrollbar->disconnect();
        sliderValue->disconnect();
        btnVScrollbar->setChecked(itemData->vScrollbar());
        sliderValue->setValue(itemData->value());
        connect(btnVScrollbar, SIGNAL(clicked(bool)), sliderValue, SLOT(setVisible(bool)));
        connect(btnVScrollbar, SIGNAL(clicked(bool)), doc, SLOT(setVScrollBar(bool)));
        connect(sliderValue, SIGNAL(valueChanged(int)), doc, SLOT(setValue(int)));
        labelScrollbar->show();
        btnVScrollbar->show();
        hlayoutScrollbar->setContentsMargins(0,rowMargin,0,rowMargin);
        rows ++;
    }

    //font
    if ((props & P_Font) == 0)
    {
        labelTextFont->hide();
        btnFontBold->hide();
        btnFontItalic->hide();
        btnFontUnderline->hide();
        cmbFontSize->hide();
        hlayoutFont->setMargin(0);
    }
    else
    {
        btnFontBold->disconnect();
        btnFontItalic->disconnect();
        btnFontUnderline->disconnect();
        cmbFontSize->disconnect();
        
        btnFontBold->setChecked(itemData->fontBold());
        btnFontItalic->setChecked(itemData->fontItalic());
        btnFontUnderline->setChecked(itemData->fontUnderline());
        cmbFontSize->setCurrentIndex(cmbFontSize->findText(QString::number(itemData->fontSize())));

        connect(btnFontBold, SIGNAL(clicked(bool)), doc, SLOT(setFontBold(bool)));
        connect(btnFontItalic, SIGNAL(clicked(bool)), doc, SLOT(setFontItalic(bool)));
        connect(btnFontUnderline, SIGNAL(clicked(bool)), doc, SLOT(setFontUnderline(bool)));
        connect(cmbFontSize, SIGNAL(currentIndexChanged(const QString &)), doc, SLOT(setFontSize(const QString &)));

        labelTextFont->show();
        btnFontBold->show();
        btnFontItalic->show();
        btnFontUnderline->show();
        cmbFontSize->show();
        hlayoutFont->setContentsMargins(0,rowMargin,0,rowMargin);
        rows ++;
    }

    int rowHeight = 27;
    int margin = 45;
    QPoint pos = dockWidget()->geometry().topLeft();
    int w = dockWidget()->width();
    int h = margin + rows * rowHeight;
    QPoint ptDocTopRight = doc->mapToGlobal(doc->geometry().topRight());
    int offsetx = 30;
    int offsety = 10;
    if (!dockWidget()->isVisible())
        pos = QPoint(ptDocTopRight.x() - w - offsetx, ptDocTopRight.y() + offsety);
    dockWidget()->setGeometry(pos.x(), pos.y(), w, h);
    dockWidget()->show();
}
