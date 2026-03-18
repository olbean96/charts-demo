#pragma once

#include "graphseriesmodel.h"

/*!
 * \brief Модель легенды, показывающая цвет и название графиков.
 */
class LegendModel : public QAbstractListModel
{
    Q_OBJECT

public:
    /*!
     * \brief Роли модели легенды.
     */
    enum Roles
    {
        NameRole = Qt::UserRole + 1,
        ColorRole
    };
    Q_ENUM(Roles)

    /*!
     * \brief Создает модель легенды.
     * \param parent Родительский объект Qt.
     */
    explicit LegendModel(QObject *parent = nullptr);

    /*!
     * \brief Возвращает количество записей легенды.
     */
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    /*!
     * \brief Возвращает данные легенды по роли.
     * \param index Индекс строки.
     * \param role Роль модели.
     */
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    /*!
     * \brief Возвращает список имен ролей.
     */
    QHash<int, QByteArray> roleNames() const override;

    /*!
     * \brief Устанавливает источник данных для легенды.
     * \param seriesModel Модель графиков.
     */
    void setSeriesModel(GraphSeriesModel *seriesModel);

private:
    GraphSeriesModel *m_seriesModel = nullptr;
};
