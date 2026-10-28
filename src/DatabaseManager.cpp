#include "DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QStandardPaths>
#include <QDir>
#include <QDate>
#include <QDebug>

DatabaseManager& DatabaseManager::instance() {
    static DatabaseManager instance;
    return instance;
}

DatabaseManager::DatabaseManager() = default;

DatabaseManager::~DatabaseManager() {
    closeDatabase();
}

bool DatabaseManager::initDatabase(const QString &dbPath) {
    QString finalPath = dbPath;
    if (finalPath.isEmpty()) {
        QString appData = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        QDir dir(appData);
        if (!dir.exists()) {
            dir.mkpath(".");
        }
        finalPath = dir.filePath("booknest.db");
    }

    if (QSqlDatabase::contains(m_connectionName)) {
        m_db = QSqlDatabase::database(m_connectionName);
    } else {
        m_db = QSqlDatabase::addDatabase("QSQLITE", m_connectionName);
    }

    m_db.setDatabaseName(finalPath);
    if (!m_db.open()) {
        qWarning() << "Failed to open SQLite database:" << m_db.lastError().text();
        return false;
    }

    // Enable foreign keys
    QSqlQuery pragma(m_db);
    pragma.exec("PRAGMA foreign_keys = ON;");

    if (!createTables()) {
        return false;
    }

    // Seed books if table is empty
    QSqlQuery countQuery("SELECT COUNT(*) FROM books;", m_db);
    if (countQuery.next() && countQuery.value(0).toInt() == 0) {
        seedInitialBooks();
    }

    return true;
}

void DatabaseManager::closeDatabase() {
    if (m_db.isOpen()) {
        m_db.close();
    }
}

bool DatabaseManager::createTables() {
    QSqlQuery query(m_db);

    const QString createStudents = R"(
        CREATE TABLE IF NOT EXISTS students (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL,
            academic_year TEXT NOT NULL,
            course TEXT NOT NULL,
            preferences TEXT DEFAULT '',
            theme TEXT DEFAULT 'Cream & Forest',
            avatar_color TEXT DEFAULT '#234E2A',
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP
        );
    )";

    const QString createBooks = R"(
        CREATE TABLE IF NOT EXISTS books (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            title TEXT NOT NULL,
            author TEXT NOT NULL,
            category TEXT NOT NULL,
            isbn TEXT UNIQUE,
            target_year TEXT DEFAULT 'All',
            course TEXT DEFAULT 'All',
            is_academic INTEGER DEFAULT 1,
            total_copies INTEGER DEFAULT 3,
            available_copies INTEGER DEFAULT 3,
            description TEXT,
            cover_color TEXT DEFAULT '#1B4332',
            cover_accent TEXT DEFAULT '#D4A373',
            rating REAL DEFAULT 4.5,
            published_year INTEGER DEFAULT 2022,
            pages INTEGER DEFAULT 350
        );
    )";

    const QString createIssues = R"(
        CREATE TABLE IF NOT EXISTS issues (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            student_id INTEGER NOT NULL,
            book_id INTEGER NOT NULL,
            issue_date TEXT NOT NULL,
            due_date TEXT NOT NULL,
            return_date TEXT,
            status TEXT DEFAULT 'ISSUED',
            FOREIGN KEY(student_id) REFERENCES students(id) ON DELETE CASCADE,
            FOREIGN KEY(book_id) REFERENCES books(id) ON DELETE CASCADE
        );
    )";

    const QString createReadingList = R"(
        CREATE TABLE IF NOT EXISTS reading_list (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            student_id INTEGER NOT NULL,
            book_id INTEGER NOT NULL,
            added_date TEXT NOT NULL,
            UNIQUE(student_id, book_id),
            FOREIGN KEY(student_id) REFERENCES students(id) ON DELETE CASCADE,
            FOREIGN KEY(book_id) REFERENCES books(id) ON DELETE CASCADE
        );
    )";

    return query.exec(createStudents) &&
           query.exec(createBooks) &&
           query.exec(createIssues) &&
           query.exec(createReadingList);
}

