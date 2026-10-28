#include "BookDetailDialog.h"
#include "CoverArtGenerator.h"
#include "Theme.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QGraphicsDropShadowEffect>

BookDetailDialog::BookDetailDialog(const Book &book, bool isIssued, bool inReadingList, QWidget *parent)
    : QDialog(parent), m_book(book), m_isIssued(isIssued), m_inReadingList(inReadingList)
{
    setupUI();
}

void BookDetailDialog::setupUI() {
    setWindowTitle(QString("%1 - BookNest Details").arg(m_book.title));
    setModal(true);
    setFixedSize(680, 500);

    setStyleSheet(QString(R"(
        QDialog {
            background-color: %1;
        }
        QLabel {
            color: %2;
        }
    )").arg(Theme::ColorCreamBase, Theme::ColorTextDark));

    auto *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(24, 24, 24, 24);
    mainLayout->setSpacing(24);

    // Left Column: Cover Image
    auto *leftCol = new QVBoxLayout();
    leftCol->setSpacing(12);

    auto *coverLabel = new QLabel(this);
    coverLabel->setPixmap(CoverArtGenerator::generateCover(m_book, 190, 275));
    coverLabel->setAlignment(Qt::AlignCenter);

    auto *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(20);
    shadow->setColor(QColor(0, 0, 0, 30));
    shadow->setOffset(0, 5);
    coverLabel->setGraphicsEffect(shadow);

    leftCol->addWidget(coverLabel);

    // Small status label under cover
    m_statusLabel = new QLabel(this);
    m_statusLabel->setAlignment(Qt::AlignCenter);
    if (m_isIssued) {
        m_statusLabel->setText("Currently in your possession");
        m_statusLabel->setStyleSheet("color: #1F4324; font-weight: 600; font-size: 11px;");
    } else if (m_book.availableCopies <= 0) {
        m_statusLabel->setText("All copies checked out");
        m_statusLabel->setStyleSheet("color: #C2410C; font-weight: 600; font-size: 11px;");
    } else {
        m_statusLabel->setText("Ready for borrowing");
        m_statusLabel->setStyleSheet("color: #2E6B38; font-weight: 600; font-size: 11px;");
    }
    leftCol->addWidget(m_statusLabel);

    leftCol->addStretch();
    mainLayout->addLayout(leftCol, 0);

    // Right Column: Book Details & Synopsis
    auto *rightCol = new QVBoxLayout();
    rightCol->setSpacing(12);

    // Meta row (Category badge + Rating)
    auto *metaRow = new QHBoxLayout();
    auto *catBadge = new QLabel(m_book.category.toUpper(), this);
    catBadge->setStyleSheet(R"(
        background-color: #E9F0EA;
        color: #1F4324;
        border: 1px solid #B8D3BC;
        font-size: 11px;
        font-weight: 700;
        border-radius: 6px;
        padding: 4px 10px;
    )");
    metaRow->addWidget(catBadge);

    if (m_book.isAcademic) {
        auto *academicBadge = new QLabel(QString("Academic: %1").arg(m_book.targetYear), this);
        academicBadge->setStyleSheet(R"(
            background-color: #FEF3C7;
            color: #92400E;
            border: 1px solid #FCD34D;
            font-size: 11px;
            font-weight: 700;
            border-radius: 6px;
            padding: 4px 10px;
        )");
        metaRow->addWidget(academicBadge);
    }

    metaRow->addStretch();

    auto *rating = new QLabel(QString("★ %1 / 5.0").arg(QString::number(m_book.rating, 'f', 1)), this);
    rating->setStyleSheet(QString("color: %1; font-weight: 700; font-size: 13px;").arg(Theme::ColorGoldAccent));
    metaRow->addWidget(rating);

    rightCol->addLayout(metaRow);

    // Title (Dark Forest Green)
    auto *title = new QLabel(m_book.title, this);
    title->setWordWrap(true);
    title->setStyleSheet(QString("font-size: 18px; font-weight: 800; color: %1;").arg(Theme::ColorTextForest));
    rightCol->addWidget(title);

    // Author (Readable Sage Charcoal)
    auto *author = new QLabel(QString("by %1").arg(m_book.author), this);
    author->setStyleSheet(QString("font-size: 13px; color: %1; font-style: italic; font-weight: 500;").arg(Theme::ColorTextMuted));
    rightCol->addWidget(author);

    // Metadata Grid
    auto *grid = new QGridLayout();
    grid->setContentsMargins(0, 4, 0, 4);
    grid->setHorizontalSpacing(16);
    grid->setVerticalSpacing(4);

    auto addGridRow = [grid, this](int row, const QString &label, const QString &val) {
        auto *l = new QLabel(label, this);
        l->setStyleSheet("color: #3D5243; font-size: 11px; font-weight: 700;");
        auto *v = new QLabel(val, this);
        v->setStyleSheet("color: #263229; font-size: 12px; font-weight: 600;");
        grid->addWidget(l, row, 0);
        grid->addWidget(v, row, 1);
    };

    addGridRow(0, "ISBN:", m_book.isbn.isEmpty() ? "N/A" : m_book.isbn);
    addGridRow(1, "Pages:", QString("%1 pages").arg(m_book.pages));
    addGridRow(2, "Published:", QString::number(m_book.publishedYear));
    addGridRow(3, "Course:", m_book.course);

    rightCol->addLayout(grid);

    // Divider
    auto *line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setStyleSheet(QString("color: %1;").arg(Theme::ColorCreamBorder));
    rightCol->addWidget(line);

    // Synopsis header & Description
    auto *synopsisHeader = new QLabel("Book Overview & Curriculum Context", this);
    synopsisHeader->setStyleSheet("font-size: 13px; font-weight: 700; color: #1F4324;");
    rightCol->addWidget(synopsisHeader);

    auto *descLabel = new QLabel(m_book.description, this);
    descLabel->setWordWrap(true);
    descLabel->setStyleSheet("font-size: 13px; color: #263229; line-height: 1.4; font-weight: 400;");
    rightCol->addWidget(descLabel);

    // Copies Progress
    auto *availRow = new QHBoxLayout();
    auto *availText = new QLabel(QString("Library Inventory: <b>%1 of %2 Available</b>")
                                 .arg(m_book.availableCopies).arg(m_book.totalCopies), this);
    availText->setStyleSheet("font-size: 12px; color: #263229;");
    availRow->addWidget(availText);

    auto *prog = new QProgressBar(this);
    prog->setRange(0, m_book.totalCopies);
    prog->setValue(m_book.availableCopies);
    prog->setFixedHeight(8);
    prog->setTextVisible(false);
    prog->setStyleSheet(QString(R"(
        QProgressBar {
            background-color: #D8D1C3;
            border-radius: 4px;
        }
        QProgressBar::chunk {
            background-color: %1;
            border-radius: 4px;
        }
    )").arg(m_book.availableCopies > 0 ? Theme::ColorForestPrimary : "#DC2626"));
    availRow->addWidget(prog);
    rightCol->addLayout(availRow);

    rightCol->addStretch();

    // Bottom Action Buttons
    auto *btnRow = new QHBoxLayout();
    btnRow->setSpacing(10);

    m_issueBtn = new QPushButton(this);
    m_issueBtn->setProperty("primary", true);
    m_issueBtn->setFixedHeight(36);
    m_issueBtn->setCursor(Qt::PointingHandCursor);

    if (m_isIssued) {
        m_issueBtn->setText("Already Borrowed ✓");
        m_issueBtn->setEnabled(false);
    } else if (m_book.availableCopies <= 0) {
        m_issueBtn->setText("Unavailable");
        m_issueBtn->setEnabled(false);
    } else {
        m_issueBtn->setText("Borrow Book (14 Days)");
    }
    connect(m_issueBtn, &QPushButton::clicked, this, &BookDetailDialog::onIssueClicked);
    btnRow->addWidget(m_issueBtn);

    m_readingListBtn = new QPushButton(this);
    m_readingListBtn->setProperty("outline", true);
    m_readingListBtn->setFixedHeight(36);
    m_readingListBtn->setCursor(Qt::PointingHandCursor);
    m_readingListBtn->setText(m_inReadingList ? "★ In Reading List" : "☆ Save to Reading List");
    connect(m_readingListBtn, &QPushButton::clicked, this, &BookDetailDialog::onReadingListClicked);
    btnRow->addWidget(m_readingListBtn);

    auto *closeBtn = new QPushButton("Close", this);
    closeBtn->setFixedHeight(36);
    closeBtn->setCursor(Qt::PointingHandCursor);
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
    btnRow->addWidget(closeBtn);

    rightCol->addLayout(btnRow);
    mainLayout->addLayout(rightCol, 1);
}

void BookDetailDialog::onIssueClicked() {
    emit issueRequested(m_book.id);
    accept();
}

void BookDetailDialog::onReadingListClicked() {
    emit readingListToggled(m_book.id);
    m_inReadingList = !m_inReadingList;
    m_readingListBtn->setText(m_inReadingList ? "★ In Reading List" : "☆ Save to Reading List");
}
