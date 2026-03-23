#pragma once

#include "axisconfig.h"
#include "graphseries.h"

#include <QImage>
#include <QObject>
#include <QPolygonF>
#include <QRectF>

/*!
 * \brief Выполняет отрисовку графиков в QImage через QPainter.
 */
class GraphRenderer : public QObject
{
    Q_OBJECT

public:
    /*!
     * \brief Результат поиска ближайшей точки по оси X.
     */
    struct SelectionResult
    {
        /*!
         * \brief Признак успешного поиска.
         */
        bool found = false;

        /*!
         * \brief Найденное значение по оси X.
         */
        qreal x = 0.0;
    };

    /*!
     * \brief Создает объект рендерера.
     * \param parent Родительский объект Qt.
     */
    explicit GraphRenderer(QObject *parent = nullptr);

    /*!
     * \brief Строит изображение со всеми графиками и осями.
     * \param size Размер выходного изображения.
     * \param series Список графиков.
     * \param xAxis Настройки оси X.
     * \param yAxis Настройки оси Y.
     * \param selectedX Выбранное значение оси X.
     * \param hasSelection Наличие активного выбора.
     */
    QImage render(const QSize &size,
                  const QList<GraphSeries *> &series,
                  const AxisConfig &xAxis,
                  const AxisConfig &yAxis,
                  qreal selectedX,
                  bool hasSelection) const;

    /*!
     * \brief Ищет ближайшую к экранной координате точку по оси X.
     * \param graphArea Область построения без полей.
     * \param clickX Экранная координата клика по оси X.
     * \param series Список графиков.
     * \param xAxis Настройки оси X.
     */
    SelectionResult selectNearestX(const QRectF &graphArea,
                                   qreal clickX,
                                   const QList<GraphSeries *> &series,
                                   const AxisConfig &xAxis) const;

    /*!
     * \brief Возвращает рекомендуемый размер области рисования для текущих осей.
     * \param xAxis Настройки оси X.
     * \param yAxis Настройки оси Y.
     */
    QSize contentSize(const AxisConfig &xAxis, const AxisConfig &yAxis) const;

    /*!
     * \brief Возвращает рабочую область графика внутри виджета.
     * \param size Полный размер области рисования.
     */
    QRectF chartRect(const QSize &size) const;

private:
    QPointF mapToPixel(const GraphPoint &point,
                       const QRectF &graphArea,
                       const AxisConfig &xAxis,
                       const AxisConfig &yAxis) const;
    QPolygonF buildPolyline(const QVector<GraphPoint> &points,
                            GraphSeries::LineStyle lineStyle,
                            const QRectF &graphArea,
                            const AxisConfig &xAxis,
                            const AxisConfig &yAxis) const;
    QString formatXAxisDate(qreal xValue) const;
    QString formatXAxisTime(qreal xValue) const;
    void drawAxes(QPainter &painter,
                  const QRectF &graphArea,
                  const AxisConfig &xAxis,
                  const AxisConfig &yAxis) const;
    void drawSeries(QPainter &painter,
                    const QRectF &graphArea,
                    const QList<GraphSeries *> &series,
                    const AxisConfig &xAxis,
                    const AxisConfig &yAxis) const;
    void drawSelection(QPainter &painter,
                       const QRectF &graphArea,
                       const AxisConfig &xAxis,
                       qreal selectedX) const;
};
