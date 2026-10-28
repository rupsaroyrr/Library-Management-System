#pragma once

#include "Models.h"
#include <QString>
#include <QVector>
#include <QSqlDatabase>
#include <memory>

class DatabaseManager {
public:
    static DatabaseManager& instance();

    bool initDatabase(const QString &dbPath = "");
    void closeDatabase();

    // Student operations
    Student getOrCreateStudent(const QString &name, const QString &academicYear, const QString &course);
    Student getStudentById(int id);
    bool updateStudent(const Student &student);
    QVector<Student> getAllStudents();

    // Book queries
    Book getBookById(int bookId);
    QVector<Book> getAllBooks(const QString &searchQuery = "", const QString &category = "");
    QVector<Book> getStudyLibraryBooks(const QString &academicYear, const QString &course, const QString &searchQuery = "", const QString &category = "");
    QVector<Book> getRecommendedBooks(int studentId, const QString &academicYear, const QString &preferences, const QString &searchQuery = "");
    QVector<Book> getExploreBooks(const QString &searchQuery = "", const QString &category = "");
    QStringList getCategories(bool academicOnly = false, bool nonAcademicOnly = false);

    // Issue & Return operations
    bool issueBook(int studentId, int bookId, int daysDuration, QString *outError = nullptr);
    bool returnBook(int issueId, QString *outError = nullptr);
    bool isBookIssuedByStudent(int studentId, int bookId);
    QVector<IssueRecord> getCurrentlyIssuedBooks(int studentId);
    QVector<IssueRecord> getPreviousIssues(int studentId);
    QVector<IssueRecord> getAllIssueHistory(int studentId);

    // Reading List / Saved Books
    bool addToReadingList(int studentId, int bookId);
    bool removeFromReadingList(int studentId, int bookId);
    bool isBookInReadingList(int studentId, int bookId);
    QVector<Book> getReadingListBooks(int studentId, const QString &searchQuery = "");

    // Quick Summary Statistics
    void getStudentStats(int studentId, int &currentlyIssuedCount, int &totalReturnedCount, int &readingListCount);

    // Demonstration Data
    void ensureStudentDemoData(int studentId);

    // Maintenance / Demo reset
    void resetToSampleData();

private:
    DatabaseManager();
    ~DatabaseManager();
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    bool createTables();
    void seedInitialBooks();
    void seedInitialStudentActivity(int studentId);

    QSqlDatabase m_db;
    QString m_connectionName = "BookNestDB";
};
