#include "graphrenderer.h"

#include <QDateTime>
#include <QPainter>
#include <QPen>
#include <QVector>

#include <algorithm>
#include <cmath>
#include <limits>

namespace {
constexpr int kLeftMargin = 92;
constexpr int kRightMargin = 36;
constexpr int kTopMargin = 20;
constexpr int kBottomMargin = 110;
constexpr qreal kPointRadius = 3.5;
constexpr qreal kXAxisLabelWidth = 126.0;
constexpr qreal kXAxisDateTop = 6.0;
constexpr qreal kXAxisTimeTop = 22.0;
constexpr qreal kXAxisLineHeight = 14.0;
constexpr int kMinimumChartWidth = 720;
constexpr int kMinimumChartHeight = 360;
constexpr int kPixelsPerXTick = 116;
constexpr int kPixelsPerYTick = 32;

int tickCount(const AxisConfig &axis)
{
    if (axis.tickStep <= 0.0 || axis.maximum <= axis.minimum) {
        return 2;
    }

    return qMax(2, int(std::floor((axis.maximum - axis.minimum) / axis.tickStep + 0.5)) + 1);
}
}

GraphRenderer::GraphRenderer(QObject *parent)
    : QObject(parent)
{
}

QImage GraphRenderer::render(const QSize &size,
                             const QList<GraphSeries *> &series,
                             const AxisConfig &xAxis,
                             const AxisConfig &yAxis,
                             qreal selectedX,
                             bool hasSelection) const
{
    if (size.isEmpty()) {
        return {};
    }

    QImage image(size, QImage::Format_ARGB32_Premultiplied);
    image.fill(QColor("#fffdf8"));

    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);

    const QRectF graphArea = chartRect(size);
    drawAxes(painter, graphArea, xAxis, yAxis);
    drawSeries(painter, graphArea, series, xAxis, yAxis);

    if (hasSelection) {
        drawSelection(painter, graphArea, xAxis, selectedX);
    }

    return image;
}

GraphRenderer::SelectionResult GraphRenderer::selectNearestX(const QRectF &graphArea,
                                                             qreal clickX,
                                                             const QList<GraphSeries *> &series,
                                                             const AxisConfig &xAxis) const
{
    // Поиск выполняется только по X и использует бинарный поиск в каждом наборе точек.
    // Это позволяет не перебирать все точки даже при крупных массивах данных.
    if (series.isEmpty() || graphArea.width() <= 0.0 || xAxis.maximum <= xAxis.minimum) {
        return {};
    }

    const qreal normalized = std::clamp((clickX - graphArea.left()) / graphArea.width(), 0.0, 1.0);
    const qreal targetX = xAxis.minimum + normalized * (xAxis.maximum - xAxis.minimum);

    qreal bestDistance = std::numeric_limits<qreal>::max();
    qreal bestX = 0.0;
    bool found = false;

    for (const GraphSeries *seriesItem : series) {
        const QVector<GraphPoint> &points = seriesItem->points();
        if (points.isEmpty()) {
            continue;
        }

        auto it = std::lower_bound(points.begin(), points.end(), targetX, [](const GraphPoint &point, qreal value) {
            return point.x < value;
        });

        auto checkCandidate = [&](QVector<GraphPoint>::const_iterator candidate) {
            if (candidate == points.end()) {
                return;
            }

            const qreal distance = std::abs(candidate->x - targetX);
            if (!found || distance < bestDistance) {
                bestDistance = distance;
                bestX = candidate->x;
                found = true;
            }
        };

        checkCandidate(it);
        if (it != points.begin()) {
            checkCandidate(std::prev(it));
        }
    }

    return {found, bestX};
}

QSize GraphRenderer::contentSize(const AxisConfig &xAxis, const AxisConfig &yAxis) const
{
    const int contentWidth = qMax(kMinimumChartWidth, tickCount(xAxis) * kPixelsPerXTick);
    const int contentHeight = qMax(kMinimumChartHeight, tickCount(yAxis) * kPixelsPerYTick);

    return QSize(kLeftMargin + contentWidth + kRightMargin,
                 kTopMargin + contentHeight + kBottomMargin);
}

