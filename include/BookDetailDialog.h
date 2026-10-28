#pragma once

#include "Models.h"
#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>

class BookDetailDialog : public QDialog {
    Q_OBJECT
public:
    explicit BookDetailDialog(const Book &book, bool isIssued, bool inReadingList, QWidget *parent = nullptr);

signals:
    void issueRequested(int bookId);
    void readingListToggled(int bookId);

private slots:
    void onIssueClicked();
    void onReadingListClicked();

private:
    void setupUI();

    Book m_book;
    bool m_isIssued;
    bool m_inReadingList;

    QPushButton *m_issueBtn = nullptr;
    QPushButton *m_readingListBtn = nullptr;
    QLabel *m_statusLabel = nullptr;
};