void DatabaseManager::seedInitialBooks() {
    struct SeedBook {
        const char* title;
        const char* author;
        const char* category;
        const char* isbn;
        const char* targetYear;
        const char* course;
        int isAcademic;
        int totalCopies;
        int availableCopies;
        const char* description;
        const char* coverColor;
        const char* coverAccent;
        double rating;
        int publishedYear;
        int pages;
    };

    const SeedBook books[] = {
        // Study Library: Computer Science / Engineering / Mathematics
        {
            "Data Structures & Algorithm Analysis in C++",
            "Mark Allen Weiss",
            "Computer Science",
            "978-0132847377",
            "2nd Year",
            "Computer Science & Engineering",
            1, 4, 3,
            "Comprehensive guide to data structures, algorithm efficiency, Big-O notation, trees, graphs, sorting, and modern C++ implementations.",
            "#1B4332", "#E9C46A", 4.8, 2021, 650
        },
        {
            "Operating System Concepts (10th Edition)",
            "Silberschatz, Galvin & Gagne",
            "Computer Science",
            "978-1118063330",
            "3rd Year",
            "Computer Science & Engineering",
            1, 3, 2,
            "The classic 'dinosaur book' covering process management, virtual memory, concurrency, file systems, distributed OS, and virtualization.",
            "#0D2818", "#D4A373", 4.7, 2020, 976
        },
        {
            "Introduction to Algorithms (CLRS)",
            "Cormen, Leiserson, Rivest & Stein",
            "Computer Science",
            "978-0262046305",
            "2nd Year",
            "Computer Science & Engineering",
            1, 5, 4,
            "The quintessential algorithms reference worldwide covering dynamic programming, greedy algorithms, graph theory, NP-completeness, and multithreading.",
            "#203A43", "#E29578", 4.9, 2022, 1312
        },
        {
            "Database System Concepts",
            "Silberschatz, Korth & Sudarshan",
            "Computer Science",
            "978-0078022159",
            "2nd Year",
            "Computer Science & Engineering",
            1, 3, 3,
            "Fundamental concepts of database management, relational algebra, SQL optimization, transactions, ACID properties, and NoSQL systems.",
            "#243B55", "#E9C46A", 4.6, 2019, 1376
        },
        {
            "Artificial Intelligence: A Modern Approach",
            "Stuart Russell & Peter Norvig",
            "Artificial Intelligence",
            "978-0134610993",
            "4th Year",
            "Computer Science & Engineering",
            1, 2, 1,
            "The leading authority in AI education, covering intelligent agents, informed search, heuristic optimization, neural networks, and reinforcement learning.",
            "#1E3F20", "#D4A373", 4.9, 2021, 1152
        },
        {
            "Computer Networking: A Top-Down Approach",
            "James Kurose & Keith Ross",
            "Computer Science",
            "978-0136681557",
            "3rd Year",
            "Computer Science & Engineering",
            1, 4, 3,
            "Innovative top-down approach focusing on application layer, transport layer protocols TCP/UDP, IP routing, wireless networks, and network security.",
            "#2D5A27", "#E9C46A", 4.7, 2021, 864
        },
        {
            "Discrete Mathematics & Its Applications",
            "Kenneth H. Rosen",
            "Mathematics",
            "978-1259676512",
            "1st Year",
            "All",
            1, 5, 4,
            "Essential foundations for computing: propositional logic, set theory, induction, recurrence relations, combinatorics, and graph algorithms.",
            "#16311A", "#C88A3E", 4.6, 2018, 1024
        },
        {
            "Engineering Mathematics",
            "K.A. Stroud & Dexter J. Booth",
            "Mathematics",
            "978-1352010275",
            "1st Year",
            "All",
            1, 5, 3,
            "Self-paced learning guide with stepped techniques for calculus, differential equations, linear algebra, Laplace transforms, and complex numbers.",
            "#2E4F4F", "#E29578", 4.8, 2020, 1184
        },
        {
            "Digital Design & Computer Architecture",
            "David Harris & Sarah Harris",
            "Electronics",
            "978-0128000564",
            "2nd Year",
            "Computer Science & Engineering",
            1, 3, 2,
            "Takes students from digital logic gates to building a complete MIPS and ARM microprocessor, bridging hardware and assembly programming.",
            "#2C3E50", "#F39C12", 4.7, 2016, 736
        },
        {
            "Compilers: Principles, Techniques & Tools",
            "Aho, Lam, Sethi & Ullman",
            "Computer Science",
            "978-0321486813",
            "3rd Year",
            "Computer Science & Engineering",
            1, 3, 2,
            "The iconic 'Dragon Book' presenting lexical analysis, syntax-directed translation, code optimization, and code generation.",
            "#19381F", "#E9C46A", 4.6, 2006, 1040
        },
        {
            "Clean Code: A Handbook of Agile Craftsmanship",
            "Robert C. Martin",
            "Software Engineering",
            "978-0132350884",
            "3rd Year",
            "Computer Science & Engineering",
            1, 3, 1,
            "Crucial software engineering best practices: meaningful naming, single responsibility functions, error handling, unit testing, and refactoring.",
            "#1E2B20", "#D4A373", 4.8, 2008, 464
        },
        {
            "Signals and Systems (2nd Edition)",
            "Alan V. Oppenheim & Alan S. Willsky",
            "Electronics",
            "978-0138147570",
            "2nd Year",
            "Electronics & Communication",
            1, 3, 3,
            "Comprehensive treatment of continuous-time and discrete-time signals, linear time-invariant systems, Fourier and Laplace transform analysis.",
            "#234E2A", "#E29578", 4.5, 1996, 957
        },

        // Explore Books: Novels, Fiction, History, Science, Philosophy, Biographies
        {
            "To Kill a Mockingbird",
            "Harper Lee",
            "Fiction",
            "978-0061120084",
            "All",
            "All",
            0, 3, 2,
            "Pulitzer Prize-winning masterpiece exploring racial injustice, compassion, courage, and moral growth in the American Deep South.",
            "#3D3A45", "#E0A96D", 4.9, 1960, 336
        },
        {
            "Sapiens: A Brief History of Humankind",
            "Yuval Noah Harari",
            "History",
            "978-0062316097",
            "All",
            "All",
            0, 4, 3,
            "A groundbreaking exploration of how Homo sapiens conquered Earth through cognitive, agricultural, and scientific revolutions.",
            "#4A3E3D", "#D4A373", 4.7, 2014, 498
        },
        {
            "Dune",
            "Frank Herbert",
            "Sci-Fi",
            "978-0441172719",
            "All",
            "All",
            0, 3, 2,
            "The monumental epic set on the desert planet Arrakis, chronicling politics, religion, ecology, and human evolution through Paul Atreides.",
            "#7F4F24", "#E9C46A", 4.8, 1965, 896
        },
        {
            "A Brief History of Time",
            "Stephen Hawking",
            "Science",
            "978-0553380163",
            "All",
            "All",
            0, 2, 2,
            "Cosmological landmark exploring black holes, the Big Bang, general relativity, quantum mechanics, and the elusive Theory of Everything.",
            "#1A2530", "#81B29A", 4.7, 1988, 256
        },
        {
            "The Pragmatic Programmer",
            "Andrew Hunt & David Thomas",
            "Philosophy",
            "978-0135957059",
            "All",
            "All",
            0, 3, 2,
            "Timeless wisdom on software craftsmanship, professional career growth, pragmatic thinking, automation, and fighting software rot.",
            "#264653", "#2A9D8F", 4.9, 2019, 352
        },
        {
            "1984",
            "George Orwell",
            "Fiction",
            "978-0451524935",
            "All",
            "All",
            0, 3, 2,
            "The chilling dystopian classic warning against totalitarianism, government surveillance, censorship, and psychological manipulation.",
            "#2B2D42", "#E76F51", 4.8, 1949, 328
        },
        {
            "Cosmos",
            "Carl Sagan",
            "Science",
            "978-0345539434",
            "All",
            "All",
            0, 3, 3,
            "A poetic journey through fifteen billion years of cosmic evolution, science discovery, and the profound destiny of human civilization.",
            "#0B132B", "#48CAE4", 4.9, 1980, 384
        },
        {
            "Atomic Habits",
            "James Clear",
            "Psychology",
            "978-0735211292",
            "All",
            "All",
            0, 4, 4,
            "A proven framework for improving every day: small 1% adjustments, cues, craving, response, reward, and identity-based habit transformation.",
            "#582F0E", "#E9C46A", 4.9, 2018, 320
        },
        {
            "Steve Jobs: The Exclusive Biography",
            "Walter Isaacson",
            "Biography",
            "978-1451648539",
            "All",
            "All",
            0, 2, 1,
            "Unflinching portrait of the visionary creative entrepreneur whose passion for perfection revolutionized six major global industries.",
            "#333333", "#D4A373", 4.7, 2011, 656
        },
        {
            "The Alchemist",
            "Paulo Coelho",
            "Fiction",
            "978-0062315007",
            "All",
            "All",
            0, 4, 3,
            "The inspiring fable of Santiago, an Andalusian shepherd boy who journeys in search of worldly treasure and learns to listen to his heart.",
            "#6B4D2C", "#F3C68F", 4.7, 1988, 208
        }
    };

    QSqlQuery insert(m_db);
    insert.prepare(R"(
        INSERT INTO books (title, author, category, isbn, target_year, course, is_academic,
                           total_copies, available_copies, description, cover_color,
                           cover_accent, rating, published_year, pages)
        VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);
    )");

    m_db.transaction();
    for (const auto &b : books) {
        insert.addBindValue(b.title);
        insert.addBindValue(b.author);
        insert.addBindValue(b.category);
        insert.addBindValue(b.isbn);
        insert.addBindValue(b.targetYear);
        insert.addBindValue(b.course);
        insert.addBindValue(b.isAcademic);
        insert.addBindValue(b.totalCopies);
        insert.addBindValue(b.availableCopies);
        insert.addBindValue(b.description);
        insert.addBindValue(b.coverColor);
        insert.addBindValue(b.coverAccent);
        insert.addBindValue(b.rating);
        insert.addBindValue(b.publishedYear);
        insert.addBindValue(b.pages);
        insert.exec();
    }
    m_db.commit();
}