QRectF GraphRenderer::chartRect(const QSize &size) const
{
    return QRectF(kLeftMargin,
                  kTopMargin,
                  std::max(0, size.width() - kLeftMargin - kRightMargin),
                  std::max(0, size.height() - kTopMargin - kBottomMargin));
}

QPointF GraphRenderer::mapToPixel(const GraphPoint &point,
                                  const QRectF &graphArea,
                                  const AxisConfig &xAxis,
                                  const AxisConfig &yAxis) const
{
    const qreal xRatio = (point.x - xAxis.minimum) / (xAxis.maximum - xAxis.minimum);
    const qreal yRatio = (point.y - yAxis.minimum) / (yAxis.maximum - yAxis.minimum);
    const qreal x = graphArea.left() + xRatio * graphArea.width();
    const qreal y = graphArea.bottom() - yRatio * graphArea.height();
    return {x, y};
}

QPolygonF GraphRenderer::buildPolyline(const QVector<GraphPoint> &points,
                                       GraphSeries::LineStyle lineStyle,
                                       const QRectF &graphArea,
                                       const AxisConfig &xAxis,
                                       const AxisConfig &yAxis) const
{
    QPolygonF polyline;
    if (points.isEmpty()) {
        return polyline;
    }

    if (lineStyle == GraphSeries::LineStyle::Stepped) {
        polyline.reserve(points.size() * 2);
        polyline.append(mapToPixel(points.first(), graphArea, xAxis, yAxis));

        for (int index = 1; index < points.size(); ++index) {
            const GraphPoint &previous = points.at(index - 1);
            const GraphPoint &current = points.at(index);

            polyline.append(mapToPixel({current.x, previous.y, previous.status}, graphArea, xAxis, yAxis));
            polyline.append(mapToPixel(current, graphArea, xAxis, yAxis));
        }
        return polyline;
    }

    polyline.reserve(points.size());
    for (const GraphPoint &point : points) {
        polyline.append(mapToPixel(point, graphArea, xAxis, yAxis));
    }
    return polyline;
}

QString GraphRenderer::formatXAxisDate(qreal xValue) const
{
    const QDateTime dateTime = QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(xValue));
    return dateTime.toString(QStringLiteral("dd.MM.yyyy"));
}

QString GraphRenderer::formatXAxisTime(qreal xValue) const
{
    const QDateTime dateTime = QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(xValue));
    return dateTime.toString(QStringLiteral("HH:mm"));
}

