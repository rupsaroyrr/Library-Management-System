#pragma once

#include <QString>
#include <QColor>

namespace Theme {
    // Primary Forest Green Palette
    inline const QString ColorForestDark      = "#16311A"; // Deep forest
    inline const QString ColorForestPrimary   = "#1F4324"; // Main brand green
    inline const QString ColorForestMedium    = "#2B5E33"; // Medium forest green
    inline const QString ColorForestHover     = "#36713E"; // Hover state
    inline const QString ColorForestLight     = "#4E9B5B"; // Lighter accent
    inline const QString ColorSageSoft        = "#E9F0EA"; // Very soft sage background
    inline const QString ColorSageBorder      = "#B8D3BC"; // Soft border green

    // Warm Cream Palette
    inline const QString ColorCreamBase       = "#F7F5EE"; // Main application background
    inline const QString ColorCreamCard       = "#FFFFFF"; // Elevated card surface (pure white for contrast)
    inline const QString ColorCreamWarm       = "#F0ECE1"; // Sub-surface / active items / table headers
    inline const QString ColorCreamBorder     = "#D8D1C3"; // Clean neutral border
    inline const QString ColorCreamInput      = "#FFFFFF"; // Crisp white input box fill

    // Accents
    inline const QString ColorGoldAccent      = "#C88A3E"; // Warm gold / bookmark

    // Error toast colors
    inline const QString ColorCrimsonSoft     = "#FEE2E2";
    inline const QString ColorCrimsonDanger   = "#991B1B";
    inline const QString ColorCrimsonBorder   = "#FCA5A5";

    // Status Badges (Green = available/returned, Amber = due soon, Red = overdue)
    inline const QString ColorBadgeGreenBg    = "#E2EFE3";
    inline const QString ColorBadgeGreenText  = "#1B4D22";
    inline const QString ColorBadgeGreenBorder= "#B7D9BA";

    inline const QString ColorBadgeAmberBg    = "#FEF3C7";
    inline const QString ColorBadgeAmberText  = "#92400E";
    inline const QString ColorBadgeAmberBorder= "#FCD34D";

    inline const QString ColorBadgeRedBg      = "#FEE2E2";
    inline const QString ColorBadgeRedText    = "#991B1B";
    inline const QString ColorBadgeRedBorder  = "#FCA5A5";

    // Typography Colors - High Contrast
    inline const QString ColorTextDark        = "#263229"; // Dark charcoal for general text
    inline const QString ColorTextForest      = "#1F4324"; // Dark forest green for headings & titles
    inline const QString ColorTextMuted       = "#3D5243"; // Deep readable sage charcoal (never washed out)
    inline const QString ColorTextPlaceholder = "#6B7F70"; // Readable placeholder text
    inline const QString ColorTextLight       = "#FFFFFF"; // Pure white only on dark green backgrounds

    // Helper to generate CSS for status badges
    inline QString badgeStyle(const QString &bg, const QString &text, const QString &border) {
        return QString(R"(
            background-color: %1;
            color: %2;
            border: 1px solid %3;
            border-radius: 4px;
            padding: 2px 7px;
            font-size: 11px;
            font-weight: 700;
        )").arg(bg, text, border);
    }

    inline QString badgeGreenStyle() {
        return badgeStyle(ColorBadgeGreenBg, ColorBadgeGreenText, ColorBadgeGreenBorder);
    }

    inline QString badgeAmberStyle() {
        return badgeStyle(ColorBadgeAmberBg, ColorBadgeAmberText, ColorBadgeAmberBorder);
    }

    inline QString badgeRedStyle() {
        return badgeStyle(ColorBadgeRedBg, ColorBadgeRedText, ColorBadgeRedBorder);
    }

