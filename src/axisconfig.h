#pragma once

#include <QString>

/*!
 * \brief Настройки отображения оси.
 */
struct AxisConfig
{
    /*!
     * \brief Минимальное значение оси.
     */
    qreal minimum = 0.0;

    /*!
     * \brief Максимальное значение оси.
     */
    qreal maximum = 10.0;

    /*!
     * \brief Шаг между метками оси.
     */
    qreal tickStep = 1.0;

    /*!
     * \brief Подпись оси.
     */
    QString title;
};
