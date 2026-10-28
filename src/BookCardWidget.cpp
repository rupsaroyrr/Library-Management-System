#include "BookCardWidget.h"
#include "CoverArtGenerator.h"
#include "Theme.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGraphicsDropShadowEffect>

BookCardWidget::BookCardWidget(const Book &book, bool isIssuedByStudent, bool isInReadingList, QWidget *parent)
    : QFrame(parent), m_book(book), m_isIssued(isIssuedByStudent), m_inReadingList(isInReadingList)
{
    setupUI();
}

void BookCardWidget::setupUI() {
    setFixedWidth(200);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    setObjectName("bookCard");
    setProperty("card", true);

    setStyleSheet(QString(R"(
        QFrame#bookCard {
            background-color: #FFFFFF;
            border: 1px solid %1;
            border-radius: 12px;
        }
        QFrame#bookCard:hover {
            border: 1.5px solid %2;
            background-color: #FDFAF5;
        }
    )").arg(Theme::ColorCreamBorder, Theme::ColorForestMedium));

    // Drop shadow
    auto *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(16);
    shadow->setColor(QColor(0, 0, 0, 18));
    shadow->setOffset(0, 3);
    setGraphicsEffect(shadow);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(12, 12, 12, 12);
    mainLayout->setSpacing(8);

    // Book Cover Container
    m_coverLabel = new QLabel(this);
    m_coverLabel->setAlignment(Qt::AlignCenter);
    m_coverLabel->setPixmap(CoverArtGenerator::generateCover(m_book, 150, 210));
    m_coverLabel->setCursor(Qt::PointingHandCursor);
    mainLayout->addWidget(m_coverLabel);

    // Category Badge & Rating Row
    auto *metaRow = new QHBoxLayout();
    metaRow->setContentsMargins(0, 0, 0, 0);

    m_categoryBadge = new QLabel(m_book.category, this);
    m_categoryBadge->setStyleSheet(R"(
        background-color: #E9F0EA;
        color: #1F4324;
        border: 1px solid #B8D3BC;
        font-size: 11px;
        font-weight: 700;
        border-radius: 4px;
        padding: 2px 6px;
    )");
    metaRow->addWidget(m_categoryBadge);

    metaRow->addStretch();

    m_ratingLabel = new QLabel(QString("★ %1").arg(QString::number(m_book.rating, 'f', 1)), this);
    m_ratingLabel->setStyleSheet(QString(R"(
        color: %1;
        font-size: 12px;
        font-weight: 700;
    )").arg(Theme::ColorGoldAccent));
    metaRow->addWidget(m_ratingLabel);

    mainLayout->addLayout(metaRow);

    // Book Title (Dark Forest Green)
    m_titleLabel = new QLabel(m_book.title, this);
    m_titleLabel->setWordWrap(true);
    m_titleLabel->setMaximumHeight(40);
    m_titleLabel->setStyleSheet(QString(R"(
        font-size: 15px;
        font-weight: 700;
        color: %1;
        line-height: 1.2;
    )").arg(Theme::ColorTextForest));
    mainLayout->addWidget(m_titleLabel);

    // Author (Readable Sage Charcoal)
    m_authorLabel = new QLabel(m_book.author, this);
    m_authorLabel->setStyleSheet(QString(R"(
        font-size: 12px;
        font-weight: 500;
        color: %1;
    )").arg(Theme::ColorTextMuted));
    mainLayout->addWidget(m_authorLabel);

    // Availability Badge
    m_availabilityBadge = new QLabel(this);
    mainLayout->addWidget(m_availabilityBadge);

    // Action Buttons
    auto *btnRow = new QHBoxLayout();
    btnRow->setContentsMargins(0, 4, 0, 0);
    btnRow->setSpacing(6);

    m_issueBtn = new QPushButton(this);
    m_issueBtn->setProperty("primary", true);
    m_issueBtn->setFixedHeight(32);
    m_issueBtn->setCursor(Qt::PointingHandCursor);
    connect(m_issueBtn, &QPushButton::clicked, this, &BookCardWidget::onIssueClicked);
    btnRow->addWidget(m_issueBtn, 1);

    m_detailsBtn = new QPushButton("Info", this);
    m_detailsBtn->setFixedHeight(32);
    m_detailsBtn->setFixedWidth(44);
    m_detailsBtn->setCursor(Qt::PointingHandCursor);
    m_detailsBtn->setToolTip("View Book Details");
    connect(m_detailsBtn, &QPushButton::clicked, this, &BookCardWidget::onDetailsClicked);
    btnRow->addWidget(m_detailsBtn);

    m_bookmarkBtn = new QPushButton(this);
    m_bookmarkBtn->setFixedHeight(32);
    m_bookmarkBtn->setFixedWidth(36);
    m_bookmarkBtn->setCursor(Qt::PointingHandCursor);
    connect(m_bookmarkBtn, &QPushButton::clicked, this, &BookCardWidget::onBookmarkClicked);
    btnRow->addWidget(m_bookmarkBtn);

    mainLayout->addLayout(btnRow);

    // Connect clicking on cover to details
    // We can filter mouse clicks on m_coverLabel
    m_coverLabel->installEventFilter(this);

    updateState(m_isIssued, m_inReadingList, m_book.availableCopies);
}

void BookCardWidget::updateState(bool isIssued, bool inReadingList, int availableCopies) {
    m_isIssued = isIssued;
    m_inReadingList = inReadingList;
    m_book.availableCopies = availableCopies;

    if (m_isIssued) {
        m_issueBtn->setText("Issued ✓");
        m_issueBtn->setEnabled(false);
        m_issueBtn->setStyleSheet(R"(
            background-color: #E2EFE3;
            color: #1B4D22;
            border: 1px solid #B7D9BA;
            font-weight: 700;
        )");
    } else if (m_book.availableCopies <= 0) {
        m_issueBtn->setText("Out of Stock");
        m_issueBtn->setEnabled(false);
        m_issueBtn->setStyleSheet(R"(
            background-color: #FEE2E2;
            color: #991B1B;
            border: 1px solid #FCA5A5;
            font-weight: 700;
        )");
    } else {
        m_issueBtn->setText("Borrow");
        m_issueBtn->setEnabled(true);
        m_issueBtn->setStyleSheet(QString(R"(
            QPushButton {
                background-color: %1;
                color: #FFFFFF;
                border: none;
                border-radius: 6px;
                font-weight: 700;
            }
            QPushButton:hover {
                background-color: %2;
            }
        )").arg(Theme::ColorForestPrimary, Theme::ColorForestHover));
    }

    // Availability Badge Text
    if (m_book.availableCopies > 0) {
        m_availabilityBadge->setText(QString("● %1 of %2 Available").arg(m_book.availableCopies).arg(m_book.totalCopies));
        m_availabilityBadge->setStyleSheet(R"(
            color: #1B4D22;
            font-size: 12px;
            font-weight: 700;
        )");
    } else {
        m_availabilityBadge->setText("● Out of Stock");
        m_availabilityBadge->setStyleSheet(R"(
            color: #991B1B;
            font-size: 11px;
            font-weight: 700;
        )");
    }

    // Bookmark button styling
    if (m_inReadingList) {
        m_bookmarkBtn->setText("★");
        m_bookmarkBtn->setToolTip("Saved in Reading List (Click to Remove)");
        m_bookmarkBtn->setStyleSheet(R"(
            background-color: #FEF3C7;
            color: #92400E;
            border: 1px solid #FCD34D;
            border-radius: 6px;
            font-size: 14px;
            font-weight: bold;
        )");
    } else {
        m_bookmarkBtn->setText("☆");
        m_bookmarkBtn->setToolTip("Save to Reading List");
        m_bookmarkBtn->setStyleSheet(R"(
            background-color: #FFFFFF;
            color: #3D5243;
            border: 1.5px solid #D8D1C3;
            border-radius: 6px;
            font-size: 14px;
        )");
    }
}

void BookCardWidget::onIssueClicked() {
    emit issueRequested(m_book.id);
}

void BookCardWidget::onDetailsClicked() {
    emit detailsRequested(m_book.id);
}

void BookCardWidget::onBookmarkClicked() {
    emit readingListToggled(m_book.id);
}
