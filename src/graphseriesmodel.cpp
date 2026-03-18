#include "graphseriesmodel.h"

GraphSeriesModel::GraphSeriesModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int GraphSeriesModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    return m_series.size();
}

QVariant GraphSeriesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_series.size()) {
        return {};
    }

    const GraphSeries *series = m_series.at(index.row());
    switch (role) {
    case IdRole:
        return series->id();
    case NameRole:
        return series->name();
    case ColorRole:
        return series->color();
    case SeriesRole:
        return QVariant::fromValue(static_cast<QObject *>(m_series.at(index.row())));
    default:
        return {};
    }
}

QHash<int, QByteArray> GraphSeriesModel::roleNames() const
{
    return {
        {IdRole, "seriesId"},
        {NameRole, "name"},
        {ColorRole, "color"},
        {SeriesRole, "seriesObject"},
    };
}

void GraphSeriesModel::addSeries(GraphSeries *series)
{
    const int newRow = m_series.size();
    beginInsertRows(QModelIndex(), newRow, newRow);
    m_series.append(series);
    endInsertRows();

    const auto updateRow = [this, series]() {
        const int row = m_series.indexOf(series);
        if (row < 0) {
            return;
        }

        emit dataChanged(index(row, 0), index(row, 0), {NameRole, ColorRole, SeriesRole});
    };

    connect(series, &GraphSeries::nameChanged, this, updateRow);
    connect(series, &GraphSeries::colorChanged, this, updateRow);
}

bool GraphSeriesModel::removeSeries(const QString &id)
{
    for (int row = 0; row < m_series.size(); ++row) {
        if (m_series.at(row)->id() != id) {
            continue;
        }

        beginRemoveRows(QModelIndex(), row, row);
        m_series.removeAt(row);
        endRemoveRows();
        return true;
    }

    return false;
}

GraphSeries *GraphSeriesModel::findById(const QString &id) const
{
    for (GraphSeries *series : m_series) {
        if (series->id() == id) {
            return series;
        }
    }

    return nullptr;
}

QList<GraphSeries *> GraphSeriesModel::series() const
{
    return m_series;
}

void GraphSeriesModel::clear()
{
    if (m_series.isEmpty()) {
        return;
    }

    beginResetModel();
    m_series.clear();
    endResetModel();
}
