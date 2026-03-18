#include "graphseries.h"

#include <algorithm>

GraphSeries::GraphSeries(QString id, QObject *parent)
    : QObject(parent)
    , m_id(std::move(id))
{
}

QString GraphSeries::id() const
{
    return m_id;
}

QString GraphSeries::name() const
{
    return m_name;
}

void GraphSeries::setName(const QString &name)
{
    if (m_name == name) {
        return;
    }

    m_name = name;
    emit nameChanged();
}

QColor GraphSeries::color() const
{
    return m_color;
}

void GraphSeries::setColor(const QColor &color)
{
    if (m_color == color) {
        return;
    }

    m_color = color;
    emit colorChanged();
}

const QVector<GraphPoint> &GraphSeries::points() const
{
    return m_points;
}

GraphSeries::LineStyle GraphSeries::lineStyle() const
{
    return m_lineStyle;
}

void GraphSeries::setLineStyle(LineStyle lineStyle)
{
    if (m_lineStyle == lineStyle) {
        return;
    }

    m_lineStyle = lineStyle;
    emit lineStyleChanged();
}

void GraphSeries::setPoints(QVector<GraphPoint> points)
{
    m_points = std::move(points);
    sortPointsByX();
    emit pointsChanged();
}

void GraphSeries::sortPointsByX()
{
    std::sort(m_points.begin(), m_points.end(), [](const GraphPoint &left, const GraphPoint &right) {
        return left.x < right.x;
    });
}
