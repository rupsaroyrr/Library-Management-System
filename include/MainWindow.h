#pragma once

#include "Models.h"
#include <QMainWindow>
#include <QStackedWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QGridLayout>
#include <QTableWidget>
#include <QTimer>

class SidebarWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(const Student &student, QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void onNavIndexChanged(int index);
    void onSearchChanged(const QString &query);
    void onCategoryFilterChanged(int index);
    void onIssueBookRequested(int bookId);
    void onReturnBookRequested(int issueId);
    void onReadingListToggled(int bookId);
    void onBookDetailsRequested(int bookId);
    void onLogout();
    void onSaveSettings();
    void onResetCatalog();

private:
    void setupUI();
    void refreshAll();
    void showToast(const QString &message, bool isError = false);

    // Page Builders & Refreshers
    QWidget* createStudyLibraryPage();
    void refreshStudyLibrary();

    QWidget* createRecommendedPage();
    void refreshRecommended();

    QWidget* createExplorePage();
    void refreshExplore();

    QWidget* createReadingListPage();
    void refreshReadingList();

    QWidget* createReadingHistoryPage();
    void refreshReadingHistory();

    QWidget* createSettingsPage();
    void refreshSettings();

    void populateBookGrid(QGridLayout *grid, const QVector<Book> &books);
    void updateTopBarInfo();

    Student m_student;
    int m_currentPageIndex = 0;

    // Sidebar
    SidebarWidget *m_sidebar = nullptr;

    // Top Bar Controls
    QLabel *m_pageTitleLabel = nullptr;
    QLabel *m_pageSubtitleLabel = nullptr;
    QLineEdit *m_searchEdit = nullptr;
    QComboBox *m_topCategoryCombo = nullptr;
    QLabel *m_studentStatusChip = nullptr;
    QLabel *m_toastLabel = nullptr;
    QTimer *m_toastTimer = nullptr;

    // Stacked Pages
    QStackedWidget *m_stackedWidget = nullptr;

    // Page 0: Study Library
    QWidget *m_studyPage = nullptr;
    QGridLayout *m_studyGrid = nullptr;
    QComboBox *m_studyCategoryCombo = nullptr;

    // Page 1: Recommended
    QWidget *m_recommendedPage = nullptr;
    QGridLayout *m_recommendedGrid = nullptr;
    QLabel *m_prefTagsLabel = nullptr;

    // Page 2: Explore
    QWidget *m_explorePage = nullptr;
    QGridLayout *m_exploreGrid = nullptr;
    QComboBox *m_exploreCategoryCombo = nullptr;

    // Page 3: Reading List
    QWidget *m_readingListPage = nullptr;
    QGridLayout *m_readingListGrid = nullptr;
    QLabel *m_readingListEmptyLabel = nullptr;

    // Page 4: Reading History
    QWidget *m_historyPage = nullptr;
    QTableWidget *m_historyTable = nullptr;
    QLineEdit *m_historySearchEdit = nullptr;

    // Page 5: Settings
    QWidget *m_settingsPage = nullptr;
    QLineEdit *m_settingsNameEdit = nullptr;
    QComboBox *m_settingsYearCombo = nullptr;
    QComboBox *m_settingsCourseCombo = nullptr;
    QLineEdit *m_settingsPrefEdit = nullptr;
    QComboBox *m_settingsThemeCombo = nullptr;
    QLabel *m_settingsSavedMsg = nullptr;
};
