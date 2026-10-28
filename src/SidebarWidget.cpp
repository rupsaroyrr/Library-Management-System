#include "SidebarWidget.h"
#include "CoverArtGenerator.h"
#include "DatabaseManager.h"
#include "Theme.h"
#include <QHBoxLayout>
#include <QFrame>

SidebarWidget::SidebarWidget(QWidget *parent) : QWidget(parent) {
    setupUI();
}

void SidebarWidget::setupUI() {
    setFixedWidth(285);
    setObjectName("sidebar");

    setStyleSheet(QString(R"(
        QWidget#sidebar {
            background-color: %1;
            border-right: 1px solid #162C18;
        }
    )").arg(Theme::ColorForestPrimary));

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 1. Top Logo Area (BookNest Logo)
    auto *logoWidget = new QWidget(this);
    logoWidget->setStyleSheet(QString("background-color: %1; border-bottom: 1px solid #18351B;").arg(Theme::ColorForestDark));
    auto *logoLayout = new QHBoxLayout(logoWidget);
    logoLayout->setContentsMargins(18, 16, 18, 16);
    logoLayout->setSpacing(12);

    m_logoLabel = new QLabel(logoWidget);
    m_logoLabel->setPixmap(CoverArtGenerator::generateLogo(42));
    logoLayout->addWidget(m_logoLabel);

    auto *titleBox = new QVBoxLayout();
    titleBox->setSpacing(2);

    m_titleLabel = new QLabel("BookNest", logoWidget);
    m_titleLabel->setStyleSheet("font-size: 20px; font-weight: 800; color: #FFFFFF; letter-spacing: 0.5px;");
    titleBox->addWidget(m_titleLabel);

    auto *subtitle = new QLabel("Campus Library", logoWidget);
    subtitle->setStyleSheet(QString("font-size: 11px; color: %1; font-weight: 600;").arg(Theme::ColorGoldAccent));
    titleBox->addWidget(subtitle);

    logoLayout->addLayout(titleBox);
    logoLayout->addStretch();
    mainLayout->addWidget(logoWidget);

    // 2. Middle Scrollable Area for Navigation & Dropdowns
    auto *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setStyleSheet("background: transparent; border: none;");

    auto *scrollContent = new QWidget(scrollArea);
    scrollContent->setStyleSheet("background: transparent;");
    auto *contentLayout = new QVBoxLayout(scrollContent);
    contentLayout->setContentsMargins(12, 14, 12, 14);
    contentLayout->setSpacing(6);

    // Section Header: Navigation
    auto *navHeader = new QLabel("EXPLORE LIBRARY", scrollContent);
    navHeader->setStyleSheet("color: #A3C9A8; font-size: 11px; font-weight: 800; letter-spacing: 1px; padding: 4px 8px;");
    contentLayout->addWidget(navHeader);

    // Nav Items:
    m_navButtons.append(createNavButton(QString::fromUtf8("📚"), "Study Library", 0));
    m_navButtons.append(createNavButton(QString::fromUtf8("✨"), "Recommended for You", 1));
    m_navButtons.append(createNavButton(QString::fromUtf8("🧭"), "Explore Books", 2));
    m_navButtons.append(createNavButton(QString::fromUtf8("🔖"), "My Reading List", 3));
    m_navButtons.append(createNavButton(QString::fromUtf8("📜"), "Reading History", 4));
    m_navButtons.append(createNavButton(QString::fromUtf8("⚙️"), "Settings", 5));

    for (auto *btn : m_navButtons) {
        contentLayout->addWidget(btn);
    }

    // Divider
    auto *divider1 = new QFrame(scrollContent);
    divider1->setFrameShape(QFrame::HLine);
    divider1->setStyleSheet("color: #2D5432; margin: 10px 4px;");
    contentLayout->addWidget(divider1);

    // Section Header: Circulation / Issues
    auto *circHeader = new QLabel("MY CIRCULATION", scrollContent);
    circHeader->setStyleSheet("color: #A3C9A8; font-size: 11px; font-weight: 800; letter-spacing: 1px; padding: 4px 8px;");
    contentLayout->addWidget(circHeader);

    // 3. Collapsible Section 1: Currently Issued Dropdown
    m_currentlyIssuedHeader = new QPushButton(QString::fromUtf8("▼  Currently Issued (0)"), scrollContent);
    m_currentlyIssuedHeader->setCursor(Qt::PointingHandCursor);
    m_currentlyIssuedHeader->setStyleSheet(R"(
        QPushButton {
            text-align: left;
            background-color: #264C2B;
            border: 1px solid #386B3E;
            border-radius: 8px;
            color: #FFFFFF;
            font-size: 13px;
            font-weight: 700;
            padding: 8px 12px;
        }
        QPushButton:hover {
            background-color: #2E5C34;
        }
    )");
    connect(m_currentlyIssuedHeader, &QPushButton::clicked, this, &SidebarWidget::toggleCurrentlyIssued);
    contentLayout->addWidget(m_currentlyIssuedHeader);

    m_currentlyIssuedContainer = new QWidget(scrollContent);
    m_currentlyIssuedLayout = new QVBoxLayout(m_currentlyIssuedContainer);
    m_currentlyIssuedLayout->setContentsMargins(2, 4, 2, 4);
    m_currentlyIssuedLayout->setSpacing(8);
    contentLayout->addWidget(m_currentlyIssuedContainer);

    // 4. Collapsible Section 2: Previous Issues Dropdown
    m_previousIssuesHeader = new QPushButton(QString::fromUtf8("▶  Previous Issues (0)"), scrollContent);
    m_previousIssuesHeader->setCursor(Qt::PointingHandCursor);
    m_previousIssuesHeader->setStyleSheet(R"(
        QPushButton {
            text-align: left;
            background-color: #264C2B;
            border: 1px solid #386B3E;
            border-radius: 8px;
            color: #FFFFFF;
            font-size: 13px;
            font-weight: 700;
            padding: 8px 12px;
        }
        QPushButton:hover {
            background-color: #2E5C34;
        }
    )");
    connect(m_previousIssuesHeader, &QPushButton::clicked, this, &SidebarWidget::togglePreviousIssues);
    contentLayout->addWidget(m_previousIssuesHeader);

    m_previousIssuesContainer = new QWidget(scrollContent);
    m_previousIssuesLayout = new QVBoxLayout(m_previousIssuesContainer);
    m_previousIssuesLayout->setContentsMargins(2, 4, 2, 4);
    m_previousIssuesLayout->setSpacing(8);
    m_previousIssuesContainer->setVisible(false); // Collapsed by default
    contentLayout->addWidget(m_previousIssuesContainer);

    contentLayout->addStretch();
    scrollArea->setWidget(scrollContent);
    mainLayout->addWidget(scrollArea, 1);

    // 5. Bottom Student Profile Card
    auto *profileCard = new QWidget(this);
    profileCard->setStyleSheet(QString(R"(
        background-color: %1;
        border-top: 1px solid #162C18;
    )").arg(Theme::ColorForestDark));

    auto *pLayout = new QHBoxLayout(profileCard);
    pLayout->setContentsMargins(14, 12, 14, 12);
    pLayout->setSpacing(10);

    m_avatarLabel = new QLabel(profileCard);
    m_avatarLabel->setFixedSize(36, 36);
    pLayout->addWidget(m_avatarLabel);

    auto *pInfo = new QVBoxLayout();
    pInfo->setSpacing(2);

    m_studentNameLabel = new QLabel("Student Name", profileCard);
    m_studentNameLabel->setStyleSheet("font-size: 13px; font-weight: 700; color: #FFFFFF;");
    pInfo->addWidget(m_studentNameLabel);

    m_studentYearLabel = new QLabel("1st Year • CS", profileCard);
    m_studentYearLabel->setStyleSheet("font-size: 12px; color: #C4DFC7; font-weight: 500;");
    pInfo->addWidget(m_studentYearLabel);

    pLayout->addLayout(pInfo);
    pLayout->addStretch();

    m_logoutBtn = new QPushButton(QString::fromUtf8("⇥"), profileCard);
    m_logoutBtn->setFixedSize(32, 32);
    m_logoutBtn->setToolTip("Log Out / Switch Student");
    m_logoutBtn->setCursor(Qt::PointingHandCursor);
    m_logoutBtn->setStyleSheet(R"(
        QPushButton {
            background-color: rgba(255, 255, 255, 0.12);
            color: #FFFFFF;
            border: 1px solid rgba(255, 255, 255, 0.25);
            border-radius: 6px;
            font-size: 14px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #DC2626;
            border-color: #DC2626;
        }
    )");
    connect(m_logoutBtn, &QPushButton::clicked, this, &SidebarWidget::logoutRequested);
    pLayout->addWidget(m_logoutBtn);

    mainLayout->addWidget(profileCard);

    setActiveNavIndex(0);
}

QPushButton* SidebarWidget::createNavButton(const QString &icon, const QString &text, int index) {
    auto *btn = new QPushButton(QString("%1  %2").arg(icon, text), this);
    btn->setCursor(Qt::PointingHandCursor);
    btn->setFixedHeight(38);
    btn->setStyleSheet(R"(
        QPushButton {
            text-align: left;
            background-color: transparent;
            border: none;
            border-radius: 8px;
            color: #E2ECE3;
            font-size: 14px;
            font-weight: 600;
            padding: 6px 14px;
        }
        QPushButton:hover {
            background-color: rgba(255, 255, 255, 0.12);
            color: #FFFFFF;
        }
    )");

    connect(btn, &QPushButton::clicked, this, [this, index]() {
        setActiveNavIndex(index);
        emit navItemClicked(index);
    });

    return btn;
}

void SidebarWidget::setActiveNavIndex(int index) {
    m_currentIndex = index;
    for (int i = 0; i < m_navButtons.size(); ++i) {
        if (i == index) {
            m_navButtons[i]->setStyleSheet(QString(R"(
                QPushButton {
                    text-align: left;
                    background-color: %1;
                    border: 1px solid #4E9B5B;
                    border-radius: 8px;
                    color: #FFFFFF;
                    font-size: 14px;
                    font-weight: 700;
                    padding: 6px 14px;
                }
            )").arg(Theme::ColorForestMedium));
        } else {
            m_navButtons[i]->setStyleSheet(R"(
                QPushButton {
                    text-align: left;
                    background-color: transparent;
                    border: none;
                    border-radius: 8px;
                    color: #E2ECE3;
                    font-size: 14px;
                    font-weight: 600;
                    padding: 6px 14px;
                }
                QPushButton:hover {
                    background-color: rgba(255, 255, 255, 0.12);
                    color: #FFFFFF;
                }
            )");
        }
    }
}

void SidebarWidget::setStudent(const Student &student) {
    m_student = student;
    m_studentNameLabel->setText(student.name.isEmpty() ? "Student" : student.name);
    m_studentYearLabel->setText(QString("%1 • %2").arg(student.academicYear, student.course));
    m_avatarLabel->setPixmap(CoverArtGenerator::generateStudentAvatar(student.name, student.avatarColor, 36));
}

void SidebarWidget::toggleCurrentlyIssued() {
    m_currentlyIssuedExpanded = !m_currentlyIssuedExpanded;
    m_currentlyIssuedContainer->setVisible(m_currentlyIssuedExpanded);
    QString arrow = m_currentlyIssuedExpanded ? QString::fromUtf8("▼") : QString::fromUtf8("▶");
    m_currentlyIssuedHeader->setText(QString("%1  Currently Issued").arg(arrow));
}

void SidebarWidget::togglePreviousIssues() {
    m_previousIssuesExpanded = !m_previousIssuesExpanded;
    m_previousIssuesContainer->setVisible(m_previousIssuesExpanded);
    QString arrow = m_previousIssuesExpanded ? QString::fromUtf8("▼") : QString::fromUtf8("▶");
    m_previousIssuesHeader->setText(QString("%1  Previous Issues").arg(arrow));
}

void SidebarWidget::updateIssues(const QVector<IssueRecord> &currentlyIssued, const QVector<IssueRecord> &previousIssues) {
    // 1. Update Currently Issued Header & Container
    QString arrow1 = m_currentlyIssuedExpanded ? QString::fromUtf8("▼") : QString::fromUtf8("▶");
    m_currentlyIssuedHeader->setText(QString("%1  Currently Issued (%2)").arg(arrow1).arg(currentlyIssued.size()));

    // Clear existing
    while (auto item = m_currentlyIssuedLayout->takeAt(0)) {
        if (item->widget()) delete item->widget();
        delete item;
    }

    if (currentlyIssued.isEmpty()) {
        auto *emptyFrame = new QFrame(m_currentlyIssuedContainer);
        emptyFrame->setStyleSheet(R"(
            QFrame {
                background-color: #FFFFFF;
                border: 1px solid #D8D1C3;
                border-radius: 8px;
                padding: 10px;
            }
        )");
        auto *emptyLayout = new QVBoxLayout(emptyFrame);
        emptyLayout->setContentsMargins(6, 6, 6, 6);
        emptyLayout->setSpacing(4);

        auto *emptyTitle = new QLabel("No Active Borrowings", emptyFrame);
        emptyTitle->setStyleSheet("color: #1F4324; font-size: 11px; font-weight: 700;");
        emptyLayout->addWidget(emptyTitle);

        auto *emptyDesc = new QLabel("You have no books currently checked out. Browse the Study Library or Explore section to borrow books!", emptyFrame);
        emptyDesc->setWordWrap(true);
        emptyDesc->setStyleSheet("color: #3D5243; font-size: 11px; font-weight: 500;");
        emptyLayout->addWidget(emptyDesc);

        m_currentlyIssuedLayout->addWidget(emptyFrame);
    } else {
        for (const auto &rec : currentlyIssued) {
            m_currentlyIssuedLayout->addWidget(createCurrentlyIssuedItem(rec));
        }
    }

    // 2. Update Previous Issues Header & Container
    QString arrow2 = m_previousIssuesExpanded ? QString::fromUtf8("▼") : QString::fromUtf8("▶");
    m_previousIssuesHeader->setText(QString("%1  Previous Issues (%2)").arg(arrow2).arg(previousIssues.size()));

    // Clear existing
    while (auto item = m_previousIssuesLayout->takeAt(0)) {
        if (item->widget()) delete item->widget();
        delete item;
    }

    if (previousIssues.isEmpty()) {
        auto *emptyFrame = new QFrame(m_previousIssuesContainer);
        emptyFrame->setStyleSheet(R"(
            QFrame {
                background-color: #FFFFFF;
                border: 1px solid #D8D1C3;
                border-radius: 8px;
                padding: 10px;
            }
        )");
        auto *emptyLayout = new QVBoxLayout(emptyFrame);
        emptyLayout->setContentsMargins(6, 6, 6, 6);
        emptyLayout->setSpacing(4);

        auto *emptyTitle = new QLabel("No Previous Issues", emptyFrame);
        emptyTitle->setStyleSheet("color: #1F4324; font-size: 11px; font-weight: 700;");
        emptyLayout->addWidget(emptyTitle);

        auto *emptyDesc = new QLabel("Returned books will appear here once you return a borrowed book.", emptyFrame);
        emptyDesc->setWordWrap(true);
        emptyDesc->setStyleSheet("color: #3D5243; font-size: 11px; font-weight: 500;");
        emptyLayout->addWidget(emptyDesc);

        m_previousIssuesLayout->addWidget(emptyFrame);
    } else {
        for (const auto &rec : previousIssues) {
            m_previousIssuesLayout->addWidget(createPreviousIssueItem(rec));
        }
    }
}

QWidget* SidebarWidget::createCurrentlyIssuedItem(const IssueRecord &record) {
    auto *card = new QFrame(m_currentlyIssuedContainer);
    card->setObjectName("drawerCard");
    card->setStyleSheet(R"(
        QFrame#drawerCard {
            background-color: #FFFFFF;
            border: 1px solid #D8D1C3;
            border-radius: 8px;
        }
        QFrame#drawerCard:hover {
            border: 1.5px solid #1F4324;
        }
    )");

    auto *mainRow = new QHBoxLayout(card);
    mainRow->setContentsMargins(8, 8, 8, 8);
    mainRow->setSpacing(8);

    // Book Cover Thumbnail
    Book book = DatabaseManager::instance().getBookById(record.bookId);
    if (book.id <= 0) {
        book.title = record.bookTitle;
        book.author = record.bookAuthor;
        book.category = record.category;
        book.coverColor = record.coverColor;
        book.coverAccent = record.coverAccent;
    }

    auto *coverLabel = new QLabel(card);
    coverLabel->setPixmap(CoverArtGenerator::generateThumbnail(book, 38, 52));
    coverLabel->setFixedSize(38, 52);
    mainRow->addWidget(coverLabel, 0, Qt::AlignTop);

    // Right Column Information
    auto *infoLayout = new QVBoxLayout();
    infoLayout->setContentsMargins(0, 0, 0, 0);
    infoLayout->setSpacing(3);

    // Title
    auto *titleLabel = new QLabel(record.bookTitle, card);
    titleLabel->setWordWrap(true);
    titleLabel->setStyleSheet("color: #111827; font-size: 13px; font-weight: 800; line-height: 1.2;");
    infoLayout->addWidget(titleLabel);

    // Author
    auto *authorLabel = new QLabel(record.bookAuthor, card);
    authorLabel->setStyleSheet("color: #263229; font-size: 11px; font-weight: 600;");
    infoLayout->addWidget(authorLabel);

    // Dates
    auto *datesLabel = new QLabel(QString("Issued: %1\nDue: %2").arg(record.issueDate, record.dueDate), card);
    datesLabel->setStyleSheet("color: #263229; font-size: 10px; font-weight: 500;");
    infoLayout->addWidget(datesLabel);

    // Status Badge & Return Button Row
    auto *actionRow = new QHBoxLayout();
    actionRow->setContentsMargins(0, 2, 0, 0);
    actionRow->setSpacing(6);

    auto *badgeLabel = new QLabel(card);
    int days = record.daysRemaining();
    if (record.isOverdue()) {
        badgeLabel->setText(QString("⚠️ Overdue (%1d)").arg(qAbs(days)));
        badgeLabel->setStyleSheet(Theme::badgeRedStyle());
    } else if (record.isDueSoon()) {
        badgeLabel->setText(QString("Due in %1d").arg(days));
        badgeLabel->setStyleSheet(Theme::badgeAmberStyle());
    } else {
        badgeLabel->setText(QString("Due in %1d").arg(days));
        badgeLabel->setStyleSheet(Theme::badgeGreenStyle());
    }
    actionRow->addWidget(badgeLabel);
    actionRow->addStretch();

    auto *returnBtn = new QPushButton("Return", card);
    returnBtn->setCursor(Qt::PointingHandCursor);
    returnBtn->setFixedHeight(24);
    returnBtn->setFixedWidth(54);
    returnBtn->setStyleSheet(R"(
        QPushButton {
            background-color: #1F4324;
            color: #FFFFFF;
            border: none;
            border-radius: 4px;
            font-size: 10px;
            font-weight: 700;
        }
        QPushButton:hover {
            background-color: #2B5E33;
        }
        QPushButton:pressed {
            background-color: #16311A;
        }
    )");
    int issueId = record.id;
    connect(returnBtn, &QPushButton::clicked, this, [this, issueId]() {
        emit returnBookRequested(issueId);
    });
    actionRow->addWidget(returnBtn);

    infoLayout->addLayout(actionRow);
    mainRow->addLayout(infoLayout, 1);

    return card;
}

