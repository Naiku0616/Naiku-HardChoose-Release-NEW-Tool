#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringList>
#include <QVector>
#include <QPoint>

class QLabel;
class QPushButton;
class QTextEdit;
class QListWidget;
class QTimer;
class QWidget;
class QVBoxLayout;
class QDialog;
class QSizeGrip;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void mousePressEvent(QMouseEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;
    void mouseDoubleClickEvent(QMouseEvent* e) override;
    void changeEvent(QEvent* e) override;
    void resizeEvent(QResizeEvent* e) override;

private slots:
    void onDraw();
    void onTick();
    void onFinish();
    void openSettings();
    void onMinimize();
    void onMaximizeRestore();
    void onClose();

private:
    void buildUI();
    void buildSettingsDialog();
    void saveEvents();
    void loadEvents();
    void setResultSize(const QString& text);
    bool isTopBarArea(const QPoint& pos) const;
    void updateMaxRestoreIcon();
    void refreshHistoryEmpty();

    QWidget*     m_container  = nullptr;
    QVBoxLayout* m_rootLayout = nullptr;
    QSizeGrip*   m_sizeGrip   = nullptr;

    bool   m_dragging = false;
    QPoint m_dragStart;

    //顶栏
    QWidget*     m_topBar         = nullptr;
    QPushButton* m_btnSettings    = nullptr;
    QPushButton* m_btnMin         = nullptr;
    QPushButton* m_btnMaxRestore  = nullptr;
    QPushButton* m_btnClose       = nullptr;

    //主舞台
    QWidget*     m_stage       = nullptr;
    QLabel*      m_statusLabel = nullptr;
    QLabel*      m_resultLabel = nullptr;
    QPushButton* m_btnDraw     = nullptr;
    QLabel*      m_hintLabel   = nullptr;

    //设置对话框（独立窗口，与主窗口布局完全隔离）
    QDialog*     m_settingsDialog = nullptr;
    QTextEdit*   m_eventEdit      = nullptr;
    QListWidget* m_historyList    = nullptr;

    //抽取状态
    QTimer*       m_rollTimer = nullptr;
    QStringList   m_events;
    QString       m_targetText;
    QVector<bool> m_locked;
    QVector<int>  m_unlockTick;
    int           m_currentTick = 0;
    int           m_totalTicks  = 40;
};

#endif