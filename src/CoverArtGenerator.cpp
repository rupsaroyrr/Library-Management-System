#include "CoverArtGenerator.h"
#include <QPainterPath>
#include <QLinearGradient>
#include <QFont>
#include <QFontMetrics>

QPixmap CoverArtGenerator::generateCover(const Book &book, int width, int height) {
    QPixmap pixmap(width, height);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);

    // Book base with rounded corners
    QRectF bookRect(0, 0, width, height);
    QPainterPath path;
    path.addRoundedRect(bookRect, 8, 8);
    painter.setClipPath(path);

    // Background Gradient (Cover base color to deeper tone)
    QColor baseColor(book.coverColor.isEmpty() ? "#1B4332" : book.coverColor);
    QColor darkColor = baseColor.darker(140);
    QLinearGradient bgGrad(0, 0, width, height);
    bgGrad.setColorAt(0.0, baseColor);
    bgGrad.setColorAt(0.7, baseColor.darker(115));
    bgGrad.setColorAt(1.0, darkColor);
    painter.fillRect(bookRect, bgGrad);

    // Subtle leather/linen pattern effect with soft diagonal waves
    QColor subtleLines(255, 255, 255, 8);
    painter.setPen(QPen(subtleLines, 1));
    for (int y = -height; y < height * 2; y += 12) {
        painter.drawLine(0, y, width, y + width);
    }

    // Inset Decorative Foil Border
    QColor accentColor(book.coverAccent.isEmpty() ? "#D4A373" : book.coverAccent);
    QRectF innerBorder = bookRect.adjusted(12, 12, -12, -12);
    painter.setPen(QPen(accentColor, 1.2, Qt::SolidLine));
    painter.setBrush(Qt::NoBrush);
    painter.drawRoundedRect(innerBorder, 4, 4);

    // Subtle corner accents
    int cSize = 6;
    painter.fillRect(QRectF(innerBorder.left() - 1, innerBorder.top() - 1, cSize, cSize), accentColor);
    painter.fillRect(QRectF(innerBorder.right() - cSize + 1, innerBorder.top() - 1, cSize, cSize), accentColor);
    painter.fillRect(QRectF(innerBorder.left() - 1, innerBorder.bottom() - cSize + 1, cSize, cSize), accentColor);
    painter.fillRect(QRectF(innerBorder.right() - cSize + 1, innerBorder.bottom() - cSize + 1, cSize, cSize), accentColor);

    // Category Pill / Tag at top
    drawCategoryBadge(painter, width, height, book.category);

    // Central Academic / Genre Ornament
    drawOrnament(painter, width, height, accentColor.name(), book.isAcademic);

    // Title Typography (Centered, wrapped, proportional font)
    int titleFontSize = qMax(10, width / 14);
    QFont titleFont("Georgia", titleFontSize, QFont::Bold);
    painter.setFont(titleFont);
    painter.setPen(QColor("#FFFFFF"));

    int textMargin = 18;
    int titleTop = height * 0.32;
    int titleHeight = height * 0.36;
    QRect titleRect(textMargin, titleTop, width - 2 * textMargin, titleHeight);
    painter.drawText(titleRect, Qt::AlignCenter | Qt::TextWordWrap, book.title);

    // Foil separator line
    int sepY = titleTop + titleHeight + 4;
    painter.setPen(QPen(accentColor, 1.0));
    painter.drawLine(width / 2 - 25, sepY, width / 2 + 25, sepY);

    // Diamond symbol in center of separator
    painter.setBrush(accentColor);
    QPolygonF diamond;
    diamond << QPointF(width / 2, sepY - 3)
            << QPointF(width / 2 + 3, sepY)
            << QPointF(width / 2, sepY + 3)
            << QPointF(width / 2 - 3, sepY);
    painter.drawPolygon(diamond);

    // Author Name Typography
    int authorFontSize = qMax(8, width / 18);
    QFont authorFont("Segoe UI", authorFontSize, QFont::Medium);
    painter.setFont(authorFont);
    painter.setPen(accentColor.lighter(120));
    QRect authorRect(textMargin, sepY + 8, width - 2 * textMargin, height * 0.16);
    painter.drawText(authorRect, Qt::AlignHCenter | Qt::AlignTop | Qt::TextWordWrap, book.author);

    // 3D Book Spine Effect (Left edge shading)
    drawSpineEffect(painter, width, height);

    // Corner Ribbon Bookmark (Top right)
    drawRibbon(painter, width, height, accentColor.name());

    return pixmap;
}

