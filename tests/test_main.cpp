#include <cassert>
#include <iostream>
#include <QCoreApplication>
#include <QFile>
#include "DatabaseManager.h"

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    std::cout << "[Test] Initializing BookNest SQLite Database test..." << std::endl;
    QString testDb = "test_booknest.db";
    if (QFile::exists(testDb)) {
        QFile::remove(testDb);
    }

    bool initOk = DatabaseManager::instance().initDatabase(testDb);
    assert(initOk && "Database init should succeed");
    std::cout << "[Test] Database initialized successfully." << std::endl;

    // 1. Verify books seeded
    auto allBooks = DatabaseManager::instance().getAllBooks();
    std::cout << "[Test] Total seeded books: " << allBooks.size() << std::endl;
    assert(allBooks.size() >= 20 && "At least 20 sample books should be seeded");

    // 2. Student creation / login
    Student student = DatabaseManager::instance().getOrCreateStudent("Alex Rivers", "2nd Year", "Computer Science & Engineering");
    assert(student.id > 0 && "Student should have valid positive ID");
    assert(student.name == "Alex Rivers");
    std::cout << "[Test] Student created: " << student.name.toStdString() << " (ID: " << student.id << ")" << std::endl;

    // 3. Study library filtering
    auto studyBooks = DatabaseManager::instance().getStudyLibraryBooks("2nd Year", "Computer Science & Engineering");
    std::cout << "[Test] Study library books for 2nd Year CS: " << studyBooks.size() << std::endl;
    assert(!studyBooks.isEmpty() && "Study library books should not be empty");

    // 4. Explore books
    auto exploreBooks = DatabaseManager::instance().getExploreBooks();
    std::cout << "[Test] Explore non-curriculum books: " << exploreBooks.size() << std::endl;
    assert(!exploreBooks.isEmpty() && "Explore books should not be empty");

    // 5. Test Issue Book
    Book targetBook = allBooks[0];
    int initialAvail = targetBook.availableCopies;
    std::cout << "[Test] Issuing book: " << targetBook.title.toStdString() << " (Available: " << initialAvail << ")" << std::endl;

    QString error;
    bool issueOk = DatabaseManager::instance().issueBook(student.id, targetBook.id, 14, &error);
    assert(issueOk && "Issue should succeed");

    Book afterIssue = DatabaseManager::instance().getBookById(targetBook.id);
    assert(afterIssue.availableCopies == initialAvail - 1 && "Available copies should decrement by 1");

    // 6. Test duplicate active issue prevention
    bool dupIssue = DatabaseManager::instance().issueBook(student.id, targetBook.id, 14, &error);
    assert(!dupIssue && "Duplicate active issue of the same book must be rejected");
    std::cout << "[Test] Duplicate issue prevented: " << error.toStdString() << std::endl;

    // 7. Verify currently issued list
    auto issuedList = DatabaseManager::instance().getCurrentlyIssuedBooks(student.id);
    assert(!issuedList.isEmpty() && "Currently issued list should contain the borrowed book");
    std::cout << "[Test] Currently issued count: " << issuedList.size() << std::endl;

    // 8. Test Return Book
    int issueRecordId = issuedList[0].id;
    bool returnOk = DatabaseManager::instance().returnBook(issueRecordId, &error);
    assert(returnOk && "Return should succeed");

    Book afterReturn = DatabaseManager::instance().getBookById(targetBook.id);
    assert(afterReturn.availableCopies == initialAvail && "Available copies should increment back to original");
    std::cout << "[Test] Book successfully returned. Restored copies: " << afterReturn.availableCopies << std::endl;

    // 9. Test Reading List
    bool addListOk = DatabaseManager::instance().addToReadingList(student.id, targetBook.id);
    assert(addListOk && "Add to reading list should succeed");
    assert(DatabaseManager::instance().isBookInReadingList(student.id, targetBook.id) && "Book should be in reading list");

    bool removeListOk = DatabaseManager::instance().removeFromReadingList(student.id, targetBook.id);
    assert(removeListOk && "Remove from reading list should succeed");
    assert(!DatabaseManager::instance().isBookInReadingList(student.id, targetBook.id) && "Book should no longer be in reading list");
    std::cout << "[Test] Reading list toggle verified." << std::endl;

    // Cleanup
    DatabaseManager::instance().closeDatabase();
    QFile::remove(testDb);

    std::cout << "\n>>> ALL 9 TESTS PASSED SUCCESSFULLY! <<<" << std::endl;
    return 0;
}
