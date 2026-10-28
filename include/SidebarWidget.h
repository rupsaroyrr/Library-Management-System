#pragma once

#include "Models.h"
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QScrollArea>

class SidebarWidget : public QWidget {
    Q_OBJECT
public:
    explicit SidebarWidget(QWidget *parent = nullptr);

    void setStudent(const Student &student);
    void updateIssues(const QVector<IssueRecord> &currentlyIssued, const QVector<IssueRecord> &previousIssues);
    void setActiveNavIndex(int index);

signals:
    void navItemClicked(int index);
    void returnBookRequested(int issueId);
    void viewBookDetailsRequested(int bookId);
    void issueBookRequested(int bookId);
    void logoutRequested();

private slots:
    void toggleCurrentlyIssued();
    void togglePreviousIssues();

private:
    void setupUI();
    QPushButton* createNavButton(const QString &icon, const QString &text, int index);
    QWidget* createCurrentlyIssuedItem(const IssueRecord &record);
    QWidget* createPreviousIssueItem(const IssueRecord &record);

    Student m_student;
    int m_currentIndex = 0;

    // Header Logo
    QLabel *m_logoLabel = nullptr;
    QLabel *m_titleLabel = nullptr;

    // Navigation buttons
    QVector<QPushButton*> m_navButtons;

    // Collapsible section 1: Currently Issued
    QPushButton *m_currentlyIssuedHeader = nullptr;
    QWidget *m_currentlyIssuedContainer = nullptr;
    QVBoxLayout *m_currentlyIssuedLayout = nullptr;
    bool m_currentlyIssuedExpanded = true;

    // Collapsible section 2: Previous Issues
    QPushButton *m_previousIssuesHeader = nullptr;
    QWidget *m_previousIssuesContainer = nullptr;
    QVBoxLayout *m_previousIssuesLayout = nullptr;
    bool m_previousIssuesExpanded = false;

    // Bottom Student Profile
    QLabel *m_avatarLabel = nullptr;
    QLabel *m_studentNameLabel = nullptr;
    QLabel *m_studentYearLabel = nullptr;
    QPushButton *m_logoutBtn = nullptr;
};
