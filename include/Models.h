#pragma once

#include <QString>
#include <QDateTime>
#include <QDate>

struct Student {
    int id = 0;
    QString name;
    QString academicYear;
    QString course;
    QString preferences;
    QString theme = "Cream & Forest";
    QString avatarColor = "#234E2A";
    QString createdAt;
};

struct Book {
    int id = 0;
    QString title;
    QString author;
    QString category;
    QString isbn;
    QString targetYear;      // "1st Year", "2nd Year", "3rd Year", "4th Year", "All"
    QString course;          // "Computer Science & Engineering", "All", etc.
    bool isAcademic = true;  // true -> Study Library, false -> Explore Books
    int totalCopies = 3;
    int availableCopies = 3;
    QString description;
    QString coverColor = "#1B4332";   // Primary spine/cover color
    QString coverAccent = "#D4A373";  // Foil/accent color
    double rating = 4.5;
    int publishedYear = 2022;
    int pages = 350;

    bool isAvailable() const {
        return availableCopies > 0;
    }
};

struct IssueRecord {
    int id = 0;
    int studentId = 0;
    int bookId = 0;
    QString bookTitle;
    QString bookAuthor;
    QString category;
    QString coverColor = "#1B4332";
    QString coverAccent = "#D4A373";
    int availableCopies = 1;
    QString issueDate;       // YYYY-MM-DD
    QString dueDate;         // YYYY-MM-DD
    QString returnDate;      // YYYY-MM-DD (empty if still issued)
    QString status = "ISSUED"; // "ISSUED", "RETURNED", "OVERDUE"

    bool isReturned() const {
        return status == "RETURNED" || !returnDate.isEmpty();
    }

    int daysRemaining() const {
        if (isReturned()) return 0;
        QDate due = QDate::fromString(dueDate, "yyyy-MM-dd");
        if (!due.isValid()) return 0;
        return QDate::currentDate().daysTo(due);
    }

    bool isOverdue() const {
        if (isReturned()) return false;
        return daysRemaining() < 0;
    }

    bool isDueSoon() const {
        if (isReturned() || isOverdue()) return false;
        return daysRemaining() <= 3;
    }

    QString statusBadgeText() const {
        if (isReturned()) {
            return "Returned";
        }
        if (isOverdue()) {
            int days = qAbs(daysRemaining());
            return QString("Overdue (%1 %2)").arg(days).arg(days == 1 ? "day" : "days");
        }
        if (daysRemaining() == 0) {
            return "Due today";
        }
        return QString("Due in %1 %2").arg(daysRemaining()).arg(daysRemaining() == 1 ? "day" : "days");
    }

    QString statusColorType() const {
        if (isReturned()) return "green";
        if (isOverdue()) return "red";
        if (isDueSoon()) return "amber";
        return "green";
    }
};

struct ReadingListItem {
    int id = 0;
    int studentId = 0;
    int bookId = 0;
    QString addedDate;
    Book book;
};
