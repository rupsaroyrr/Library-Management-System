#include <QApplication>
#include <QIcon>
#include <QDir>
#include <QStandardPaths>
#include "DatabaseManager.h"
#include "LoginDialog.h"
#include "MainWindow.h"
#include "CoverArtGenerator.h"
#include "Theme.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setFont(QFont("Segoe UI", 11));

    // Apply global high-contrast cream & forest green theme
    app.setStyleSheet(Theme::getApplicationStyle());

    app.setApplicationName("BookNest");
    app.setApplicationDisplayName("BookNest - Campus Library Management System");
    app.setOrganizationName("BookNest");

    // Set Window Icon procedurally generated
    QPixmap iconPixmap = CoverArtGenerator::generateLogo(64);
    app.setWindowIcon(QIcon(iconPixmap));

    // Initialize Database
    QString appDataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(appDataDir);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    QString dbPath = dir.filePath("booknest.db");

    if (!DatabaseManager::instance().initDatabase(dbPath)) {
        qWarning("Failed to initialize BookNest SQLite database.");
    }

    // Launch Student Login Dialog
    LoginDialog loginDialog;
    if (loginDialog.exec() != QDialog::Accepted) {
        return 0;
    }

    // Launch Main Dashboard with logged in student profile
    MainWindow mainWindow(loginDialog.getStudent());
    mainWindow.show();

    return app.exec();
}