    // Comprehensive modern stylesheet with maximum legibility
    inline QString getApplicationStyle() {
        return QString(R"(
            /* Global Application Styling */
            QWidget {
                font-family: 'Segoe UI', -apple-system, BlinkMacSystemFont, 'Inter', Roboto, sans-serif;
                font-size: 14px;
                color: %1;
            }

            QMainWindow {
                background-color: %2;
            }

            /* Tooltips */
            QToolTip {
                background-color: %3;
                color: #FFFFFF;
                border: 1px solid #336139;
                border-radius: 6px;
                padding: 6px 10px;
                font-size: 12px;
                font-weight: 500;
            }

            /* Menus and Popups */
            QMenu {
                background-color: #FFFFFF;
                border: 1px solid %5;
                border-radius: 8px;
                padding: 6px;
                color: %1;
            }
            QMenu::item {
                padding: 6px 24px 6px 12px;
                border-radius: 4px;
                color: %1;
            }
            QMenu::item:selected {
                background-color: %7;
                color: %8;
                font-weight: 600;
            }
            QMenu::separator {
                height: 1px;
                background-color: %5;
                margin: 4px 0px;
            }

            /* Scrollbars */
            QScrollBar:vertical {
                border: none;
                background: transparent;
                width: 8px;
                margin: 0px;
            }
            QScrollBar::handle:vertical {
                background: #C4BCAC;
                min-height: 28px;
                border-radius: 4px;
            }
            QScrollBar::handle:vertical:hover {
                background: #AFA694;
            }
            QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
                height: 0px;
            }
            QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {
                background: transparent;
            }

            QScrollBar:horizontal {
                border: none;
                background: transparent;
                height: 8px;
            }
            QScrollBar::handle:horizontal {
                background: #C4BCAC;
                min-width: 28px;
                border-radius: 4px;
            }
            QScrollBar::handle:horizontal:hover {
                background: #AFA694;
            }
            QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {
                width: 0px;
            }

            /* LineEdits */
            QLineEdit {
                background-color: #FFFFFF;
                border: 1.5px solid %5;
                border-radius: 8px;
                padding: 8px 14px;
                font-size: 14px;
                font-weight: 500;
                color: %1;
                selection-background-color: %6;
                selection-color: #FFFFFF;
            }
            QLineEdit:hover {
                border-color: #A39B89;
                background-color: #FFFFFF;
            }
            QLineEdit:focus {
                border: 2px solid %6;
                background-color: #FFFFFF;
            }
            QLineEdit::placeholder {
                color: #6B7F70;
            }

            /* ComboBoxes */
            QComboBox {
                background-color: #FFFFFF;
                border: 1.5px solid %5;
                border-radius: 8px;
                padding: 7px 14px;
                font-size: 14px;
                font-weight: 500;
                color: %1;
            }
            QComboBox:hover {
                border-color: #A39B89;
                background-color: #FFFFFF;
            }
            QComboBox:focus {
                border: 2px solid %6;
                background-color: #FFFFFF;
            }
            QComboBox::drop-down {
                subcontrol-origin: padding;
                subcontrol-position: top right;
                width: 28px;
                border-left-width: 0px;
                border-top-right-radius: 8px;
                border-bottom-right-radius: 8px;
            }
            QComboBox QAbstractItemView {
                background-color: #FFFFFF;
                border: 1px solid %5;
                border-radius: 8px;
                padding: 4px;
                color: %1;
                selection-background-color: %7;
                selection-color: %8;
                outline: none;
            }

            /* PushButtons */
            QPushButton {
                background-color: #FFFFFF;
                border: 1.5px solid %5;
                border-radius: 8px;
                padding: 8px 16px;
                font-weight: 600;
                font-size: 14px;
                color: %1;
            }
            QPushButton:hover {
                background-color: #F5F1E8;
                border-color: %6;
                color: %6;
            }
            QPushButton:pressed {
                background-color: #EDE7DA;
            }

            /* Primary Button (Forest Green) */
            QPushButton[primary="true"] {
                background-color: %6;
                border: 1px solid %6;
                color: #FFFFFF;
                font-weight: 700;
                font-size: 14px;
            }
            QPushButton[primary="true"]:hover {
                background-color: %9;
                border-color: %9;
            }
            QPushButton[primary="true"]:pressed {
                background-color: %3;
                border-color: %3;
            }
            QPushButton[primary="true"]:disabled {
                background-color: #9DB3A1;
                border-color: #9DB3A1;
                color: #E6ECE7;
            }

            /* Danger Button */
            QPushButton[danger="true"] {
                background-color: #FFFFFF;
                border: 1.5px solid %10;
                color: %10;
                font-weight: 600;
            }
            QPushButton[danger="true"]:hover {
                background-color: %11;
            }
            QPushButton[danger="true"]:pressed {
                background-color: #FCE7EB;
            }

            /* Outline Button */
            QPushButton[outline="true"] {
                background-color: #FFFFFF;
                border: 1.5px solid %6;
                color: %6;
                font-weight: 600;
            }
            QPushButton[outline="true"]:hover {
                background-color: %7;
            }
            QPushButton[outline="true"]:pressed {
                background-color: #DDE8DE;
            }

            /* Card Frame */
            QFrame[card="true"] {
                background-color: #FFFFFF;
                border: 1px solid %5;
                border-radius: 12px;
            }

            /* Table Widgets */
            QTableWidget {
                background-color: #FFFFFF;
                border: 1px solid %5;
                border-radius: 10px;
                gridline-color: #EDE8DC;
                color: %1;
                selection-background-color: %7;
                selection-color: %8;
                outline: none;
            }
            QHeaderView::section {
                background-color: %12;
                color: %6;
                font-weight: 700;
                font-size: 13px;
                padding: 10px 14px;
                border: none;
                border-bottom: 2px solid %5;
            }
            QTableWidget::item {
                padding: 8px 12px;
                border-bottom: 1px solid #F5F1E8;
                color: %1;
            }
            QTableWidget::item:selected {
                background-color: %7;
                color: %8;
                font-weight: 600;
            }

            /* TabBar Styling */
            QTabWidget::pane {
                border: none;
                background-color: transparent;
            }
            QTabBar::tab {
                background-color: transparent;
                border: none;
                border-bottom: 3px solid transparent;
                padding: 10px 18px;
                font-weight: 600;
                font-size: 14px;
                color: %13;
                margin-right: 8px;
            }
            QTabBar::tab:selected {
                color: %6;
                font-weight: 700;
                border-bottom: 3px solid %6;
            }
            QTabBar::tab:hover:!selected {
                color: %6;
                background-color: rgba(31, 67, 36, 0.06);
                border-radius: 6px 6px 0px 0px;
            }

            /* Checkboxes */
            QCheckBox {
                spacing: 8px;
                font-size: 14px;
                color: %1;
                font-weight: 500;
            }
            QCheckBox::indicator {
                width: 18px;
                height: 18px;
                border-radius: 4px;
                border: 1.5px solid %5;
                background-color: #FFFFFF;
            }
            QCheckBox::indicator:hover {
                border-color: %6;
            }
            QCheckBox::indicator:checked {
                background-color: %6;
                border-color: %6;
            }
        )")
        .arg(ColorTextDark)       // %1
        .arg(ColorCreamBase)      // %2
        .arg(ColorForestDark)     // %3
        .arg(ColorCreamInput)     // %4
        .arg(ColorCreamBorder)    // %5
        .arg(ColorForestPrimary)  // %6
        .arg(ColorSageSoft)       // %7
        .arg(ColorForestPrimary)  // %8
        .arg(ColorForestHover)    // %9
        .arg("#DC2626")           // %10 (Crimson Danger)
        .arg(ColorBadgeRedBg)     // %11 (Crimson Soft)
        .arg(ColorCreamWarm)      // %12
        .arg(ColorTextMuted);     // %13
    }
}
