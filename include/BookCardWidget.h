#pragma once

#include "Models.h"
#include <QFrame>
#include <QLabel>
#include <QPushButton>

class BookCardWidget : public QFrame {
    Q_OBJECT
public:
    explicit BookCardWidget(const Book &book, bool isIssuedByStudent, bool isInReadingList, QWidget *parent = nullptr);

    int bookId() const { return m_book.id; }
    void updateState(bool isIssued, bool inReadingList, int availableCopies);

signals:
    void issueRequested(int bookId);
    void detailsRequested(int bookId);
    void readingListToggled(int bookId);

private slots:
    void onIssueClicked();
    void onDetailsClicked();
    void onBookmarkClicked();

private:
    void setupUI();

    Book m_book;
    bool m_isIssued = false;
    bool m_inReadingList = false;

    QLabel *m_coverLabel = nullptr;
    QLabel *m_titleLabel = nullptr;
    QLabel *m_authorLabel = nullptr;
    QLabel *m_categoryBadge = nullptr;
    QLabel *m_availabilityBadge = nullptr;
    QLabel *m_ratingLabel = nullptr;
    QPushButton *m_issueBtn = nullptr;
    QPushButton *m_detailsBtn = nullptr;
    QPushButton *m_bookmarkBtn = nullptr;
};