QPixmap CoverArtGenerator::generateThumbnail(const Book &book, int width, int height) {
    QPixmap pixmap(width, height);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    QRectF bookRect(0, 0, width, height);
    QPainterPath path;
    path.addRoundedRect(bookRect, 4, 4);
    painter.setClipPath(path);

    QColor baseColor(book.coverColor.isEmpty() ? "#1B4332" : book.coverColor);
    QColor darkColor = baseColor.darker(140);
    QLinearGradient bgGrad(0, 0, width, height);
    bgGrad.setColorAt(0.0, baseColor);
    bgGrad.setColorAt(1.0, darkColor);
    painter.fillRect(bookRect, bgGrad);

    // Spine shadow
    QLinearGradient spineGrad(0, 0, 6, 0);
    spineGrad.setColorAt(0.0, QColor(0, 0, 0, 90));
    spineGrad.setColorAt(1.0, QColor(0, 0, 0, 0));
    painter.fillRect(QRect(0, 0, 6, height), spineGrad);

    // Foil border
    QColor accent(book.coverAccent.isEmpty() ? "#D4A373" : book.coverAccent);
    painter.setPen(QPen(accent, 0.8));
    painter.setBrush(Qt::NoBrush);
    painter.drawRoundedRect(bookRect.adjusted(3, 3, -3, -3), 2, 2);

    // Initial
    QString initial = book.title.isEmpty() ? "B" : book.title.left(1).toUpper();
    QFont f("Georgia", qMax(8, width / 3), QFont::Bold);
    painter.setFont(f);
    painter.setPen(accent.lighter(130));
    painter.drawText(bookRect.adjusted(3, 0, 0, 0), Qt::AlignCenter, initial);

    // Corner ribbon
    int ribbonW = 5;
    int ribbonH = 10;
    int ribbonX = width - 8;
    QPainterPath ribbon;
    ribbon.moveTo(ribbonX, 0);
    ribbon.lineTo(ribbonX + ribbonW, 0);
    ribbon.lineTo(ribbonX + ribbonW, ribbonH);
    ribbon.lineTo(ribbonX + ribbonW / 2, ribbonH - 2);
    ribbon.lineTo(ribbonX, ribbonH);
    ribbon.closeSubpath();
    painter.setBrush(accent);
    painter.setPen(Qt::NoPen);
    painter.drawPath(ribbon);

    return pixmap;
}

void CoverArtGenerator::drawSpineEffect(QPainter &p, int w, int h) {
    // Spine gradient simulating light reflecting off the book hinge
    QLinearGradient spineGrad(0, 0, 16, 0);
    spineGrad.setColorAt(0.0, QColor(0, 0, 0, 90));
    spineGrad.setColorAt(0.35, QColor(255, 255, 255, 45));
    spineGrad.setColorAt(0.7, QColor(0, 0, 0, 50));
    spineGrad.setColorAt(1.0, QColor(0, 0, 0, 0));

    p.fillRect(QRect(0, 0, 16, h), spineGrad);

    // Crease line
    p.setPen(QColor(0, 0, 0, 60));
    p.drawLine(14, 0, 14, h);
    p.setPen(QColor(255, 255, 255, 40));
    p.drawLine(15, 0, 15, h);
}

void CoverArtGenerator::drawRibbon(QPainter &p, int w, int h, const QString &accentColor) {
    int ribbonW = 12;
    int ribbonH = 34;
    int ribbonX = w - 28;

    QPainterPath ribbon;
    ribbon.moveTo(ribbonX, 0);
    ribbon.lineTo(ribbonX + ribbonW, 0);
    ribbon.lineTo(ribbonX + ribbonW, ribbonH);
    ribbon.lineTo(ribbonX + ribbonW / 2, ribbonH - 6);
    ribbon.lineTo(ribbonX, ribbonH);
    ribbon.closeSubpath();

    p.setBrush(QColor(accentColor));
    p.setPen(Qt::NoPen);
    p.drawPath(ribbon);

    // Ribbon subtle fold shadow
    p.setBrush(QColor(0, 0, 0, 40));
    p.drawRect(ribbonX, 0, ribbonW, 2);
}

void CoverArtGenerator::drawCategoryBadge(QPainter &p, int w, int h, const QString &category) {
    int pillW = qMin(w - 40, 110);
    int pillH = 18;
    int pillX = (w - pillW) / 2;
    int pillY = 20;

    QRectF pillRect(pillX, pillY, pillW, pillH);
    p.setBrush(QColor(0, 0, 0, 70));
    p.setPen(QColor(255, 255, 255, 60));
    p.drawRoundedRect(pillRect, 9, 9);

    QFont badgeFont("Segoe UI", 7, QFont::Bold);
    p.setFont(badgeFont);
    p.setPen(QColor("#FFFFFF"));
    p.drawText(pillRect, Qt::AlignCenter, category.toUpper());
}

