#include "graphmanager.h"

#include <QDateTime>

#include <algorithm>
#include <cmath>

namespace {
constexpr qreal kInvalidSelection = std::numeric_limits<qreal>::quiet_NaN();
}

GraphManager::GraphManager(QObject *parent)
    : QObject(parent)
{
    m_legendModel.setSeriesModel(&m_seriesModel);
    setXAxis(0.0, 10.0, 1.0, QStringLiteral("Ось X"));
    setYAxis(0.0, 100.0, 10.0, QStringLiteral("Ось Y"));
}

GraphSeriesModel *GraphManager::seriesModel() const
{
    return const_cast<GraphSeriesModel *>(&m_seriesModel);
}

LegendModel *GraphManager::legendModel() const
{
    return const_cast<LegendModel *>(&m_legendModel);
}

AxisConfig GraphManager::xAxis() const
{
    return m_xAxis;
}

AxisConfig GraphManager::yAxis() const
{
    return m_yAxis;
}

qreal GraphManager::xMinimum() const
{
    return m_xAxis.minimum;
}

qreal GraphManager::xMaximum() const
{
    return m_xAxis.maximum;
}

qreal GraphManager::yMinimum() const
{
    return m_yAxis.minimum;
}

qreal GraphManager::yMaximum() const
{
    return m_yAxis.maximum;
}

bool GraphManager::hasSelection() const
{
    return m_hasSelection;
}

qreal GraphManager::selectedX() const
{
    return m_selectedX;
}

GraphRenderer *GraphManager::renderer() const
{
    return const_cast<GraphRenderer *>(&m_renderer);
}

int GraphManager::contentWidth() const
{
    return m_renderer.contentSize(m_xAxis, m_yAxis).width();
}

int GraphManager::contentHeight() const
{
    return m_renderer.contentSize(m_xAxis, m_yAxis).height();
}

bool GraphManager::isLegendExpanded() const
{
    return m_legendExpanded;
}

void GraphManager::setLegendExpanded(bool expanded)
{
    if (m_legendExpanded == expanded) {
        return;
    }

    m_legendExpanded = expanded;
    emit legendExpandedChanged();
}

QObject *GraphManager::addSeries(const QString &id, const QString &name, const QColor &color, int lineStyle)
{
    if (GraphSeries *existing = m_seriesModel.findById(id)) {
        return existing;
    }

    auto *series = new GraphSeries(id, this);
    series->setName(name);
    series->setColor(color);
    series->setLineStyle(static_cast<GraphSeries::LineStyle>(lineStyle));
    attachSeries(series);
    m_seriesModel.addSeries(series);
    emit seriesChanged();
    return series;
}

bool GraphManager::removeSeries(const QString &id)
{
    GraphSeries *series = m_seriesModel.findById(id);
    if (!series) {
        return false;
    }

    const bool removed = m_seriesModel.removeSeries(id);
    if (removed) {
        series->deleteLater();
        emit seriesChanged();
    }
    return removed;
}

bool GraphManager::setSeriesPoints(const QString &id, const QVector<GraphPoint> &points)
{
    GraphSeries *series = m_seriesModel.findById(id);
    if (!series) {
        return false;
    }

    series->setPoints(points);
    emit seriesChanged();
    return true;
}

bool GraphManager::setSeriesLineStyle(const QString &id, int lineStyle)
{
    GraphSeries *series = m_seriesModel.findById(id);
    if (!series) {
        return false;
    }

    series->setLineStyle(static_cast<GraphSeries::LineStyle>(lineStyle));
    emit seriesChanged();
    return true;
}

void GraphManager::setXAxis(qreal minimum, qreal maximum, qreal tickStep, const QString &title)
{
    if (maximum <= minimum || tickStep <= 0.0) {
        return;
    }

    m_xAxis = {minimum, maximum, tickStep, title};
    m_xAxisBounds = m_xAxis;
    emit xAxisChanged();
    emit contentSizeChanged();
}

void GraphManager::setYAxis(qreal minimum, qreal maximum, qreal tickStep, const QString &title)
{
    if (maximum <= minimum || tickStep <= 0.0) {
        return;
    }

    m_yAxis = {minimum, maximum, tickStep, title};
    emit yAxisChanged();
    emit contentSizeChanged();
}

void GraphManager::zoomX(qreal factor, qreal centerRatio)
{
    if (factor <= 0.0 || m_xAxis.maximum <= m_xAxis.minimum) {
        return;
    }

    centerRatio = qBound(0.0, centerRatio, 1.0);
    const qreal currentRange = m_xAxis.maximum - m_xAxis.minimum;
    const qreal centerValue = m_xAxis.minimum + currentRange * centerRatio;
    const qreal minimumRange = qMax<qreal>(m_xAxisBounds.tickStep / 8.0, 60.0 * 1000.0);
    const qreal maximumRange = m_xAxisBounds.maximum - m_xAxisBounds.minimum;
    const qreal newRange = qBound(minimumRange, currentRange * factor, maximumRange);

    if (qFuzzyCompare(newRange, maximumRange)) {
        m_xAxis.minimum = m_xAxisBounds.minimum;
        m_xAxis.maximum = m_xAxisBounds.maximum;
        emit xAxisChanged();
        emit contentSizeChanged();
        return;
    }

    const qreal leftPart = centerRatio * newRange;
    const qreal rightPart = newRange - leftPart;
    qreal minimum = centerValue - leftPart;
    qreal maximum = centerValue + rightPart;

    if (minimum < m_xAxisBounds.minimum) {
        maximum += m_xAxisBounds.minimum - minimum;
        minimum = m_xAxisBounds.minimum;
    }

    if (maximum > m_xAxisBounds.maximum) {
        minimum -= maximum - m_xAxisBounds.maximum;
        maximum = m_xAxisBounds.maximum;
    }

    m_xAxis.minimum = minimum;
    m_xAxis.maximum = maximum;
    emit xAxisChanged();
    emit contentSizeChanged();
}