Student DatabaseManager::getOrCreateStudent(const QString &name, const QString &academicYear, const QString &course) {
    Student student;
    QSqlQuery query(m_db);
    query.prepare("SELECT id, name, academic_year, course, preferences, theme, avatar_color, created_at FROM students WHERE name = ?;");
    query.addBindValue(name.trimmed());

    if (query.exec() && query.next()) {
        student.id = query.value(0).toInt();
        student.name = query.value(1).toString();
        student.academicYear = query.value(2).toString();
        student.course = query.value(3).toString();
        student.preferences = query.value(4).toString();
        student.theme = query.value(5).toString();
        student.avatarColor = query.value(6).toString();
        student.createdAt = query.value(7).toString();

        // If academic year or course was updated at login
        if (student.academicYear != academicYear || student.course != course) {
            student.academicYear = academicYear;
            student.course = course;
            updateStudent(student);
        }
        return student;
    }

    // Assign pleasing forest/sage color palette for avatar
    QStringList colors = {"#1B4332", "#2D6A4F", "#234E2A", "#386641", "#1E3F20", "#285238"};
    int colorIdx = qAbs(qHash(name)) % colors.size();

    QSqlQuery insert(m_db);
    insert.prepare("INSERT INTO students (name, academic_year, course, preferences, theme, avatar_color) VALUES (?, ?, ?, ?, ?, ?);");
    insert.addBindValue(name.trimmed());
    insert.addBindValue(academicYear);
    insert.addBindValue(course);
    insert.addBindValue("Computer Science, Algorithms, Sci-Fi");
    insert.addBindValue("Cream & Forest");
    insert.addBindValue(colors[colorIdx]);

    if (insert.exec()) {
        student.id = insert.lastInsertId().toInt();
        student.name = name.trimmed();
        student.academicYear = academicYear;
        student.course = course;
        student.preferences = "Computer Science, Algorithms, Sci-Fi";
        student.theme = "Cream & Forest";
        student.avatarColor = colors[colorIdx];

        seedInitialStudentActivity(student.id);
    }

    return student;
}

void DatabaseManager::seedInitialStudentActivity(int studentId) {
    // Seed one active issue and one returned issue for demo realism
    QDate today = QDate::currentDate();

    // 1. Find book 1 (Data Structures)
    Book b1 = getBookById(1);
    if (b1.id > 0 && b1.availableCopies > 0) {
        QSqlQuery issue(m_db);
        issue.prepare(R"(
            INSERT INTO issues (student_id, book_id, issue_date, due_date, status)
            VALUES (?, ?, ?, ?, 'ISSUED');
        )");
        issue.addBindValue(studentId);
        issue.addBindValue(b1.id);
        issue.addBindValue(today.addDays(-4).toString("yyyy-MM-dd"));
        issue.addBindValue(today.addDays(10).toString("yyyy-MM-dd"));
        if (issue.exec()) {
            QSqlQuery update(m_db);
            update.prepare("UPDATE books SET available_copies = available_copies - 1 WHERE id = ?;");
            update.addBindValue(b1.id);
            update.exec();
        }
    }

    // 2. Previous returned issue (Discrete Mathematics, book id 7)
    Book b7 = getBookById(7);
    if (b7.id > 0) {
        QSqlQuery prev(m_db);
        prev.prepare(R"(
            INSERT INTO issues (student_id, book_id, issue_date, due_date, return_date, status)
            VALUES (?, ?, ?, ?, ?, 'RETURNED');
        )");
        prev.addBindValue(studentId);
        prev.addBindValue(b7.id);
        prev.addBindValue(today.addDays(-28).toString("yyyy-MM-dd"));
        prev.addBindValue(today.addDays(-14).toString("yyyy-MM-dd"));
        prev.addBindValue(today.addDays(-15).toString("yyyy-MM-dd"));
        prev.exec();
    }

    // 3. Add one book to Reading List (Dune, book id 15)
    Book b15 = getBookById(15);
    if (b15.id > 0) {
        addToReadingList(studentId, b15.id);
    }
}

