#pragma once

#include "Models.h"
#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>

class LoginDialog : public QDialog {
    Q_OBJECT
public:
    explicit LoginDialog(QWidget *parent = nullptr);

    Student getStudent() const { return m_student; }

private slots:
    void onLoginClicked();
    void onProfileSelected(int index);

private:
    void setupUI();
    void loadExistingProfiles();

    QLineEdit *m_nameEdit = nullptr;
    QComboBox *m_yearCombo = nullptr;
    QComboBox *m_courseCombo = nullptr;
    QComboBox *m_existingProfilesCombo = nullptr;
    QPushButton *m_loginBtn = nullptr;
    QLabel *m_errorLabel = nullptr;

    Student m_student;
    QVector<Student> m_existingStudents;
};