void GraphRenderer::drawAxes(QPainter &painter,
                             const QRectF &graphArea,
                             const AxisConfig &xAxis,
                             const AxisConfig &yAxis) const
{
    // Отрисовка осей включает рамку области графика, подписи осей и сетку.
    // Для оси X метки выводятся в две строки: дата и время.
    painter.save();

    const QPen axisPen(QColor("#3d3d3d"), 1.0);
    painter.setPen(axisPen);
    painter.drawRect(graphArea);

    painter.setPen(QColor("#676767"));
    painter.drawText(QRectF(graphArea.left(), graphArea.bottom() + 58, graphArea.width(), 22),
                     Qt::AlignCenter,
                     xAxis.title);
    painter.save();
    painter.translate(18, graphArea.center().y());
    painter.rotate(-90);
    painter.drawText(QRectF(-graphArea.height() / 2.0, 0, graphArea.height(), 24),
                     Qt::AlignCenter,
                     yAxis.title);
    painter.restore();

    const QPen gridPen(QColor("#e3ddd4"), 1.0, Qt::DashLine);
    painter.setPen(gridPen);

    QFont tickFont = painter.font();
    tickFont.setPixelSize(10);
    painter.setFont(tickFont);

    QVector<qreal> xTickValues;
    for (qreal xValue = xAxis.minimum; xValue <= xAxis.maximum + 0.0001; xValue += xAxis.tickStep) {
        xTickValues.append(xValue);
    }

    for (int index = 0; index < xTickValues.size(); ++index) {
        const qreal xValue = xTickValues.at(index);
        const qreal ratio = (xValue - xAxis.minimum) / (xAxis.maximum - xAxis.minimum);
        const qreal x = graphArea.left() + ratio * graphArea.width();
        painter.drawLine(QPointF(x, graphArea.top()), QPointF(x, graphArea.bottom()));

        if (index == xTickValues.size() - 1) {
            continue;
        }

        painter.setPen(QColor("#4d4d4d"));
        qreal labelLeft = std::clamp(x + 2.0,
                                     graphArea.left(),
                                     graphArea.right() - kXAxisLabelWidth);

        painter.drawText(QRectF(labelLeft, graphArea.bottom() + kXAxisDateTop, kXAxisLabelWidth, kXAxisLineHeight),
                         Qt::AlignLeft | Qt::AlignTop,
                         formatXAxisDate(xValue));
        painter.drawText(QRectF(labelLeft, graphArea.bottom() + kXAxisTimeTop, kXAxisLabelWidth, kXAxisLineHeight),
                         Qt::AlignLeft | Qt::AlignTop,
                         formatXAxisTime(xValue));
        painter.setPen(gridPen);
    }

    for (qreal yValue = yAxis.minimum; yValue <= yAxis.maximum + 0.0001; yValue += yAxis.tickStep) {
        const qreal ratio = (yValue - yAxis.minimum) / (yAxis.maximum - yAxis.minimum);
        const qreal y = graphArea.bottom() - ratio * graphArea.height();
        painter.drawLine(QPointF(graphArea.left(), y), QPointF(graphArea.right(), y));
        painter.setPen(QColor("#4d4d4d"));
        painter.drawText(QRectF(24, y - 9, kLeftMargin - 36, 18),
                         Qt::AlignRight | Qt::AlignVCenter,
                         QString::number(yValue, 'f', 1));
        painter.setPen(gridPen);
    }

    painter.restore();
}

void GraphRenderer::drawSeries(QPainter &painter,
                               const QRectF &graphArea,
                               const QList<GraphSeries *> &series,
                               const AxisConfig &xAxis,
                               const AxisConfig &yAxis) const
{
    // Сначала рисуется соединительная линия выбранного типа,
    // затем поверх наносятся маркеры исходных точек набора.
    painter.save();
    painter.setClipRect(graphArea.adjusted(-1, -1, 1, 1));

    for (const GraphSeries *seriesItem : series) {
        const QVector<GraphPoint> &points = seriesItem->points();
        if (points.isEmpty()) {
            continue;
        }

        const QPolygonF polyline = buildPolyline(points, seriesItem->lineStyle(), graphArea, xAxis, yAxis);

        painter.setPen(QPen(seriesItem->color(), 2.0));
        if (polyline.size() >= 2) {
            painter.drawPolyline(polyline);
        }

        painter.setBrush(seriesItem->color());
        painter.setPen(Qt::NoPen);
        for (const GraphPoint &point : points) {
            const QPointF pointPixel = mapToPixel(point, graphArea, xAxis, yAxis);
            painter.drawEllipse(pointPixel, kPointRadius, kPointRadius);
        }
    }

    painter.restore();
}

void GraphRenderer::drawSelection(QPainter &painter,
                                  const QRectF &graphArea,
                                  const AxisConfig &xAxis,
                                  qreal selectedX) const
{
    const qreal ratio = (selectedX - xAxis.minimum) / (xAxis.maximum - xAxis.minimum);
    const qreal x = graphArea.left() + ratio * graphArea.width();

    painter.save();
    painter.setPen(QPen(QColor("#222222"), 1.0, Qt::DashLine));
    painter.drawLine(QPointF(x, graphArea.top()), QPointF(x, graphArea.bottom()));
    painter.restore();
}