Student DatabaseManager::getStudentById(int id) {
    Student s;
    QSqlQuery query(m_db);
    query.prepare("SELECT id, name, academic_year, course, preferences, theme, avatar_color, created_at FROM students WHERE id = ?;");
    query.addBindValue(id);
    if (query.exec() && query.next()) {
        s.id = query.value(0).toInt();
        s.name = query.value(1).toString();
        s.academicYear = query.value(2).toString();
        s.course = query.value(3).toString();
        s.preferences = query.value(4).toString();
        s.theme = query.value(5).toString();
        s.avatarColor = query.value(6).toString();
        s.createdAt = query.value(7).toString();
    }
    return s;
}

bool DatabaseManager::updateStudent(const Student &student) {
    QSqlQuery query(m_db);
    query.prepare(R"(
        UPDATE students
        SET name = ?, academic_year = ?, course = ?, preferences = ?, theme = ?, avatar_color = ?
        WHERE id = ?;
    )");
    query.addBindValue(student.name);
    query.addBindValue(student.academicYear);
    query.addBindValue(student.course);
    query.addBindValue(student.preferences);
    query.addBindValue(student.theme);
    query.addBindValue(student.avatarColor);
    query.addBindValue(student.id);
    return query.exec();
}

QVector<Student> DatabaseManager::getAllStudents() {
    QVector<Student> list;
    QSqlQuery query("SELECT id, name, academic_year, course, preferences, theme, avatar_color, created_at FROM students ORDER BY name ASC;", m_db);
    while (query.next()) {
        Student s;
        s.id = query.value(0).toInt();
        s.name = query.value(1).toString();
        s.academicYear = query.value(2).toString();
        s.course = query.value(3).toString();
        s.preferences = query.value(4).toString();
        s.theme = query.value(5).toString();
        s.avatarColor = query.value(6).toString();
        s.createdAt = query.value(7).toString();
        list.append(s);
    }
    return list;
}

Book DatabaseManager::getBookById(int bookId) {
    Book b;
    QSqlQuery q(m_db);
    q.prepare(R"(
        SELECT id, title, author, category, isbn, target_year, course, is_academic,
               total_copies, available_copies, description, cover_color, cover_accent,
               rating, published_year, pages
        FROM books WHERE id = ?;
    )");
    q.addBindValue(bookId);
    if (q.exec() && q.next()) {
        b.id = q.value(0).toInt();
        b.title = q.value(1).toString();
        b.author = q.value(2).toString();
        b.category = q.value(3).toString();
        b.isbn = q.value(4).toString();
        b.targetYear = q.value(5).toString();
        b.course = q.value(6).toString();
        b.isAcademic = q.value(7).toInt() != 0;
        b.totalCopies = q.value(8).toInt();
        b.availableCopies = q.value(9).toInt();
        b.description = q.value(10).toString();
        b.coverColor = q.value(11).toString();
        b.coverAccent = q.value(12).toString();
        b.rating = q.value(13).toDouble();
        b.publishedYear = q.value(14).toInt();
        b.pages = q.value(15).toInt();
    }
    return b;
}

static Book parseBookRecord(const QSqlQuery &q) {
    Book b;
    b.id = q.value(0).toInt();
    b.title = q.value(1).toString();
    b.author = q.value(2).toString();
    b.category = q.value(3).toString();
    b.isbn = q.value(4).toString();
    b.targetYear = q.value(5).toString();
    b.course = q.value(6).toString();
    b.isAcademic = q.value(7).toInt() != 0;
    b.totalCopies = q.value(8).toInt();
    b.availableCopies = q.value(9).toInt();
    b.description = q.value(10).toString();
    b.coverColor = q.value(11).toString();
    b.coverAccent = q.value(12).toString();
    b.rating = q.value(13).toDouble();
    b.publishedYear = q.value(14).toInt();
    b.pages = q.value(15).toInt();
    return b;
}

QVector<Book> DatabaseManager::getAllBooks(const QString &searchQuery, const QString &category) {
    QVector<Book> list;
    QString sql = "SELECT id, title, author, category, isbn, target_year, course, is_academic, "
                  "total_copies, available_copies, description, cover_color, cover_accent, "
                  "rating, published_year, pages FROM books WHERE 1=1 ";

    if (!category.isEmpty() && category != "All Categories" && category != "All") {
        sql += " AND category = :cat";
    }
    if (!searchQuery.trimmed().isEmpty()) {
        sql += " AND (title LIKE :query OR author LIKE :query OR category LIKE :query OR isbn LIKE :query)";
    }
    sql += " ORDER BY title ASC;";

    QSqlQuery q(m_db);
    q.prepare(sql);
    if (!category.isEmpty() && category != "All Categories" && category != "All") {
        q.bindValue(":cat", category);
    }
    if (!searchQuery.trimmed().isEmpty()) {
        q.bindValue(":query", "%" + searchQuery.trimmed() + "%");
    }

    if (q.exec()) {
        while (q.next()) {
            list.append(parseBookRecord(q));
        }
    }
    return list;
}

