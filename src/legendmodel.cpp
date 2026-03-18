#include "legendmodel.h"

LegendModel::LegendModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int LegendModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid() || !m_seriesModel) {
        return 0;
    }

    return m_seriesModel->rowCount();
}

QVariant LegendModel::data(const QModelIndex &index, int role) const
{
    if (!m_seriesModel) {
        return {};
    }

    return m_seriesModel->data(index, role == NameRole ? GraphSeriesModel::NameRole : GraphSeriesModel::ColorRole);
}

QHash<int, QByteArray> LegendModel::roleNames() const
{
    return {
        {NameRole, "name"},
        {ColorRole, "color"},
    };
}

void LegendModel::setSeriesModel(GraphSeriesModel *seriesModel)
{
    if (m_seriesModel == seriesModel) {
        return;
    }

    beginResetModel();
    if (m_seriesModel) {
        disconnect(m_seriesModel, nullptr, this, nullptr);
    }

    m_seriesModel = seriesModel;

    if (m_seriesModel) {
        auto resetModel = [this]() {
            beginResetModel();
            endResetModel();
        };
        connect(m_seriesModel, &QAbstractItemModel::modelReset, this, resetModel);
        connect(m_seriesModel, &QAbstractItemModel::rowsInserted, this, resetModel);
        connect(m_seriesModel, &QAbstractItemModel::rowsRemoved, this, resetModel);
        connect(m_seriesModel, &QAbstractItemModel::dataChanged, this, [this](const QModelIndex &topLeft, const QModelIndex &bottomRight) {
            emit dataChanged(index(topLeft.row(), 0), index(bottomRight.row(), 0), {});
        });
    }

    endResetModel();
}
