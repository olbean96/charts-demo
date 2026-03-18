#pragma once

#include "graphseries.h"

#include <QAbstractListModel>

/*!
 * \brief Модель списка графиков для доступа к объектам из QML и других модулей.
 */
class GraphSeriesModel : public QAbstractListModel
{
    Q_OBJECT

public:
    /*!
     * \brief Роли модели графиков.
     */
    enum Roles
    {
        IdRole = Qt::UserRole + 1,
        NameRole,
        ColorRole,
        SeriesRole
    };
    Q_ENUM(Roles)

    /*!
     * \brief Создает модель графиков.
     * \param parent Родительский объект Qt.
     */
    explicit GraphSeriesModel(QObject *parent = nullptr);

    /*!
     * \brief Возвращает количество графиков.
     */
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    /*!
     * \brief Возвращает данные графика по роли.
     * \param index Индекс строки.
     * \param role Роль модели.
     */
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    /*!
     * \brief Возвращает список имен ролей.
     */
    QHash<int, QByteArray> roleNames() const override;

    /*!
     * \brief Добавляет график в модель.
     * \param series Новый график.
     */
    void addSeries(GraphSeries *series);

    /*!
     * \brief Удаляет график из модели по идентификатору.
     * \param id Идентификатор графика.
     * \return true, если график найден и удален.
     */
    bool removeSeries(const QString &id);

    /*!
     * \brief Возвращает график по идентификатору.
     * \param id Идентификатор графика.
     * \return Указатель на график либо nullptr.
     */
    GraphSeries *findById(const QString &id) const;

    /*!
     * \brief Возвращает список графиков.
     */
    QList<GraphSeries *> series() const;

    /*!
     * \brief Полностью очищает модель графиков.
     */
    void clear();

private:
    QList<GraphSeries *> m_series;
};