QVector<Book> DatabaseManager::getStudyLibraryBooks(const QString &academicYear, const QString &course, const QString &searchQuery, const QString &category) {
    QVector<Book> list;
    // Academic curriculum books: either matching student's year/course, or "All"
    QString sql = "SELECT id, title, author, category, isbn, target_year, course, is_academic, "
                  "total_copies, available_copies, description, cover_color, cover_accent, "
                  "rating, published_year, pages FROM books WHERE is_academic = 1 ";

    if (!academicYear.isEmpty() && academicYear != "All") {
        sql += " AND (target_year = :year OR target_year = 'All') ";
    }
    if (!course.isEmpty() && course != "All" && course != "General Studies") {
        sql += " AND (course = :course OR course = 'All') ";
    }
    if (!category.isEmpty() && category != "All Categories" && category != "All") {
        sql += " AND category = :cat ";
    }
    if (!searchQuery.trimmed().isEmpty()) {
        sql += " AND (title LIKE :query OR author LIKE :query OR category LIKE :query OR isbn LIKE :query) ";
    }
    sql += " ORDER BY target_year ASC, title ASC;";

    QSqlQuery q(m_db);
    q.prepare(sql);
    if (!academicYear.isEmpty() && academicYear != "All") {
        q.bindValue(":year", academicYear);
    }
    if (!course.isEmpty() && course != "All" && course != "General Studies") {
        q.bindValue(":course", course);
    }
    if (!category.isEmpty() && category != "All Categories" && category != "All") {
        q.bindValue(":cat", category);
    }
    if (!searchQuery.trimmed().isEmpty()) {
        q.bindValue(":query", "%" + searchQuery.trimmed() + "%");
    }

    if (q.exec()) {
        while (q.next()) {
            list.append(parseBookRecord(q));
        }
    }

    // If no books match strict year filter, fallback to showing all academic books
    if (list.isEmpty() && searchQuery.trimmed().isEmpty() && (category.isEmpty() || category == "All")) {
        return getAllBooks("", "");
    }

    return list;
}

QVector<Book> DatabaseManager::getRecommendedBooks(int studentId, const QString &academicYear, const QString &preferences, const QString &searchQuery) {
    Q_UNUSED(studentId);
    QVector<Book> list;

    // Build preference keywords
    QStringList tokens;
    for (const QString &token : preferences.split(',', Qt::SkipEmptyParts)) {
        tokens.append(token.trimmed());
    }

    QString sql = "SELECT id, title, author, category, isbn, target_year, course, is_academic, "
                  "total_copies, available_copies, description, cover_color, cover_accent, "
                  "rating, published_year, pages FROM books WHERE 1=1 ";

    if (!searchQuery.trimmed().isEmpty()) {
        sql += " AND (title LIKE :query OR author LIKE :query OR category LIKE :query) ";
    }

    // Sort by rating desc, prioritising student year and preference keywords
    sql += " ORDER BY rating DESC, published_year DESC;";

    QSqlQuery q(m_db);
    q.prepare(sql);
    if (!searchQuery.trimmed().isEmpty()) {
        q.bindValue(":query", "%" + searchQuery.trimmed() + "%");
    }

    if (q.exec()) {
        QVector<Book> highMatch;
        QVector<Book> generalMatch;

        while (q.next()) {
            Book b = parseBookRecord(q);
            bool isPrefMatch = false;
            for (const QString &tok : tokens) {
                if (b.category.contains(tok, Qt::CaseInsensitive) ||
                    b.title.contains(tok, Qt::CaseInsensitive) ||
                    b.description.contains(tok, Qt::CaseInsensitive)) {
                    isPrefMatch = true;
                    break;
                }
            }
            if (b.targetYear == academicYear || isPrefMatch) {
                highMatch.append(b);
            } else {
                generalMatch.append(b);
            }
        }

        list.append(highMatch);
        list.append(generalMatch);
    }

    return list;
}

QVector<Book> DatabaseManager::getExploreBooks(const QString &searchQuery, const QString &category) {
    QVector<Book> list;
    QString sql = "SELECT id, title, author, category, isbn, target_year, course, is_academic, "
                  "total_copies, available_copies, description, cover_color, cover_accent, "
                  "rating, published_year, pages FROM books WHERE is_academic = 0 ";

    if (!category.isEmpty() && category != "All Categories" && category != "All") {
        sql += " AND category = :cat";
    }
    if (!searchQuery.trimmed().isEmpty()) {
        sql += " AND (title LIKE :query OR author LIKE :query OR category LIKE :query OR description LIKE :query)";
    }
    sql += " ORDER BY rating DESC, title ASC;";

    QSqlQuery q(m_db);
    q.prepare(sql);
    if (!category.isEmpty() && category != "All Categories" && category != "All") {
        q.bindValue(":cat", category);
    }
    if (!searchQuery.trimmed().isEmpty()) {
        q.bindValue(":query", "%" + searchQuery.trimmed() + "%");
    }

    if (q.exec()) {
        while (q.next()) {
            list.append(parseBookRecord(q));
        }
    }
    return list;
}

QStringList DatabaseManager::getCategories(bool academicOnly, bool nonAcademicOnly) {
    QStringList cats;
    cats.append("All Categories");

    QString sql = "SELECT DISTINCT category FROM books ";
    if (academicOnly) sql += " WHERE is_academic = 1 ";
    else if (nonAcademicOnly) sql += " WHERE is_academic = 0 ";
    sql += " ORDER BY category ASC;";

    QSqlQuery q(sql, m_db);
    while (q.next()) {
        QString cat = q.value(0).toString().trimmed();
        if (!cat.isEmpty() && !cats.contains(cat)) {
            cats.append(cat);
        }
    }
    return cats;
}

