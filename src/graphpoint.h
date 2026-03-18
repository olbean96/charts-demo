#pragma once

#include <QtGlobal>

/*!
 * \brief Статус точки графика.
 */
enum class GraphPointStatus
{
    Normal = 0,
    Warning,
    Alarm
};

/*!
 * \brief Описывает одну точку графика.
 */
struct GraphPoint
{
    /*!
     * \brief Координата точки по оси X.
     */
    qreal x = 0.0;

    /*!
     * \brief Координата точки по оси Y.
     */
    qreal y = 0.0;

    /*!
     * \brief Статус точки.
     */
    GraphPointStatus status = GraphPointStatus::Normal;
};
