#include "MainWindow.h"
#include "SidebarWidget.h"
#include "BookCardWidget.h"
#include "BookDetailDialog.h"
#include "DatabaseManager.h"
#include "CoverArtGenerator.h"
#include "Theme.h"
#include "LoginDialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QMessageBox>
#include <QHeaderView>
#include <QGroupBox>
#include <QGraphicsDropShadowEffect>

MainWindow::MainWindow(const Student &student, QWidget *parent)
    : QMainWindow(parent), m_student(student)
{
    // Ensure realistic demonstration borrowing records exist for demo student
    DatabaseManager::instance().ensureStudentDemoData(m_student.id);

    setupUI();
    refreshAll();
}

MainWindow::~MainWindow() = default;

void MainWindow::setupUI() {
    setWindowTitle("BookNest - Campus Library Management System");
    setMinimumSize(1150, 720);
    resize(1240, 780);

    // Apply main application style
    setStyleSheet(Theme::getApplicationStyle());

    auto *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    auto *rootLayout = new QHBoxLayout(centralWidget);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    // 1. Sidebar on Left
    m_sidebar = new SidebarWidget(this);
    m_sidebar->setStudent(m_student);
    connect(m_sidebar, &SidebarWidget::navItemClicked, this, &MainWindow::onNavIndexChanged);
    connect(m_sidebar, &SidebarWidget::returnBookRequested, this, &MainWindow::onReturnBookRequested);
    connect(m_sidebar, &SidebarWidget::viewBookDetailsRequested, this, &MainWindow::onBookDetailsRequested);
    connect(m_sidebar, &SidebarWidget::issueBookRequested, this, &MainWindow::onIssueBookRequested);
    connect(m_sidebar, &SidebarWidget::logoutRequested, this, &MainWindow::onLogout);
    rootLayout->addWidget(m_sidebar);

    // 2. Right Content Area (Top Bar + Stacked Pages)
    auto *contentArea = new QWidget(this);
    contentArea->setStyleSheet(QString("background-color: %1;").arg(Theme::ColorCreamBase));
    auto *contentLayout = new QVBoxLayout(contentArea);
    contentLayout->setContentsMargins(28, 20, 28, 20);
    contentLayout->setSpacing(16);

    // Top Header Bar
    auto *topBar = new QHBoxLayout();
    topBar->setSpacing(16);

    // Title & Subtitle
    auto *titleBox = new QVBoxLayout();
    titleBox->setSpacing(2);

    m_pageTitleLabel = new QLabel("Study Library", contentArea);
    m_pageTitleLabel->setStyleSheet(QString("font-size: 22px; font-weight: 800; color: %1;").arg(Theme::ColorTextDark));
    titleBox->addWidget(m_pageTitleLabel);

    m_pageSubtitleLabel = new QLabel(QString("Curriculum & academic resources for %1 (%2)").arg(m_student.academicYear, m_student.course), contentArea);
    m_pageSubtitleLabel->setStyleSheet(QString("font-size: 13px; color: %1;").arg(Theme::ColorTextMuted));
    titleBox->addWidget(m_pageSubtitleLabel);

    topBar->addLayout(titleBox);
    topBar->addStretch();

    // Universal Search Bar
    m_searchEdit = new QLineEdit(contentArea);
    m_searchEdit->setPlaceholderText("🔍 Search by title, author, category or ISBN...");
    m_searchEdit->setFixedWidth(320);
    m_searchEdit->setFixedHeight(36);
    m_searchEdit->setClearButtonEnabled(true);
    connect(m_searchEdit, &QLineEdit::textChanged, this, &MainWindow::onSearchChanged);
    topBar->addWidget(m_searchEdit);

    // Top Category Filter
    m_topCategoryCombo = new QComboBox(contentArea);
    m_topCategoryCombo->setFixedHeight(36);
    m_topCategoryCombo->setMinimumWidth(150);
    connect(m_topCategoryCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onCategoryFilterChanged);
    topBar->addWidget(m_topCategoryCombo);

    // Student Status Chip
    m_studentStatusChip = new QLabel(contentArea);
    m_studentStatusChip->setStyleSheet(QString(R"(
        background-color: %1;
        border: 1px solid %2;
        border-radius: 8px;
        padding: 6px 12px;
        font-size: 12px;
        font-weight: 700;
        color: %3;
    )").arg(Theme::ColorSageSoft, Theme::ColorSageBorder, Theme::ColorForestPrimary));
    topBar->addWidget(m_studentStatusChip);

    contentLayout->addLayout(topBar);

    // Toast Notification Banner (Hidden by default)
    m_toastLabel = new QLabel(contentArea);
    m_toastLabel->setFixedHeight(36);
    m_toastLabel->setAlignment(Qt::AlignCenter);
    m_toastLabel->setVisible(false);
    contentLayout->addWidget(m_toastLabel);

    m_toastTimer = new QTimer(this);
    m_toastTimer->setSingleShot(true);
    connect(m_toastTimer, &QTimer::timeout, this, [this]() {
        m_toastLabel->setVisible(false);
    });

    // Stacked Widget containing all main views
    m_stackedWidget = new QStackedWidget(contentArea);
    m_stackedWidget->setStyleSheet("background-color: transparent; border: none;");

    // Page 0: Study Library
    m_studyPage = createStudyLibraryPage();
    m_stackedWidget->addWidget(m_studyPage);

    // Page 1: Recommended for You
    m_recommendedPage = createRecommendedPage();
    m_stackedWidget->addWidget(m_recommendedPage);

    // Page 2: Explore Books
    m_explorePage = createExplorePage();
    m_stackedWidget->addWidget(m_explorePage);

    // Page 3: My Reading List
    m_readingListPage = createReadingListPage();
    m_stackedWidget->addWidget(m_readingListPage);

    // Page 4: Reading History
    m_historyPage = createReadingHistoryPage();
    m_stackedWidget->addWidget(m_historyPage);

    // Page 5: Settings
    m_settingsPage = createSettingsPage();
    m_stackedWidget->addWidget(m_settingsPage);

    contentLayout->addWidget(m_stackedWidget, 1);
    rootLayout->addWidget(contentArea, 1);
}

void MainWindow::showToast(const QString &message, bool isError) {
    m_toastLabel->setText(message);
    if (isError) {
        m_toastLabel->setStyleSheet(QString(R"(
            background-color: %1;
            color: %2;
            border: 1px solid %3;
            border-radius: 8px;
            font-weight: 600;
            font-size: 12px;
        )").arg(Theme::ColorCrimsonSoft, Theme::ColorCrimsonDanger, Theme::ColorCrimsonBorder));
    } else {
        m_toastLabel->setStyleSheet(QString(R"(
            background-color: %1;
            color: %2;
            border: 1px solid %3;
            border-radius: 8px;
            font-weight: 600;
            font-size: 12px;
        )").arg(Theme::ColorSageSoft, Theme::ColorForestPrimary, Theme::ColorSageBorder));
    }
    m_toastLabel->setVisible(true);
    m_toastTimer->start(4500);
}

void MainWindow::onNavIndexChanged(int index) {
    m_currentPageIndex = index;
    m_stackedWidget->setCurrentIndex(index);
    m_searchEdit->clear();

    switch (index) {
        case 0:
            m_pageTitleLabel->setText("Study Library");
            m_pageSubtitleLabel->setText(QString("Curriculum books for %1 • %2").arg(m_student.academicYear, m_student.course));
            m_topCategoryCombo->setVisible(true);
            refreshStudyLibrary();
            break;
        case 1:
            m_pageTitleLabel->setText("Recommended for You");
            m_pageSubtitleLabel->setText(QString("Personalized picks tailored to your reading profile (%1)").arg(m_student.preferences));
            m_topCategoryCombo->setVisible(false);
            refreshRecommended();
            break;
        case 2:
            m_pageTitleLabel->setText("Explore Books");
            m_pageSubtitleLabel->setText("Novels, fiction, history, science, philosophy, and general literature");
            m_topCategoryCombo->setVisible(true);
            refreshExplore();
            break;
        case 3:
            m_pageTitleLabel->setText("My Reading List");
            m_pageSubtitleLabel->setText("Saved books and bookmarks to read or borrow later");
            m_topCategoryCombo->setVisible(false);
            refreshReadingList();
            break;
        case 4:
            m_pageTitleLabel->setText("Reading History & Circulation");
            m_pageSubtitleLabel->setText("Complete timeline of all borrowed, returned, and active books");
            m_topCategoryCombo->setVisible(false);
            refreshReadingHistory();
            break;
        case 5:
            m_pageTitleLabel->setText("Student Settings");
            m_pageSubtitleLabel->setText("Update academic profile, interests, and library preferences");
            m_topCategoryCombo->setVisible(false);
            refreshSettings();
            break;
    }
    updateTopBarInfo();
}

void MainWindow::updateTopBarInfo() {
    int currentlyIssued = 0;
    int totalRead = 0;
    int readingList = 0;
    DatabaseManager::instance().getStudentStats(m_student.id, currentlyIssued, totalRead, readingList);

    m_studentStatusChip->setText(QString("📚 %1 Borrowed  •  ✓ %2 Returned  •  🔖 %3 Saved")
                                 .arg(currentlyIssued).arg(totalRead).arg(readingList));

    // Update categories in top bar combo
    m_topCategoryCombo->blockSignals(true);
    m_topCategoryCombo->clear();
    QStringList cats = DatabaseManager::instance().getCategories(m_currentPageIndex == 0, m_currentPageIndex == 2);
    m_topCategoryCombo->addItems(cats);
    m_topCategoryCombo->blockSignals(false);

    // Refresh sidebar issues
    auto currentIssues = DatabaseManager::instance().getCurrentlyIssuedBooks(m_student.id);
    auto previousIssues = DatabaseManager::instance().getPreviousIssues(m_student.id);
    m_sidebar->updateIssues(currentIssues, previousIssues);
    m_sidebar->setStudent(m_student);
}

void MainWindow::refreshAll() {
    updateTopBarInfo();
    refreshStudyLibrary();
    refreshRecommended();
    refreshExplore();
    refreshReadingList();
    refreshReadingHistory();
}

void MainWindow::onSearchChanged(const QString &query) {
    Q_UNUSED(query);
    switch (m_currentPageIndex) {
        case 0: refreshStudyLibrary(); break;
        case 1: refreshRecommended(); break;
        case 2: refreshExplore(); break;
        case 3: refreshReadingList(); break;
        case 4: refreshReadingHistory(); break;
        default: break;
    }
}

void MainWindow::onCategoryFilterChanged(int index) {
    Q_UNUSED(index);
    if (m_currentPageIndex == 0) {
        refreshStudyLibrary();
    } else if (m_currentPageIndex == 2) {
        refreshExplore();
    }
}

// ==========================================
// 1. Study Library Page
// ==========================================
QWidget* MainWindow::createStudyLibraryPage() {
    auto *page = new QWidget();
    auto *layout = new QVBoxLayout(page);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(12);

    auto *scrollArea = new QScrollArea(page);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setStyleSheet("background: transparent; border: none;");

    auto *scrollContent = new QWidget(scrollArea);
    m_studyGrid = new QGridLayout(scrollContent);
    m_studyGrid->setContentsMargins(4, 4, 4, 16);
    m_studyGrid->setSpacing(18);
    m_studyGrid->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    scrollArea->setWidget(scrollContent);
    layout->addWidget(scrollArea);

    return page;
}

void MainWindow::refreshStudyLibrary() {
    QString search = m_searchEdit->text();
    QString category = m_topCategoryCombo->currentText();
    auto books = DatabaseManager::instance().getStudyLibraryBooks(m_student.academicYear, m_student.course, search, category);
    populateBookGrid(m_studyGrid, books);
}

// ==========================================
// 2. Recommended Page
// ==========================================
QWidget* MainWindow::createRecommendedPage() {
    auto *page = new QWidget();
    auto *layout = new QVBoxLayout(page);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(12);

    // Header banner with preference tags
    auto *prefCard = new QFrame(page);
    prefCard->setStyleSheet(QString(R"(
        background-color: #FFFFFF;
        border: 1px solid %1;
        border-radius: 10px;
        padding: 10px 16px;
    )").arg(Theme::ColorCreamBorder));

    auto *prefLayout = new QHBoxLayout(prefCard);
    prefLayout->setContentsMargins(12, 10, 12, 10);
    prefLayout->setSpacing(10);

    auto *sparkle = new QLabel("✨", prefCard);
    sparkle->setStyleSheet("font-size: 18px;");
    prefLayout->addWidget(sparkle);

    m_prefTagsLabel = new QLabel(prefCard);
    m_prefTagsLabel->setStyleSheet("font-size: 12px; font-weight: 600; color: #1F4324;");
    prefLayout->addWidget(m_prefTagsLabel);
    prefLayout->addStretch();

    layout->addWidget(prefCard);

    auto *scrollArea = new QScrollArea(page);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setStyleSheet("background: transparent; border: none;");

    auto *scrollContent = new QWidget(scrollArea);
    m_recommendedGrid = new QGridLayout(scrollContent);
    m_recommendedGrid->setContentsMargins(4, 4, 4, 16);
    m_recommendedGrid->setSpacing(18);
    m_recommendedGrid->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    scrollArea->setWidget(scrollContent);
    layout->addWidget(scrollArea, 1);

    return page;
}

void MainWindow::refreshRecommended() {
    m_prefTagsLabel->setText(QString("Recommendations based on: <b>%1</b> • Class: <b>%2</b> • Interests: <i>%3</i>")
                            .arg(m_student.course, m_student.academicYear, m_student.preferences));

    QString search = m_searchEdit->text();
    auto books = DatabaseManager::instance().getRecommendedBooks(m_student.id, m_student.academicYear, m_student.preferences, search);
    populateBookGrid(m_recommendedGrid, books);
}

// ==========================================
// 3. Explore Books Page
// ==========================================
QWidget* MainWindow::createExplorePage() {
    auto *page = new QWidget();
    auto *layout = new QVBoxLayout(page);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(12);

    auto *scrollArea = new QScrollArea(page);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setStyleSheet("background: transparent; border: none;");

    auto *scrollContent = new QWidget(scrollArea);
    m_exploreGrid = new QGridLayout(scrollContent);
    m_exploreGrid->setContentsMargins(4, 4, 4, 16);
    m_exploreGrid->setSpacing(18);
    m_exploreGrid->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    scrollArea->setWidget(scrollContent);
    layout->addWidget(scrollArea);

    return page;
}

void MainWindow::refreshExplore() {
    QString search = m_searchEdit->text();
    QString category = m_topCategoryCombo->currentText();
    auto books = DatabaseManager::instance().getExploreBooks(search, category);
    populateBookGrid(m_exploreGrid, books);
}

// ==========================================
// 4. Reading List Page
// ==========================================
QWidget* MainWindow::createReadingListPage() {
    auto *page = new QWidget();
    auto *layout = new QVBoxLayout(page);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(12);

    m_readingListEmptyLabel = new QLabel("Your reading list is currently empty.\nClick the ☆ bookmark icon on any book to save it for later!", page);
    m_readingListEmptyLabel->setAlignment(Qt::AlignCenter);
    m_readingListEmptyLabel->setStyleSheet("color: #607263; font-size: 14px; font-weight: 500; padding: 40px;");
    layout->addWidget(m_readingListEmptyLabel);

    auto *scrollArea = new QScrollArea(page);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setStyleSheet("background: transparent; border: none;");

    auto *scrollContent = new QWidget(scrollArea);
    m_readingListGrid = new QGridLayout(scrollContent);
    m_readingListGrid->setContentsMargins(4, 4, 4, 16);
    m_readingListGrid->setSpacing(18);
    m_readingListGrid->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    scrollArea->setWidget(scrollContent);
    layout->addWidget(scrollArea, 1);

    return page;
}

void MainWindow::refreshReadingList() {
    QString search = m_searchEdit->text();
    auto books = DatabaseManager::instance().getReadingListBooks(m_student.id, search);

    if (books.isEmpty()) {
        m_readingListEmptyLabel->setVisible(true);
    } else {
        m_readingListEmptyLabel->setVisible(false);
    }
    populateBookGrid(m_readingListGrid, books);
}

// ==========================================
// 5. Reading History Page
// ==========================================
QWidget* MainWindow::createReadingHistoryPage() {
    auto *page = new QWidget();
    auto *layout = new QVBoxLayout(page);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(14);

    // Filter search row
    auto *filterRow = new QHBoxLayout();
    m_historySearchEdit = new QLineEdit(page);
    m_historySearchEdit->setPlaceholderText("Filter borrowing records...");
    m_historySearchEdit->setFixedWidth(280);
    connect(m_historySearchEdit, &QLineEdit::textChanged, this, &MainWindow::refreshReadingHistory);
    filterRow->addWidget(m_historySearchEdit);
    filterRow->addStretch();
    layout->addLayout(filterRow);

    // History Table
    m_historyTable = new QTableWidget(page);
    m_historyTable->setColumnCount(7);
    m_historyTable->setHorizontalHeaderLabels({
        "Book Title", "Author", "Category", "Issue Date", "Due Date", "Return Date / Status", "Action"
    });
    m_historyTable->horizontalHeader()->setStretchLastSection(false);
    m_historyTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_historyTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_historyTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_historyTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_historyTable->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    m_historyTable->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);
    m_historyTable->horizontalHeader()->setSectionResizeMode(6, QHeaderView::ResizeToContents);
    m_historyTable->verticalHeader()->setVisible(false);
    m_historyTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_historyTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    layout->addWidget(m_historyTable, 1);
    return page;
}

void MainWindow::refreshReadingHistory() {
    m_historyTable->setRowCount(0);
    auto records = DatabaseManager::instance().getAllIssueHistory(m_student.id);
    QString filter = m_historySearchEdit ? m_historySearchEdit->text().trimmed().toLower() : "";

    int row = 0;
    for (const auto &rec : records) {
        if (!filter.isEmpty()) {
            if (!rec.bookTitle.toLower().contains(filter) &&
                !rec.bookAuthor.toLower().contains(filter) &&
                !rec.category.toLower().contains(filter)) {
                continue;
            }
        }

        m_historyTable->insertRow(row);

        auto *itemTitle = new QTableWidgetItem(rec.bookTitle);
        itemTitle->setFont(QFont("Segoe UI", 10, QFont::Bold));
        m_historyTable->setItem(row, 0, itemTitle);

        m_historyTable->setItem(row, 1, new QTableWidgetItem(rec.bookAuthor));
        m_historyTable->setItem(row, 2, new QTableWidgetItem(rec.category));
        m_historyTable->setItem(row, 3, new QTableWidgetItem(rec.issueDate));
        m_historyTable->setItem(row, 4, new QTableWidgetItem(rec.dueDate));

        // Status Item with custom badge color
        auto *statusItem = new QTableWidgetItem();
        if (rec.isReturned()) {
            statusItem->setText(QString("Returned (%1)").arg(rec.returnDate));
            statusItem->setForeground(QBrush(QColor("#1F4324")));
        } else if (rec.isOverdue()) {
            statusItem->setText(QString("Overdue by %1 days").arg(qAbs(rec.daysRemaining())));
            statusItem->setForeground(QBrush(QColor("#991B1B")));
        } else if (rec.isDueSoon()) {
            statusItem->setText(QString("Due Soon (%1d left)").arg(rec.daysRemaining()));
            statusItem->setForeground(QBrush(QColor("#92400E")));
        } else {
            statusItem->setText(QString("Active (%1d left)").arg(rec.daysRemaining()));
            statusItem->setForeground(QBrush(QColor("#1F4324")));
        }
        statusItem->setFont(QFont("Segoe UI", 10, QFont::DemiBold));
        m_historyTable->setItem(row, 5, statusItem);

        // Action Widget
        if (!rec.isReturned()) {
            auto *returnBtn = new QPushButton("Return Book", m_historyTable);
            returnBtn->setCursor(Qt::PointingHandCursor);
            returnBtn->setStyleSheet(R"(
                QPushButton {
                    background-color: #FEE2E2;
                    color: #991B1B;
                    border: 1px solid #FCA5A5;
                    border-radius: 6px;
                    padding: 4px 12px;
                    font-weight: 700;
                    font-size: 11px;
                }
                QPushButton:hover {
                    background-color: #FDCFCF;
                }
            )");
            int issueId = rec.id;
            connect(returnBtn, &QPushButton::clicked, this, [this, issueId]() {
                onReturnBookRequested(issueId);
            });
            m_historyTable->setCellWidget(row, 6, returnBtn);
        } else {
            if (rec.availableCopies > 0) {
                auto *borrowAgainBtn = new QPushButton("Borrow Again", m_historyTable);
                borrowAgainBtn->setCursor(Qt::PointingHandCursor);
                borrowAgainBtn->setStyleSheet(R"(
                    QPushButton {
                        background-color: #E2EFE3;
                        color: #1F4324;
                        border: 1px solid #B8D6BC;
                        border-radius: 6px;
                        padding: 4px 12px;
                        font-weight: 700;
                        font-size: 11px;
                    }
                    QPushButton:hover {
                        background-color: #D1E5D3;
                    }
                )");
                int bookId = rec.bookId;
                connect(borrowAgainBtn, &QPushButton::clicked, this, [this, bookId]() {
                    onIssueBookRequested(bookId);
                });
                m_historyTable->setCellWidget(row, 6, borrowAgainBtn);
            } else {
                auto *outBtn = new QPushButton("Out of Stock", m_historyTable);
                outBtn->setEnabled(false);
                outBtn->setStyleSheet(R"(
                    QPushButton {
                        background-color: #F3F4F6;
                        color: #9CA3AF;
                        border: 1px solid #E5E7EB;
                        border-radius: 6px;
                        padding: 4px 12px;
                        font-weight: 600;
                        font-size: 11px;
                    }
                )");
                m_historyTable->setCellWidget(row, 6, outBtn);
            }
        }

        row++;
    }
}

// ==========================================
// 6. Settings Page
// ==========================================
QWidget* MainWindow::createSettingsPage() {
    auto *page = new QWidget();
    auto *layout = new QVBoxLayout(page);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(16);

    auto *scrollArea = new QScrollArea(page);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setStyleSheet("background: transparent; border: none;");

    auto *scrollContent = new QWidget(scrollArea);
    auto *contentLayout = new QVBoxLayout(scrollContent);
    contentLayout->setContentsMargins(4, 4, 4, 16);
    contentLayout->setSpacing(16);

    // Profile Box
    auto *profileCard = new QFrame(scrollContent);
    profileCard->setProperty("card", true);
    profileCard->setStyleSheet(QString(R"(
        background-color: #FFFFFF;
        border: 1px solid %1;
        border-radius: 12px;
        padding: 16px;
    )").arg(Theme::ColorCreamBorder));

    auto *cardLayout = new QVBoxLayout(profileCard);
    cardLayout->setSpacing(12);

    auto *h1 = new QLabel("Student Profile & Academic Record", profileCard);
    h1->setStyleSheet("font-size: 16px; font-weight: 800; color: #1F4324;");
    cardLayout->addWidget(h1);

    auto *grid = new QGridLayout();
    grid->setHorizontalSpacing(16);
    grid->setVerticalSpacing(12);

    auto createLabel = [profileCard](const QString &text) {
        auto *lbl = new QLabel(text, profileCard);
        lbl->setStyleSheet("font-size: 12px; font-weight: 700; color: #263229;");
        return lbl;
    };

    // Name
    grid->addWidget(createLabel("Full Name:"), 0, 0);
    m_settingsNameEdit = new QLineEdit(m_student.name, profileCard);
    grid->addWidget(m_settingsNameEdit, 0, 1);

    // Academic Year
    grid->addWidget(createLabel("Academic Year / Class:"), 1, 0);
    m_settingsYearCombo = new QComboBox(profileCard);
    m_settingsYearCombo->addItems({"1st Year", "2nd Year", "3rd Year", "4th Year", "Postgraduate"});
    m_settingsYearCombo->setCurrentText(m_student.academicYear);
    grid->addWidget(m_settingsYearCombo, 1, 1);

    // Course
    grid->addWidget(createLabel("Course / Department:"), 2, 0);
    m_settingsCourseCombo = new QComboBox(profileCard);
    m_settingsCourseCombo->addItems({
        "Computer Science & Engineering",
        "Information Technology",
        "Electronics & Communication",
        "Mechanical Engineering",
        "Civil Engineering",
        "General Studies"
    });
    m_settingsCourseCombo->setCurrentText(m_student.course);
    grid->addWidget(m_settingsCourseCombo, 2, 1);

    // Preferences
    grid->addWidget(createLabel("Reading Preferences (comma-separated):"), 3, 0);
    m_settingsPrefEdit = new QLineEdit(m_student.preferences, profileCard);
    grid->addWidget(m_settingsPrefEdit, 3, 1);

    // Theme
    grid->addWidget(createLabel("Appearance Palette:"), 4, 0);
    m_settingsThemeCombo = new QComboBox(profileCard);
    m_settingsThemeCombo->addItems({"Cream & Forest (Standard)", "Sage Minimalist", "Classic Sepia"});
    m_settingsThemeCombo->setCurrentIndex(0);
    grid->addWidget(m_settingsThemeCombo, 4, 1);

    cardLayout->addLayout(grid);

    // Save Button
    auto *saveBtn = new QPushButton("Save Profile Changes", profileCard);
    saveBtn->setProperty("primary", true);
    saveBtn->setFixedWidth(180);
    saveBtn->setFixedHeight(36);
    saveBtn->setCursor(Qt::PointingHandCursor);
    connect(saveBtn, &QPushButton::clicked, this, &MainWindow::onSaveSettings);
    cardLayout->addWidget(saveBtn);

    m_settingsSavedMsg = new QLabel(profileCard);
    m_settingsSavedMsg->setStyleSheet("color: #235C2B; font-weight: 600; font-size: 12px;");
    m_settingsSavedMsg->setVisible(false);
    cardLayout->addWidget(m_settingsSavedMsg);

    contentLayout->addWidget(profileCard);

    // Maintenance / Demo Card
    auto *demoCard = new QFrame(scrollContent);
    demoCard->setProperty("card", true);
    demoCard->setStyleSheet(QString(R"(
        background-color: #FFFFFF;
        border: 1px solid %1;
        border-radius: 12px;
        padding: 16px;
    )").arg(Theme::ColorCreamBorder));

    auto *demoLayout = new QVBoxLayout(demoCard);
    demoLayout->setSpacing(10);

    auto *h2 = new QLabel("Database & Demonstration Tools", demoCard);
    h2->setStyleSheet("font-size: 15px; font-weight: 700; color: #1F4324;");
    demoLayout->addWidget(h2);

    auto *dbInfo = new QLabel("Persistent SQLite Engine: <b>QSQLITE</b> active with 22 initial books seeded.", demoCard);
    dbInfo->setStyleSheet("font-size: 12px; color: #607263;");
    demoLayout->addWidget(dbInfo);

    auto *resetBtn = new QPushButton("Reset Library to Default Catalog", demoCard);
    resetBtn->setProperty("danger", true);
    resetBtn->setFixedWidth(240);
    resetBtn->setFixedHeight(34);
    resetBtn->setCursor(Qt::PointingHandCursor);
    connect(resetBtn, &QPushButton::clicked, this, &MainWindow::onResetCatalog);
    demoLayout->addWidget(resetBtn);

    contentLayout->addWidget(demoCard);
    contentLayout->addStretch();

    scrollArea->setWidget(scrollContent);
    layout->addWidget(scrollArea);

    return page;
}

void MainWindow::refreshSettings() {
    m_settingsNameEdit->setText(m_student.name);
    m_settingsYearCombo->setCurrentText(m_student.academicYear);
    m_settingsCourseCombo->setCurrentText(m_student.course);
    m_settingsPrefEdit->setText(m_student.preferences);
}

void MainWindow::onSaveSettings() {
    m_student.name = m_settingsNameEdit->text().trimmed();
    m_student.academicYear = m_settingsYearCombo->currentText();
    m_student.course = m_settingsCourseCombo->currentText();
    m_student.preferences = m_settingsPrefEdit->text().trimmed();

    if (DatabaseManager::instance().updateStudent(m_student)) {
        showToast("Profile and academic settings updated successfully!");
        m_sidebar->setStudent(m_student);
        updateTopBarInfo();
    } else {
        showToast("Failed to save changes to database.", true);
    }
}

void MainWindow::onResetCatalog() {
    auto res = QMessageBox::question(this, "Reset Catalog",
                                     "Are you sure you want to re-seed the sample books and reset test issues?",
                                     QMessageBox::Yes | QMessageBox::No);
    if (res == QMessageBox::Yes) {
        DatabaseManager::instance().resetToSampleData();
        refreshAll();
        showToast("Library catalog reset to default sample books!");
    }
}

// ==========================================
// Book Grid Helper
// ==========================================
void MainWindow::populateBookGrid(QGridLayout *grid, const QVector<Book> &books) {
    // Clear old items
    while (auto item = grid->takeAt(0)) {
        if (item->widget()) delete item->widget();
        delete item;
    }

    if (books.isEmpty()) {
        auto *emptyLabel = new QLabel("No books matched the current search query or filter.", grid->parentWidget());
        emptyLabel->setAlignment(Qt::AlignCenter);
        emptyLabel->setStyleSheet("color: #607263; font-size: 13px; font-weight: 500; padding: 40px;");
        grid->addWidget(emptyLabel, 0, 0, 1, 4, Qt::AlignCenter);
        return;
    }

    int colCount = 4; // 4 cards per row
    int r = 0;
    int c = 0;

    for (const auto &book : books) {
        bool isIssued = DatabaseManager::instance().isBookIssuedByStudent(m_student.id, book.id);
        bool inReadingList = DatabaseManager::instance().isBookInReadingList(m_student.id, book.id);

        auto *card = new BookCardWidget(book, isIssued, inReadingList, grid->parentWidget());
        connect(card, &BookCardWidget::issueRequested, this, &MainWindow::onIssueBookRequested);
        connect(card, &BookCardWidget::detailsRequested, this, &MainWindow::onBookDetailsRequested);
        connect(card, &BookCardWidget::readingListToggled, this, &MainWindow::onReadingListToggled);

        grid->addWidget(card, r, c);
        c++;
        if (c >= colCount) {
            c = 0;
            r++;
        }
    }
}

// ==========================================
// Core Issue & Return Logic
// ==========================================
void MainWindow::onIssueBookRequested(int bookId) {
    Book book = DatabaseManager::instance().getBookById(bookId);
    if (book.id <= 0) return;

    QString error;
    if (DatabaseManager::instance().issueBook(m_student.id, bookId, 14, &error)) {
        showToast(QString("📖 Successfully borrowed '%1'! Due in 14 days.").arg(book.title));
        refreshAll();
    } else {
        showToast(error.isEmpty() ? "Could not issue book." : error, true);
    }
}

void MainWindow::onReturnBookRequested(int issueId) {
    QString error;
    if (DatabaseManager::instance().returnBook(issueId, &error)) {
        showToast("✓ Book returned successfully to library inventory!");
        refreshAll();
    } else {
        showToast(error.isEmpty() ? "Could not process return." : error, true);
    }
}

void MainWindow::onReadingListToggled(int bookId) {
    bool inList = DatabaseManager::instance().isBookInReadingList(m_student.id, bookId);
    Book book = DatabaseManager::instance().getBookById(bookId);

    if (inList) {
        DatabaseManager::instance().removeFromReadingList(m_student.id, bookId);
        showToast(QString("Removed '%1' from Reading List.").arg(book.title));
    } else {
        DatabaseManager::instance().addToReadingList(m_student.id, bookId);
        showToast(QString("★ Added '%1' to Reading List!").arg(book.title));
    }
    refreshAll();
}

void MainWindow::onBookDetailsRequested(int bookId) {
    Book book = DatabaseManager::instance().getBookById(bookId);
    if (book.id <= 0) return;

    bool isIssued = DatabaseManager::instance().isBookIssuedByStudent(m_student.id, bookId);
    bool inReadingList = DatabaseManager::instance().isBookInReadingList(m_student.id, bookId);

    BookDetailDialog dlg(book, isIssued, inReadingList, this);
    connect(&dlg, &BookDetailDialog::issueRequested, this, &MainWindow::onIssueBookRequested);
    connect(&dlg, &BookDetailDialog::readingListToggled, this, &MainWindow::onReadingListToggled);
    dlg.exec();
}

void MainWindow::onLogout() {
    auto res = QMessageBox::question(this, "Log Out", "Do you want to switch or log out of your student profile?",
                                     QMessageBox::Yes | QMessageBox::No);
    if (res == QMessageBox::Yes) {
        close();
        LoginDialog loginDlg;
        if (loginDlg.exec() == QDialog::Accepted) {
            auto *newWin = new MainWindow(loginDlg.getStudent());
            newWin->show();
        }
    }
}