bool DatabaseManager::isBookIssuedByStudent(int studentId, int bookId) {
    QSqlQuery q(m_db);
    q.prepare(R"(
        SELECT COUNT(*) FROM issues
        WHERE student_id = ? AND book_id = ?
          AND (status = 'ISSUED' OR status = 'OVERDUE' OR return_date IS NULL OR return_date = '')
          AND status != 'RETURNED';
    )");
    q.addBindValue(studentId);
    q.addBindValue(bookId);
    if (q.exec() && q.next()) {
        return q.value(0).toInt() > 0;
    }
    return false;
}

bool DatabaseManager::issueBook(int studentId, int bookId, int daysDuration, QString *outError) {
    // 1. Check if already issued by this student
    if (isBookIssuedByStudent(studentId, bookId)) {
        if (outError) *outError = "You have already borrowed this book and have not returned it yet.";
        return false;
    }

    // 2. Check available copies
    Book book = getBookById(bookId);
    if (book.id <= 0) {
        if (outError) *outError = "Book record not found in system.";
        return false;
    }
    if (book.availableCopies <= 0) {
        if (outError) *outError = "Sorry, all copies of this book are currently checked out.";
        return false;
    }

    m_db.transaction();

    // 3. Insert issue record
    QDate today = QDate::currentDate();
    QDate dueDate = today.addDays(daysDuration > 0 ? daysDuration : 14);

    QSqlQuery q(m_db);
    q.prepare(R"(
        INSERT INTO issues (student_id, book_id, issue_date, due_date, status)
        VALUES (?, ?, ?, ?, 'ISSUED');
    )");
    q.addBindValue(studentId);
    q.addBindValue(bookId);
    q.addBindValue(today.toString("yyyy-MM-dd"));
    q.addBindValue(dueDate.toString("yyyy-MM-dd"));

    if (!q.exec()) {
        m_db.rollback();
        if (outError) *outError = "Failed to record book issue in database.";
        return false;
    }

    // 4. Decrement available copies
    QSqlQuery update(m_db);
    update.prepare("UPDATE books SET available_copies = available_copies - 1 WHERE id = ? AND available_copies > 0;");
    update.addBindValue(bookId);
    if (!update.exec()) {
        m_db.rollback();
        if (outError) *outError = "Failed to update available book copies.";
        return false;
    }

    m_db.commit();
    return true;
}

bool DatabaseManager::returnBook(int issueId, QString *outError) {
    // 1. Fetch issue details
    QSqlQuery q(m_db);
    q.prepare("SELECT book_id, status FROM issues WHERE id = ?;");
    q.addBindValue(issueId);
    if (!q.exec() || !q.next()) {
        if (outError) *outError = "Issue record not found.";
        return false;
    }

    int bookId = q.value(0).toInt();
    QString currentStatus = q.value(1).toString();

    if (currentStatus == "RETURNED") {
        if (outError) *outError = "This book has already been returned.";
        return false;
    }

    m_db.transaction();

    // 2. Update issue record
    QDate today = QDate::currentDate();
    QSqlQuery updateIssue(m_db);
    updateIssue.prepare("UPDATE issues SET return_date = ?, status = 'RETURNED' WHERE id = ?;");
    updateIssue.addBindValue(today.toString("yyyy-MM-dd"));
    updateIssue.addBindValue(issueId);
    if (!updateIssue.exec()) {
        m_db.rollback();
        if (outError) *outError = "Failed to record return date.";
        return false;
    }

    // 3. Increment available copies
    QSqlQuery updateBook(m_db);
    updateBook.prepare("UPDATE books SET available_copies = available_copies + 1 WHERE id = ?;");
    updateBook.addBindValue(bookId);
    if (!updateBook.exec()) {
        m_db.rollback();
        if (outError) *outError = "Failed to restore book copies count.";
        return false;
    }

    m_db.commit();
    return true;
}

QVector<IssueRecord> DatabaseManager::getCurrentlyIssuedBooks(int studentId) {
    QVector<IssueRecord> list;
    QSqlQuery q(m_db);
    q.prepare(R"(
        SELECT i.id, i.student_id, i.book_id, b.title, b.author, b.category, b.cover_color,
               i.issue_date, i.due_date, i.return_date, i.status, b.cover_accent, b.available_copies
        FROM issues i
        JOIN books b ON i.book_id = b.id
        WHERE i.student_id = ?
          AND (i.status = 'ISSUED' OR i.status = 'OVERDUE' OR i.return_date IS NULL OR i.return_date = '')
          AND i.status != 'RETURNED'
        ORDER BY i.due_date ASC, i.id ASC;
    )");
    q.addBindValue(studentId);

    if (q.exec()) {
        QDate today = QDate::currentDate();
        while (q.next()) {
            IssueRecord r;
            r.id = q.value(0).toInt();
            r.studentId = q.value(1).toInt();
            r.bookId = q.value(2).toInt();
            r.bookTitle = q.value(3).toString();
            r.bookAuthor = q.value(4).toString();
            r.category = q.value(5).toString();
            r.coverColor = q.value(6).toString();
            r.issueDate = q.value(7).toString();
            r.dueDate = q.value(8).toString();
            r.returnDate = q.value(9).toString();
            r.status = q.value(10).toString();
            r.coverAccent = q.value(11).toString();
            r.availableCopies = q.value(12).toInt();

            QDate due = QDate::fromString(r.dueDate, "yyyy-MM-dd");
            if (due.isValid() && due < today && r.status != "RETURNED") {
                r.status = "OVERDUE";
            }
            list.append(r);
        }
    }
    return list;
}

