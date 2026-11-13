#pragma once
#include <QString>

inline QString temaOscuro()
{
    return QStringLiteral(R"(
        QWidget { background: #08090c; color: #f4f4f5; font-family: 'Segoe UI'; font-size: 14px; }
        QFrame#sidebar { background: #141416; border-right: 1px solid #2b292a; }
        QFrame#card { background: rgba(23, 23, 25, 245); border: 1px solid #2e2b2b; border-top-color: #484248; border-bottom-color: #141416; border-radius: 16px; }
        QFrame#card QLabel { background: transparent; }
        QLabel#title { font-size: 26px; font-weight: 600; }
        QLabel#brand { font-size: 18px; font-weight: 700; letter-spacing: 2px; }
        QLabel#brandAccess { font-size: 23px; font-weight: 600; }
        QLabel#brandSidebar { font-size: 17px; font-weight: 600; }
        QLabel#avatarAdmin { background: #33241f; color: #ed916f; border: 1px solid #624235; border-radius: 20px; font-size: 20px; font-weight: 600; }
        QLabel#brandSlogan { font-size: 30px; font-weight: 600; }
        QLabel#passwordHint { color: #ed916f; font-size: 12px; }
        QLabel#sampleNotice { color: #d4afa0; font-size: 12px; background: #27201e; border-radius: 8px; padding: 10px; }
        QWidget#dashboard QPushButton#quickTerm { padding: 7px 6px; font-size: 12px; }
        QWidget#dashboard QPushButton#helpLink { background: transparent; border: none; color: #ed916f; text-align: left; padding: 8px 0; }
        QWidget#dashboard QPushButton#helpLink:focus { border: 1px solid #ed916f; }
        QTableWidget#amortizationTable::item { padding: 0 8px; }
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
        QPushButton:disabled { background: #242124; color: #aaa0a0; border-color: #3c3432; }
        QTableWidget { background: #111116; alternate-background-color: #1b181c; border: 1px solid #302e31; border-radius: 8px; gridline-color: #352d31; selection-background-color: #633e31; selection-color: #ffffff; }
        QHeaderView::section { background: #222124; color: #c4b9b5; padding: 10px; border: none; border-bottom: 1px solid #353033; }
        QLabel { background: transparent; }
        QLabel#eyebrow { color: #ed916f; font-size: 12px; font-weight: 600; letter-spacing: 2px; }
        QFrame#sidebar QPushButton { text-align: left; background: transparent; border: 1px solid transparent; padding: 12px; }
        QFrame#sidebar QPushButton:hover { background: #242022; }
        QFrame#sidebar QPushButton:checked { background: #33241f; color: #f1a281; border-color: #624235; }
        QWidget#page { background: transparent; }
        QWidget#activityFilterGroup { background: transparent; }
        QWidget#activityFilterGroup QComboBox, QWidget#activityFilterGroup QDateEdit { padding: 7px 40px 7px 10px; }
        QWidget#helpContent { background: transparent; }
        QWidget#comparisonContent { background: transparent; }
        QLabel#comparisonNotice { background: #27201e; color: #d4afa0; border: 1px solid #49362e; border-radius: 8px; padding: 12px; }
        QWidget#comparisonContent QComboBox QLineEdit { background: transparent; border: none; padding: 0; }
        QWidget#comparisonContent QTableWidget::item { padding: 0 10px; }
        QPushButton#topicCard { background: rgba(23,23,25,245); border: 1px solid #484248; border-radius: 16px; text-align: left; }
        QPushButton#topicCard:hover { background: #242022; border-color: #ed916f; }
        QPushButton#topicCard:focus { border: 2px solid #ed916f; }
        QPushButton#topicCard:pressed { background: #33241f; }
        QPushButton#topicCard QLabel { background: transparent; border: none; }
        QLabel#topicLink { color: #ed916f; font-size: 13px; font-weight: 600; }
        QWidget#topicIcon { background: transparent; }
        QWidget#historyActionCell { background: transparent; }
        QPushButton#deleteRow { background: transparent; color: #b9ada9; border: 1px solid transparent; border-radius: 6px; padding: 0; font-size: 24px; }
        QPushButton#deleteRow:hover, QPushButton#deleteRow:focus { background: #33241f; color: #ed916f; border-color: #ed916f; }
        QFrame#topbar { background: #141416; border-bottom: 1px solid #2b292a; }
        QLabel#section { font-size: 17px; font-weight: 600; }
        QLabel#caption { font-size: 12px; color: #a89f9c; }
        QLabel#badge { font-size: 11px; color: #d4afa0; background: #27201e; border: 1px solid #49362e; border-radius: 6px; padding: 5px 8px; }
        QLabel#metricAccent { font-size: 29px; font-weight: 600; color: #ed916f; }
        QFrame#card QPushButton#primary:disabled { background: #402c24; color: #d2a28e; border-color: #634535; }
        QWidget#dashboard QLineEdit, QWidget#dashboard QComboBox,
        QWidget#dashboard QSpinBox, QWidget#dashboard QDoubleSpinBox { padding: 7px 10px; }
        QWidget#dashboard QPushButton { padding: 9px 12px; }
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
        QFrame#authCard { background: #171719; border: 1px solid #3d3330; border-radius: 20px; }
        QLabel#hero { font-size: 37px; font-weight: 600; }
        QFrame#authCard QLabel#title { font-size: 28px; }
        QFrame#authCard QLabel#muted { color: #aaa5a5; font-size: 12px; }
        QFrame#authCard QLineEdit { background: #1b1b20; border: 1px solid #484248; border-radius: 8px; padding: 11px; }
        QFrame#authCard QLineEdit:focus { border-color: #ed916f; }
        QFrame#authCard QPushButton { background: #202025; border: 1px solid #494047; border-radius: 8px; }
        QFrame#authCard QPushButton:hover { background: #30272a; border-color: #ed916f; }
        QFrame#authCard QPushButton#primary { background: #ed916f; border-color: #ed916f; color: #17100e; }
        QFrame#authCard QPushButton#primary:hover { background: #ffab89; }
        QFrame#authCard QPushButton:disabled { background: #242124; color: #aaa0a0; border-color: #3c3432; }
        QFrame#authCard QPushButton#link { background: transparent; border: none; color: #efaa8f; padding: 8px; }

        /* Controles con indicadores blancos independientes del tema de Windows. */
        QDateEdit { background: #1b1b20; border: 1px solid #484248; border-radius: 8px; padding: 11px; padding-right: 40px; }
        QDateEdit:focus { border-color: #ed916f; }
        QComboBox, QSpinBox, QDoubleSpinBox { padding-right: 40px; }
        QWidget#dashboard QComboBox, QWidget#dashboard QSpinBox, QWidget#dashboard QDoubleSpinBox { padding-right: 40px; }
        QComboBox:hover, QDateEdit:hover { border-color: #a57561; }
        QComboBox::drop-down, QDateEdit::drop-down { subcontrol-origin: border; subcontrol-position: top right; width: 32px; border-left: 1px solid #484248; border-top-right-radius: 8px; border-bottom-right-radius: 8px; background: #242126; }
        QComboBox::drop-down:hover, QDateEdit::drop-down:hover { background: #493128; }
        QComboBox::down-arrow, QDateEdit::down-arrow { image: url(:/controles/abajo.xpm); width: 12px; height: 12px; }
        QComboBox QAbstractItemView { padding: 6px; outline: 0; border: 1px solid #705044; border-radius: 8px; background: #1b1b20; selection-background-color: #493128; selection-color: #ffffff; }
        QComboBox QAbstractItemView::item { min-height: 32px; padding: 4px 10px; }
        QSpinBox::up-button, QDoubleSpinBox::up-button { subcontrol-origin: border; subcontrol-position: top right; width: 30px; background: #29252a; border-left: 1px solid #484248; border-top-right-radius: 8px; }
        QSpinBox::down-button, QDoubleSpinBox::down-button { subcontrol-origin: border; subcontrol-position: bottom right; width: 30px; background: #29252a; border-left: 1px solid #484248; border-top: 1px solid #484248; border-bottom-right-radius: 8px; }
        QSpinBox::up-button:hover, QDoubleSpinBox::up-button:hover, QSpinBox::down-button:hover, QDoubleSpinBox::down-button:hover { background: #493128; }
        QSpinBox::up-arrow, QDoubleSpinBox::up-arrow { image: url(:/controles/arriba.xpm); width: 12px; height: 12px; }
        QSpinBox::down-arrow, QDoubleSpinBox::down-arrow { image: url(:/controles/abajo.xpm); width: 12px; height: 12px; }
        QCalendarWidget { background: #171719; color: white; }
        QCalendarWidget QWidget#qt_calendar_navigationbar { background: #27201e; padding: 6px; }
        QCalendarWidget QToolButton { color: white; background: transparent; border: 1px solid transparent; border-radius: 6px; padding: 7px; font-weight: 600; }
        QCalendarWidget QToolButton:hover { background: #493128; border-color: #ed916f; }
        QCalendarWidget QToolButton#qt_calendar_prevmonth { qproperty-icon: url(:/controles/izquierda.xpm); }
        QCalendarWidget QToolButton#qt_calendar_nextmonth { qproperty-icon: url(:/controles/derecha.xpm); }
        QCalendarWidget QMenu { background: #202025; color: white; border: 1px solid #705044; }
        QCalendarWidget QMenu::item { padding: 8px 20px; }
        QCalendarWidget QMenu::item:selected { background: #493128; }
        QCalendarWidget QAbstractItemView { background: #171719; alternate-background-color: #242126; color: #f4f4f5; selection-background-color: #ed916f; selection-color: #17100e; outline: 0; border: 1px solid #484248; font-size: 14px; }
        QCalendarWidget QAbstractItemView:disabled { color: #777077; }
    )");
}
