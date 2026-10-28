#include "LoginDialog.h"
#include "DatabaseManager.h"
#include "CoverArtGenerator.h"
#include "Theme.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QGraphicsDropShadowEffect>

LoginDialog::LoginDialog(QWidget *parent) : QDialog(parent) {
    setupUI();
    loadExistingProfiles();
}

void LoginDialog::setupUI() {
    setWindowTitle("BookNest - Student Access");
    setFixedSize(480, 580);
    setModal(true);

    setStyleSheet(QString(R"(
        QDialog {
            background-color: %1;
        }
        QLabel {
            color: %2;
        }
    )").arg(Theme::ColorCreamBase, Theme::ColorTextDark));

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(32, 32, 32, 32);
    mainLayout->setSpacing(16);

    // Elevated Card Container
    auto *card = new QFrame(this);
    card->setObjectName("loginCard");
    card->setStyleSheet(QString(R"(
        QFrame#loginCard {
            background-color: #FFFFFF;
            border: 1px solid %1;
            border-radius: 16px;
        }
    )").arg(Theme::ColorCreamBorder));

    auto *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(24);
    shadow->setColor(QColor(0, 0, 0, 18));
    shadow->setOffset(0, 6);
    card->setGraphicsEffect(shadow);

    auto *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(28, 28, 28, 28);
    cardLayout->setSpacing(14);

    // Top Brand Logo
    auto *logoLayout = new QVBoxLayout();
    logoLayout->setSpacing(8);
    logoLayout->setAlignment(Qt::AlignCenter);

    auto *logo = new QLabel(card);
    logo->setPixmap(CoverArtGenerator::generateLogo(54));
    logo->setAlignment(Qt::AlignCenter);
    logoLayout->addWidget(logo);

    auto *title = new QLabel("BookNest", card);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet(QString("font-size: 24px; font-weight: 800; color: %1;").arg(Theme::ColorForestPrimary));
    logoLayout->addWidget(title);

    auto *subtitle = new QLabel("Student Portal & Library Management", card);
    subtitle->setAlignment(Qt::AlignCenter);
    subtitle->setStyleSheet(QString("font-size: 14px; font-weight: 600; color: %1;").arg(Theme::ColorTextMuted));
    logoLayout->addWidget(subtitle);

    cardLayout->addLayout(logoLayout);

    // Quick Existing Profiles Switcher
    auto *quickLabel = new QLabel("Existing Profile (Optional Quick Switch):", card);
    quickLabel->setStyleSheet("font-size: 13px; font-weight: 700; color: #3D5243;");
    cardLayout->addWidget(quickLabel);

    m_existingProfilesCombo = new QComboBox(card);
    m_existingProfilesCombo->addItem("-- New Student Registration --");
    connect(m_existingProfilesCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &LoginDialog::onProfileSelected);
    cardLayout->addWidget(m_existingProfilesCombo);

    // Student Name Input
    auto *nameLabel = new QLabel("Student Full Name *", card);
    nameLabel->setStyleSheet("font-size: 13px; font-weight: 700; color: #1F4324;");
    cardLayout->addWidget(nameLabel);

    m_nameEdit = new QLineEdit(card);
    m_nameEdit->setPlaceholderText("e.g. Alex Rivers");
    m_nameEdit->setText("Alex Rivers"); // Friendly default
    cardLayout->addWidget(m_nameEdit);

    // Academic Year
    auto *yearLabel = new QLabel("Academic Year / Class *", card);
    yearLabel->setStyleSheet("font-size: 13px; font-weight: 700; color: #1F4324;");
    cardLayout->addWidget(yearLabel);

    m_yearCombo = new QComboBox(card);
    m_yearCombo->addItems({
        "1st Year",
        "2nd Year",
        "3rd Year",
        "4th Year",
        "Postgraduate"
    });
    m_yearCombo->setCurrentIndex(1); // 2nd Year
    cardLayout->addWidget(m_yearCombo);

    // Department / Course
    auto *courseLabel = new QLabel("Department / Major *", card);
    courseLabel->setStyleSheet("font-size: 13px; font-weight: 700; color: #1F4324;");
    cardLayout->addWidget(courseLabel);

    m_courseCombo = new QComboBox(card);
    m_courseCombo->addItems({
        "Computer Science & Engineering",
        "Information Technology",
        "Electronics & Communication",
        "Mechanical Engineering",
        "Civil Engineering",
        "General Studies"
    });
    m_courseCombo->setCurrentIndex(0);
    cardLayout->addWidget(m_courseCombo);

    // Error Feedback Label
    m_errorLabel = new QLabel(card);
    m_errorLabel->setAlignment(Qt::AlignCenter);
    m_errorLabel->setStyleSheet("color: #991B1B; font-size: 13px; font-weight: 700;");
    m_errorLabel->setVisible(false);
    cardLayout->addWidget(m_errorLabel);

    // Enter Library Button
    m_loginBtn = new QPushButton("Enter Library Dashboard →", card);
    m_loginBtn->setProperty("primary", true);
    m_loginBtn->setFixedHeight(42);
    m_loginBtn->setCursor(Qt::PointingHandCursor);
    m_loginBtn->setStyleSheet(QString(R"(
        QPushButton {
            background-color: %1;
            color: #FFFFFF;
            border: none;
            border-radius: 8px;
            font-size: 14px;
            font-weight: 700;
        }
        QPushButton:hover {
            background-color: %2;
        }
    )").arg(Theme::ColorForestPrimary, Theme::ColorForestHover));
    connect(m_loginBtn, &QPushButton::clicked, this, &LoginDialog::onLoginClicked);
    cardLayout->addWidget(m_loginBtn);

    mainLayout->addWidget(card);

    connect(m_nameEdit, &QLineEdit::returnPressed, this, &LoginDialog::onLoginClicked);
}

void LoginDialog::loadExistingProfiles() {
    m_existingStudents = DatabaseManager::instance().getAllStudents();
    for (const auto &s : m_existingStudents) {
        m_existingProfilesCombo->addItem(QString("%1 (%2 • %3)").arg(s.name, s.academicYear, s.course));
    }
}

void LoginDialog::onProfileSelected(int index) {
    if (index > 0 && index - 1 < m_existingStudents.size()) {
        const auto &s = m_existingStudents[index - 1];
        m_nameEdit->setText(s.name);
        int yearIdx = m_yearCombo->findText(s.academicYear);
        if (yearIdx >= 0) m_yearCombo->setCurrentIndex(yearIdx);
        int courseIdx = m_courseCombo->findText(s.course);
        if (courseIdx >= 0) m_courseCombo->setCurrentIndex(courseIdx);
    }
}

void LoginDialog::onLoginClicked() {
    QString name = m_nameEdit->text().trimmed();
    if (name.isEmpty()) {
        m_errorLabel->setText("Please enter student name to proceed.");
        m_errorLabel->setVisible(true);
        m_nameEdit->setFocus();
        return;
    }

    m_errorLabel->setVisible(false);
    QString year = m_yearCombo->currentText();
    QString course = m_courseCombo->currentText();

    // Fetch or create student in persistent database
    m_student = DatabaseManager::instance().getOrCreateStudent(name, year, course);
    accept();
}