QVector<IssueRecord> DatabaseManager::getPreviousIssues(int studentId) {
    QVector<IssueRecord> list;
    QSqlQuery q(m_db);
    q.prepare(R"(
        SELECT i.id, i.student_id, i.book_id, b.title, b.author, b.category, b.cover_color,
               i.issue_date, i.due_date, i.return_date, i.status, b.cover_accent, b.available_copies
        FROM issues i
        JOIN books b ON i.book_id = b.id
        WHERE i.student_id = ?
          AND (i.status = 'RETURNED' OR (i.return_date IS NOT NULL AND i.return_date != ''))
        ORDER BY i.return_date DESC, i.id DESC;
    )");
    q.addBindValue(studentId);

    if (q.exec()) {
        while (q.next()) {
            IssueRecord r;
            r.id = q.value(0).toInt();
            r.studentId = q.value(1).toInt();
            r.bookId = q.value(2).toInt();
            r.bookTitle = q.value(3).toString();
            r.bookAuthor = q.value(4).toString();
            r.category = q.value(5).toString();
            r.coverColor = q.value(6).toString();
            r.issueDate = q.value(7).toString();
            r.dueDate = q.value(8).toString();
            r.returnDate = q.value(9).toString();
            r.status = "RETURNED";
            r.coverAccent = q.value(11).toString();
            r.availableCopies = q.value(12).toInt();
            list.append(r);
        }
    }
    return list;
}

QVector<IssueRecord> DatabaseManager::getAllIssueHistory(int studentId) {
    QVector<IssueRecord> list;
    QSqlQuery q(m_db);
    q.prepare(R"(
        SELECT i.id, i.student_id, i.book_id, b.title, b.author, b.category, b.cover_color,
               i.issue_date, i.due_date, i.return_date, i.status, b.cover_accent, b.available_copies
        FROM issues i
        JOIN books b ON i.book_id = b.id
        WHERE i.student_id = ?
        ORDER BY i.issue_date DESC, i.id DESC;
    )");
    q.addBindValue(studentId);

    if (q.exec()) {
        QDate today = QDate::currentDate();
        while (q.next()) {
            IssueRecord r;
            r.id = q.value(0).toInt();
            r.studentId = q.value(1).toInt();
            r.bookId = q.value(2).toInt();
            r.bookTitle = q.value(3).toString();
            r.bookAuthor = q.value(4).toString();
            r.category = q.value(5).toString();
            r.coverColor = q.value(6).toString();
            r.issueDate = q.value(7).toString();
            r.dueDate = q.value(8).toString();
            r.returnDate = q.value(9).toString();
            r.status = q.value(10).toString();
            r.coverAccent = q.value(11).toString();
            r.availableCopies = q.value(12).toInt();

            if (!r.isReturned()) {
                QDate due = QDate::fromString(r.dueDate, "yyyy-MM-dd");
                if (due.isValid() && due < today) {
                    r.status = "OVERDUE";
                }
            }
            list.append(r);
        }
    }
    return list;
}

bool DatabaseManager::addToReadingList(int studentId, int bookId) {
    QSqlQuery q(m_db);
    q.prepare(R"(
        INSERT OR IGNORE INTO reading_list (student_id, book_id, added_date)
        VALUES (?, ?, ?);
    )");
    q.addBindValue(studentId);
    q.addBindValue(bookId);
    q.addBindValue(QDate::currentDate().toString("yyyy-MM-dd"));
    return q.exec();
}

bool DatabaseManager::removeFromReadingList(int studentId, int bookId) {
    QSqlQuery q(m_db);
    q.prepare("DELETE FROM reading_list WHERE student_id = ? AND book_id = ?;");
    q.addBindValue(studentId);
    q.addBindValue(bookId);
    return q.exec();
}

bool DatabaseManager::isBookInReadingList(int studentId, int bookId) {
    QSqlQuery q(m_db);
    q.prepare("SELECT COUNT(*) FROM reading_list WHERE student_id = ? AND book_id = ?;");
    q.addBindValue(studentId);
    q.addBindValue(bookId);
    if (q.exec() && q.next()) {
        return q.value(0).toInt() > 0;
    }
    return false;
}

QVector<Book> DatabaseManager::getReadingListBooks(int studentId, const QString &searchQuery) {
    QVector<Book> list;
    QString sql = R"(
        SELECT b.id, b.title, b.author, b.category, b.isbn, b.target_year, b.course, b.is_academic,
               b.total_copies, b.available_copies, b.description, b.cover_color, b.cover_accent,
               b.rating, b.published_year, b.pages
        FROM reading_list rl
        JOIN books b ON rl.book_id = b.id
        WHERE rl.student_id = :sid
    )";

    if (!searchQuery.trimmed().isEmpty()) {
        sql += " AND (b.title LIKE :query OR b.author LIKE :query OR b.category LIKE :query) ";
    }
    sql += " ORDER BY rl.added_date DESC;";

    QSqlQuery q(m_db);
    q.prepare(sql);
    q.bindValue(":sid", studentId);
    if (!searchQuery.trimmed().isEmpty()) {
        q.bindValue(":query", "%" + searchQuery.trimmed() + "%");
    }

    if (q.exec()) {
        while (q.next()) {
            list.append(parseBookRecord(q));
        }
    }
    return list;
}

void DatabaseManager::getStudentStats(int studentId, int &currentlyIssuedCount, int &totalReturnedCount, int &readingListCount) {
    currentlyIssuedCount = 0;
    totalReturnedCount = 0;
    readingListCount = 0;

    QSqlQuery q1(m_db);
    q1.prepare(R"(
        SELECT COUNT(*) FROM issues
        WHERE student_id = ?
          AND (status = 'ISSUED' OR status = 'OVERDUE' OR return_date IS NULL OR return_date = '')
          AND status != 'RETURNED';
    )");
    q1.addBindValue(studentId);
    if (q1.exec() && q1.next()) currentlyIssuedCount = q1.value(0).toInt();

    QSqlQuery q2(m_db);
    q2.prepare(R"(
        SELECT COUNT(*) FROM issues
        WHERE student_id = ?
          AND (status = 'RETURNED' OR (return_date IS NOT NULL AND return_date != ''));
    )");
    q2.addBindValue(studentId);
    if (q2.exec() && q2.next()) totalReturnedCount = q2.value(0).toInt();

    QSqlQuery q3(m_db);
    q3.prepare("SELECT COUNT(*) FROM reading_list WHERE student_id = ?;");
    q3.addBindValue(studentId);
    if (q3.exec() && q3.next()) readingListCount = q3.value(0).toInt();
}