void GraphManager::selectNearestPointX(const QRectF &graphArea, qreal clickX)
{
    const GraphRenderer::SelectionResult result =
        m_renderer.selectNearestX(graphArea, clickX, m_seriesModel.series(), m_xAxis);

    if (!result.found) {
        clearSelection();
        return;
    }

    m_selectedX = result.x;
    m_hasSelection = true;
    emit selectionChanged();
    emit xValueSelected(m_selectedX);
}

void GraphManager::selectNearestPointForXValue(qreal xValue)
{
    const qreal nearestX = findNearestSeriesX(xValue);
    if (qIsNaN(nearestX)) {
        clearSelection();
        return;
    }

    m_selectedX = nearestX;
    m_hasSelection = true;
    emit selectionChanged();
    emit xValueSelected(m_selectedX);
}

void GraphManager::clearSelection()
{
    if (!m_hasSelection) {
        return;
    }

    m_hasSelection = false;
    emit selectionChanged();
}

void GraphManager::toggleLegendExpanded()
{
    setLegendExpanded(!m_legendExpanded);
}

void GraphManager::createDemoData()
{
    while (m_seriesModel.rowCount() > 0) {
        removeSeries(m_seriesModel.series().constFirst()->id());
    }

    struct SeriesSeed {
        QString id;
        QString name;
        QColor color;
        GraphSeries::LineStyle lineStyle;
        QVector<GraphPoint> points;
    };

    const QDateTime startDateTime(QDate(2026, 3, 18), QTime(8, 0));
    const auto timePoint = [&startDateTime](int hourOffset, qreal y) {
        return GraphPoint {qreal(startDateTime.addSecs(hourOffset * 3600).toMSecsSinceEpoch()), y, GraphPointStatus::Normal};
    };

    const QList<SeriesSeed> seeds = {
        {"temperature", "Температура", QColor("#c84c09"), GraphSeries::LineStyle::Straight,
         {timePoint(0, 12), timePoint(1, 18), timePoint(2, 25), timePoint(3, 31), timePoint(4, 28), timePoint(5, 34), timePoint(6, 39), timePoint(7, 42)}},
        {"pressure", "Давление", QColor("#165baa"), GraphSeries::LineStyle::Stepped,
         {timePoint(0, 44), timePoint(1, 42), timePoint(2, 41), timePoint(3, 47), timePoint(4, 55), timePoint(5, 58), timePoint(6, 53), timePoint(7, 61)}},
        {"speed", "Скорость", QColor("#118a52"), GraphSeries::LineStyle::Straight,
         {timePoint(0, 18), timePoint(1, 22), timePoint(2, 20), timePoint(3, 27), timePoint(4, 29), timePoint(5, 32), timePoint(6, 30), timePoint(7, 35)}},
        {"load", "Нагрузка", QColor("#7b3db6"), GraphSeries::LineStyle::Stepped,
         {timePoint(0, 70), timePoint(1, 65), timePoint(2, 62), timePoint(3, 60), timePoint(4, 58), timePoint(5, 63), timePoint(6, 67), timePoint(7, 69)}},
    };

    for (const SeriesSeed &seed : seeds) {
        addSeries(seed.id, seed.name, seed.color, static_cast<int>(seed.lineStyle));
        setSeriesPoints(seed.id, seed.points);
    }

    setXAxis(qreal(startDateTime.toMSecsSinceEpoch()),
             qreal(startDateTime.addSecs(7 * 3600).toMSecsSinceEpoch()),
             qreal(3600 * 1000),
             QStringLiteral("Дата и время"));
    setYAxis(0.0, 80.0, 10.0, QStringLiteral("Значение"));
    clearSelection();
}

void GraphManager::attachSeries(GraphSeries *series)
{
    connect(series, &GraphSeries::nameChanged, this, &GraphManager::seriesChanged);
    connect(series, &GraphSeries::colorChanged, this, &GraphManager::seriesChanged);
    connect(series, &GraphSeries::lineStyleChanged, this, &GraphManager::seriesChanged);
    connect(series, &GraphSeries::pointsChanged, this, &GraphManager::seriesChanged);
}

qreal GraphManager::findNearestSeriesX(qreal targetX) const
{
    qreal bestDistance = std::numeric_limits<qreal>::max();
    qreal bestX = kInvalidSelection;

    for (const GraphSeries *seriesItem : m_seriesModel.series()) {
        const QVector<GraphPoint> &points = seriesItem->points();
        if (points.isEmpty()) {
            continue;
        }

        auto it = std::lower_bound(points.begin(), points.end(), targetX, [](const GraphPoint &point, qreal value) {
            return point.x < value;
        });

        auto tryPoint = [&](QVector<GraphPoint>::const_iterator candidate) {
            if (candidate == points.end()) {
                return;
            }

            const qreal distance = std::abs(candidate->x - targetX);
            if (distance < bestDistance) {
                bestDistance = distance;
                bestX = candidate->x;
            }
        };

        tryPoint(it);
        if (it != points.begin()) {
            tryPoint(std::prev(it));
        }
    }

    return bestX;
}