void CoverArtGenerator::drawOrnament(QPainter &p, int w, int h, const QString &accentHex, bool isAcademic) {
    int cx = w / 2;
    int cy = h * 0.23;
    QColor accent(accentHex);
    p.setPen(QPen(accent, 1.2));
    p.setBrush(Qt::NoBrush);

    if (isAcademic) {
        // Draw elegant open book emblem
        int bw = 12;
        int bh = 8;
        QPainterPath leftPage;
        leftPage.moveTo(cx, cy + bh);
        leftPage.quadTo(cx - bw / 2, cy + bh - 2, cx - bw, cy + bh - 4);
        leftPage.lineTo(cx - bw, cy - 4);
        leftPage.quadTo(cx - bw / 2, cy - 2, cx, cy);
        leftPage.closeSubpath();

        QPainterPath rightPage;
        rightPage.moveTo(cx, cy + bh);
        rightPage.quadTo(cx + bw / 2, cy + bh - 2, cx + bw, cy + bh - 4);
        rightPage.lineTo(cx + bw, cy - 4);
        rightPage.quadTo(cx + bw / 2, cy - 2, cx, cy);
        rightPage.closeSubpath();

        p.setBrush(accent.lighter(130));
        p.drawPath(leftPage);
        p.drawPath(rightPage);
    } else {
        // Star / Compass motif
        int r = 8;
        p.drawLine(cx, cy - r, cx, cy + r);
        p.drawLine(cx - r, cy, cx + r, cy);
        p.setPen(QPen(accent, 0.8));
        p.drawEllipse(QPointF(cx, cy), r - 2, r - 2);
    }
}

QPixmap CoverArtGenerator::generateLogo(int size) {
    QPixmap pixmap(size, size);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    // Circular base with forest green gradient
    QRectF circleRect(2, 2, size - 4, size - 4);
    QLinearGradient grad(0, 0, size, size);
    grad.setColorAt(0.0, QColor("#1B4332"));
    grad.setColorAt(1.0, QColor("#2D6A4F"));
    painter.setBrush(grad);
    painter.setPen(QPen(QColor("#E8E2D5"), 1.5));
    painter.drawEllipse(circleRect);

    // Open book symbol in gold & cream
    float cx = size / 2.0f;
    float cy = size / 2.0f + 2.0f;
    float bw = size * 0.28f;
    float bh = size * 0.22f;

    QPainterPath bookLeft;
    bookLeft.moveTo(cx, cy + bh * 0.6f);
    bookLeft.quadTo(cx - bw * 0.5f, cy + bh * 0.45f, cx - bw, cy + bh * 0.2f);
    bookLeft.lineTo(cx - bw, cy - bh * 0.6f);
    bookLeft.quadTo(cx - bw * 0.5f, cy - bh * 0.35f, cx, cy - bh * 0.2f);
    bookLeft.closeSubpath();

    QPainterPath bookRight;
    bookRight.moveTo(cx, cy + bh * 0.6f);
    bookRight.quadTo(cx + bw * 0.5f, cy + bh * 0.45f, cx + bw, cy + bh * 0.2f);
    bookRight.lineTo(cx + bw, cy - bh * 0.6f);
    bookRight.quadTo(cx + bw * 0.5f, cy - bh * 0.35f, cx, cy - bh * 0.2f);
    bookRight.closeSubpath();

    painter.setBrush(QColor("#FDFBF7"));
    painter.setPen(QPen(QColor("#D4A373"), 1.0));
    painter.drawPath(bookLeft);
    painter.drawPath(bookRight);

    // Golden sprout/leaf emerging from center of book
    QPainterPath leaf;
    leaf.moveTo(cx, cy - bh * 0.2f);
    leaf.quadTo(cx + 4, cy - bh * 0.8f, cx, cy - bh * 1.1f);
    leaf.quadTo(cx - 4, cy - bh * 0.8f, cx, cy - bh * 0.2f);
    leaf.closeSubpath();

    painter.setBrush(QColor("#E9C46A"));
    painter.setPen(Qt::NoPen);
    painter.drawPath(leaf);

    return pixmap;
}

QPixmap CoverArtGenerator::generateStudentAvatar(const QString &name, const QString &colorHex, int size) {
    QPixmap pixmap(size, size);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    // Background circle
    QColor bg(colorHex.isEmpty() ? "#1F4324" : colorHex);
    painter.setBrush(bg);
    painter.setPen(QPen(QColor("#E2DCce"), 1.5));
    painter.drawEllipse(1, 1, size - 2, size - 2);

    // Initials
    QString initials;
    const auto parts = name.split(' ', Qt::SkipEmptyParts);
    if (!parts.isEmpty()) {
        initials += parts[0][0].toUpper();
        if (parts.size() > 1) {
            initials += parts[1][0].toUpper();
        }
    } else {
        initials = "S";
    }

    QFont font("Segoe UI", size * 0.38, QFont::Bold);
    painter.setFont(font);
    painter.setPen(QColor("#FFFFFF"));
    painter.drawText(QRect(0, 0, size, size), Qt::AlignCenter, initials);

    return pixmap;
}