void DatabaseManager::ensureStudentDemoData(int studentId) {
    if (studentId <= 0) return;

    QDate today = QDate::currentDate();

    // Check existing issues for this student
    QSet<int> activeBookIds;
    QSet<int> returnedBookIds;
    bool hasOverdue = false;
    int futureActiveCount = 0;

    QSqlQuery q(m_db);
    q.prepare(R"(
        SELECT book_id, due_date, return_date, status
        FROM issues
        WHERE student_id = ?;
    )");
    q.addBindValue(studentId);
    if (q.exec()) {
        while (q.next()) {
            int bId = q.value(0).toInt();
            QString dueDateStr = q.value(1).toString();
            QString returnDateStr = q.value(2).toString();
            QString status = q.value(3).toString();

            bool isRet = (status == "RETURNED" || (!returnDateStr.isEmpty() && returnDateStr != "NULL"));
            if (isRet) {
                returnedBookIds.insert(bId);
            } else {
                activeBookIds.insert(bId);
                QDate due = QDate::fromString(dueDateStr, "yyyy-MM-dd");
                if (due.isValid() && due < today) {
                    hasOverdue = true;
                } else {
                    futureActiveCount++;
                }
            }
        }
    }

    m_db.transaction();

    // 1. Ensure at least two active issues with future due dates
    const int futureCandidates[] = {2, 4, 6, 3};
    int futureOffsets[] = {10, 14, 8, 12};
    int futureIssueDaysAgo[] = {4, 2, 6, 3};

    int cIdx = 0;
    while (futureActiveCount < 2 && cIdx < 4) {
        int bId = futureCandidates[cIdx];
        if (!activeBookIds.contains(bId)) {
            Book book = getBookById(bId);
            if (book.id > 0 && book.availableCopies > 0) {
                QSqlQuery ins(m_db);
                ins.prepare(R"(
                    INSERT INTO issues (student_id, book_id, issue_date, due_date, return_date, status)
                    VALUES (?, ?, ?, ?, NULL, 'ISSUED');
                )");
                ins.addBindValue(studentId);
                ins.addBindValue(bId);
                ins.addBindValue(today.addDays(-futureIssueDaysAgo[cIdx]).toString("yyyy-MM-dd"));
                ins.addBindValue(today.addDays(futureOffsets[cIdx]).toString("yyyy-MM-dd"));
                if (ins.exec()) {
                    QSqlQuery dec(m_db);
                    dec.prepare("UPDATE books SET available_copies = available_copies - 1 WHERE id = ? AND available_copies > 0;");
                    dec.addBindValue(bId);
                    dec.exec();

                    activeBookIds.insert(bId);
                    futureActiveCount++;
                }
            }
        }
        cIdx++;
    }

    // 2. Ensure one overdue book
    if (!hasOverdue) {
        const int overdueCandidates[] = {11, 1, 9};
        for (int bId : overdueCandidates) {
            if (!activeBookIds.contains(bId)) {
                Book book = getBookById(bId);
                if (book.id > 0 && book.availableCopies > 0) {
                    QSqlQuery ins(m_db);
                    ins.prepare(R"(
                        INSERT INTO issues (student_id, book_id, issue_date, due_date, return_date, status)
                        VALUES (?, ?, ?, ?, NULL, 'OVERDUE');
                    )");
                    ins.addBindValue(studentId);
                    ins.addBindValue(bId);
                    ins.addBindValue(today.addDays(-20).toString("yyyy-MM-dd"));
                    ins.addBindValue(today.addDays(-6).toString("yyyy-MM-dd"));
                    if (ins.exec()) {
                        QSqlQuery dec(m_db);
                        dec.prepare("UPDATE books SET available_copies = available_copies - 1 WHERE id = ? AND available_copies > 0;");
                        dec.addBindValue(bId);
                        dec.exec();

                        activeBookIds.insert(bId);
                        hasOverdue = true;
                        break;
                    }
                }
            }
        }
    }

    // 3. Ensure three previously issued and returned books
    const int retCandidates[] = {7, 8, 14, 15, 13};
    int retIssueOffsets[] = {45, 60, 75, 90, 105};
    int retDueOffsets[]   = {31, 46, 61, 76, 91};
    int retDateOffsets[]  = {33, 48, 63, 78, 93};

    int rIdx = 0;
    while (returnedBookIds.size() < 3 && rIdx < 5) {
        int bId = retCandidates[rIdx];
        if (!returnedBookIds.contains(bId) && !activeBookIds.contains(bId)) {
            Book book = getBookById(bId);
            if (book.id > 0) {
                QSqlQuery ins(m_db);
                ins.prepare(R"(
                    INSERT INTO issues (student_id, book_id, issue_date, due_date, return_date, status)
                    VALUES (?, ?, ?, ?, ?, 'RETURNED');
                )");
                ins.addBindValue(studentId);
                ins.addBindValue(bId);
                ins.addBindValue(today.addDays(-retIssueOffsets[rIdx]).toString("yyyy-MM-dd"));
                ins.addBindValue(today.addDays(-retDueOffsets[rIdx]).toString("yyyy-MM-dd"));
                ins.addBindValue(today.addDays(-retDateOffsets[rIdx]).toString("yyyy-MM-dd"));
                if (ins.exec()) {
                    returnedBookIds.insert(bId);
                }
            }
        }
        rIdx++;
    }

    m_db.commit();
}

void DatabaseManager::resetToSampleData() {
    QSqlQuery q(m_db);
    q.exec("DELETE FROM reading_list;");
    q.exec("DELETE FROM issues;");
    q.exec("DELETE FROM books;");
    seedInitialBooks();
}
