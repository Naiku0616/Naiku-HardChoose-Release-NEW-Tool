#include "mainwindow.h"
#include <QApplication>
#include <QFont>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QFont f;
    f.setFamilies({
        "Microsoft YaHei UI",
        "Microsoft YaHei",
        "PingFang SC",
        "Segoe UI",
        "Helvetica Neue",
        "sans-serif"
    });
    f.setPointSize(10);
    f.setStyleStrategy(QFont::PreferAntialias);
    f.setHintingPreference(QFont::PreferNoHinting);
    a.setFont(f);

    //============全局滚动条样式============
    a.setStyleSheet(R"(
        QScrollBar:vertical {
            background: transparent;
            width: 8px;
            margin: 4px 2px 4px 0;
            border: none;
        }
        QScrollBar::handle:vertical {
            background: rgba(255,255,255,0.14);
            border-radius: 3px;
            min-height: 32px;
        }
        QScrollBar::handle:vertical:hover {
            background: rgba(255,255,255,0.26);
        }
        QScrollBar::handle:vertical:pressed {
            background: rgba(255,255,255,0.36);
        }
        QScrollBar::add-line:vertical,
        QScrollBar::sub-line:vertical {
            height: 0;
            background: transparent;
            border: none;
        }
        QScrollBar::add-page:vertical,
        QScrollBar::sub-page:vertical {
            background: transparent;
        }

        QScrollBar:horizontal {
            background: transparent;
            height: 8px;
            margin: 0 4px 2px 4px;
            border: none;
        }
        QScrollBar::handle:horizontal {
            background: rgba(255,255,255,0.14);
            border-radius: 3px;
            min-width: 32px;
        }
        QScrollBar::handle:horizontal:hover {
            background: rgba(255,255,255,0.26);
        }
        QScrollBar::handle:horizontal:pressed {
            background: rgba(255,255,255,0.36);
        }
        QScrollBar::add-line:horizontal,
        QScrollBar::sub-line:horizontal {
            width: 0;
            background: transparent;
            border: none;
        }
        QScrollBar::add-page:horizontal,
        QScrollBar::sub-page:horizontal {
            background: transparent;
        }
    )");

    MainWindow w;
    w.show();
    return a.exec();
}