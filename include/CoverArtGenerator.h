#pragma once

#include "Models.h"
#include <QPixmap>
#include <QCache>
#include <QPainter>

class CoverArtGenerator {
public:
    static QPixmap generateCover(const Book &book, int width = 160, int height = 230);
    static QPixmap generateThumbnail(const Book &book, int width = 38, int height = 52);
    static QPixmap generateLogo(int size = 44);
    static QPixmap generateStudentAvatar(const QString &name, const QString &colorHex, int size = 40);

private:
    static void drawSpineEffect(QPainter &p, int w, int h);
    static void drawRibbon(QPainter &p, int w, int h, const QString &accentColor);
    static void drawCategoryBadge(QPainter &p, int w, int h, const QString &category);
    static void drawOrnament(QPainter &p, int w, int h, const QString &accentHex, bool isAcademic);
};
