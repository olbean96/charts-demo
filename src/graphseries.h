#pragma once

#include "graphpoint.h"

#include <QColor>
#include <QObject>
#include <QVector>

/*!
 * \brief Описывает один график с набором точек и параметрами отображения.
 */
class GraphSeries : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString id READ id CONSTANT)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(LineStyle lineStyle READ lineStyle WRITE setLineStyle NOTIFY lineStyleChanged)

public:
    /*!
     * \brief Режим соединения точек графика.
     */
    enum class LineStyle
    {
        Straight = 0,
        Stepped
    };
    Q_ENUM(LineStyle)

    /*!
     * \brief Создает объект графика.
     * \param id Уникальный идентификатор графика.
     * \param parent Родительский объект Qt.
     */
    explicit GraphSeries(QString id, QObject *parent = nullptr);

    /*!
     * \brief Возвращает идентификатор графика.
     */
    QString id() const;

    /*!
     * \brief Возвращает отображаемое название графика.
     */
    QString name() const;

    /*!
     * \brief Устанавливает отображаемое название графика.
     * \param name Новое название графика.
     */
    void setName(const QString &name);

    /*!
     * \brief Возвращает цвет линии графика.
     */
    QColor color() const;

    /*!
     * \brief Устанавливает цвет линии графика.
     * \param color Новый цвет линии.
     */
    void setColor(const QColor &color);

    /*!
     * \brief Возвращает набор точек графика.
     */
    const QVector<GraphPoint> &points() const;

    /*!
     * \brief Возвращает стиль соединения точек.
     */
    LineStyle lineStyle() const;

    /*!
     * \brief Устанавливает стиль соединения точек.
     * \param lineStyle Новый стиль линии.
     */
    void setLineStyle(LineStyle lineStyle);

    /*!
     * \brief Полностью заменяет набор точек графика.
     * \param points Новый набор точек.
     */
    void setPoints(QVector<GraphPoint> points);

signals:
    /*!
     * \brief Сигнал отправляется при изменении названия графика.
     */
    void nameChanged();

    /*!
     * \brief Сигнал отправляется при изменении цвета линии.
     */
    void colorChanged();

    /*!
     * \brief Сигнал отправляется при изменении набора точек.
     */
    void pointsChanged();

    /*!
     * \brief Сигнал отправляется при изменении стиля линии.
     */
    void lineStyleChanged();

protected:
    /*!
     * \brief Сортирует точки по оси X для быстрого поиска и корректной отрисовки.
     */
    void sortPointsByX();

private:
    QString m_id;
    QString m_name;
    QColor m_color = Qt::green;
    LineStyle m_lineStyle = LineStyle::Straight;
    QVector<GraphPoint> m_points;
};