QWidget* SidebarWidget::createPreviousIssueItem(const IssueRecord &record) {
    auto *card = new QFrame(m_previousIssuesContainer);
    card->setObjectName("drawerCard");
    card->setStyleSheet(R"(
        QFrame#drawerCard {
            background-color: #FFFFFF;
            border: 1px solid #D8D1C3;
            border-radius: 8px;
        }
        QFrame#drawerCard:hover {
            border: 1.5px solid #1F4324;
        }
    )");

    auto *mainRow = new QHBoxLayout(card);
    mainRow->setContentsMargins(8, 8, 8, 8);
    mainRow->setSpacing(8);

    // Book Cover Thumbnail
    Book book = DatabaseManager::instance().getBookById(record.bookId);
    if (book.id <= 0) {
        book.title = record.bookTitle;
        book.author = record.bookAuthor;
        book.category = record.category;
        book.coverColor = record.coverColor;
        book.coverAccent = record.coverAccent;
    }

    auto *coverLabel = new QLabel(card);
    coverLabel->setPixmap(CoverArtGenerator::generateThumbnail(book, 38, 52));
    coverLabel->setFixedSize(38, 52);
    mainRow->addWidget(coverLabel, 0, Qt::AlignTop);

    // Right Column
    auto *infoLayout = new QVBoxLayout();
    infoLayout->setContentsMargins(0, 0, 0, 0);
    infoLayout->setSpacing(3);

    // Title
    auto *titleLabel = new QLabel(record.bookTitle, card);
    titleLabel->setWordWrap(true);
    titleLabel->setStyleSheet("color: #111827; font-size: 13px; font-weight: 800; line-height: 1.2;");
    infoLayout->addWidget(titleLabel);

    // Author
    auto *authorLabel = new QLabel(record.bookAuthor, card);
    authorLabel->setStyleSheet("color: #263229; font-size: 11px; font-weight: 600;");
    infoLayout->addWidget(authorLabel);

    // Dates & Returned Badge
    auto *datesLabel = new QLabel(QString("Issued: %1\nReturned: %2").arg(record.issueDate, record.returnDate), card);
    datesLabel->setStyleSheet("color: #263229; font-size: 10px; font-weight: 500;");
    infoLayout->addWidget(datesLabel);

    auto *badgeLabel = new QLabel("Returned ✓", card);
    badgeLabel->setStyleSheet(Theme::badgeGreenStyle());
    infoLayout->addWidget(badgeLabel);

    // Action Buttons: View Details & Borrow Again
    auto *btnRow = new QHBoxLayout();
    btnRow->setContentsMargins(0, 4, 0, 0);
    btnRow->setSpacing(6);

    auto *detailsBtn = new QPushButton("Details", card);
    detailsBtn->setCursor(Qt::PointingHandCursor);
    detailsBtn->setFixedHeight(24);
    detailsBtn->setStyleSheet(R"(
        QPushButton {
            background-color: #FFFFFF;
            color: #1F4324;
            border: 1px solid #B8D3BC;
            border-radius: 4px;
            font-size: 10px;
            font-weight: 700;
            padding: 2px 6px;
        }
        QPushButton:hover {
            background-color: #E9F0EA;
        }
    )");
    int bookId = record.bookId;
    connect(detailsBtn, &QPushButton::clicked, this, [this, bookId]() {
        emit viewBookDetailsRequested(bookId);
    });
    btnRow->addWidget(detailsBtn);

    auto *borrowAgainBtn = new QPushButton(card);
    borrowAgainBtn->setFixedHeight(24);
    borrowAgainBtn->setCursor(Qt::PointingHandCursor);

    if (book.availableCopies > 0) {
        borrowAgainBtn->setText("Borrow");
        borrowAgainBtn->setStyleSheet(R"(
            QPushButton {
                background-color: #1F4324;
                color: #FFFFFF;
                border: none;
                border-radius: 4px;
                font-size: 10px;
                font-weight: 700;
                padding: 2px 6px;
            }
            QPushButton:hover {
                background-color: #2B5E33;
            }
        )");
        connect(borrowAgainBtn, &QPushButton::clicked, this, [this, bookId]() {
            emit issueBookRequested(bookId);
        });
    } else {
        borrowAgainBtn->setText("Out of Stock");
        borrowAgainBtn->setEnabled(false);
        borrowAgainBtn->setStyleSheet(R"(
            QPushButton {
                background-color: #FEE2E2;
                color: #991B1B;
                border: 1px solid #FCA5A5;
                border-radius: 4px;
                font-size: 9px;
                font-weight: 700;
                padding: 2px 4px;
            }
        )");
    }
    btnRow->addWidget(borrowAgainBtn);

    infoLayout->addLayout(btnRow);
    mainRow->addLayout(infoLayout, 1);

    return card;
}
