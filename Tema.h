#pragma once
#include <QString>

inline QString temaOscuro()
{
    return QStringLiteral(R"(
        QWidget { background: #08090c; color: #f4f4f5; font-family: 'Segoe UI'; font-size: 14px; }
        QFrame#sidebar { background: #0e0e12; border-right: 1px solid #51413e; }
        QFrame#card { background: rgba(12, 13, 17, 235); border: 1px solid #51413e; border-radius: 20px; }
        QFrame#card QLabel { background: transparent; }
        QLabel#title { font-size: 30px; font-weight: 600; }
        QLabel#brand { font-size: 21px; font-weight: 700; letter-spacing: 2px; }
        QLabel#muted { color: #b9ada9; }
        QLabel#metric { font-size: 25px; font-weight: 600; }
        QLineEdit, QComboBox, QSpinBox, QDoubleSpinBox { background: #1b1b20; border: 1px solid #484248; border-radius: 8px; padding: 11px; selection-background-color: #eeeeee; selection-color: #111111; }
        QLineEdit:focus, QComboBox:focus, QSpinBox:focus, QDoubleSpinBox:focus { border: 1px solid #ed916f; }
        QPushButton { background: #202025; border: 1px solid #494047; border-radius: 8px; padding: 11px 16px; font-weight: 600; }
        QPushButton:hover { background: #30272a; border-color: #ed916f; }
        QPushButton:focus { border: 1px solid #ed916f; }
        QPushButton:checked { background: #ed916f; color: #17100e; }
        QPushButton#primary { background: #ed916f; color: #17100e; border-color: #ed916f; }
        QPushButton#primary:hover { background: #ffab89; }
        QPushButton:disabled { background: #302320; color: #b89a90; border-color: #544039; }
        QTableWidget { background: #111116; alternate-background-color: #1b181c; border: 1px solid #51413e; border-radius: 12px; gridline-color: #352d31; selection-background-color: #633e31; selection-color: #ffffff; }
        QHeaderView::section { background: #302320; color: #f3c2af; padding: 12px; border: none; border-bottom: 1px solid #51413e; }
        QLabel { background: transparent; }
        QLabel#eyebrow { color: #ed916f; font-size: 12px; font-weight: 600; letter-spacing: 2px; }
        QFrame#sidebar QPushButton { text-align: left; }
        QFrame#sidebar QPushButton:checked { background: #ed916f; color: #17100e; border-color: #ed916f; }
        QWidget#page { background: transparent; }
        QWidget#assetIllustration { background: transparent; }
        QComboBox QAbstractItemView { background: #1b1b20; color: #f4f4f5; selection-background-color: #633e31; selection-color: white; border: 1px solid #51413e; }
        QScrollBar:vertical { background: #121216; width: 10px; margin: 0; }
        QScrollBar::handle:vertical { background: #6e5148; border-radius: 5px; min-height: 30px; }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }
        QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical { background: transparent; }
        QScrollArea { border: none; }
        QToolTip { background: #eeeeee; color: #111111; }
        QWidget#access, QFrame#illustration, QStackedWidget#authStack,
        QStackedWidget#authStack QWidget { background: transparent; }
        QFrame#illustration QLabel { background: transparent; }
        QFrame#authCard { background: rgba(12, 13, 17, 225); border: 1px solid #51413e; border-radius: 24px; }
        QLabel#hero { font-size: 37px; font-weight: 600; }
        QFrame#authCard QLabel#title { font-size: 28px; }
        QFrame#authCard QLabel#muted { color: #aaa5a5; font-size: 12px; }
        QFrame#authCard QLineEdit { background: #1b1b20; border: 1px solid #484248; border-radius: 8px; padding: 11px; }
        QFrame#authCard QLineEdit:focus { border-color: #ed916f; }
        QFrame#authCard QPushButton { background: #202025; border: 1px solid #494047; border-radius: 8px; }
        QFrame#authCard QPushButton:hover { background: #30272a; border-color: #ed916f; }
        QFrame#authCard QPushButton#primary { background: #ed916f; border-color: #ed916f; color: #17100e; }
        QFrame#authCard QPushButton#primary:hover { background: #ffab89; }
        QFrame#authCard QPushButton:disabled { background: #302320; color: #b89a90; border-color: #544039; }
        QFrame#authCard QPushButton#link { background: transparent; border: none; color: #efaa8f; padding: 8px; }
    )");
}
