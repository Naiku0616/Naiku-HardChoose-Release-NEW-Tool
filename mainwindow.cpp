#include "mainwindow.h"

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QTextEdit>
#include <QPushButton>
#include <QListWidget>
#include <QFrame>
#include <QDialog>
#include <QStackedWidget>
#include <QButtonGroup>
#include <QTimer>
#include <QRandomGenerator>
#include <QSettings>
#include <QShortcut>
#include <QDateTime>
#include <QPainter>
#include <QPainterPath>
#include <QPixmap>
#include <QIcon>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QSizeGrip>
#include <QWindow>
#include <QGraphicsDropShadowEffect>
#include <QApplication>
#include <QScreen>

// ============ 配色 ============
namespace C {
const QString Bg       = "#0A0A0B";
const QString Panel    = "#141519";
const QString Line     = "rgba(255,255,255,0.06)";
const QString Line2    = "rgba(255,255,255,0.10)";
const QString TextHi   = "#EDEEF0";
const QString TextMid  = "rgba(255,255,255,0.50)";
const QString TextLow  = "rgba(255,255,255,0.28)";
const QString Accent   = "#6E8FF5";
const QString Green    = "#3DD68C";
}

namespace {

const QString kSample =
    "喝一杯水\n"
    "做 20 个深蹲\n"
    "听一首歌\n"
    "站起来活动 1 分钟\n"
    "给朋友发条消息\n"
    "写 100 字日记\n"
    "冥想 3 分钟\n"
    "看一段喜欢的视频";

const QString kCjkPool =
    QString::fromUtf8("0123456789"
                      "abcdefghijklmnopquvrtwxyz"
                      "ABCDEFGHIJKLMNOPQUVRTWXYZ"
                      "WWYNaikuLOVEMUSIC");

QChar randomGlitchCharFor(QChar target) {
    if (target.unicode() >= 0x4E00 && target.unicode() <= 0x9FFF) {
        return kCjkPool[QRandomGenerator::global()->bounded(kCjkPool.length())];
    }
    if (target.isDigit()) {
        return QChar('0' + QRandomGenerator::global()->bounded(10));
    }
    if (target.isLetter()) {
        static const QString pool = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        return pool[QRandomGenerator::global()->bounded(pool.length())];
    }
    return target;
}

QPixmap makeLogoPixmap(int size) {
    const qreal dpr = 2.0;
    QPixmap pm(int(size * dpr), int(size * dpr));
    pm.setDevicePixelRatio(dpr);
    pm.fill(Qt::transparent);

    QPainter p(&pm);
    p.setRenderHint(QPainter::Antialiasing);

    QLinearGradient g(0, 0, size, size);
    g.setColorAt(0.0, QColor(C::Accent));
    g.setColorAt(1.0, QColor(140, 120, 250));

    const qreal cx = size / 2.0;
    const qreal cy = size / 2.0;

    QRectF arcRect(cx - size * 0.36, cy - size * 0.36,
                   size * 0.72, size * 0.72);
    QPen pen(QBrush(g), size * 0.085,
             Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    p.setPen(pen);
    p.setBrush(Qt::NoBrush);
    p.drawArc(arcRect, 90 * 16, 270 * 16);

    p.setPen(Qt::NoPen);
    p.setBrush(g);
    p.drawEllipse(QPointF(cx, cy), size * 0.10, size * 0.10);
    return pm;
}

QPixmap makeSlidersIcon(int size, const QColor& color) {
    const qreal dpr = 2.0;
    QPixmap pm(int(size * dpr), int(size * dpr));
    pm.setDevicePixelRatio(dpr);
    pm.fill(Qt::transparent);

    QPainter p(&pm);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(color, 1.5, Qt::SolidLine, Qt::RoundCap));
    p.setBrush(Qt::NoBrush);

    const qreal s = size;
    const qreal x1 = s * 0.20, x2 = s * 0.80;
    const qreal y1 = s * 0.30, y2 = s * 0.50, y3 = s * 0.70;
    const qreal r  = s * 0.07;

    p.drawLine(QPointF(x1, y1), QPointF(x2, y1));
    p.drawLine(QPointF(x1, y2), QPointF(x2, y2));
    p.drawLine(QPointF(x1, y3), QPointF(x2, y3));

    p.setBrush(color);
    p.setPen(Qt::NoPen);
    p.drawEllipse(QPointF(x1 + (x2 - x1) * 0.70, y1), r, r);
    p.drawEllipse(QPointF(x1 + (x2 - x1) * 0.30, y2), r, r);
    p.drawEllipse(QPointF(x1 + (x2 - x1) * 0.55, y3), r, r);

    return pm;
}

QPixmap makeWindowIcon(int boxSize, const QString& type, const QColor& color) {
    const qreal dpr = 2.0;
    QPixmap pm(int(boxSize * dpr), int(boxSize * dpr));
    pm.setDevicePixelRatio(dpr);
    pm.fill(Qt::transparent);

    QPainter p(&pm);
    p.setRenderHint(QPainter::Antialiasing);
    p.setBrush(Qt::NoBrush);

    const qreal s = boxSize;
    const qreal pen = qMax(1.15, s * 0.09);
    p.setPen(QPen(color, pen, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

    if (type == "min") {
        p.drawLine(QPointF(s * 0.30, s * 0.52), QPointF(s * 0.70, s * 0.52));
    }
    else if (type == "max") {
        QRectF r(s * 0.30, s * 0.30, s * 0.40, s * 0.40);
        p.drawRoundedRect(r, 1.6, 1.6);
    }
    else if (type == "restore") {
        QRectF r1(s * 0.30, s * 0.36, s * 0.36, s * 0.36);
        QRectF r2(s * 0.36, s * 0.28, s * 0.36, s * 0.36);
        p.drawRoundedRect(r2, 1.6, 1.6);
        p.drawRoundedRect(r1, 1.6, 1.6);
    }
    else if (type == "close") {
        p.drawLine(QPointF(s * 0.31, s * 0.31), QPointF(s * 0.69, s * 0.69));
        p.drawLine(QPointF(s * 0.69, s * 0.31), QPointF(s * 0.31, s * 0.69));
    }
    return pm;
}

QIcon makeWindowBtnIcon(const QString& type, bool isClose = false) {
    QIcon icon;
    QColor n = isClose ? QColor(255, 255, 255, 130) : QColor(255, 255, 255, 128);
    QColor a = QColor(255, 255, 255, 235);

    icon.addPixmap(makeWindowIcon(14, type, n), QIcon::Normal,   QIcon::Off);
    icon.addPixmap(makeWindowIcon(14, type, a), QIcon::Active,   QIcon::Off);
    icon.addPixmap(makeWindowIcon(14, type, a), QIcon::Selected, QIcon::Off);
    return icon;
}

QString iconBtnStyle() {
    return QString(R"(
        QPushButton {
            background: transparent;
            border: none;
            border-radius: 8px;
        }
        QPushButton:hover {
            background: rgba(255,255,255,0.07);
        }
    )");
}

QString windowBtnStyle(bool isClose = false) {
    if (isClose) {
        return QString(R"(
            QPushButton {
                background: transparent;
                border: none;
                border-radius: 7px;
                padding: 0;
            }
            QPushButton:hover { background: #E5484D; }
        )");
    }
    return QString(R"(
        QPushButton {
            background: transparent;
            border: none;
            border-radius: 7px;
            padding: 0;
        }
        QPushButton:hover { background: rgba(255,255,255,0.07); }
    )");
}

void applyWindowShadow(QWidget* c) {
    auto* sh = new QGraphicsDropShadowEffect(c);
    sh->setBlurRadius(50);
    sh->setOffset(0, 16);
    sh->setColor(QColor(0, 0, 0, 160));
    c->setGraphicsEffect(sh);
}

}  // namespace


// ================================================
// =================== MainWindow =================
// ================================================
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setWindowTitle("抽选");
    resize(1100, 720);
    setMinimumSize(820, 560);

    setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
    setAttribute(Qt::WA_TranslucentBackground);

    buildUI();
    buildSettingsDialog();
    loadEvents();

    m_rollTimer = new QTimer(this);
    m_rollTimer->setInterval(50);
    connect(m_rollTimer, &QTimer::timeout, this, &MainWindow::onTick);

    refreshHistoryEmpty();
}

MainWindow::~MainWindow() {}


// ================================================
// =================== 构建 UI ====================
// ================================================
void MainWindow::buildUI() {
    auto* central = new QWidget;
    central->setStyleSheet("background:transparent;");
    setCentralWidget(central);

    m_rootLayout = new QVBoxLayout(central);
    m_rootLayout->setContentsMargins(20, 20, 20, 20);
    m_rootLayout->setSpacing(0);

    // ============ 根容器 ============
    m_container = new QWidget;
    m_container->setObjectName("WindowRoot");
    m_container->setStyleSheet(QString(R"(
        #WindowRoot {
            background: %1;
            border-radius: 16px;
            border: 1px solid rgba(255,255,255,0.05);
        }
    )").arg(C::Bg));
    applyWindowShadow(m_container);
    m_rootLayout->addWidget(m_container);

    auto* rootV = new QVBoxLayout(m_container);
    rootV->setContentsMargins(0, 0, 0, 0);
    rootV->setSpacing(0);

    // ============ 顶栏 ============
    m_topBar = new QWidget;
    m_topBar->setFixedHeight(48);
    m_topBar->setStyleSheet(QString(
                                "background:transparent; border-bottom:1px solid %1;").arg(C::Line));

    auto* topL = new QHBoxLayout(m_topBar);
    topL->setContentsMargins(20, 0, 14, 0);
    topL->setSpacing(0);

    auto* logo = new QLabel;
    logo->setFixedSize(18, 18);
    logo->setPixmap(makeLogoPixmap(18));
    logo->setAttribute(Qt::WA_TransparentForMouseEvents);
    topL->addWidget(logo);
    topL->addSpacing(10);

    auto* title = new QLabel("抽选");
    title->setAttribute(Qt::WA_TransparentForMouseEvents);
    title->setStyleSheet(QString(
                             "color:%1; font-size:14px; font-weight:600;"
                             "background:transparent;").arg(C::TextHi));
    topL->addWidget(title);
    topL->addSpacing(10);

    auto* sub = new QLabel("RANDOM");
    sub->setAttribute(Qt::WA_TransparentForMouseEvents);
    sub->setStyleSheet(QString(
                           "color:%1; font-size:9px; font-weight:600; letter-spacing:3px;"
                           "background:transparent;").arg(C::TextLow));
    topL->addWidget(sub);

    topL->addStretch();

    m_btnSettings = new QPushButton;
    m_btnSettings->setFixedSize(34, 30);
    m_btnSettings->setCursor(Qt::PointingHandCursor);
    m_btnSettings->setStyleSheet(iconBtnStyle());
    m_btnSettings->setToolTip("设置");
    {
        QIcon ic;
        ic.addPixmap(makeSlidersIcon(16, QColor(255,255,255,180)), QIcon::Normal);
        ic.addPixmap(makeSlidersIcon(16, QColor(255,255,255,245)), QIcon::Active);
        m_btnSettings->setIcon(ic);
        m_btnSettings->setIconSize(QSize(16, 16));
    }
    topL->addWidget(m_btnSettings);
    topL->addSpacing(14);

    auto* sep = new QFrame;
    sep->setFixedSize(1, 14);
    sep->setStyleSheet(QString("background:%1;").arg(C::Line2));
    topL->addWidget(sep);
    topL->addSpacing(8);

    m_btnMin = new QPushButton;
    m_btnMin->setIcon(makeWindowBtnIcon("min"));
    m_btnMin->setIconSize(QSize(14, 14));
    m_btnMin->setFixedSize(34, 28);
    m_btnMin->setCursor(Qt::PointingHandCursor);
    m_btnMin->setStyleSheet(windowBtnStyle(false));

    m_btnMaxRestore = new QPushButton;
    m_btnMaxRestore->setIcon(makeWindowBtnIcon("max"));
    m_btnMaxRestore->setIconSize(QSize(14, 14));
    m_btnMaxRestore->setFixedSize(34, 28);
    m_btnMaxRestore->setCursor(Qt::PointingHandCursor);
    m_btnMaxRestore->setStyleSheet(windowBtnStyle(false));

    m_btnClose = new QPushButton;
    m_btnClose->setIcon(makeWindowBtnIcon("close", true));
    m_btnClose->setIconSize(QSize(14, 14));
    m_btnClose->setFixedSize(34, 28);
    m_btnClose->setCursor(Qt::PointingHandCursor);
    m_btnClose->setStyleSheet(windowBtnStyle(true));

    topL->addWidget(m_btnMin);
    topL->addSpacing(2);
    topL->addWidget(m_btnMaxRestore);
    topL->addSpacing(2);
    topL->addWidget(m_btnClose);

    rootV->addWidget(m_topBar);

    // ============ 主舞台 ============
    m_stage = new QWidget;
    m_stage->setObjectName("Stage");
    m_stage->setStyleSheet(QString(R"(
        QWidget#Stage {
            background: qradialgradient(
                cx:0.5, cy:0.5, radius:0.85,
                stop:0    rgba(110,143,245,0.06),
                stop:0.6  rgba(110,143,245,0.015),
                stop:1    rgba(0,0,0,0)
            );
        }
    )"));

    auto* stageL = new QVBoxLayout(m_stage);
    stageL->setContentsMargins(60, 40, 60, 60);
    stageL->setSpacing(0);

    stageL->addStretch(1);

    m_statusLabel = new QLabel("准 备 就 绪");
    m_statusLabel->setAlignment(Qt::AlignCenter);
    m_statusLabel->setStyleSheet(QString(
                                     "color:%1; font-size:11px; font-weight:500; letter-spacing:8px;"
                                     "background:transparent;").arg(C::TextLow));
    stageL->addWidget(m_statusLabel, 0, Qt::AlignCenter);
    stageL->addSpacing(14);

    m_resultLabel = new QLabel("?");
    m_resultLabel->setAlignment(Qt::AlignCenter);
    m_resultLabel->setWordWrap(false);
    m_resultLabel->setFixedHeight(300);
    m_resultLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
    m_resultLabel->setStyleSheet(QString(
                                     "color:%1; font-size:220px; font-weight:600;"
                                     "background:transparent;").arg(C::TextHi));
    stageL->addWidget(m_resultLabel, 0);
    stageL->addSpacing(60);

    m_btnDraw = new QPushButton("抽   取");
    m_btnDraw->setCursor(Qt::PointingHandCursor);
    m_btnDraw->setFixedSize(148, 46);
    m_btnDraw->setStyleSheet(QString(R"(
        QPushButton {
            background: transparent;
            color: %1;
            border: 1px solid rgba(255,255,255,0.20);
            border-radius: 23px;
            font-size: 13px;
            font-weight: 600;
            letter-spacing: 4px;
            padding-left: 4px;
        }
        QPushButton:hover {
            background: #EDEEF0;
            color: #0A0A0B;
            border-color: #EDEEF0;
        }
        QPushButton:pressed {
            background: #D5D7DB;
            color: #0A0A0B;
            border-color: #D5D7DB;
        }
        QPushButton:disabled {
            background: transparent;
            color: rgba(255,255,255,0.18);
            border-color: rgba(255,255,255,0.08);
        }
    )").arg(C::TextHi));
    stageL->addWidget(m_btnDraw, 0, Qt::AlignCenter);
    stageL->addSpacing(16);

    m_hintLabel = new QLabel("按 空格键 快速抽取");
    m_hintLabel->setAlignment(Qt::AlignCenter);
    m_hintLabel->setStyleSheet(QString(
                                   "color:%1; font-size:11px;"
                                   "background:transparent;").arg(C::TextLow));
    stageL->addWidget(m_hintLabel, 0, Qt::AlignCenter);

    stageL->addStretch(1);

    rootV->addWidget(m_stage, 1);

    // ============ QSizeGrip（加大到 20x20） ============
    m_sizeGrip = new QSizeGrip(m_container);
    m_sizeGrip->setFixedSize(20, 20);
    m_sizeGrip->setStyleSheet("background:transparent;");
    m_sizeGrip->raise();

    // 信号
    connect(m_btnDraw, &QPushButton::clicked, this, &MainWindow::onDraw);
    connect(m_btnSettings, &QPushButton::clicked, this, &MainWindow::openSettings);
    connect(m_btnMin, &QPushButton::clicked, this, &MainWindow::onMinimize);
    connect(m_btnMaxRestore, &QPushButton::clicked, this, &MainWindow::onMaximizeRestore);
    connect(m_btnClose, &QPushButton::clicked, this, &MainWindow::onClose);

    auto* space = new QShortcut(QKeySequence(Qt::Key_Space), this);
    connect(space, &QShortcut::activated, this, &MainWindow::onDraw);
}


// ================================================
// ============== 设置对话框（独立窗口） ===========
// ================================================
void MainWindow::buildSettingsDialog() {
    m_settingsDialog = new QDialog(this);
    m_settingsDialog->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    m_settingsDialog->setAttribute(Qt::WA_TranslucentBackground);
    m_settingsDialog->setModal(true);
    m_settingsDialog->setStyleSheet("background: rgba(0,0,0,0.65);");

    auto* dlgL = new QVBoxLayout(m_settingsDialog);
    dlgL->setContentsMargins(60, 60, 60, 60);
    dlgL->setAlignment(Qt::AlignCenter);

    auto* panel = new QFrame;
    panel->setObjectName("SettingsPanel");
    panel->setFixedSize(620, 500);
    panel->setStyleSheet(QString(R"(
        #SettingsPanel {
            background: %1;
            border-radius: 16px;
            border: 1px solid rgba(255,255,255,0.07);
        }
    )").arg(C::Panel));

    auto* panelShadow = new QGraphicsDropShadowEffect(panel);
    panelShadow->setBlurRadius(60);
    panelShadow->setOffset(0, 20);
    panelShadow->setColor(QColor(0, 0, 0, 200));
    panel->setGraphicsEffect(panelShadow);

    auto* panelL = new QVBoxLayout(panel);
    panelL->setContentsMargins(0, 0, 0, 0);
    panelL->setSpacing(0);

    // ---- 头部 ----
    auto* header = new QWidget;
    header->setObjectName("SettingsHeader");
    header->setFixedHeight(60);
    header->setStyleSheet(QString(
                              "#SettingsHeader { background: transparent; border-bottom: 1px solid %1; }")
                              .arg(C::Line));

    auto* headerL = new QHBoxLayout(header);
    headerL->setContentsMargins(24, 0, 16, 0);
    headerL->setSpacing(0);

    auto* hTitle = new QLabel("设 置");
    hTitle->setStyleSheet(QString(
                              "color:%1; font-size:14px; font-weight:600; letter-spacing:2px;"
                              "background:transparent;").arg(C::TextHi));
    headerL->addWidget(hTitle);
    headerL->addStretch();

    auto* closeBtn = new QPushButton;
    closeBtn->setFixedSize(28, 28);
    closeBtn->setCursor(Qt::PointingHandCursor);
    closeBtn->setStyleSheet(R"(
        QPushButton {
            background: transparent;
            border: none;
            border-radius: 7px;
        }
        QPushButton:hover { background: rgba(255,255,255,0.07); }
    )");
    {
        QIcon ic;
        ic.addPixmap(makeWindowIcon(12, "close", QColor(255,255,255,150)), QIcon::Normal);
        ic.addPixmap(makeWindowIcon(12, "close", QColor(255,255,255,240)), QIcon::Active);
        closeBtn->setIcon(ic);
        closeBtn->setIconSize(QSize(12, 12));
    }
    connect(closeBtn, &QPushButton::clicked, m_settingsDialog, &QDialog::accept);
    headerL->addWidget(closeBtn);

    panelL->addWidget(header);

    // ---- Tab 行 ----
    auto* tabBar = new QWidget;
    tabBar->setObjectName("SettingsTabBar");
    tabBar->setFixedHeight(58);
    tabBar->setStyleSheet(QString(
                              "#SettingsTabBar { background: transparent; border-bottom: 1px solid %1; }")
                              .arg(C::Line));

    auto* tabL = new QHBoxLayout(tabBar);
    tabL->setContentsMargins(20, 0, 20, 0);
    tabL->setSpacing(8);

    QString tabStyle = QString(R"(
        QPushButton {
            background: transparent;
            border: none;
            color: rgba(255,255,255,0.48);
            font-size: 12.5px;
            font-weight: 500;
            padding: 6px 18px;
            border-radius: 8px;
        }
        QPushButton:hover {
            background: rgba(255,255,255,0.05);
            color: rgba(255,255,255,0.85);
        }
        QPushButton:checked {
            background: rgba(110,143,245,0.18);
            color: %1;
        }
        QPushButton:checked:hover {
            background: rgba(110,143,245,0.24);
        }
    )").arg(C::Accent);

    auto* tabEvents  = new QPushButton("事 件 库");
    auto* tabHistory = new QPushButton("历 史");
    auto* tabGroup   = new QButtonGroup(m_settingsDialog);
    tabGroup->setExclusive(true);

    QPushButton* tabs[2] = { tabEvents, tabHistory };
    for (int i = 0; i < 2; ++i) {
        tabs[i]->setCheckable(true);
        tabs[i]->setCursor(Qt::PointingHandCursor);
        tabs[i]->setStyleSheet(tabStyle);
        tabs[i]->setFixedHeight(34);
        tabGroup->addButton(tabs[i], i);
    }
    tabEvents->setChecked(true);

    tabL->addWidget(tabEvents);
    tabL->addWidget(tabHistory);
    tabL->addStretch();

    panelL->addWidget(tabBar);

    // ---- 内容 ----
    auto* contentStack = new QStackedWidget;
    contentStack->setStyleSheet("background:transparent;");

    // 页 1：事件库
    auto* pageEvents = new QWidget;
    {
        auto* l = new QVBoxLayout(pageEvents);
        l->setContentsMargins(24, 20, 24, 20);
        l->setSpacing(14);

        auto* hint = new QLabel("每行写一个事件");
        hint->setWordWrap(true);
        hint->setStyleSheet(QString(
                                "color:%1; font-size:11.5px; background:transparent;")
                                .arg(C::TextLow));
        l->addWidget(hint);

        m_eventEdit = new QTextEdit;
        m_eventEdit->setPlaceholderText(
            "喝一杯水\n做 20 个深蹲\n听一首歌\n写 100 字日记");
        m_eventEdit->setStyleSheet(R"(
            QTextEdit {
                background: rgba(0,0,0,0.35);
                border: 1px solid rgba(255,255,255,0.06);
                border-radius: 10px;
                padding: 14px;
                font-size: 13px;
                color: #EDEEF0;
                selection-background-color: #6E8FF5;
            }
            QTextEdit:focus {
                border: 1px solid rgba(110,143,245,0.55);
                background: rgba(0,0,0,0.45);
            }
        )");
        l->addWidget(m_eventEdit, 1);

        auto* clearBtn = new QPushButton("清 空 事 件 库");
        clearBtn->setFixedHeight(36);
        clearBtn->setCursor(Qt::PointingHandCursor);
        clearBtn->setStyleSheet(R"(
            QPushButton {
                background: transparent;
                border: 1px solid rgba(255,255,255,0.08);
                border-radius: 8px;
                color: rgba(255,255,255,0.55);
                font-size: 12px;
                letter-spacing: 2px;
            }
            QPushButton:hover {
                background: rgba(255,255,255,0.04);
                color: #EDEEF0;
                border-color: rgba(255,255,255,0.16);
            }
        )");
        connect(clearBtn, &QPushButton::clicked, [this]{
            m_eventEdit->clear();
            saveEvents();
        });
        connect(m_eventEdit, &QTextEdit::textChanged, this, &MainWindow::saveEvents);
        l->addWidget(clearBtn);
    }

    // 页 2：历史
    auto* pageHistory = new QWidget;
    {
        auto* l = new QVBoxLayout(pageHistory);
        l->setContentsMargins(24, 20, 24, 20);
        l->setSpacing(14);

        auto* hint = new QLabel("最近的抽取记录");
        hint->setStyleSheet(QString(
                                "color:%1; font-size:11.5px; background:transparent;")
                                .arg(C::TextLow));
        l->addWidget(hint);

        m_historyList = new QListWidget;
        m_historyList->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
        m_historyList->setStyleSheet(R"(
            QListWidget {
                background: rgba(0,0,0,0.35);
                border: 1px solid rgba(255,255,255,0.06);
                border-radius: 10px;
                padding: 8px;
                font-size: 12.5px;
                color: #EDEEF0;
                outline: 0;
            }
            QListWidget::item {
                padding: 11px 10px;
                border-radius: 8px;
                margin: 2px 0;
            }
            QListWidget::item:hover {
                background: rgba(255,255,255,0.04);
            }
            QListWidget::item:selected {
                background: rgba(110,143,245,0.18);
                color: #EDEEF0;
            }
        )");
        l->addWidget(m_historyList, 1);

        auto* clearBtn = new QPushButton("清 空 历 史");
        clearBtn->setFixedHeight(36);
        clearBtn->setCursor(Qt::PointingHandCursor);
        clearBtn->setStyleSheet(R"(
            QPushButton {
                background: transparent;
                border: 1px solid rgba(255,255,255,0.08);
                border-radius: 8px;
                color: rgba(255,255,255,0.55);
                font-size: 12px;
                letter-spacing: 2px;
            }
            QPushButton:hover {
                background: rgba(255,255,255,0.04);
                color: #EDEEF0;
                border-color: rgba(255,255,255,0.16);
            }
        )");
        connect(clearBtn, &QPushButton::clicked, [this]{
            m_historyList->clear();
            refreshHistoryEmpty();
        });
        l->addWidget(clearBtn);
    }

    contentStack->addWidget(pageEvents);
    contentStack->addWidget(pageHistory);
    panelL->addWidget(contentStack, 1);

    dlgL->addWidget(panel, 0, Qt::AlignCenter);

    connect(tabGroup, &QButtonGroup::idClicked, this, [contentStack](int id){
        contentStack->setCurrentIndex(id);
    });
}


// ================================================
// =================== 打开设置 ===================
// ================================================
void MainWindow::openSettings() {
    if (!m_settingsDialog) return;
    QPoint topLeft = mapToGlobal(QPoint(0, 0));
    m_settingsDialog->setGeometry(topLeft.x(), topLeft.y(), width(), height());
    m_settingsDialog->exec();
}


// ================================================
// =================== 历史空状态 =================
// ================================================
void MainWindow::refreshHistoryEmpty() {
    if (!m_historyList) return;
    if (m_historyList->count() > 0) {
        auto* first = m_historyList->item(0);
        if (first && first->flags() == Qt::NoItemFlags) {
            delete m_historyList->takeItem(0);
        }
    }
    if (m_historyList->count() == 0) {
        auto* ph = new QListWidgetItem("    暂无记录");
        ph->setForeground(QColor(255, 255, 255, 55));
        ph->setFlags(Qt::NoItemFlags);
        ph->setTextAlignment(Qt::AlignCenter);
        m_historyList->addItem(ph);
    }
}


// ================================================
// =================== 窗口事件 ===================
// ================================================
bool MainWindow::isTopBarArea(const QPoint& pos) const {
    if (!m_topBar) return false;
    QPoint local = m_topBar->mapFrom(const_cast<MainWindow*>(this), pos);
    if (!m_topBar->rect().contains(local)) return false;

    QWidget* w = m_topBar->childAt(local);
    while (w) {
        if (qobject_cast<QPushButton*>(w)) return false;
        if (w == m_topBar) break;
        w = w->parentWidget();
    }
    return true;
}

void MainWindow::mousePressEvent(QMouseEvent* e) {
    if (e->button() == Qt::LeftButton && isTopBarArea(e->pos())) {
        m_dragging = true;
        m_dragStart = e->globalPosition().toPoint() - frameGeometry().topLeft();
        e->accept();
        return;
    }
    QMainWindow::mousePressEvent(e);
}

void MainWindow::mouseMoveEvent(QMouseEvent* e) {
    if (m_dragging && (e->buttons() & Qt::LeftButton)) {
        if (isMaximized()) {
            showNormal();
            m_dragStart = QPoint(width() / 2, 20);
        }
        move(e->globalPosition().toPoint() - m_dragStart);
        e->accept();
        return;
    }
    QMainWindow::mouseMoveEvent(e);
}

void MainWindow::mouseReleaseEvent(QMouseEvent* e) {
    m_dragging = false;
    QMainWindow::mouseReleaseEvent(e);
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent* e) {
    if (e->button() == Qt::LeftButton && isTopBarArea(e->pos())) {
        onMaximizeRestore();
        e->accept();
        return;
    }
    QMainWindow::mouseDoubleClickEvent(e);
}

void MainWindow::changeEvent(QEvent* e) {
    QMainWindow::changeEvent(e);
    if (e->type() != QEvent::WindowStateChange) return;

    updateMaxRestoreIcon();

    if (isMaximized() || isFullScreen()) {
        m_rootLayout->setContentsMargins(0, 0, 0, 0);
        if (m_container) {
            m_container->setGraphicsEffect(nullptr);
            m_container->setStyleSheet(QString(
                                           "#WindowRoot { background: %1; border: none; border-radius: 0; }")
                                           .arg(C::Bg));
        }
        if (m_sizeGrip) m_sizeGrip->hide();
    } else {
        m_rootLayout->setContentsMargins(20, 20, 20, 20);
        if (m_container) {
            applyWindowShadow(m_container);
            m_container->setStyleSheet(QString(R"(
                #WindowRoot {
                    background: %1;
                    border-radius: 16px;
                    border: 1px solid rgba(255,255,255,0.05);
                }
            )").arg(C::Bg));
        }
        if (m_sizeGrip) m_sizeGrip->show();
    }
}

void MainWindow::updateMaxRestoreIcon() {
    if (!m_btnMaxRestore) return;
    m_btnMaxRestore->setIcon(makeWindowBtnIcon(isMaximized() ? "restore" : "max"));
}

void MainWindow::resizeEvent(QResizeEvent* e) {
    QMainWindow::resizeEvent(e);
    if (m_sizeGrip && m_container) {
        // QSizeGrip 位置内收 6px（避免压住圆角）
        m_sizeGrip->move(m_container->width()  - m_sizeGrip->width()  - 6,
                         m_container->height() - m_sizeGrip->height() - 6);
    }
    if (m_resultLabel && !m_targetText.isEmpty()) {
        setResultSize(m_targetText);
    }
}

void MainWindow::onMinimize() { showMinimized(); }
void MainWindow::onMaximizeRestore() {
    if (isMaximized()) showNormal(); else showMaximized();
}
void MainWindow::onClose() { close(); }


// ================================================
// =================== 持久化 ====================
// ================================================
void MainWindow::saveEvents() {
    if (!m_eventEdit) return;
    QSettings s("RandomDraw", "App");
    s.setValue("events", m_eventEdit->toPlainText());
}

void MainWindow::loadEvents() {
    if (!m_eventEdit) return;
    QSettings s("RandomDraw", "App");
    QString text = s.value("events").toString();
    if (text.isEmpty()) text = kSample;
    m_eventEdit->setPlainText(text);
}


// ================================================
// =================== 结果字号 ==================
// ================================================
void MainWindow::setResultSize(const QString& text) {
    if (!m_resultLabel) return;

    int len = text.length();
    int base;
    if      (len <= 1)  base = 280;
    else if (len <= 2)  base = 240;
    else if (len <= 3)  base = 200;
    else if (len <= 4)  base = 170;
    else if (len <= 5)  base = 144;
    else if (len <= 6)  base = 122;
    else if (len <= 7)  base = 104;
    else if (len <= 8)  base = 92;
    else if (len <= 10) base = 78;
    else if (len <= 12) base = 66;
    else if (len <= 15) base = 54;
    else if (len <= 20) base = 44;
    else if (len <= 26) base = 36;
    else                base = 28;

    int avail = 0;
    if (m_stage && m_stage->width() > 200) {
        avail = int(m_stage->width() * 0.82);
    }

    if (avail > 0 && len > 0) {
        double est = 0.0;
        for (QChar c : text) {
            if (c.unicode() >= 0x4E00 && c.unicode() <= 0x9FFF) {
                est += base * 0.98;
            } else if (c.isDigit() || c.isLetter()) {
                est += base * 0.60;
            } else {
                est += base * 0.35;
            }
        }
        if (est > avail) {
            base = qMax(20, int(base * double(avail) / est));
        }
    }

    m_resultLabel->setStyleSheet(QString(
                                     "color:%1; font-size:%2px; font-weight:600;"
                                     "background:transparent;").arg(C::TextHi).arg(base));
}


// ================================================
// =================== 抽取流程 ==================
// ================================================
void MainWindow::onDraw() {
    if (m_rollTimer->isActive()) return;
    if (m_settingsDialog && m_settingsDialog->isVisible()) return;
    if (!m_eventEdit) return;    //←保险防空指针

    m_events = m_eventEdit->toPlainText().split('\n', Qt::SkipEmptyParts);
    for (auto& e : m_events) e = e.trimmed();
    m_events.removeAll("");

    // ============ 空状态 ============
    if (m_events.isEmpty()) {
        m_resultLabel->setText("?");
        m_resultLabel->setStyleSheet(QString(
            "color:rgba(255,255,255,0.05); font-size:220px; font-weight:600;"
            "background:transparent;"));
        m_statusLabel->setText("事件库为空打开设置添加事件");
        m_statusLabel->setStyleSheet(QString(
            "color:rgba(255,255,255,0.32); font-size:12px; font-weight:400;"
            "letter-spacing:2px; background:transparent;"));
        return;
    }

    int idx = QRandomGenerator::global()->bounded(m_events.size());
    m_targetText = m_events[idx];

    const int len = m_targetText.length();
    m_locked = QVector<bool>(len, false);
    m_unlockTick = QVector<int>(len);

    const int minTick = 6;
    const int maxTick = m_totalTicks - 2;
    for (int i = 0; i < len; ++i) {
        m_unlockTick[i] = minTick + QRandomGenerator::global()->bounded(maxTick - minTick + 1);
    }

    m_currentTick = 0;

    m_statusLabel->setText("想 法 正 在 抉 择");
    m_statusLabel->setStyleSheet(QString(
                                     "color:%1; font-size:11px; font-weight:500; letter-spacing:8px;"
                                     "background:transparent;").arg(C::TextMid));

    setResultSize(m_targetText);

    m_btnDraw->setEnabled(false);
    m_rollTimer->start();
}

void MainWindow::onTick() {
    m_currentTick++;

    const int len = m_targetText.length();
    for (int i = 0; i < len; ++i) {
        if (!m_locked[i] && m_currentTick >= m_unlockTick[i]) {
            m_locked[i] = true;
        }
    }

    QString display;
    display.reserve(len);
    for (int i = 0; i < len; ++i) {
        if (m_locked[i]) display += m_targetText[i];
        else             display += randomGlitchCharFor(m_targetText[i]);
    }
    m_resultLabel->setText(display);

    if (m_currentTick >= m_totalTicks) {
        m_rollTimer->stop();
        onFinish();
    }
}

void MainWindow::onFinish() {
    m_resultLabel->setText(m_targetText);
    setResultSize(m_targetText);

    m_statusLabel->setText("已 抽 取");
    m_statusLabel->setStyleSheet(QString(
                                     "color:%1; font-size:11px; font-weight:600; letter-spacing:8px;"
                                     "background:transparent;").arg(C::Green));

    if (m_historyList) {
        if (m_historyList->count() > 0) {
            auto* first = m_historyList->item(0);
            if (first && first->flags() == Qt::NoItemFlags) {
                delete m_historyList->takeItem(0);
            }
        }
        QString entry = QString("%1    %2")
                            .arg(QDateTime::currentDateTime().toString("HH:mm:ss"))
                            .arg(m_targetText);
        m_historyList->insertItem(0, entry);
    }

    m_btnDraw->setEnabled(true);
}